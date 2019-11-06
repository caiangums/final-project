// EPOS NIC Test Programs

#include <time.h>
#include <machine/display.h>
#include <machine/uart.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Asking time to Serial..." << endl;
    UART uart(0, 115200, 8, 0, 1); // using 0 for network, 1 is default for console (-serial mon:stdio)
    uart.loopback(true);
    uart.put('z');
    
    int current_time = uart.get();
    cout << "Received time: " << current_time << endl;
    return current_time;
    return 0;
}


