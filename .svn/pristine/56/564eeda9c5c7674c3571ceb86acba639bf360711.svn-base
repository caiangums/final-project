// EPOS Realview PBX (ARM Cortex-A9) Mediator Declarations

#ifndef __realview_pbx_h
#define __realview_pbx_h

#define __common_only__
#include <machine/machine.h>
#include <machine/rtc.h>
#undef __common_only__
#include <machine/cortex/engines/cortex_a9/gic.h>
#include <machine/cortex/engines/cortex_a9/scu.h>
#include "memory_map.h"
#include <system.h>

__BEGIN_SYS

class Machine_Engine: public Machine_Common
{
private:
    static const unsigned int GIC_CPU_BASE = Memory_Map::GIC_CPU_BASE;
    static const unsigned int GIC_DIST_BASE = Memory_Map::GIC_DIST_BASE;
    static const unsigned int SCU_BASE = Memory_Map::SCU_BASE;

    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;
    typedef RTC_Common::Microsecond Microsecond;

public:

public:
    Machine_Engine() {}

    static void delay(const Microsecond & time);

    static void reboot();
    static void poweroff() { reboot(); }

    static const UUID & uuid() { return System::info()->bm.uuid; }

public:
    static void smp_init(unsigned int n_cpus) {
        gic_distributor()->smp_init(n_cpus);
    }
    static void pre_init();
    static void init() {}

private:
    static GIC_CPU * gic_cpu() { return reinterpret_cast<GIC_CPU *>(GIC_CPU_BASE); }
    static GIC_Distributor * gic_distributor() { return reinterpret_cast<GIC_Distributor *>(GIC_DIST_BASE); }
    static SCU * scu() { return reinterpret_cast<SCU *>(SCU_BASE); }

private:
    static volatile unsigned int _cores;
};

__END_SYS

#endif
