// EPOS Communicator Declarations

#ifndef __communicator_h
#define __communicator_h

#include <network.h>
#include <machine/nic.h>
#include <synchronizer.h>
#include <utility/list.h>

__BEGIN_SYS

// Commonalities for connectionless channels
template<typename Channel, bool connectionless>
class Communicator_Common: protected Channel::Observer, private Concurrent_Observer<typename Channel::Observer::Observed_Data, typename Channel::Observer::Observing_Condition>
{
private:
    static const unsigned int HEADERS_SIZE = Channel::HEADERS_SIZE;

    typedef typename Channel::Observer::Observing_Condition Observing_Condition;
    typedef Concurrent_Observer<typename Channel::Observer::Observed_Data, typename Channel::Observer::Observing_Condition> Observer;

public:
    // List to hold received Buffers
    typedef typename Channel::Buffer Buffer;
    typedef typename Buffer::List List;
    typedef typename List::Element Element;

    // Addresses
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

protected:
    Communicator_Common(const Local_Address & local): _local(local) {
        Channel::attach(this, local);
    }

public:
    ~Communicator_Common() {
        Channel::detach(this, _local);
    }

    template<typename Message>
    int send(const Message & message) {
        return Channel::send(message);
    }
    int send(const Address & to, const void * data, unsigned int size) {
        return Channel::send(_local, to, data, size);
    }
    int send(const Local_Address & from, const Address & to, const void * data, unsigned int size) {
        return Channel::send(from, to, data, size);
    }

    template<typename Message>
    int receive(const Message & message) {
        Buffer * buf = updated();
        return Channel::receive(buf, message);
    }
    int receive(void * data, unsigned int size) {
        Buffer * buf = updated();
        return Channel::receive(buf, data, size);
    }
    int receive(Address * from, void * data, unsigned int size) {
        Buffer * buf = updated();
        return Channel::receive(buf, from, data, size);
    }

    int receive_all(void * data, unsigned int size) {
        int r = 0;
        for(unsigned int received = 0, coppied = 0; received < size; received += coppied) {
            Buffer * buf = updated();
            r += Channel::receive(buf, data + received, coppied = ((received + (buf->size() - HEADERS_SIZE)) > size ? (size - received) : (buf->size() - HEADERS_SIZE)));
        }
        return r;
    }
    int receive_all(Address * from, void * data, unsigned int size) {
        int r = 0;
        for(unsigned int received = 0, coppied = 0; received < size; received += coppied) {
            Buffer * buf = updated();
            r += Channel::receive(buf, data + received, coppied = ((received + (buf->size() - HEADERS_SIZE)) > size ? (size - received) : (buf->size() - HEADERS_SIZE)));
        }
        return r;
    }

    template<typename Message>
    int reply(const Message & message) {
        return Channel::reply(message);
    }

private:
    void update(typename Channel::Observed * obs, const Observing_Condition & c, Buffer * buf) { Observer::update(c, buf); }
    Buffer * updated() { return Observer::updated(); }

private:
    Local_Address _local;
};

// Commonalities for connection-oriented channels
template<typename Channel>
class Communicator_Common<Channel, false>: protected Channel::Observer, private Concurrent_Observer<typename Channel::Observer::Observed_Data, typename Channel::Observer::Observing_Condition>
{
private:
    static const unsigned int HEADERS_SIZE = Channel::HEADERS_SIZE;

    typedef typename Channel::Observer::Observing_Condition Observing_Condition;
    typedef Concurrent_Observer<typename Channel::Observer::Observed_Data, typename Channel::Observer::Observing_Condition> Observer;

public:
    // List to hold received Buffers
    typedef typename Channel::Buffer Buffer;
    typedef typename Buffer::List List;
    typedef typename List::Element Element;

    // Addresses
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

protected:
    Communicator_Common(const Local_Address & local, const Address & peer): _local(local) {
        _connection = Channel::attach(this, local, peer);
    }

public:
    ~Communicator_Common() {
        Channel::detach(this, _connection);
    }

    int send(const void * data, unsigned int size) {
        return _connection->send(data, size);
    }

    int receive_some(void * data, unsigned int size) {
        Buffer * buf = updated();
        return _connection->receive(buf, data, size);
    }

    int receive(void * d, unsigned int size) {
        char * data = reinterpret_cast<char *>(d);
        unsigned int received = 0;
        do {
            Buffer * buf = updated();
            unsigned int segment_size = _connection->receive(buf, data, size);
            data += segment_size;
            received += segment_size;
        } while(received <= size);
        return size;
    }

    int receive_all(void * d, unsigned int size) {
        char * data = reinterpret_cast<char *>(d);
        int r = 0;
        for(unsigned int received = 0, coppied = 0; received < size; received += coppied) {
            Buffer * buf = updated();
            r += _connection->receive(buf, data + received, coppied = ((received + (buf->size() - HEADERS_SIZE)) > size ? (size - received) : (buf->size() - HEADERS_SIZE)));
        }
        return r;
    }

private:
    void update(typename Channel::Observed * obs, const Observing_Condition & c, Buffer * buf) { Observer::update(c, buf); }
    Buffer * updated() { return Observer::updated(); }

protected:
    Local_Address _local;

