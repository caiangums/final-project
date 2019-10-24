// EPOS NIC Test Programs

#include <machine/nic.h>
#include <process.h>
#include <time.h>

using namespace EPOS;

OStream cout;

#define FIBN 30

int fib(unsigned int n) {
    if (n <= 1) {
        return 1;
    } else {
        return fib(n-1) + fib(n-2);
    }
}

int cpu_test(void) {
    cout << "CPU-Bound Test" << endl;
    int f = fib(FIBN);
    cout << "Fibonacci: " << f << endl;
    cout << "End CPU Test" << endl;
    return 0;
}

int network_test(void) {
    cout << "NIC Test" << endl;

    NIC<Ethernet> * nic = Traits<Ethernet>::DEVICES::Get<0>::Result::get(0);

    NIC<Ethernet>::Address src, dst;
    NIC<Ethernet>::Protocol prot;
    char data[nic->mtu()];

    NIC<Ethernet>::Address self = nic->address();
    cout << "  MAC: " << self << endl;

    if(self[5] % 2) { // sender
        Delay (3000000);

        for(int i = 0; i < 10; i++) {
            memset(data, '0' + i, nic->mtu());
            data[nic->mtu() - 1] = '\n';
            nic->send(nic->broadcast(), 0x8888, data, nic->mtu());
        }
    } else { // receiver
        for(int i = 0; i < 10; i++) {
           nic->receive(&src, &prot, data, nic->mtu());
           cout << "  Data: " << data;
        }
    }

    NIC<Ethernet>::Statistics stat = nic->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << "\n";
}

Thread *cpu_t;
Thread *nic_t;

int main()
{
    cout << "Test 3 (both)" << endl;
    Chronometer chrono;
    chrono.start();
    cpu_t = new Thread(&cpu_test);
    nic_t = new Thread(&network_test);
    cpu_t->join();
    nic_t->join();
    chrono.stop();
    cout << "Combined time: " << chrono.read()/1000 << "ms" << endl;
}
