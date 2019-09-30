// EPOS Raspberry Pi3 (Cortex-A53) Mediator Declarations

#ifndef __raspberry_pi3_h
#define __raspberry_pi3_h

#include <architecture/cpu.h>
#include <architecture/tsc.h>
#include <machine/rtc.h>
#include <system.h>

__BEGIN_SYS

class Raspberry_Pi3
{
protected:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    static const unsigned int IRQS = 96;
    static const unsigned int TIMERS = 3;       // ARM Timer + 2 x System Timer
    static const unsigned int UARTS = 2;        // PL011 AND AUX
    static const unsigned int GPIO_PORTS = 0;   // FIXME: Not implemented yet
    static const bool supports_gpio_power_up = false;

    // Base addresses for memory-mapped I/O devices
    enum {
        I2C_BASE                    = 0x10002000, // Versatile I2C
        AAC_BASE                    = 0x10004000, // PrimeCell PL041 Advanced Audio CODEC
        MMCI_BASE                   = 0x10005000, // PrimeCell PL181 MultiMedia Card Interface
        KBD0_BASE                   = 0x10006000, // PrimeCell PL050 PS2 Keyboard/Mouse Interface
        KBD1_BASE                   = 0x10007000, // PrimeCell PL050 PS2 Keyboard/Mouse Interface
        UART0_BASE                  = 0x10009000, // PrimeCell PL011 UART
        UART1_BASE                  = 0x3f215000, // PrimeCell PL011 UART
        UART2_BASE                  = 0x1000b000, // PrimeCell PL011 UART
        UART3_BASE                  = 0x1000c000, // PrimeCell PL011 UART
        GPIO0_BASE                  = 0x3f200000, // PrimeCell PL061 GPIO
        GPIO1_BASE                  = 0x10014000, // PrimeCell PL061 GPIO
        GPIO2_BASE                  = 0x10015000, // PrimeCell PL061 GPIO
        RTC_BASE                    = 0x10017000, // PrimeCell PL031 RTC
        LCD_BASE                    = 0x10020000, // PrimeCell PL110 Color LCD Controller
        DMA_BASE                    = 0x3fe05000, // PrimeCell PL080 DMA Controller
        MBOX_COM                    = 0x3ef00000, // basic memory for device-os communication
        MBOX_COM_CPU_OFFSET         = 0x00040000, // memory offset per cpu
        MBOX_SIZE                   = 0x00000100, // mbox size (rounded up)

        PPS_BASE                    = 0x3f000000, // A9 Private Peripheral Space
        SCU_BASE                    = 0x1f000000, // MP Snoop Control Unit
        GIC_CPU_BASE                = 0x1f000100,
        GLOBAL_TIMER_BASE           = 0x1f000200,
        PRIVATE_TIMER_BASE          = 0x1f000600,
        GIC_DIST_BASE               = 0x1f001000,
        IC_BASE                     = 0x3f00b200, // BCM2870 IC
        ARM_TIMER_BASE              = 0x3f00b400, // ARM Timer (frequency relative to processor frequency)
        SYSTEM_TIMER_BASE           = 0x3f003000, // System Timer (free running)
        BMC_CONTROL                 = 0x40000000
    };

    // BMC_CONTROL Registers
    enum {
        GPU_INT_ROUTING             = 0x00C,
        PMU_INT_ROUTING_SET         = 0x010,
        PMU_INT_ROUTING_CLR         = 0x014,
        LOCAL_INT_ROUTING           = 0x024, // Timer Interrupt
        LOCAL_TIMER_CS              = 0x034, // Control / Satus
        LOCAL_TIMER_WFLAGS          = 0x038, //

        CORE0_TIMER_INT_CTRL        = 0x040,
        CORE1_TIMER_INT_CTRL        = 0x044,
        CORE2_TIMER_INT_CTRL        = 0x048,
        CORE3_TIMER_INT_CTRL        = 0x04c,

        CORE0_MBOX_INT_CTRL         = 0x050,
        CORE1_MBOX_INT_CTRL         = 0x054,
        CORE2_MBOX_INT_CTRL         = 0x058,
        CORE3_MBOX_INT_CTRL         = 0x05C,

        CORE0_IRQ_SRC               = 0x060,
        CORE1_IRQ_SRC               = 0x064,
        CORE2_IRQ_SRC               = 0x068,
        CORE3_IRQ_SRC               = 0x06c,

