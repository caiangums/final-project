// EPOS NIC Test Programs

#include <time.h>
#include <machine/nic.h>
#include <communicator.h>

using namespace EPOS;

OStream cout;

const int DATA_SIZE = 10;
const int ITER = 1000;
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
    cout << "Network Bound Test" << endl;

    DIR_Protocol * comm = new DIR_Protocol();

    DIR_Protocol::Address self_addr = comm->address();

    char data[DATA_SIZE];
    if(self_addr[5] % 2) {  // sender
        Delay(2000000);

        for(int i = 0; i < ITER; i++) {
            memset(data, '0' + i%10, DATA_SIZE);
            data[DATA_SIZE - 1] = '\n';
            cout << " Sending: " << data;
            comm->send(data, DATA_SIZE);
        }
        cout << "Data sent." << endl;
    } else {  // receiver
        DIR_Protocol::Address from = self_addr;
        ++from[5];
        cout << "  Receiving from: " << from << endl;

        for(int i = 0; i < ITER; i++) {
            comm->receive(&from, data, DATA_SIZE);
            cout << "Received: " << data << endl;
        }
        cout << "Done." << endl;
    }
    cout << "End Network Test" << endl;
    return 0;
}

int main()
{
    cout << "Test 1 (CPU-bound)" << endl;
    Chronometer chrono;
    chrono.start();
    cpu_test();
    chrono.stop();
    int a = chrono.read()/1000;
    cout << "CPU Time: " << a << "ms" << endl;

    cout << "Test 2 (Network-bound)" << endl;
    chrono.reset();
    chrono.start();
    network_test();
    chrono.stop();
    int b = chrono.read()/1000;
    cout << "Network time: " << b << "ms" << endl;
}
