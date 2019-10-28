// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>
#include <time.h>

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

    unsigned short port = to.port();
    Ethernet::Address mac_addr = dirp->address().mac();

    unsigned int port_size = sizeof(Address::Local);
    unsigned int mac_size = sizeof(Ethernet::Address);
    unsigned int ack_size = sizeof(Code);
    char packet[size + port_size + mac_size + ack_size];

    Code noack = Code::NOTHING;

    memcpy(packet, &port, port_size);  // add port
    memcpy(&packet[port_size], &mac_addr, mac_size);  // add MAC after port
    memcpy(&packet[port_size + mac_size], &noack, ack_size);  // add ACK after MAC
    memcpy(&packet[port_size + mac_size + ack_size], data, size); // add data after ACK
    dirp->nic()->send(to.mac(), dirp->PROTOCOL, reinterpret_cast<void *>(packet), sizeof(packet));

    // Retransmission: creates handler to resend message
    bool timed_out = false;
    int retries = Traits<Network>::RETRIES;
    int timeout = Traits<Network>::TIMEOUT;
    DIRP::DIRP_Sender dirp_sender(from, to, reinterpret_cast<void *>(packet), sizeof(packet), dirp, &timed_out, retries);
    dirp->_handler = new Functor_Handler<DIRP::DIRP_Sender>(&(DIRP::retry_send), &dirp_sender);

    // creates alarm that resend the message in case of no ACK received
    dirp->_alarm = new Alarm((DELAY_SECONDS * (int) timeout/retries), dirp->_handler, retries + 1);

    // wait for ACK
    Buffer* buf = _observed.notified(from);

    // at this point: either an ACK arrived or a timeout occurred.

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
    char * data = buf->frame()->data<char>();

    acknowledged(data);

    unsigned int port_size = sizeof(Address::Local);
    unsigned int mac_size = sizeof(Ethernet::Address);
    unsigned int header_size = port_size + mac_size + sizeof(Code);

    memcpy(d, &(data[header_size]), s);  // n bytes is the port length
    buf->nic()->free(buf);

    return s;
}

// TODOr use abstractions like Packet and Header
void DIRP::update(Observed* obs, const Protocol& prot, Buffer* buf)
{
    // TODOr 5 is: DATA_SIZE from main()
    unsigned int size = 5 + 4 + 4 + 6;
    char data[size];

    memcpy(data, buf->frame()->data<char>(), size);

    // TODOr: remember that if we are getting 1 byte here, the DIRP port is limited to 127.
    // the first byte of data represents an unsigned int, not a char
    unsigned short port = (unsigned short)((unsigned char)(data[3])) << 24
                        | (unsigned short)((unsigned char)(data[2])) << 16
                        | (unsigned short)((unsigned char)(data[1])) << 8
                        | (unsigned short)((unsigned char)(data[0]));

    buf->nic(_nic);
    if(!notify(port, buf))
        buf->nic()->free(buf);
}

void DIRP::acknowledged(char * data)
{
    unsigned int port_size = sizeof(Address::Local);
    unsigned int mac_size = sizeof(Ethernet::Address);
    unsigned int ack_size = sizeof(Code);
    char packet[port_size + mac_size + ack_size];

    // get port
    char port[port_size];
    memcpy(port, data, port_size);

    // get mac address
    Ethernet::Address mac_addr;
    memcpy(&mac_addr, &data[port_size], mac_size);

    Code ack = Code::ACK;

    memcpy(packet, &port, port_size);  // add port
    memcpy(&packet[port_size], &mac_addr, mac_size);  // add MAC after port
    memcpy(&packet[port_size + mac_size], &ack, ack_size);  // add ACK after MAC

    Delay (1000000);
    DIRP* dirp = get_by_nic(0);
    dirp->nic()->send(mac_addr, dirp->PROTOCOL, reinterpret_cast<void *>(packet), sizeof(packet));
}


__END_SYS

#endif

