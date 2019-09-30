// EPOS Timer Test Program

#include <machine.h>

using namespace EPOS;

OStream cout;

void handler(const IC::Interrupt_Id & i)
{
    static int elapsed;

    int lin, col;
    Display::position(&lin, &col);
    Display::position(0, 60 + CPU::id() * 2);
    Display::putc((elapsed++ % 10) + 48);
    Display::position(lin, col);
}

int main()
{
    cout << "Timer test" << endl;

    User_Timer timer(0, 10000, handler);

    for(int i = 0; i < 10000; i++);
    cout << "count = " << timer.read() << "" << endl;
    for(int i = 0; i < 10000; i++);
    cout << "count = " << timer.read() << "" << endl;
    for(int i = 0; i < 10000; i++);
    cout << "count = " << timer.read() << "" << endl;
    for(int i = 0; i < 10000; i++);
    cout << "count = " << timer.read() << "" << endl;
    for(int i = 0; i < 10000; i++);
    cout << "count = " << timer.read() << "" << endl;
    for(int i = 0; i < 10000; i++);
    cout << "count = " << timer.read() << "" << endl;

    cout << "The End!" << endl;

    return 0;
}
