// EPOS PC Mediator Declarations

#ifndef __pc_h
#define __pc_h

#include <architecture.h>
#include <machine/machine.h>
#include <machine/rtc.h>
#include <machine/display.h>
#include <system/info.h>
#include <system/memory_map.h>
#include "ic.h"
#include <system.h>

__BEGIN_SYS

class Machine: public Machine_Common
{
    friend class Init_System;
    friend class First_Object;

private:
    static const bool smp = Traits<System>::multicore;

    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    Machine() {}

    static void delay(const RTC::Microsecond & time) {
        assert(Traits<TSC>::enabled);
        TSC::Time_Stamp end = TSC::time_stamp() + time * (TSC::frequency() / 1000000);
        while(end > TSC::time_stamp());
    }

    static void panic();
    static void reboot();
    static void poweroff();

    static const UUID & uuid() { return System::info()->bm.uuid; }

private:
    static void pre_init(System_Info * si) {
        if(CPU::id() == 0) {
            Display::init();

            if(Traits<System>::multicore)
                CPU::smp_init(si->bm.n_cpus);
        }
    }

    static void init();
};

__END_SYS

#endif
