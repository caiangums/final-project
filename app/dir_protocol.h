#include <machine/nic.h>

// A simple abstraction for connectionless channels, that simply forwards data
// from the application to the next lower level. DIR_Protocol is responsible for
// decoupling the application from the network communication.
public class DIR_Protocol: public NIC<Ethernet>//Concurrent_Observer<NIC<Ethernet>::Observer::Observed_Data, Ethernet::Observer::Observing_Condition>
{
    //friend class System;
    //friend class Network_Common;

public:
    //static const unsigned int TIMEOUT = Traits<IP>::TIMEOUT * 1000000;  // :R: criaremos um Traits para nosso protocolo?

    // DIR Protocol Id
    //static const unsigned int PROTOCOL = Ethernet::PROTO_IP;  // :R: usar um número qualquer par o DIR?

    // Addresses
    typedef Ethernet::Address MAC_Address;
    typedef NIC_Common::Address<4> Address;  // :R: Address<4>, esse número tem que ser diferente?

    typedef Ethernet::Protocol Protocol;

    // RFC 1700 Protocols

    // Buffers received by the NIC, eventually linked into a list
    typedef Ethernet::Buffer Buffer;

    // DIR and NIC observer/d
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

    // DIR Header
    /* classe Header do IP, onde provávelmente teremos a nossa no futuro
     * Por enquanto será usado o do Ethernet. */


    // DIR Pseudo Header for checksum calculations
    /* IP Pseudo_Header */

    // DIR Packet
    /* IP Packet + MFS, MTU e Data[MTU] */
    static const unsigned int MTU = 1500;  // MTU of Ethernet

    //typedef Packet PDU;

private:
    // Fragment key = f(from, id) = (from & ~_netmask) << 16 | id (fragmentation can only happen on localnet)
    typedef unsigned long Key;

    // List to hold received Buffers containing datagram fragments
    class Fragmented;

    /* IP class Fragmented */

    //class Router;

    /* IP class Route */
    /* IP class Router */


protected:
    template<unsigned int UNIT = 0>  // :R: trocar esse número?
    DIR_Protocol(unsigned int nic = UNIT);

public:
    ~DIR_Protocol();

    /*
    void reconfigure(const Address & a, const Address & m, const Address & g) {
        _address = a;
        _netmask = m;
        _broadcast = (a & m) | ~m;
        _gateway = g;
    }
    */

    NIC<Ethernet> * nic() { return _nic; }
    //Router * router() { return &_router; }

    const Address & address() const { return _address; }
    const Address & broadcast() const { return _broadcast; }
    //const Address & gateway() const { return _gateway; }
    //const Address & netmask() const { return _netmask; }

    static DIR_Protocol * get_by_nic(unsigned int unit) {
        if(unit >= Traits<Ethernet>::UNITS) {
            db<DIR_Protocol>(WRN) << "DIR_Protocol::get_by_nic: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _networks[unit];
    }

    //static Route * route(const Address & to) { return _router.search(to); }

    static const unsigned int mtu() { return MTU; }

    //static unsigned short checksum(const void * data, unsigned int size);

    static void attach(Observer * obs, const Protocol & prot) { _observed.attach(obs, prot); }
    static void detach(Observer * obs, const Protocol & prot) { _observed.detach(obs, prot); }

    friend Debug & operator<<(Debug & db, const DIR_Protocol & dir) {
        db << "{a=" << dir._address
           << ",m=" << dir._netmask
           << ",b=" << dir._broadcast
           //<< ",g=" << ip._gateway
           << ",nic=" << &dir._nic
           << "}";
        return db;
    }

private:
    void config_by_mac() { _address[sizeof(Address) -1] = _nic->address()[sizeof(MAC_Address) - 1]; }

    void update(Ethernet::Observed * obs, const Ethernet::Protocol & prot, Buffer * buf);

    static bool notify(const Protocol & prot, Buffer * buf) { return _observed.notify(prot, buf); }

    static void init(unsigned int unit);

protected:
    NIC<Ethernet> * _nic;

    Address _address;
    Address _netmask;
    Address _broadcast;
    //Address _gateway;

    static DIR_Protocol * _networks[Traits<Ethernet>::UNITS];
    //static Router _router;
    static Observed _observed; // shared by all DIR_Protocol instances
    
};

