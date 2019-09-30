// EPOS Cortex-M3 System Control Block Module Mediator Declarations

#ifndef __cortex_m3_scb_h
#define __cortex_m3_scb_h

#include <architecture/cpu.h>

__BEGIN_SYS

class SCB
{
    // This is a hardware object.
    // Use with something like "new (Memory_Map::SCS_BASE) SCB".

private:
    typedef CPU::Reg32 Reg32;

public:
    // Registers' offsets in System Control Space
    enum {                              // Description                                          Type    Value after reset
        ACTLR           = 0x008,        // Auxiliary Control Register                           rw      0x????????
        CPUID           = 0xd00,        // CPUID Base Register                                  ro      0x410fc231
        INTCTRL         = 0xd04,        // Interrupt Control and State Register                 rw      0x00000000
        VTOR            = 0xd08,        // Vector Table Offset Register                         rw      0x00000000
        AIRCR           = 0xd0c,        // Application Interrupt/Reset Control Register         rw
        SCR             = 0xd10,        // System Control Register                              rw      0x00000000
        CCR             = 0xd14,        // Configuration Control Register                       rw      0x00000000
        SHPR1           = 0xd18,        // System Handlers 4-7 Priority                         rw      0x00000000
        SHPR2           = 0xd1c,        // System Handlers 8-11 Priority                        rw      0x00000000
        SHPR3           = 0xd20,        // System Handlers 12-15 Priority                       rw      0x00000000
        SHCSR           = 0xd24,        // System Handler Control and State Register            rw      0x00000000
        CFSR            = 0xd28,        // Configurable Fault Status Register                   rw      0x00000000
        HFSR            = 0xd2c,        // Hard Fault Status Register
    };

    // Useful Bits in the Interrupt Control and State Register (INTCTRL)
    enum {                              // Description                                          Type    Value after reset
        ICSR_ACTIVE     = 1 <<  0,      // Active exceptions (IPSR mirror, 0 -> thread mode)    ro
        ICSR_PENDING    = 1 << 12,      // Pending exceptions (0 -> none)                       ro
        ICSR_ISRPENDING = 1 << 22,      // Pending NVIC IRQ                                     ro
        ICSR_SYSPENDING = 1 << 25       // Clear pending SysTick                                wo
    };

    // Useful Bits in the Application Interrupt/Reset Control Register (AIRCR)
    enum {                              // Description                                          Type    Value after reset
        VECTRESET       = 1 << 0,       // Reserved for debug                                   wo      0
        VECTCLRACT      = 1 << 1,       // Reserved for debug                                   wo      0
        SYSRESREQ       = 1 << 2,       // System Reset Request                                 wo      0
        VECTKEY         = 1 << 16,      // Register Key                                         rw      0xfa05
                                        // This field is used to guard against accidental
                                        // writes to this register.  0x05FA must be written
                                        // to this field in order to change the bits in this
                                        // register. On a read, 0xFA05 is returned.
    };

    // Useful Bits in the System Control Register (SCR)
    enum {                              // Description                                          Type    Value after reset
        SLEEPDEEP = 1 << 2,             // Makes the chip will enter power modes in accordance
                                        // to SysCtrl(PMCTL) when the operational mode initiator
                                        // (WFI instruction) is asserted.
    };

    // Useful Bits in the Configuration Control Register (CCR)
    enum {                              // Description                                          Type    Value after reset
        BASETHR         = 1 <<  0,      // Thread state can be entered at any level of int.     rw      0
        MAINPEND        = 1 <<  1,      // SWTRIG can be written to in user mode                rw      0
        UNALIGNED       = 1 <<  3,      // Trap on unaligned memory access                      rw      0
        DIV0            = 1 <<  4,      // Trap on division by zero                             rw      0
        BFHFNMIGN       = 1 <<  8,      // Ignore Precise Data Access Faults for pri -1 and -2  rw      0
        STKALIGN        = 1 <<  9       // Align stack point on exception entry to 8 butes      rw      0
    };

public:
    SCB() {}

    void reboot() {
        Reg32 val = scs(AIRCR) & (~((-1u / VECTKEY) * VECTKEY));
        val |= 0x05fa * VECTKEY | SYSRESREQ;
        scs(AIRCR) = val;
    }

    void sleepdeep() { scs(SCR) |= SLEEPDEEP; }
    void unsleepdeep() { scs(SCR) &= ~SLEEPDEEP; }

    void enter_thread_state() { scs(CCR) |= BASETHR; }

private:
    volatile Reg32 & scs(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
};

__END_SYS

#endif
