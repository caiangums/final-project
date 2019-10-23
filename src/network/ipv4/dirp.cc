// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;

int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size) {
    NIC<Ethernet> * nic = Traits<Ethernet>::DEVICES::Get<0>::Result::get(0);
    return nic->send(nic->broadcast(), Ethernet::PROTO_DIR, data, size);
}

int DIRP::receive(Buffer * buf, Ethernet::Address * from, void * data, unsigned int size) {
    memcpy(data, buf->frame()->data<char>(), size);
    buf->nic()->free(buf);
    return size;
}

__END_SYS

#endif

