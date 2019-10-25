// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;
DIRP* DIRP::_networks[];

// TODOr use abstractions like Packet and Header
int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size) {
    //NIC<Ethernet> * nic = Traits<Ethernet>::DEVICES::Get<0>::Result::get(0);
    //return nic->send(nic->broadcast(), Ethernet::PROTO_DIR, data, size);

    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);
    
    // Get buffer allocated by NIC
    //Buffer* pool = dirp->_nic->alloc(to.mac(), DIRP::PROTOCOL, sizeof(Header), sizeof(Header), size);

    //Packet* packet = new Packet(from, to.port(), size);
    //memcpy(packet->data<void>(), data, size);

    // our packet is the data to be sent plus the destin port (an unsigned int, 4 bytes) in the front
    char packet[size + 4];
    unsigned int port = to.port();
    memcpy(packet, &port, 4);
    memcpy(&packet[4], data, size);

    // TODOr get the second param (protocol) properly
    return dirp->nic()->send(to.mac(), 0x0666, reinterpret_cast<void *>(packet), sizeof(packet));
}

// TODOr use abstractions like Packet and Header
int DIRP::receive(Buffer * buf, void * d, unsigned int s) {
    // buf aqui vem do Communicator
    char * data = buf->frame()->data<char>();
    memcpy(d, &(data[4]), s);  // 4 bytes is the port length (unsigned int)
    buf->nic()->free(buf);
    return s;

    /*
    unsigned char * data = reinterpret_cast<unsigned char *>(d);
    Packet* packet = buf->frame()->data<Packet>();

    packet = buf->frame()->data<Packet>();
    unsigned int len = buf->size() - sizeof(Header);
    memcpy(data, packet->data<void>(), len);

    buf->nic()->free(buf);

    return buf->size();
    */
}

// TODOr use abstractions like Packet and Header
void DIRP::update(Observed* obs, const Protocol& prot, Buffer* buf) {
    char data[9];  // TODOr 9 is hardcoded: DATA_SIZE from main() + 4 (unsigned int)
    memcpy(data, buf->frame()->data<char>(), 9);

    // the first byte of data represents an unsigned int, not a char
    unsigned int port = (unsigned int)((unsigned char)(data[0]));

    buf->nic(_nic);
    if(!notify(port, buf))
        buf->nic()->free(buf);
}

__END_SYS

#endif

