// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;
DIRP* DIRP::_networks[];

// TODOr after make it work, try to use sizeof(Address) instead of Address::Local + Ethernet::Address


// TODOr use abstractions like Packet and Header
int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size) {
    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);

    unsigned int port = to.port();
    Ethernet::Address mac_addr = dirp->address().mac();
    db<DIRP>(WRN) << "SENDING: MAC "<< mac_addr << endl;

    unsigned int port_size = 4; //sizeof(to.port());
    unsigned int mac_size = 6; //sizeof(Ethernet::Address);

    // our packet is the data to be sent plus the destin port (an unsigned int, 4 bytes) in the front
    char packet[size + port_size + mac_size];

    memcpy(packet, &port, port_size);  // add port
    memcpy(&packet[port_size], &mac_addr, mac_size);  // add MAC after port
    memcpy(&packet[port_size + mac_size], data, size);  // add data after MAC

    return dirp->nic()->send(to.mac(), Ethernet::PROTO_DIRP, reinterpret_cast<void *>(packet), sizeof(packet));
}

// TODOr use abstractions like Packet and Header
int DIRP::receive(Buffer * buf, void * d, unsigned int s) {
    // buf aqui vem do Communicator
    char * data = buf->frame()->data<char>();

    unsigned int port_size = 4;//sizeof(Address::Local);
    unsigned int mac_size = 6;//sizeof(Ethernet::Address);
    unsigned int header_size = port_size + mac_size;

    // get mac address
    char mac_addr[mac_size];
    memcpy(mac_addr, &data[port_size], mac_size);  // n bytes is the port length
    Ethernet::Address mac(mac_addr);

    // send ACK
    DIRP::Address to(mac, 112);
    //DIRP::send(112, to, , );

    memcpy(d, &(data[header_size]), s);  // n bytes is the port length
    buf->nic()->free(buf);

    return s;
}

// TODOr use abstractions like Packet and Header
void DIRP::update(Observed* obs, const Protocol& prot, Buffer* buf) {
    // TODOr 5 is: DATA_SIZE from main()
    unsigned int size = 5 + 4 + 6;//sizeof(Ethernet::Address);
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

