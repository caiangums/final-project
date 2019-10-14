// EPOS NIC Test Program
//
// Interoperability between protocols.
//
// Demonstration of interoperability between DIR and ICMP. At first, ICMP data
// is sent just to confirm that the other side can see that data. Then ICMP data
// is sent again, but now only DIR data is expected to be received.
//
// The expected behavior here is that the receiver application won't print
// anything, since it expects DIR data and just ICMP data arrived to the SO.
// In other words, ICMP data is filtered and it doesn't reaches the application
// layer (although it arrives to the other side, as confirmed by the first
// sending).
//
// The receiver will only show DIR data, as noticed after the delay.

#include <time.h>
#include <utility/random.h>

#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;
const int ITERATIONS = 3;


int icmp_sender();
int icmp_receiver();
int dir_sender();
int dir_receiver();


int main()
{
    cout << ">> DIR interoperability test <<" << endl;

    DIR_Protocol * comm = new DIR_Protocol();

    DIR_Protocol::Address self_addr = comm->address();
    cout << "  MAC: " << self_addr << endl;

    // Note that ICMP data is sent twice, but only the first one is saw by the
    // receiver. This is just to confirm that that data reaches the receiver
    // application. The second will be filtered and just DIR data will reach
    // the application layer again.
    if (self_addr[5] % 2) {
        Delay (5000000);
        icmp_sender();  // The receiver is waiting for ICMP just to show that
                        // the ICMP communication is possible.
                     
        icmp_sender();  // The other side is not receiving ICMP anymore, nothing
                        // will be shown.


        Delay (5000000);  // This delay is to perceive that we just sent ICMP
                          // data, but nothing will be printed by the receiver,
                          // because now it receives DIR data only.
        dir_sender();  // The data that the receiver was waiting for.
    } else {
        icmp_receiver();  // Confirm that it is possible to receive ICMP data.

        dir_receiver();  // The sender has a delay, but the data will come.
    }

    delete comm;

    return 0;
}

int icmp_sender()
{
    cout << "  ICMP sender" << endl;

    Port<ICMP> * com;
    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;

    if (ip->address()[3] % 2) { // sender
        cout << "Sender:" << endl;

        IP::Address peer_ip = ip->address();
        peer_ip[3]--;
        ICMP::Packet packet;
        com = new Port<ICMP>(0);
        unsigned int id = Random::random();

        for(int i = 0; i < ITERATIONS; i++) {
            new (&packet) ICMP::Packet(ICMP::ECHO, 0, id, i);

            int sent = com->send(peer_ip, &packet, sizeof(ICMP::Packet));
            if(sent == sizeof(ICMP::Packet))
                cout << "  Data: " << &packet << endl;
            else
                cout << "  Data was not correctly sent. It was " << sizeof(ICMP::Packet) << " bytes long, but only " << sent << " bytes were sent!"<< endl;
            Delay(100000);
        }
    }

    delete com;

    Ethernet::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << endl;

    return stat.tx_bytes + stat.rx_bytes;
}

int icmp_receiver()
{
    cout << "  ICMP receiver" << endl;

    Port<ICMP> * com;
    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;
    cout << "Receiver:" << endl;

    IP::Address peer_ip = ip->address();
    peer_ip[3]++;
    ICMP::Packet packet;
    com = new Port<ICMP>(0);

    for(int i = 0; i < ITERATIONS; i++) {
        ICMP::Address from;
        int received = com->receive(&from, &packet, sizeof(ICMP::Packet));
        if(received == sizeof(ICMP::Packet))
            cout << "  Data: " << &packet << endl;
        else
            cout << "  Data was not correctly received. It was " << sizeof(ICMP::Packet) << " bytes long, but " << received << " bytes were received!"<< endl;

        if(packet.type() == ICMP::ECHO) {
            db<ICMP>(WRN) << "ICMP::update: echo request from " << from << endl;

            ICMP::Packet * reply = new (&packet) ICMP::Packet(ICMP::ECHO_REPLY, 0);
            com->send(from, reply, sizeof(packet));
        } else if(packet.type() == ICMP::ECHO_REPLY)
            db<ICMP>(WRN) << "ICMP::update: echo reply to " << from << endl;
    }

    delete com;

    Ethernet::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << endl;

    return stat.tx_bytes + stat.rx_bytes;
    
}

int dir_sender()
{
    DIR_Protocol * comm = new DIR_Protocol();

    DIR_Protocol::Address self_addr = comm->address();

    char data[DATA_SIZE];

    for(int i = 0; i < 10; i++) {
        memset(data, '0' + i, DATA_SIZE);
        data[DATA_SIZE - 1] = '\n';
        cout << " Sending: " << data;
        comm->send(data, DATA_SIZE);
    }
    
    delete comm;

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

