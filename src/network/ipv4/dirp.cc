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
int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size) {
    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);

    // Our packet has a header with addresses, plus the data to be sent
    Header header(Address(dirp->nic()->address(), from), to, size);
    Packet packet(header, data, size);
    
    db<DIRP>(ERR) << "Sending from: MAC " << packet.header()->from() << endl;
    db<DIRP>(ERR) << "Sending to: MAC " << packet.header()->to() << endl;
    db<DIRP>(ERR) << "Sending data: " << packet.data<char>() << endl;

    dirp->nic()->send(to.mac(), dirp->PROTOCOL, reinterpret_cast<void *>(&packet), sizeof(packet));

    // creates handler to resend message
    /* bool timeout = false;
    int retries = Traits<Network>::RETRIES;
    DIRP::DIRP_Sender dirp_sender(&timeout, from, to, dirp, reinterpret_cast<void *>(packet), sizeof(packet), retries);
    dirp->_handler = new Functor_Handler<DIRP::DIRP_Sender>(&(DIRP::retry_send), &dirp_sender);

    // creates alarm that resend the message in case of no ACK received
    dirp->_alarm = new Alarm((DELAY_SECONDS * (int)Traits<Network>::TIMEOUT/3), dirp->_handler, retries + 1);

    // wait for ACK
    Buffer* buf = _observed.notified(from);

    // cleanup alarm and handler
    delete dirp->_alarm;
    delete dirp->_handler;
 */
    /*
    * Pode chegar nesse ponto em dois casos:
    * Quando o alarme der Timeout (caso em que buffer é nulo)
    * Quando chegar um ACK, e o update() libera normalmente.
    */
/*     if (buf == nullptr || timeout) {
        db<DIRP>(WRN) << "DIRP::update() - timeout or null buffer!" << endl;
        return -1;
    } */
    //delete retries;
    return size;
}

// TODOr use abstractions like Packet and Header
int DIRP::receive(Buffer * buf, void * d, unsigned int s) {
    //DIRP* dirp = get_by_nic(0);

    char* data = buf->frame()->data<char>();
    db<DIRP>(ERR) << "Receiving from: MAC " << data << endl;

    Packet* packet = buf->frame()->data<Packet>();
    db<DIRP>(ERR) << "Receiving from: MAC " << packet->header()->from() << endl;
    db<DIRP>(ERR) << "Receiving into port: " << packet->header()->to().port() << endl;
    db<DIRP>(ERR) << "Receiving data: " << packet->data<char>() << endl;
    /* 
    // get port
    char port[port_size];
    memcpy(port, data, port_size);
    // get mac address
    Ethernet::Address mac_addr;
    memcpy(&mac_addr, &data[port_size], mac_size);  // n bytes is the port length

    char packet[port_size + mac_size + ack_size];
    Code ack = Code::ACK;

    memcpy(packet, &port, port_size);  // add port
    memcpy(&packet[port_size], &mac_addr, mac_size);  // add MAC after port
    memcpy(&packet[port_size + mac_size], &ack, ack_size);  // add ACK after MAC
    dirp->nic()->send(mac_addr, dirp->PROTOCOL, reinterpret_cast<void *>(packet), sizeof(packet));
 */

    memcpy(d, packet->data<void>(), s);
    buf->nic()->free(buf);

    return s;
}

// TODOr use abstractions like Packet and Header
void DIRP::update(Observed* obs, const Protocol& prot, Buffer* buf) {
    // TODOr 5 is: DATA_SIZE from main()
    unsigned int size = 5 + 4 + 4 + 6;
    char data[size];

    memcpy(data, buf->frame()->data<char>(), size);

    // TODOr: remember that if we are getting 1 byte here, the DIRP port is limited to 127.
    // the first byte of data represents an unsigned int, not a char
    unsigned int port = (unsigned int)((unsigned char)(data[0]));

    buf->nic(_nic);
    if(!notify(port, buf))
        buf->nic()->free(buf);
}

__END_SYS

#endif