    typename Channel::Connection * _connection;
};


// Link (point-to-point communicator) connectionless channels
template<typename Channel, bool connectionless>
class Link: public Communicator_Common<Channel, connectionless>
{
private:
    typedef Communicator_Common<Channel, connectionless> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Link(const Local_Address & local, const Address & peer = Address::NULL): Base(local), _peer(peer) {}
    ~Link() {}

    int send(const void * data, unsigned int size) { return Base::send(_peer, data, size); }
    int receive(void * data, unsigned int size) { return Base::receive(data, size); }
    int receive_all(void * data, unsigned int size) { return Base::receive_all(data, size); }

    int read(void * data, unsigned int size) { return receive_all(data, size); }
    int write(const void * data, unsigned int size) { return send(data, size); }

    const Address & peer() const { return _peer;}

private:
    Address _peer;
};

// Link (point-to-point communicator) for connection-oriented channels
template<typename Channel>
class Link<Channel, false>: public Communicator_Common<Channel, false>
{
private:
    typedef Communicator_Common<Channel, false> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Link(const Local_Address & local, const Address & peer = Address::NULL): Base(local, peer), _peer(peer) {}
    ~Link() {}

    int send(const void * data, unsigned int size) { return Base::send(data, size); }
    int receive(void * data, unsigned int size) { return Base::receive(data, size); }
    int receive_all(void * data, unsigned int size) { return Base::receive_all(data, size); }

    int read(void * data, unsigned int size) { return receive_all(data, size); }
    int write(const void * data, unsigned int size) { return send(data, size); }

    const Address & peer() const { return _peer;}

private:
    Address _peer;
};


// Port (1-to-N communicator) for connectionless channels
template<typename Channel, bool connectionless>
class Port: public Communicator_Common<Channel, connectionless>
{
private:
    typedef Communicator_Common<Channel, connectionless> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Port(const Local_Address & local): Base(local) {}
    ~Port() {}

    template<typename Message>
    int send(const Message & message) { return Base::send(message); }
    int send(const Address & to, const void * data, unsigned int size) { return Base::send(to, data, size); }

    template<typename Message>
    int receive(const Message & message) { return Base::receive(message); }
    int receive(Address * from, void * data, unsigned int size) { return Base::receive(from, data, size); }

    template<typename Message>
    int reply(const Message & message) { return Base::reply(message); }
};

// Port (1-to-N communicator) for connection-oriented channels
template<typename Channel>
class Port<Channel, false>: public Communicator_Common<Channel, false>
{
private:
    typedef Communicator_Common<Channel, false> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Port(const Local_Address & local): Base(local) {}
    ~Port() {}

    Link<Channel> * listen() { return new (SYSTEM) Link<Channel>(Channel::listen(this->_local)); }
    Link<Channel> * connect(const Address & to) { return new (SYSTEM) Link<Channel>(Channel::connect(this->_local, to)); }
};

// A simple abstraction for connectionless channels, that simply forwards data
// from the application to the next lower level. DIR_Protocol is responsible for
// decoupling the application from the network communication.
class DIR_Protocol:
    private NIC<Ethernet>::Observer,
    private Concurrent_Observer<Ethernet::Buffer, Ethernet::Protocol>
{

public:
    typedef Ethernet::Protocol Protocol;
    typedef Ethernet::Buffer Buffer;
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

    typedef unsigned short Port;

public:
    static const unsigned int   MTU      = Ethernet::MTU;
    const unsigned short PROTOCOL = Ethernet::PROTO_DIR;

    class Address
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
    DIR_Protocol(unsigned int nic = 0) :
            _nic(Traits<Ethernet>::DEVICES::Get<0>::Result::get(nic))
    {
        _nic->attach(this, PROTOCOL);
    }

    const Ethernet::Address & address() { return _nic->address(); }
    const unsigned int mtu() { return this->MTU; }

    // Add parameter port
    int send(const void * data, unsigned int size) {
        return _nic->send(_nic->broadcast(), PROTOCOL, data, size);
    }


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
    int receive(Ethernet::Address * src, void * data, unsigned int size) {
        Buffer * buff = updated();
        memcpy(data, buff->frame()->data<char>(), size);
        _nic->free(buff);
        return size;
    }

    void update(Observed* obs, const Protocol& prot, Buffer* buf) {
        // Port p = buf->getHeader()->getPort();
        // for (r : _receivers) {
        //   if (r->port() == p) {
        //     _list.insert(buf->lext());
        //     r->semaphore().v();
        //   }
        // }

        // delete this
        Concurrent_Observer<Observer::Observed_Data, Protocol>::update(prot, buf);
    }

protected:
    NIC<Ethernet> * _nic;
    Address _address;
    static Observed _observed;

private:
    class Receiver;
    typedef Simple_List<Receiver> Receivers;
    class Receiver
    {
    public:
        const Port _p;
        const Semaphore * _s;

        Receiver(const Port p, const Semaphore * s): _p(p), _s(s), _el(this) {}

        Receivers::Element * link() { return &_el; }

    private:
        Receivers::Element _el;
    };

    Simple_List<Receiver> _receivers;
};

__END_SYS

#endif
