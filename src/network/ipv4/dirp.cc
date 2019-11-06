// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>
#include <time.h>
#include <machine/uart.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;
DIRP* DIRP::_networks[];

static const int DELAY_SECONDS = 1000000;

// TODOr use abstractions like Packet and Header
int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size)
{
    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);

    // Our packet has a header with addresses, plus the data to be sent
    Header header(Address(dirp->nic()->address(), from), to, size);
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

    // wait for ACK
    Buffer* buf = _observed.notified(from);

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
    Packet* packet = buf->frame()->data<Packet>();
    db<DIRP>(INF) << "Receiving from: MAC " << packet->header()->from() << endl;
    db<DIRP>(INF) << "Receiving into port: " << packet->header()->to().port() << endl;
    db<DIRP>(INF) << "Receiving data: " << packet->data<char>() << endl;

    acknowledged(packet);

    memcpy(d, packet->data<void>(), s);
    buf->nic()->free(buf);

    return s;
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

    Address self(h->to().mac(), h->to().port());
    Header header(self, h->from(), size, Code::ACK);
    Packet packet(header, data, size);

    Delay (1000000);
    DIRP* dirp = get_by_nic(0);
    dirp->nic()->send(h->from().mac(), dirp->PROTOCOL, reinterpret_cast<void *>(&packet), sizeof(packet));
}

int DIRP::get_time() {
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

