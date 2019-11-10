// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>
#include <time.h>
#include <machine/uart.h>
#include <utility/random.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;
DIRP* DIRP::_networks[];

static const int DELAY_SECONDS = 1000000;

// TODOr use abstractions like Packet and Header
int DIRP::send(const Address::Local & from, const Address & to, const void* data, unsigned int size)
{
    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);

    // Our packet has a header with addresses, plus the data to be sent
    Header header(Address(dirp->nic()->address(), from), to, size, dirp->_clock->now());
    Packet packet(header, data, size);
    
    db<DIRP>(INF) << "Sending from MAC: " << packet.header()->from() << endl;
    db<DIRP>(INF) << "Sending to MAC: " << packet.header()->to() << endl;
    db<DIRP>(INF) << "Sending data: " << packet.data<char>() << endl;

    dirp->nic()->send(to.mac(), dirp->PROTOCOL, reinterpret_cast<void *>(&packet), sizeof(packet));

    // creates handler to resend message
    bool timed_out = false;
    int retries = Traits<Network>::RETRIES;
    int timeout = Traits<Network>::TIMEOUT;
    DIRP::DIRP_Sender dirp_sender(packet, size, dirp, &timed_out, retries);
    dirp->_handler = new Functor_Handler<DIRP::DIRP_Sender>(&(DIRP::retry_send), &dirp_sender);

    // creates alarm that resend the message in case of no ACK received
    dirp->_alarm = new Alarm((DELAY_SECONDS * (int) timeout/retries), dirp->_handler, retries + 1);

    db<DIRP>(WRN) << "DIRP::send() - _clock_start_time= " << dirp->_clock_start_time << endl;
    db<DIRP>(WRN) << "DIRP::send() - _clock->now()= " << reinterpret_cast<long unsigned int>(dirp->_clock->now()) << endl;
    db<DIRP>(WRN) << "DIRP::send() before - Alarm::elapsed()= " << reinterpret_cast<int>(Alarm::elapsed()) << endl;

    // wait for ACK
    Buffer* buf = _observed.notified(from);

    db<DIRP>(WRN) << "DIRP::send() after - Alarm::elapsed()= " << reinterpret_cast<int>(Alarm::elapsed()) << endl;
    // at this point: either an ACK arrived or a ocurred timeout.

    // cleanup alarm and handler
    delete dirp->_alarm;
    delete dirp->_handler;

    if (buf == nullptr || timed_out) {
        db<DIRP>(WRN) << "DIRP::send() - TIMEOUT!" << endl;
        return -1;
    }
    db<DIRP>(WRN) << "DIRP::send() - ACK received!" << endl;

    return size;
}

// TODOr use abstractions like Packet and Header
int DIRP::receive(Buffer * buf, void * d, unsigned int s)
{
    DIRP * dirp = DIRP::get_by_nic(0);
    
    /* Nota:
     * Mudar o _elapsed aparentemente não faz nada com o clock->now()
     * Ver o que realmente tem que mudar para atualizar o relógio.
     */
    // Add random delay to emulate desynchronizing
    Alarm::elapsed() += (Random::random()%200)- 100;

    db<DIRP>(WRN) << "DIRP::receive() - _clock_start_time= " << dirp->_clock_start_time << endl;
    db<DIRP>(WRN) << "DIRP::receive() - _clock->now()= " << reinterpret_cast<long unsigned int>(dirp->_clock->now()) << endl;
    db<DIRP>(WRN) << "DIRP::receive() - Alarm::elapsed()= " << reinterpret_cast<int>(Alarm::elapsed()) << endl;

    Packet* packet = buf->frame()->data<Packet>();
    Address from = packet->header()->from();
    db<DIRP>(WRN) << "Receiving from: MAC " << from << endl;
    db<DIRP>(WRN) << "Receiving into port: " << packet->header()->to().port() << endl;
    db<DIRP>(WRN) << "Receiving data: " << packet->data<char>() << endl;
    db<DIRP>(WRN) << "Receiving timestamp: " << packet->header()->timestamp() << endl;

    // Se o pacote estiver vindo do Master
    if (dirp->is_master(from)) {
        dirp->synchronize_time(packet->header()->timestamp());
    }


    acknowledged(packet);

    memcpy(d, packet->data<void>(), s);
    buf->nic()->free(buf);

    return s;
}

void DIRP::synchronize_time(Second timestamp) {
    /* 
     * When receives first packet from master:
     * - saves master's timestamp and its own time
     * - mark flag to indicate synchronizing algorithm started
     */
    if (!NTP.synchronizing) {
        NTP.ts[0] = timestamp;
        NTP.ts[1] = _clock->now();
        NTP.synchronizing = true;
    }
    /* When receives second packet:
     * - saves master's timestamp and its own time
     * - use all (4) saved timestamps to calculate Propagation Delay and Offset
     * - Updates own clock
     * - mark flag to indicate synchronizing is done
     */
    else {
        NTP.ts[2] = timestamp;
        NTP.ts[3] = _clock->now();

        Second PD = (NTP.ts[1] - NTP.ts[0] + NTP.ts[3] - NTP.ts[2])/2;
        Second offset = NTP.ts[1] - NTP.ts[0] - PD;
        db<DIRP>(WRN) << "offset = " << reinterpret_cast<long unsigned int>(offset) << endl;

        // Mudar o elapsed aparentemente não funciona
        db<DIRP>(WRN) << "(before) new Alarm::elapsed() = " << reinterpret_cast<int>(Alarm::elapsed()) << endl;
        int elapsed = Alarm::elapsed() - (1 + offset) * 1000;
        Alarm::elapsed(elapsed);
        NTP.synchronizing = false;
        db<DIRP>(WRN) << "new _clock->now() = " << reinterpret_cast<long unsigned int>(_clock->now()) << endl;
        db<DIRP>(WRN) << "(after) new Alarm::elapsed() = " << reinterpret_cast<int>(Alarm::elapsed()) << endl;
    }
}

void DIRP::update(Observed* obs, const Protocol& prot, Buffer* buf)
{
    Packet packet;
    memcpy(&packet, buf->frame()->data<Packet>(), sizeof(Packet));

    unsigned int port = packet.header()->to().port();

    buf->nic(_nic);
    if(!notify(port, buf))
        buf->nic()->free(buf);
}

void DIRP::acknowledged(Packet * pkt)
{
    Header * h = pkt->header();

    unsigned int size = 1;
    void * data[1];
    unsigned short nothing = 0;
    memcpy(data, &nothing, sizeof(unsigned short));

    DIRP* dirp = get_by_nic(0);
    Address self(h->to().mac(), h->to().port());
    Header header(self, h->from(), size, dirp->_clock->now(), Code::ACK);
    Packet packet(header, data, size);

    Delay (1000000);
    dirp->nic()->send(h->from().mac(), dirp->PROTOCOL, reinterpret_cast<void *>(&packet), sizeof(packet));
}

int DIRP::get_time() {
    // NOT WORKING

    db<DIRP>(INF) << "Asking time to Serial..." << endl;
    UART uart(0, 115200, 8, 0, 1); // using 0 for network, 1 is default for console (-serial mon:stdio)
    uart.loopback(true);
    uart.put('z');
    
    int current_time = uart.get();
    db<DIRP>(INF) << "Received time: " << current_time << endl;
    return current_time;
}

__END_SYS

#endif

