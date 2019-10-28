#ifndef __dirp_h
#define __dirp_h

#include <system/config.h>

#ifdef __ipv4__

#include <machine/nic.h>
#include <synchronizer.h>

__BEGIN_SYS

// DIR_Protocol as a Channel
class DIRP: private NIC<Ethernet>::Observer
{

public:
    typedef Ethernet::Protocol Protocol;

    // Buffers received by the NIC, eventually linked into a list
    typedef Ethernet::Buffer Buffer;

    // DIRP and NIC observer/d
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

    typedef unsigned int Port;

    static const unsigned int MTU      = Ethernet::MTU;
    const unsigned short      PROTOCOL = Ethernet::PROTO_DIRP;
    typedef unsigned char Data[MTU-64];

    class Address
    {
    public:
        typedef Port Local;

    public:
        Address() {}
        Address(const Ethernet::Address & mac, const Port port): _mac(mac), _port(port) {}

        const Ethernet::Address & mac() const { return _mac; }
        const Port port() const { return _port; }
        void port(const Port port) { _port = port; }
        const Local local() const { return _port; }

        operator bool() const {
            return (_mac || _port);
        }

        bool operator==(const Address & a) {
            return (_mac == a._mac) && (_port == a._port);
        }

        unsigned char & operator[](const size_t i) { return _mac[i]; }
        const unsigned char & operator[](const size_t i) const { return _mac[i]; }

        friend OStream & operator<<(OStream & db, const Address & a) {
            db << a._mac << ":" << hex << a._port;
            return db;
        }

        Ethernet::Address mac() { return _mac; }
        void mac(Ethernet::Address mac) { _mac = mac; }

    private:
        Ethernet::Address _mac;
        Port _port;
    }__attribute__((packed));

    enum Code {
        NOTHING = 0,
        ACK = 1
    };

    class Header
    {
    public:

        Header() {}
        Header(const Address &from, const Address &to, unsigned int size, const Code code = Code::NOTHING):
            _from(from), _to(to), _length(size + sizeof(Packet)), _code(code) {}

        Address from() const { return _from; }
        Address to() const { return _to; }
        unsigned short length() const { return _length; }

    protected:
        Address _from;
        Address _to;
        unsigned short _length;   // Length of datagram (header + data) in bytes
        Code _code;
    }__attribute__((packed));

    class Packet
    {
    public:
        Packet() {}
        Packet(Header &header, Data data):
            _header(header) {
                memcpy(_data, &data, sizeof(Data));
            }
        Packet(Header &header, const void* data, unsigned int size):
            _header(header) {
                memcpy(_data, data, size);
            }

        Header * header() { return &_header; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

    private:
        Header _header;
        Data _data = {0};
    }__attribute__((packed));

    static void init(unsigned int unit) {
        _networks[unit] = new (SYSTEM) DIRP(unit);
    }

    class DIRP_Sender {
    public:
        DIRP_Sender(bool * timeout, const Address::Local & from, const Address & to, DIRP * dirp, void * data, int size, int retries):
            _timeout(timeout), _from(from), _to(to), _dirp(dirp), _data(data), _size(size), _retries(retries) {}
        ~DIRP_Sender() {}

        void resend() {
            // in order to see this function called, uncomment this line
            db<DIRP_Sender>(WRN) << "DIRP_Sender::resend()" << endl;
            if (this->_retries == 0) {
                *(this->_timeout) = true;
                this->_dirp->notify(this->_from, nullptr);
                return;
            }
            this->_retries -= 1;
            this->_dirp->nic()->send(this->_to.mac(), this->_dirp->PROTOCOL, this->_data, this->_size);
        }

    private:
        bool * _timeout;
        const Address::Local _from;
        const Address _to;
        DIRP * _dirp;
        void * _data;
        int _size;
        int _retries;
    };

    //template<unsigned int UNIT = 0>  see IP()
    DIRP(unsigned int unit = 0) :
            _nic(Traits<Ethernet>::DEVICES::Get<0>::Result::get(unit))
    {
        db<Thread>(WRN) << "new DIRP()" << endl;
        _nic->attach(this, PROTOCOL);

        _address.mac(_nic->address());
        _networks[unit] = this;
    }

    static DIRP * get_by_nic(unsigned int unit) {
        if(unit >= Traits<Ethernet>::UNITS) {
            db<IP>(WRN) << "IP::get_by_nic: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _networks[unit];
    }

    ~DIRP() {
        db<DIRP>(TRC) << "DIRP::~DIRP()" << endl;
        _nic->detach(this, PROTOCOL);
    }

    const Address & address() { return _address;  }
    const unsigned int mtu()  { return this->MTU; }

    static int send(const Address::Local & from, const Address & to, const void * data, unsigned int size);

    //static int receive(Buffer * buf, Address* from, void * data, unsigned int size);
    static int receive(Buffer * buf, void * data, unsigned int size);
    void update(Observed* obs, const Protocol& prot, Buffer* buf);

    /* Since DIRP is a Channel and it is observed, it has to allow observers to attach() and detach() theirselves to it () */
    static void attach(Observer * obs, const Port & port) { _observed.attach(obs, port); }
    static void detach(Observer * obs, const Port & port) { _observed.detach(obs, port); }

    static bool notify(const Port & port, Buffer * buf) {
        return _observed.notify(port, buf);
    }

    static bool notified(const Port & port) {
        return _observed.notified(port);
    }

    NIC<Ethernet>* nic() const { return _nic; }

protected:
    static void retry_send(DIRP_Sender * dirp_sender) {
        dirp_sender->resend();
    }

    NIC<Ethernet> * _nic;
    Address _address;
    Functor_Handler<DIRP_Sender> * _handler;
    Alarm * _alarm;

    static Observed _observed;
    static DIRP * _networks[Traits<Ethernet>::UNITS];
};

__END_SYS

#endif  // __ipv4__

#endif  // __dirp_h

