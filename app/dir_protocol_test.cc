// EPOS NIC Test Programs

#include <time.h>

#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;

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
