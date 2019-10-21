// EPOS NIC Test Program
//
// Interoperability between protocols.
//
// Demonstration of interoperability between DIR and UDP. UDP and DIR data are
// sent and just DIR data is expected by the receiver application. The expected
// behavior is that only DIR data is printed by the receiver application.

#include <time.h>
#include <utility/random.h>

#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;
const int ITERATIONS = 3;
const int PDU = 200;


int udp_sender();
int dir_receiver();


int main()
{
    cout << ">> DIR interoperability test <<" << endl;

    DIR_Protocol * comm = new DIR_Protocol();

    DIR_Protocol::Address self_addr = comm->address();
    cout << "  MAC: " << self_addr << endl;

    char data[DATA_SIZE];

    // Sends UDP and DIR data
    if (self_addr[5] % 2)
    {
        Delay (5000000);

        udp_sender();

        // dir_sender()
        for(int i = 0; i < 10; i++) {
            memset(data, '0' + i, DATA_SIZE);
            data[DATA_SIZE - 1] = '\n';
            cout << " Sending: " << data;
            comm->send(data, DATA_SIZE);
        }
    }
    // Receive only DIR data
    else
    {
        dir_receiver();
    }

    /*
    DIR_Protocol::Statistics stat = nic->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << "\n";
         */
    delete comm;

    return 0;
}

int udp_sender()
{
    cout << "  UDP sender" << endl;

    char data[PDU];
    Link<UDP> * com;

    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;

    if(ip->address()[3] % 2) { // sender
        IP::Address peer_ip = ip->address();
        peer_ip[3]--;

        com = new Link<UDP>(8000, Link<UDP>::Address(peer_ip, UDP::Port(8000)));

        for(int i = 0; i < ITERATIONS; i++) {
            data[0] = '\n';
            data[1] = ' ';
            data[2] = '0' + i;
            data[3] = '0' + i;
            data[4] = '0' + i;
            data[5] = '0' + i;
            data[6] = '0' + i;
            data[7] = '0' + i;

            for(int j = 8; j < sizeof(data) - 8; j += 8) {
                data[j+0] = ' ';
                data[j+1] = '0' + i + (j / 1000000 % 10);
                data[j+2] = '0' + (j / 100000 % 10);
                data[j+3] = '0' + (j / 10000 % 10);
                data[j+4] = '0' + (j / 1000 % 10);
                data[j+5] = '0' + (j / 100 % 10);
                data[j+6] = '0' + (j / 10 % 10);
                data[j+7] = '0' + (j % 10);
            }

            data[sizeof(data) - 8] = ' ';
            data[sizeof(data) - 7] = '0' + i;
            data[sizeof(data) - 6] = '0' + i;
            data[sizeof(data) - 5] = '0' + i;
            data[sizeof(data) - 4] = '0' + i;
            data[sizeof(data) - 3] = '0' + i;
            data[sizeof(data) - 2] = '\n';
            data[sizeof(data) - 1] = 0;

            int sent = com->send(&data, sizeof(data));
            if(sent == sizeof(data))
                cout << "  Data: " << data << endl;
            else
                cout << "  Data was not correctly sent. It was " << sizeof(data) << " bytes long, but only " << sent << " bytes were sent!"<< endl;
        }
    } 

    delete com, ip;

    return 0;
}

int dir_receiver()
{
    DIR_Protocol * comm = new DIR_Protocol();

    DIR_Protocol::Address self_addr = comm->address();
    cout << "  MAC: " << self_addr << endl;

    char data[DATA_SIZE];

    ++self_addr[5];  // my address ends with 08, sender's address with 09.
    DIR_Protocol::Address from = self_addr;
    cout << "  Receiving from: " << from << endl;

    for(int i = 0; i < 10; i++) {
        comm->receive(&from, data, DATA_SIZE);
        cout << "  Received data: " << data;
    }
    
    delete comm;

    return 0;
}

