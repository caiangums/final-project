// EPOS NIC Test Programs

#include <time.h>

#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 5;
const int DATA_ITER = 1;

char data[DATA_SIZE];
DIRP::Address self_addr;
Communicator_Common<DIRP, true> * comm;


int sender(int p)
{
    DIRP::Port from_port = (unsigned int) p;
    DIRP::Port dest_port = from_port;  // could be different from 'port'
    comm = new Communicator_Common<DIRP, true>(from_port);
    cout << "\n  Send to port " << dest_port << data << endl;

    DIRP::Address dest_addr(self_addr.mac(), dest_port);

    dest_addr[5]--;
    for (int i = 0; i < DATA_ITER; i++) {
        memset(data, '0' + i + p, DATA_SIZE);
        data[DATA_SIZE - 1] = '\n';
        cout << "  Sending (to " << dest_port << "): " << data;
        comm->send(dest_addr, &data, sizeof(data));
    }

    return 0;
}

int receiver(int p)
{
    DIRP::Port from_port = (unsigned int) p;  // listen this port
    comm = new Communicator_Common<DIRP, true>(from_port);
    cout << "\n  Listening port " << from_port << data << endl;

    for (int i = 0; i < DATA_ITER; i++) {
        comm->receive(&data, DATA_SIZE);
        cout << "  Received data (on " <<  from_port << "): " << data;
    }

    return 0;
}


int main()
{
    cout << "DIRP Test" << endl;
    DIRP::init(0);

    self_addr = DIRP::get_by_nic(0)->address();
    cout << "  MAC: " << self_addr << endl;

    if (self_addr[5] % 2) {  // sender
        Delay (5000000);
        sender(1);
    } else {  // receiver
        receiver(1);  // sends ACK
    }

    delete comm;

    return 0;
}



