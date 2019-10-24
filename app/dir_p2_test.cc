// EPOS NIC Test Programs

#include <time.h>

#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;

// This is used simply for not to hard coding the MAC address
NIC<Ethernet> * nic = Traits<Ethernet>::DEVICES::Get<0>::Result::get(0);

int dir_receiver();

int main()
{
    cout << "DIRP Test" << endl;
    DIRP::init(0);
    DIRP* dirp = DIRP::get_by_nic(0);
    DIRP::Port port = 1111;
    
    Communicator_Common<DIRP, true> * comm = new Communicator_Common<DIRP, true>(port);

    DIRP::Address self_addr = dirp->address(); // Application should not access nic, DIRP is responsible for it
    cout << "  MAC: " << self_addr << endl;

    char data[DATA_SIZE];

    if(self_addr[5] % 2) {  // sender
        Delay (5000000);

        DIRP::Address dest(self_addr);
        dest[5]--;
        for(int i = 0; i < 10; i++) {
            memset(data, '0' + i, DATA_SIZE);
            data[DATA_SIZE - 1] = '\n';
            cout << " Sending: " << data;
            comm->send(dest, &data, sizeof(data));
        }
    } else {  // receiver
        //new Thread(&dir_receiver);
        /*++self_addr[5];  // my address ends with 08, sender's address with 09.
        DIRP::Address from_mac = self_addr;
        cout << "  Receiving from: " << from_mac << endl;

        comm = new Communicator_Common<DIRP, true>(port);
        DIRP::Address from(from_mac, port);
        for(int i = 0; i < 10; i++) {
            comm->receive(&from, &data, DATA_SIZE);
            cout << "  Received data: " << data;
        }*/
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

int dir_receiver() {
    Communicator_Common<DIRP, true> * comm;

    Ethernet::Address self_addr = nic->address();
    cout << "  MAC: " << self_addr << endl;

    char data[DATA_SIZE];

    DIRP::Port port = 1111;
    ++self_addr[5];  // my address ends with 08, sender's address with 09.
    Ethernet::Address from_mac = self_addr;
    cout << "  Receiving from: " << from_mac << endl;

    comm = new Communicator_Common<DIRP, true>(port);
    DIRP::Address from(from_mac, port);
    for(int i = 0; i < 10; i++) {
        comm->receive(&from, &data, DATA_SIZE);
        cout << "  Received data: " << data;
    }

    delete comm;

    return 0;
}
