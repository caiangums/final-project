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
class DIR_Protocol: private NIC<Ethernet>::Observer, private Concurrent_Observer<Ethernet::Buffer, unsigned short>
{

public:
    //static const unsigned int PROTOCOL = Ethernet::PROTO_IP;  // :R: usar um número qualquer par o DIR?

    typedef Ethernet::Protocol Protocol;
    typedef Ethernet::Buffer Buffer;
    typedef Ethernet::Address Address;
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;


public:
    const unsigned short PROTOCOL = 666; // :R: Podemos posteriormente por o número do nosso protocolo em Ethernet::DIR

    //template<unsigned int UNIT = 0>  see IP()
    DIR_Protocol(unsigned int nic = 0) : _nic(Traits<Ethernet>::DEVICES::Get<0>::Result::get(nic)) {
        _nic->attach(this, PROTOCOL);
    }

    NIC<Ethernet> * nic() { return _nic; }

    //int send(const Address & to, const Protocol & prot, const void * data, unsigned int size) { return _nic->send(to, data, size); }
    int send(const void * data) {
        return _nic->send(_nic->broadcast(), 0x888, data, _nic->mtu());
    }

    //int receive(Address * src, Protocol * prot, void * data, unsigned int size);

    void update(Observed* obs, const Protocol& prot, Buffer* buf) {
            //Concurrent_Observer<Observer::Observed_Data, Protocol>::update(prot, buf);
    }

    static bool notify(const Protocol & prot, Buffer * buf) {
        return false;
        //return _observed.notify(prot, buf);
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

    char data[1500];

    //if(self[5] % 2) { // sender
        Delay (5000000);

        for(int i = 0; i < 10; i++) {
            memset(data, '0' + i, 1500);
            data[1500 - 1] = '\n';
            cout << " Sending: " << data;
            comm->send(data);
        }
    //}
    /*
    else { // receiver
        for(int i = 0; i < 10; i++) {
           nic->receive(&src, &prot, data, nic->mtu());
           cout << "  Data: " << data;
        }
    }
    */

    /*
    DIR_Protocol::Statistics stat = nic->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << "\n";
         */
}