        MBOX_WS_BASE                = 0x080, // Each CPU has 4 Mailboxes WRITE-SET   registers of 4 Bytes
        MBOX_WC_BASE                = 0x0c0  // Each CPU has 4 Mailboxes WRITE-CLEAR registers of 4 Bytes
    };

    // SCU Registers (as offsets to SCU_BASE)
    enum {                                      // Description              Type    Value after reset
        SCU_CTRL                    = 0x00,     // Control                  r/w     0x00000128
        SCU_CONFIG                  = 0x04,     // Configuration            r/w     0x00000000
        SCU_PSTAT                   = 0x08,     // Power Status             r/w     0x00000000
        SCU_IARSS                   = 0x0c      // Invalidate All Reg Secure State
    };

    // Useful bits in the CONTROL_REG0 register
    enum {                                      // Description              Type    Value after reset
        RXRES                       = 1 << 0,   // Reset Rx data path       r/w     0
        TXRES                       = 1 << 1,   // Reset Tx data path       r/w     0
        RXEN                        = 1 << 2,   // Receive enable           r/w     0
        TXEN                        = 1 << 4    // Transmit enable          r/w     0
    };

    // Useful bits in the MODE_REG0 register
    enum {                                      // Description              Type    Value after reset
        CHRL8                       = 0 << 1,   // Character Length 8 bits  r/w     0
        CHRL7                       = 2 << 1,   // Character Length 7 bits  r/w     0
        CHRL6                       = 3 << 1,   // Character Length 6 bits  r/w     0
        PAREVEN                     = 0 << 3,   // Even parity              r/w     0
        PARODD                      = 1 << 3,   // Odd parity               r/w     0
        PARNONE                     = 4 << 3,   // No parity                r/w     0
        NBSTOP2                     = 2 << 6,   // 2 stop bits              r/w     0
        NBSTOP1                     = 0 << 6,   // 1 stop bit               r/w     0
        CHMODENORM                  = 0 << 8,   // Normal mode              r/w     0
        CHMODELB                    = 2 << 8    // Loopback mode            r/w     0
    };

    //COREX_IRQ_SRC usefull bits
    enum {
        SRC_CNTPS                   = 1 << 0,
        SRC_CNTPNS                  = 1 << 1,
        SRC_CNTHP                   = 1 << 2,
        SRC_CNTV                    = 1 << 3,
        SRC_MBOX_1                  = 1 << 4,
        SRC_MBOX_2                  = 1 << 5,
        SRC_MBOX_3                  = 1 << 6,
        SRC_MBOX_4                  = 1 << 7
    };

    // LOCAL_TIMER_CS usefull bits
    enum {
        LT_INT_ENABLE               = 1 << 29,
        LT_ENABLE                   = 1 << 28,
        LOAD_VALUE                  = 1 << 0
    };

    // LOCAL_TIMER_WFLAGS usefull bits
    enum {
        LT_INT_CLEAR               = 1 << 31,
        LT_RELOAD                  = 1 << 30
    };

    // COREX_MBOX_INT_CTRL usefull bits
    enum {
        MBOX_IRQ_0                  = 1 << 0,
        MBOX_IRQ_1                  = 1 << 1,
        MBOX_IRQ_2                  = 1 << 2,
        MBOX_IRQ_3                  = 1 << 3
    };

    // COREX_TIMER_INT_CTRL usefull bits
    enum {
        CNT_PSIRQ                   = 1 << 0,
        CNT_PNSIRQ                  = 1 << 1,
        CNT_HPIRQ                   = 1 << 2,
        CNT_VIRQ                    = 1 << 3
    };

    // IC Registers (as offsets to IC_BASE)
    enum {                                   // Type    Value after reset
        IRQ_BASIC_PENDING           = 0x000, // r       0x000
        IRQ_PENDING_1               = 0x004, // r       0x000
        IRQ_PENDING_2               = 0x008, // r/w     0x000
        FIQ_CONTROL                 = 0x00C, // r/w     0x000
        ENABLE_IRQS_1               = 0x010, // r/w     0x000
        ENABLE_IRQS_2               = 0x014, // r/w     0x000
        ENABLE_BASIC_IRQS           = 0x018, // r/w     0x000
        DISABLE_IRQS_1              = 0x01C, // r/w     0x000
        DISABLE_IRQS_2              = 0x020, // r/w     0x000
        DISABLE_BASIC_IRQS          = 0x024  // r/w     0x000
    };

