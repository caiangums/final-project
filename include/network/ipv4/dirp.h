#ifndef __dirp_h
#define __dirp_h

#include <system/config.h>

#ifdef __ipv4__

#include <machine/nic.h>

__BEGIN_SYS

// DIR_Protocol as a Channel
class DIRP: private NIC<Ethernet>::Observer
{

public:
    typedef Ethernet::Protocol Protocol;
    typedef Ethernet::Buffer Buffer;
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

    typedef unsigned short Port;

public:
    static const unsigned int MTU      = Ethernet::MTU;
    const unsigned short      PROTOCOL = Ethernet::PROTO_DIR;

    class Address  // maybe this could iherit from Ethernet::Address
    {
    public:
        typedef Port Local;

    public:
        Address() {}
        Address(const Ethernet::Address & mac, const Port port): _mac(mac), _port(port) {}

        const Ethernet::Address & mac() const { return _mac; }
        const Port port() const { return _port; }
        const Local local() const { return _port; }

        bool operator==(const Address & a) {
            return (_mac == a._mac) && (_port == a._port);
        }

    private:
        Ethernet::Address _mac;
        Port _port;
    };

    typedef unsigned short Code; // codigo de ACK ou outro
    enum {
        ACK = 1
    };

    class Header
    {
    public:

        Header() {}
        Header(const Port from, const Port to, const Code code):
            _from(from), _to(to), _code(code) {}

        Port from() const { return _from; }
        Port to() const { return _to; }

    protected:
        Port _from;
        Port _to;
        Code _code;
    };

    typedef unsigned char Data[MTU];

    class Packet
    {
    public:
        Packet(){}
        Packet(const Port from, const Port to, const Code code):
            _header(Header(from, to, code)) {}

        Header * header() { return &_header; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

    private:
        Header _header;
        Data _data;
    };

    //template<unsigned int UNIT = 0>  see IP()
    DIRP(unsigned int nic = 0) :
            _nic(Traits<Ethernet>::DEVICES::Get<0>::Result::get(nic))
    {
        _nic->attach(this, PROTOCOL);
    }

    ~DIRP() {
        db<DIRP>(TRC) << "DIRP::~DIRP()" << endl;
        _nic->detach(this, PROTOCOL);
    }

    const Address & address() { return _address;  }
    const unsigned int mtu()  { return this->MTU; }

    static int send(const Address::Local & from, const Address & to, const void * data, unsigned int size);


    /* Create a semaphore, insert it into a list with the specified port
     * and then lock the current thread on that semaphore
     */
    /*
    updated() {
        Semaphore s = new Semaphore(0);
        _receivers.insert(port, s);
        s.p();
        return _list.remove()->object();
    }
    */

    // Change Ethernet::Address to Address (which contains the port)
    static int receive(Buffer * buf, Ethernet::Address * from, void * data, unsigned int size);
        

    void update(Observed* obs, const Protocol& prot, Buffer* buf) {
        // 1. Build a Packet from buf, get the Header, get header->from().
        // 2. Find in my list _observers which one has observing condition
        //    equals to the Port obtained in the first step.
        // 3. Excecute DIRP::notify() with the current port.
        // 4. Notify find in a list _observers which one has observing condition
        //    equals to the Port obtained in the first step.
    }

    /* Since DIRP is a Channel and it is observed, it has to allow observers to attach() and detach() theirselves to it () */
    static void attach(Observer * obs, const Port & port) { _observed.attach(obs, port); }
    static void detach(Observer * obs, const Port & port) { _observed.detach(obs, port); }

    // When DIR_Protocol was a communicator, this was not necessary, because
    // just observeds do notify(), and the communicator was an observer. Now
    // that DIRP is a Channel, it is an observer of NIC<Ethernet> (just like
    // before), but it is also an observed (observed by a communicator), so it
    // does notify(). It will notify their communicators.
    static bool notify(const Port & port, Buffer * buf) {
        return _observed.notify(port, buf);
    }


protected:
    NIC<Ethernet> * _nic;
    Address _address;
    static Observed _observed;
};

__END_SYS

#endif  // __ipv4__

#endif  // __dirp_h

