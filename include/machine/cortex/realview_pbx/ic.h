// EPOS Realview PBX (ARM Cortex-A9) IC Mediator Declarations

#ifndef __model_ic_h
#define __model_ic_h

#define __common_only__
#include <machine/ic.h>
#undef __common_only__
#include <machine/cortex/engines/cortex_a9/gic.h>
#include "memory_map.h"

__BEGIN_SYS

class IC_Engine: public IC_Common
{
private:
    static const unsigned int GIC_CPU_BASE = Memory_Map::GIC_CPU_BASE;
    static const unsigned int GIC_DIST_BASE = Memory_Map::GIC_DIST_BASE;

public:
    // Interrupts
    static const unsigned int INTS = Traits<IC>::INTS;
    static const unsigned int EXC_INT = 0; // Not mapped by IC. Exceptions are hard configured by SETUP.
    static const unsigned int SOFT_INT = 0;
    enum {
        INT_TIMER       = GIC::IRQ_PRIVATE_TIMER,
        INT_USER_TIMER0 = GIC::IRQ_GLOBAL_TIMER,
        INT_USER_TIMER1 = 0,
        INT_USER_TIMER2 = 0,
        INT_USER_TIMER3 = 0,
        INT_USB0        = GIC::IRQ_USB0,
        INT_GPIOA       = GIC::IRQ_GPIO,
        INT_GPIOB       = GIC::IRQ_GPIO,
        INT_GPIOC       = GIC::IRQ_GPIO,
        INT_GPIOD       = GIC::IRQ_GPIO,
        INT_NIC0_RX     = GIC::IRQ_ETHERNET0,
        INT_NIC0_TX     = GIC::IRQ_ETHERNET0,
        INT_NIC0_ERR    = GIC::IRQ_ETHERNET0,
        INT_NIC0_TIMER  = 0,
        INT_FIRST_HARD  = GIC::HARD_INT,
        INT_LAST_HARD   = GIC::IRQ_PARITY,
        INT_RESCHEDULER = GIC::IRQ_SOFTWARE0,
        LAST_INT        = INT_RESCHEDULER
    };

public:
    static void enable() { gic_distributor()->enable(); }
    static void enable(const Interrupt_Id & id)  { gic_distributor()->enable(id); }
    static void disable() { gic_distributor()->disable(); }
    static void disable(const Interrupt_Id & id) { gic_distributor()->disable(); }

    static Interrupt_Id int_id() { return gic_cpu()->int_id(); }
    static Interrupt_Id irq2int(const Interrupt_Id & id) { return gic_distributor()->irq2int(id); }
    static Interrupt_Id int2irq(const Interrupt_Id & irq) { return gic_distributor()->int2irq(irq); }

    static void ipi(unsigned int cpu, const Interrupt_Id & id) { gic_distributor()->send_sgi(cpu, id); }

    static void init() {
        gic_distributor()->init();
        gic_cpu()->init();
    };

private:
    static GIC_CPU * gic_cpu() { return reinterpret_cast<GIC_CPU *>(GIC_CPU_BASE); }
    static GIC_Distributor * gic_distributor() { return reinterpret_cast<GIC_Distributor *>(GIC_DIST_BASE); }
};

__END_SYS

#endif