    //usefull bits in IRQ_BASIC
    enum {
        ARM_TIMER                   = 1 << 0,
        ARM_MAILBOX                 = 1 << 1
    };

    //usefull bits in IRQ_1
    enum {
        SYSTEM_C1                   = 1 << 1,
        SYSTEM_C3                   = 1 << 3
    };

    // Useful bits in the INTRPT_EN_REG0, and INTRPT_DIS_REG0 registers
    // enum {                                      // Description              Type    Value after reset
    //     INTRPT_RTRIG                = 1 << 0,   // Receiver FIFO empty      wo      0
    //     INTRPT_TTRIG                = 1 << 10   // Transmitter FIFO trigger wo      0
    // };

    // Useful bits in the CHANNEL_STS_REG0 register
    // enum {                                      // Description              Type    Value after reset
    //     STS_RTRIG                   = 1 << 0,   // Receiver FIFO trigger    ro      0
    //     STS_TFUL                    = 1 << 4    // Transmitter FIFO full    ro      0
    // };

    // SLCR Registers offsets
    // enum {                                      // Description
    //     SLCR_LOCK                   = 0x004,    // Lock the SLCR
    //     SLCR_UNLOCK                 = 0x008,    // Unlock the SLCR
    //     UART_CLK_CTRL               = 0x154,    // UART Ref Clock Control
    //     FPGA0_CLK_CTRL              = 0x170,    // PL Clock 0 Output control
    //     PSS_RST_CTRL                = 0x200,    // PS Software Reset Control
    //     FPGA_RST_CTRL               = 0x240     // FPGA Software Reset Control
    // };

    // Useful bits in SLCR_LOCK
    // enum {                                      // Description                  Type    Value after reset
    //     LOCK_KEY                    = 0x767B    // Lock key                     wo      0
    // };

    // Useful bits in SLCR_UNLOCK
    // enum {                                      // Description                  Type    Value after reset
    //     UNLOCK_KEY                  = 0xDF0D    // Unlock key                   wo      0
    // };

    // Useful bits in FPGAN_CLK_CTRL
    // enum {                                      // Description                  Type    Value after reset
    //     DIVISOR0                    = 1 << 8,   // First cascade divider        r/w     0x18
    //     DIVISOR1                    = 1 << 20   // Second cascade divider       r/w     0x1
    // };

    // SYSTEM TIMER
    enum {                                      // Description
        STCS                        = 0x00,     // Control/Status
        STCLO                       = 0x04,     // Low COUNTER
        STCHI                       = 0x08,     // High Counter
        STC0                        = 0x0C,     // Compare 0 - Used by GPU
        STC1                        = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC2                        = 0x14,     // Compare 2 - Used by GPU
        STC3                        = 0x18      // Compare 3 - Value used to generate interrupt 3
        // Interrupts mapped to "Enable IRQ 1" - c1 and c3 == irq1 and irq3
    };

    // Useful bits in CS write one to clear interrupt
    enum {
        STCSM0                      = 1 << 0, // Clear Interrup Compare 0
        STCSM1                      = 1 << 1, // Clear Interrup Compare 1
        STCSM2                      = 1 << 2, // Clear Interrup Compare 2
        STCSM3                      = 1 << 3, // Clear Interrup Compare 3
        TIMER_ENABLE                = STCSM1
    };

    // ARM TIMER
    enum {                                      // Description
        ATLR                        = 0x00,     // Load
        ATCTR                       = 0x04,     // Counter
        ATCLR                       = 0x08,     // Control
        ATISR                       = 0x0C,     // Interrupt Status
        ATRLD                       = 0x18,     // Reload (Update reload without reset counting)
        ATDIV                       = 0x1C
    };

    // Useful bits in ATCLR
    enum {                                      // Description                  Type    Value after reset
        RESET_VALUE                 = 0x003E0000,
        ARM_TIMER_ENABLE            = 1 << 7,   // Enable                       r/w     0
        IRQ_EN                      = 1 << 5,    // Enable interrupt             r/w     0
        ATDIV_VALUE                 = 0xF9
    };

    // Useful bits in PTISR
    enum {                                      // Description                  Type    Value after reset
        INT_CLR                     = 0    // Interrupt clear bit          r/w     0
    };

