// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;
DIRP* DIRP::_networks[];

int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size) {
    //NIC<Ethernet> * nic = Traits<Ethernet>::DEVICES::Get<0>::Result::get(0);
    //return nic->send(nic->broadcast(), Ethernet::PROTO_DIR, data, size);

    // Get singleton DIRP
    DIRP* dirp = DIRP::get_by_nic(0);
    
    // Get buffer allocated by NIC
    //Buffer* pool = dirp->_nic->alloc(to.mac(), DIRP::PROTOCOL, sizeof(Header), sizeof(Header), size);

    Packet* packet = new Packet(from, to.port(), size);
    memcpy(packet->data<void>(), data, size);

    return dirp->nic()->send(to.mac(), dirp->PROTOCOL, reinterpret_cast<void *>(packet), sizeof(packet));    
}

int DIRP::receive(Buffer * buf, Address * from, void * data, unsigned int size) {
    memcpy(data, buf->frame()->data<char>(), size);
    buf->nic()->free(buf);
    return size;
}

__END_SYS

#endif

