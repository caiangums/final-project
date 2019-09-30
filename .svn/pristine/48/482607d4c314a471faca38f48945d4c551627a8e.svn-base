// EPOS Cortex-M3 System Control Module Mediator Declarations

#ifndef __emote3_sysctrl_h
#define __emote3_sysctrl_h

#include <architecture/cpu.h>

__BEGIN_SYS

class SysCtrl
{
    // This is a hardware object.
    // Use with something like "new (Memory_Map::SCR_BASE) SysCtrl".

private:
    typedef CPU::Reg32 Reg32;

public:
    // Registers' offsets in SysCtrl (not to be confused with System Control Space)
    enum {                              // Description                                          Type    Value after reset
        CLOCK_CTRL      = 0x00,
        CLOCK_STA       = 0x04,
        RCGCGPT         = 0x08,
        SCGCGPT         = 0x0c,
        DCGCGPT         = 0x10,
        SRGPT           = 0x14,
        RCGCSSI         = 0x18,
        SCGCSSI         = 0x1c,
        DCGCSSI         = 0x20,
        SRSSI           = 0x24,
        RCGCUART        = 0x28,
        SCGCUART        = 0x2c,
        DCGCUART        = 0x30,
        SRUART          = 0x34,
        RCGCI2C         = 0x38,
        SCGCI2C         = 0x3c,
        DCGCI2C         = 0x40,
        SRI2C           = 0x44,
        RCGCSEC         = 0x48,
        SCGCSEC         = 0x4c,
        DCGCSEC         = 0x50,
        SRSEC           = 0x54,
        PMCTL           = 0x58,
        SRCRC           = 0x5c,
        PWRDBG          = 0x74,
        CLD             = 0x80,
        IWE             = 0x94,
        I_MAP           = 0x98,
        RCGCRFC         = 0xa8,
        SCGCRFC         = 0xac,
        DCGCRFC         = 0xb0,
        EMUOVR          = 0xb4
    };

    enum POWER_MODE {
        ACTIVE = 0,
        PMSLEEP,
        POWER_MODE_0,
        POWER_MODE_1,
        POWER_MODE_2,
        POWER_MODE_3,
    };

    enum WAKE_UP_EVENT {
        EVENT_SLEEP_MODE_TIMER = 1 << 5,
        EVENT_USB = 1 << 4,
        EVENT_GPIO_D = 1 << 3,
        EVENT_GPIO_C = 1 << 2,
        EVENT_GPIO_B = 1 << 1,
        EVENT_GPIO_A = 1 << 0,
    };

public:
    SysCtrl() {}

    void power_gpio(const Power_Mode & mode) {
        // Change in power mode will only be effective when ASM("wfi") is executed
        switch(mode) {
        case ENROLL:
            break;
        case DISMISS:
            break;
        case SAME:
            break;
        case FULL: // Active Mode
        case LIGHT: // Sleep Mode
            // TODO: remove dependency from Memory_Map
            //            SCB<Memory_Map::SCS_BASE>::unsleepdeep();
            break;
        case SLEEP: // Deep Sleep Power Mode 0
            // TODO: remove dependency from Memory_Map
            //            SCB<Memory_Map::SCS_BASE>::sleepdeep();
//            scr(PMCTL) = 0;
            break;
        case OFF: // Deep Sleep Power Mode 3
            // TODO: remove dependency from Memory_Map
            //            SCB<Memory_Map::SCS_BASE>::sleepdeep();
//            scr(PMCTL) = 3;
            break;
        }
    }

    void power(unsigned int unit, const Power_Mode & mode) {
        assert(unit < UARTS);
        switch(mode) {
        case ENROLL:
            break;
        case DISMISS:
            break;
        case SAME:
            break;
        case FULL:
            break;
        case LIGHT:
            break;
        case SLEEP:
            break;
        case OFF:
            break;
        }
    }

    void clock_init() {
        // Initialize the clock
        CPU::Reg32 rcc = scr(RCC);

        // bypass PLL and system clock divider while initializing
        rcc |= RCC_BYPASS;
        rcc &= ~RCC_USESYSDIV;
        scr(RCC) = rcc;

        // select the crystal value and oscillator source
        rcc &= ~RCC_XTAL_8192;
        rcc |= RCC_XTAL_6000;
        rcc &= ~RCC_IOSC4;
        rcc |= RCC_MOSC;

        // activate PLL by clearing PWRDN and OEN
        rcc &= ~RCC_PWRDN;
        rcc &= ~RCC_OEN;

        // set the desired system divider and the USESYSDIV bit
        rcc &= ~RCC_SYSDIV_16;
        // Clock setup
        Reg32 sys_div;
        switch(Traits<CPU>::CLOCK) {
        default:
        case 50000000: sys_div = RCC_SYSDIV_4; break;
        case 40000000: sys_div = RCC_SYSDIV_5; break;
        case 25000000: sys_div = RCC_SYSDIV_8; break;
        case 20000000: sys_div = RCC_SYSDIV_10; break;
        case 12500000: sys_div = RCC_SYSDIV_16; break;
        }
        rcc |= sys_div;
        rcc |= RCC_USESYSDIV;
        scr(RCC) = rcc;

        // wait for the PLL to lock by polling PLLLRIS
        while(!(scr(RIS) & RIS_PLLLRIS));

        // enable use of PLL by clearing BYPASS
        rcc &= ~RCC_BYPASS;
        scr(RCC) = rcc;

    }

private:
    volatile Reg32 & scr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
};

__END_SYS

#endif