    // TTB definitions
    enum {
        TTBCR_DOMAIN                = 0x55555555, // All access to client
        TTB_DESCRIPTOR              = 0x10c0A
        // According to ARMv7 Arch. Ref. Manual: (description beggining at pages 1326 to 1329)
        // REG[19] NS, 0b0 for secure address space -> no effect on  Physical Address Space. -> Page 1330
        // REG[18] = 0b0 (mapped on 1 MB), it is not a supersection -> page 1329
        // REG[17] = 0b0 the TTB is global -> page 1378
        // REG[16] = 0b1 means shareable memory -> page 1368
        // REG[15] = 0b0 means read/write memory (if 1, read-only) -> ->  page 1358
        // REG[14:12] = TEX - 0b000 means possibly shareable page -> 1367
        // REG[11:10] = 0b11 means read/write with full access ->  page 1358
        // REG[9] = 0b0, Implementation Defined. -> page 1329
        // Reg[8:5] = 0b0000, Domain - not supported (DEPRECATED) its written on MCR p15, 0, <rt>, c3, c0, 0 -
        // REG[4] = XN = 0b0, means code can be executed. 0b1 stands for not exacutable area (generates Page Fault) -> page 1359
        // REG[3] = C (cacheable) = 1 with the config of TEX it means no Write-Allocate -> page 1367
        // REG[2] = B (Bufferable) = 0 with the config of TEX and C, it means Write-Through -> page 1367
        // Reg[1] = 0b1 means it points a section (more or equal than a MEGABYTE) -> Page 1329
        // REG[0] = PXN = 0b0 means every core can run, else processor running at PL1 generates Permission Fault-> page 1359
    };

protected:
    Raspberry_Pi3() {}

