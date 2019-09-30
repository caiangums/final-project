// EPOS EPOSMote III (ARM Cortex-M3) Memory Map

#ifndef __emote3_memory_map_h
#define __emote3_memory_map_h

#include <system/memory_map.h>
#include <machine/cortex/memory_map.h>

__BEGIN_SYS

struct Memory_Map: public Cortex_Memory_Map
{
    // Physical Memory
    // Base addresses for memory-mapped control and I/O devices
    enum {
        SSI0_BASE       = 0x40008000,
        SSI1_BASE       = 0x40009000,
        UART0_BASE      = 0x4000c000, // PL011 UART
        UART1_BASE      = 0x4000d000, // PL011 UART
        I2C_MASTER_BASE = 0x40020000,
        I2C_SLAVE_BASE  = 0x40028000,
        TIMER0_BASE     = 0x40030000, // GPTM
        TIMER1_BASE     = 0x40031000, // GPTM
        TIMER2_BASE     = 0x40032000, // GPTM
        TIMER3_BASE     = 0x40033000, // GPTM
        SCR_BASE        = 0x400d2000,
        FLASH_CTRL_BASE = 0x400d3000,
        IOC_BASE        = 0x400d4000,
        GPIOA_BASE      = 0x400d9000, // PL061 GPIO Port A
        GPIOB_BASE      = 0x400da000, // PL061 GPIO Port B
        GPIOC_BASE      = 0x400db000, // PL061 GPIO Port C
        GPIOD_BASE      = 0x400dc000, // PL061 GPIO Port D
        SCS_BASE        = 0xe000e000, // System Control Space


    };

    // Logical Address Space
};

__END_SYS

#endif
