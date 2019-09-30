// EPOS ARM Cortex-A9 Mediator Implementation

#include <machine/cortex/machine.h>
#include <machine/display.h>

__BEGIN_SYS

volatile unsigned int Machine_Engine::_cores;

void Machine_Engine::reboot()
{
    db<Machine>(WRN) << "Machine::reboot()" << endl;
//TODO: reboot!
}

__END_SYS