    static void reboot() {}

    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(id) : : );
        return id & 0x3;
    }

    static unsigned int n_cpus() {
        int n;
        ASM("mrc p15, 4, %0, c15, c0, 0 \t\n\
             ldr %0, [%0, #0x004]" : "=r"(n) : : );
        return (n & 0x3) + 1;
    }

    static void smp_init(unsigned int n_cpus);
        // Realview_PBX init changed in order to only wakeup n-1 cores

    static void enable_uart(unsigned int unit) {}

    // Power Management
    static void power_uart(unsigned int unit, const Power_Mode & mode) {
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

    static void power_user_timer(unsigned int unit, const Power_Mode & mode) {
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
        case SLEEP:
        case OFF:
            break;
        }
    }

    static void branch_prediction_enable() {
        ASM("                                                                           \t\n\
            MRC     p15, 0, r0, c1, c0, 0                  // Read SCTLR                \t\n\
            ORR     r0, r0, #(1 << 11)                     // Set the Z bit (bit 11)    \t\n\
            MCR     p15, 0,r0, c1, c0, 0                   // Write SCTLR               \t\n\
            ");
    }

    static void enable_dside_prefetch() {
        ASM("MRC p15, 0, r0, c1, c0, 1 \t\n\
             ORR r0, r0, #(0x1 << 2)   \t\n\
             MCR p15, 0, r0, c1, c0, 1 ");
    }

    static void invalidate_tlb() {
        ASM("MOV r0, #0x0 \t\n MCR p15, 0, r0, c8, c7, 0"); // TLBIALL - Invalidate entire Unifed TLB
    }

    static void clear_branch_prediction_array() {
        ASM("MOV r0, #0x0 \t\n MCR p15, 0, r0, c7, c5, 6"); // BPIALL - Invalidate entire branch predictor array
    }

    static void set_domain_access() {
        // ASM("MCR p15, 0, %0, c3, c0, 0" : : "p"(Realview_PBX::TTBCR_DOMAIN) :);
        ASM("MCR p15, 0, %0, c3, c0, 0" : : "p"(0xFFFFFFFF) :);
    }

    static void enable_scu() {
        scu(0) |= 0x1;
    }

    static void secure_scu_invalidate() {
        // void secure_SCU_invalidate(unsigned int cpu, unsigned int ways)
        // cpu: 0x0=CPU 0 0x1=CPU 1 etc...
        // This function invalidates the SCU copy of the tag rams
        // for the specified core.  Typically only done at start-up.
        // Possible flow:
        //- Invalidate L1 caches
        //- Invalidate SCU copy of TAG RAMs
        //- Join SMP
        Reg32 _cpu = cpu_id();
        _cpu = _cpu << 2; // Convert into bit offset (four bits per core)
        Reg32 _ways = 0x0F; // all four ways
        _ways = _ways << _cpu; // Shift ways into the correct CPU field
        scu(SCU_IARSS) = _ways;
    }

    static void scu_enable_cache_coherence() {
        scu(SCU_CONFIG) |= 0xF0;
    }

    static void enable_maintenance_broadcast() {
        // Enable the broadcasting of cache & TLB maintenance operations
        // When enabled AND in SMP, broadcast all "inner sharable"
        // cache and TLM maintenance operations to other SMP cores
        ASM("MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR.             \t\n\
             ORR     r0, r0, #(0x01)         // Set the FW bit (bit 0). \t\n\
             MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR."
        );
    }

    static void join_smp() {
        ASM("                                                                       \t\n\
            MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR                           \t\n\
            ORR     r0, r0, #(0x01 << 6)    // Set SMP bit                          \t\n\
            MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR                          \t\n\
            ");
    }

    static void page_tables_setup() {
        Reg32 aux = 0x0;
        for (int curr_page = 1006; curr_page >= 0; curr_page--) {
            // aux = TTB_DESCRIPTOR | (curr_page << 20);
            aux = 0x90C0E | (curr_page << 20);
            reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[curr_page] = aux;
        }
        aux = 0x90C0A | (1007 << 20);
        reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[1007] = aux;
        for (int curr_page = 4095; curr_page > 1007; curr_page--) {
            aux = 0x90C16 | (curr_page << 20);
            reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[curr_page] = aux;
        }
        // reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[0] = TTB_DESCRIPTOR;
    }

    static void enable_mmu() {
        // TTB0 size is 16 kb, there is no TTB1 and no TTBCR
        // ARMv7 Architecture Reference Manual, pages 1330
        ASM ("MOV r0, #0x0 \t\n MCR p15, 0, r0, c2, c0, 2"); // Write Translation Table Base Control Register.
        ASM ("MCR p15, 0, %0, c2, c0, 0" : : "p"(Traits<Machine>::PAGE_TABLES) :); // Write Translation Table Base Register 0.

        // Enable MMU
        //-------------
        //0     - M, set to enable MMU
        // Leaving the caches disabled until after scatter loading.
        ASM ("                                                                      \t\n\
            MRC     p15, 0, r0, c1, c0, 1       // Read ACTLR                       \t\n\
            ORR     r0, r0, #(0x01 << 6)        // Set SMP bit                      \t\n\
            MCR     p15, 0, r0, c1, c0, 1       // Write ACTLR                      \t\n\
            MRC     p15, 0, r0, c1, c0, 0       // Read current control reg         \t\n\
            ORR     r0, r0, #(0x1 << 2)         // The C bit (data cache).          \t\n\
            BIC     R0, R0, #(0x1 << 29)        // Set AFE to 0 disable Access Flag.\t\n\
            ORR     r0, r0, #(0x1 << 12)        // The I bit (instruction cache).   \t\n\
            ORR     r0, r0, #0x01               // Set M bit                        \t\n\
            MCR     p15, 0, r0, c1, c0, 0       // Write reg back                   \t\n\
            ");
    }

    static void clear_bss() {
        Reg32 bss_start, bss_end;
        ASM("LDR %0, =__bss_start__" : "=r"(bss_start) :);
        ASM("LDR %0, =__bss_end__" : "=r"(bss_end) :);
        Reg32 limit = (bss_end - bss_start)/4;
        for(Reg32 i = 0; i < limit; i++) {
            reinterpret_cast<volatile Reg32 *>(bss_start)[i] = 0x0;
        }
    }

    typedef struct {
        volatile Reg32 read;
        volatile Reg32 rsvd[3];
        volatile Reg32 peek;
        volatile Reg32 sender;
        volatile Reg32 status;
        volatile Reg32 config;
        volatile Reg32 write;
    } Mailbox;


    enum {
        MBOX0_BASE              = (PPS_BASE + 0xB880), //Mailbox 0, read by ARM
        MBOX1_BASE              = (PPS_BASE + 0xB8A0), //Mailbox 0, read by GPU
        MBOX_FULL               = 0x80000000,
        MBOX_EMPTY              = 0x40000000,
        TEMPERATURE_TAG         = 0x00030006,
        CLOCK_TAG               = 0x00030002,
        VOLTAGE_TAG             = 0x00030003,

        // ids
        ARM_CLOCK_ID            = 0x3,
        TEMPERATURE_ID          = 0x0,
        VOLTAGE_ID              = 0x1
    };

#define MAILBOX0  ((volatile Mailbox*) MBOX0_BASE)

