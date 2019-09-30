// EPOS IC Mediator Common Package

#ifndef __ic_h
#define __ic_h

#include <system/config.h>

__BEGIN_SYS

class IC_Common
{
public:
    typedef unsigned int Interrupt_Id;
    typedef void (* Interrupt_Handler)(const Interrupt_Id &);

protected:
    IC_Common() {}

public:
    static Interrupt_Handler int_vector(const Interrupt_Id & id);
    static void int_vector(const Interrupt_Id & id, const Interrupt_Handler & handler);

    static void enable();
    static void enable(const Interrupt_Id & id);
    static void disable();
    static void disable(const Interrupt_Id & id);

    static Interrupt_Id irq2int(const Interrupt_Id & id);       // Offset IRQs as seen by the bus to INTs seen by the CPU (if needed)
    static Interrupt_Id int2irq(const Interrupt_Id & irq);      // Offset INTs as seen by the CPU to IRQs seen by the bus (if needed)

    static void ipi(unsigned int cpu, const Interrupt_Id & id); // Inter-processor Interrupt
};

__END_SYS

#endif

#if defined(__IC_H) && !defined(__common_only__)
#include __IC_H
#endif
