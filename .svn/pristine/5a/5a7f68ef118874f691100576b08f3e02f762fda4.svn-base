// EPOS Realview PBX (ARM Cortex-A9) Timer Mediator Declarations

#ifndef __model_timer_h
#define __model_timer_h

#define __common_only__
#include <architecture/tsc.h>
#include <machine/ic.h>
#undef __common_only__
#include <machine/cortex/engines/cortex_a9/global_timer.h>
#include <machine/cortex/engines/cortex_a9/private_timer.h>
#include "memory_map.h"
#include <utility/convert.h>

__BEGIN_SYS

class System_Timer_Engine: public Timer_Common
{
    friend Timer;

private:
    static const unsigned int TIMER_BASE = Memory_Map::PRIVATE_TIMER_BASE;

    typedef TSC_Common::Hertz Hertz;
    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef A9_Private_Timer::Count Count;

public:
    System_Timer_Engine() { new(pt()) A9_Private_Timer; }

    Count read() { return pt()->read(); }

    void enable() { pt()->enable(); }
    void disable() { pt()->disable(); }

    Hertz clock() { return pt()->clock(); }

private:
    static void eoi(const Interrupt_Id & id) { pt()->eoi(id); };

    static void init(const Hertz & frequency) {
        pt()->config(pt()->clock() / frequency, true, true);
        pt()->enable();
    }

private:
    static A9_Private_Timer * pt() { return reinterpret_cast<A9_Private_Timer *>(TIMER_BASE); }
};

// TODO: User_Timer should be mapped to ARM Dual Timers
class User_Timer_Engine: public Timer_Common
{
private:
    static const unsigned int UNITS = Traits<Timer>::UNITS;
    static const unsigned int TIMER_BASE = Memory_Map::GLOBAL_TIMER_BASE;

    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef A9_Global_Timer::Count Count;

public:
    User_Timer_Engine(unsigned int unit, const Microsecond & time, bool interrupt, bool periodic) {
        assert(unit < UNITS);
        power(FULL);
        new (gt()) A9_Global_Timer;
        gt()->config(time, interrupt, periodic);
    }
    ~User_Timer_Engine() {
        power(OFF);
    }

    Count count() { return gt()->count(); }

    void enable() { gt()->enable(); }
    void disable() { gt()->disable(); }

    Hertz clock() { return gt()->clock(); }

    void power(const Power_Mode & mode) {
        switch(mode) {
        case ENROLL:
            break;
        case DISMISS:
            break;
        case SAME:
            break;
        case FULL:
        case LIGHT:
        case SLEEP:
            // TODO: clock_gt(mode)
            break;
        case OFF:
            break;
        }
    }

protected:
    static void eoi(const Interrupt_Id & id) { gt()->eoi(id); };

private:
    static A9_Global_Timer * gt() { return reinterpret_cast<A9_Global_Timer *>(TIMER_BASE); }
};

__END_SYS

#endif