public:
    static volatile Reg32 & scu(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCU_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & global_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GLOBAL_TIMER_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & private_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SYSTEM_TIMER_BASE)[o / sizeof(Log_Addr)]; }
    static volatile Reg32 & arm_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(ARM_TIMER_BASE)[o / sizeof(Log_Addr)]; }

    struct msg_tag {
            volatile Reg32 tag_id;              // the message id
            volatile Reg32 buffer_size;         // size of the buffer (which in this case is always 8 bytes)
            volatile Reg32 respreq;             // the ID of the clock/voltage to get or set
            volatile Reg32 val;                 // the value (e.g. rate (in Hz)) to set
        volatile Reg32 val2;                    // the value (e.g. rate (in Hz)) to set
    };

    struct mailbox_msg {
            volatile Reg32 msg_size;            // simply, sizeof(struct vc_msg)
            volatile Reg32 request_code;        // holds various information like the success and number of bytes returned (refer to mailboxes wiki)
            struct msg_tag tag;                 // the tag structure above to make
            volatile Reg32 end_tag;             // an end identifier, should be set to NULL
    } __attribute__((aligned(16)));

    static void wait_mailbox_write(volatile Mailbox *m) {
        while (m->status & MBOX_FULL)
            ASM ("nop");
    }

    static void wait_mailbox_read(volatile Mailbox *m) {
        while (m->status & MBOX_EMPTY)
            ASM ("nop");
    }

    static void writeMailbox0(Reg32 data, Reg32 channel) {
         // = (Mailbox*) MBOX0_BASE;
        wait_mailbox_write(MAILBOX0);
        MAILBOX0->write = (data &~0xF) | (Reg32) (channel & 0xf);
    }

    static Reg32 readMailbox0(Reg32 channel) {
        // Mailbox* m = (Mailbox*) MBOX0_BASE;
        Reg32 res;
        Reg32 read_channel;
        wait_mailbox_read(MAILBOX0);
        do {
            res = MAILBOX0->read;
            read_channel = res & 0xf;
        } while (read_channel != channel);
        return (res>>4);
    }

    static void prepare_call(volatile struct mailbox_msg* msg, Reg32 id, Reg32 tag) {
        msg->msg_size = sizeof(struct mailbox_msg);
        msg->request_code = 0x0;
        msg->tag.tag_id = tag;
        msg->tag.buffer_size = 0x8;
        msg->tag.respreq = 0x8;
        msg->tag.val = id;
        msg->tag.val2 = 0x0;
        msg->end_tag = 0x0;
    }
    static void clean_msg(volatile struct mailbox_msg* msg) {
        msg->msg_size = 0x0;
        msg->request_code = 0x0;
        msg->tag.tag_id = 0x0;
        msg->tag.buffer_size = 0x0;
        msg->tag.respreq = 0x0;
        msg->tag.val = 0x0;
        msg->tag.val2 = 0x0;
        msg->end_tag = 0x0;
    }

    static Reg32 temperature() {
        volatile struct mailbox_msg * local= (volatile struct mailbox_msg *)(MBOX_COM+MBOX_COM_CPU_OFFSET*cpu_id());//+MBOX_COM_CPU_OFFSET*cpu_id());
        prepare_call(local, (Reg32)TEMPERATURE_ID, (Reg32)TEMPERATURE_TAG);
        writeMailbox0((unsigned long)local, 8);
        readMailbox0(8);
        return local->tag.val2;
    }

    static Reg32 arm_clock() {
        volatile struct mailbox_msg * local= (volatile struct mailbox_msg *)(MBOX_COM+MBOX_COM_CPU_OFFSET*cpu_id());//+MBOX_COM_CPU_OFFSET*cpu_id());
        prepare_call(local, (Reg32)ARM_CLOCK_ID, (Reg32)CLOCK_TAG);
        writeMailbox0((unsigned long)local, 8);
        readMailbox0(8);
        return local->tag.val2;
    }

    static Reg32 arm_voltage() {
        volatile struct mailbox_msg * local= (volatile struct mailbox_msg *)(MBOX_COM+MBOX_COM_CPU_OFFSET*cpu_id());//+MBOX_COM_CPU_OFFSET*cpu_id());
        prepare_call(local, (Reg32)VOLTAGE_ID, (Reg32)VOLTAGE_TAG);
        writeMailbox0((unsigned long)local, 8);
        readMailbox0(8);
        return local->tag.val2;
    }

    static void pre_init();
    static void init();
};

typedef Raspberry_Pi3 Machine_Model;

__END_SYS

#endif
