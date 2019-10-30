// EPOS NIC Test Programs
//
// Test showing interoperability between DIRP and ICMP, and also showing DIRP
// working properly with two Threads listening on different ports. Packets are
// send by a sender Thread only to one port, so one of the two receiver threads
// doesn't receive anything, while the other does and while ICMP communication
// is occuring.

#include <time.h>

#include <machine/nic.h>
#include <synchronizer.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;
Mutex mutex;

const int DATA_SIZE = 5;
const int DATA_ITER = 10;

char data[DATA_SIZE];
DIRP::Address self_addr;
Communicator_Common<DIRP, true> * comm;

int dirp_sender(int p)
{
    DIRP::Port from_port = (unsigned int) p;
    DIRP::Port dest_port = from_port;
    comm = new Communicator_Common<DIRP, true>(from_port);
    cout << "\n  Send to port " << dest_port << data << endl;

    DIRP::Address dest_addr(self_addr.mac(), dest_port);

    dest_addr[5]--;
    for (int i = 0; i < DATA_ITER; i++) {
        memset(data, '0' + i + p, DATA_SIZE);
        data[DATA_SIZE - 1] = '\n';
        mutex.lock();
        cout << "  Sending (to " << dest_port << "): " << data;
        mutex.unlock();
        comm->send(dest_addr, &data, sizeof(data));
    }

    return 0;
}

int dirp_receiver(int p)
{
    DIRP::Port from_port = (unsigned int) p;  // listen this port
    comm = new Communicator_Common<DIRP, true>(from_port);
    cout << "\n  Listening port " << from_port << data << endl;

    for (int i = 0; i < DATA_ITER; i++) {
        comm->receive(&data, DATA_SIZE);
        mutex.lock();
        cout << "  Received data (on " <<  from_port << "): " << data;
        mutex.unlock();
    }

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

        for(int i = 0; i < DATA_ITER; i++) {
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

    for(int i = 0; i < DATA_ITER; i++) {
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


int main()
{
    cout << "DIRP Test" << endl;
    DIRP::init(0);

    self_addr = DIRP::get_by_nic(0)->address();
    cout << "  MAC: " << self_addr << endl;

    if (self_addr[5] % 2) {  // sender
        Delay (5000000);
        new Thread(&dirp_sender, 1);
        new Thread(&icmp_sender);
    } else {  // receiver
        // There's only one sender and it sent data to port 1, the receiver on
        // 10 doesn't receive anything. ICMP works normally.
        new Thread(&dirp_receiver, 1);
        new Thread(&icmp_receiver);
        new Thread(&dirp_receiver, 10);
    }

    delete comm;

    return 0;
}



