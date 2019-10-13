// EPOS NIC Test Programs

#include <time.h>

#include <machine/nic.h>
#include <synchronizer.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;

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
    typedef Ethernet::Address Address;
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

public:
    const unsigned int MTU        = Ethernet::MTU;
    const unsigned short PROTOCOL = Ethernet::PROTO_DIR;

    //template<unsigned int UNIT = 0>  see IP()
    DIR_Protocol(unsigned int nic = 0) :
            _nic(Traits<Ethernet>::DEVICES::Get<0>::Result::get(nic))
    {
        _nic->attach(this, PROTOCOL);
    }

    const Address & address() { return _nic->address(); }
    const unsigned int mtu() { return this->MTU; }

    int send(const void * data, unsigned int size) {
        return _nic->send(_nic->broadcast(), PROTOCOL, data, size);
    }

    int receive(Address * src, void * data, unsigned int size) {
        Buffer * buff = updated();
        memcpy(data, buff->frame()->data<char>(), size);
        _nic->free(buff);
        return size;
    }

    void update(Observed* obs, const Protocol& prot, Buffer* buf) {
        Concurrent_Observer<Observer::Observed_Data, Protocol>::update(prot, buf);
    }

    static bool notify(const Protocol & prot, Buffer * buf) {
        return _observed.notify(prot, buf);
    }

protected:
    NIC<Ethernet> * _nic;
    Address _address;
    static Observed _observed;
    
};

int main()
{
    cout << "NIC Test" << endl;

    DIR_Protocol * comm = new DIR_Protocol();

    DIR_Protocol::Address self_addr = comm->address();
    cout << "  MAC: " << self_addr << endl;

    char data[DATA_SIZE];

    if(self_addr[5] % 2) {  // sender
        Delay (5000000);

        for(int i = 0; i < 10; i++) {
            memset(data, '0' + i, DATA_SIZE);
            data[DATA_SIZE - 1] = '\n';
            cout << " Sending: " << data;
            comm->send(data, DATA_SIZE);
        }
    } else {  // receiver
        ++self_addr[5];  // my address ends with 08, sender's address with 09.
        DIR_Protocol::Address from = self_addr;
        cout << "  Receiving from: " << from << endl;

        for(int i = 0; i < 10; i++) {
           comm->receive(&from, data, DATA_SIZE);
           cout << "  Received data: " << data;
        }
    }

    /*
    DIR_Protocol::Statistics stat = nic->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << "\n";
         */
}
