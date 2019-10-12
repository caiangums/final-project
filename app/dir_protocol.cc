// EPOS NIC Test Programs

#include <communicator.h>
#include <machine/nic.h>
#include <time.h>
#include "dir_protocol.h"

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;

int main()
{
    cout << "NIC Test" << endl;

    DIR_Protocol * nic = Traits<Ethernet>::DEVICES::Get<0>::Result::get(0);

    DIR_Protocol::Address src, dst;
    DIR_Protocol::Protocol prot;
    char data[nic->mtu()];

    DIR_Protocol::Address self = nic->address();
    cout << "  MAC: " << self << endl;

    if(self[5] % 2) { // sender
        Delay (5000000);

        for(int i = 0; i < 10; i++) {
            memset(data, '0' + i, nic->mtu());
            data[nic->mtu() - 1] = '\n';
            cout << " Sending: " << data;
            nic->send(nic->broadcast(), 0x8888, data, nic->mtu());
        }
    } else { // receiver
        for(int i = 0; i < 10; i++) {
           nic->receive(&src, &prot, data, nic->mtu());
           cout << "  Data: " << data;
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
