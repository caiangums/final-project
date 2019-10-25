// EPOS NIC Test Programs

#include <time.h>

#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;
const int DATA_ITER = 10;

int main()
{
    cout << "DIRP Test" << endl;
    DIRP::init(0);

    DIRP::Address self_addr = DIRP::get_by_nic(0)->address();
    cout << "  MAC: " << self_addr << endl;

    Communicator_Common<DIRP, true> * comm;
    char data[DATA_SIZE];

    if (self_addr[5] % 2) {  // sender
        Delay (5000000);

        DIRP::Port from_port = 112;
        DIRP::Port dest_port = from_port;  // could be different from 'port'
        comm = new Communicator_Common<DIRP, true>(from_port);

        DIRP::Address dest_addr(self_addr.mac(), dest_port);

        dest_addr[5]--;
        for (int i = 0; i < DATA_ITER; i++) {
            memset(data, '0' + i, DATA_SIZE);
            data[DATA_SIZE - 1] = '\n';
            cout << "  Sending: " << data;
            comm->send(dest_addr, &data, sizeof(data));
        }
    } else {  // receiver
        DIRP::Port from_port = 112;
        comm = new Communicator_Common<DIRP, true>(from_port);

        for (int i = 0; i < DATA_ITER; i++) {
            comm->receive(&data, DATA_SIZE);
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

/*
int dir_receiver() {
    Communicator_Common<DIRP, true> * comm;

    Ethernet::Address self_addr = nic->address();
    cout << "  MAC: " << self_addr << endl;

    char data[DATA_SIZE];

    DIRP::Port port = 111;
    ++self_addr[5];  // my address ends with 08, sender's address with 09.
    Ethernet::Address from_mac = self_addr;
    cout << "  Receiving from: " << from_mac << endl;

    comm = new Communicator_Common<DIRP, true>(port);
    DIRP::Address from(from_mac, port);
    for(int i = 0; i < 10; i++) {
        //comm->receive(&from, &data, DATA_SIZE);
        cout << "  Received data: " << data;
    }

    delete comm;

    return 0;
}
*/
