// EPOS ARM Cortex Mediator Declarations

#ifndef __cortex_machine_h
#define __cortex_machine_h

#include <architecture/cpu.h>
#include __HEADER_MMOD(machine)

__BEGIN_SYS

class Machine: private Machine_Engine
{
    friend class Init_System;
    friend class First_Object;

private:
    static const bool smp = Traits<System>::multicore;

    typedef Machine_Engine Engine;

public:
    Machine() {}

    using Engine::delay;

    static const UUID & uuid() { return System::info()->bm.uuid; }

    static void panic();

    static void reboot()
    {
        db<Machine>(WRN) << "Machine::reboot()" << endl;
        Engine::reboot();
    }
    static void poweroff()
    {
        db<Machine>(WRN) << "Machine::poweroff()" << endl;
        Engine::poweroff();
    }

private:
    static void pre_init(System_Info * si);
    static void init();
};

__END_SYS

#endif
