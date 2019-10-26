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
    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);
    
    // our packet is the data to be sent plus the destin port (an unsigned int, 4 bytes) in the front
    char packet[size + 4];
    unsigned int port = to.port();
    memcpy(packet, &port, 4);
    memcpy(&packet[4], data, size);

    return dirp->nic()->send(to.mac(), Ethernet::PROTO_DIRP, reinterpret_cast<void *>(packet), sizeof(packet));
}

// TODOr use abstractions like Packet and Header
int DIRP::receive(Buffer * buf, void * d, unsigned int s) {
    // buf aqui vem do Communicator
    char * data = buf->frame()->data<char>();
    memcpy(d, &(data[4]), s);  // 4 bytes is the port length (unsigned int)
    buf->nic()->free(buf);
    return s;
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

