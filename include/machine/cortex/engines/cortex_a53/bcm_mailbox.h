// EPOS ARM Cortex-A IC Mediator Declarations

#ifndef __cortex_a_ic_h
#define __cortex_a_ic_h

#include <architecture/cpu.h>
#include <machine/ic.h>
#include __MODEL_H

__BEGIN_SYS

#ifndef __mmod_rasperry_pi3__

template<unsigned int BASE>
class GIC: public IC_Common
{
private:
    typedef CPU::Reg32 Reg32;

public:
    // IRQs
    static const unsigned int IRQS = Machine_Model::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_SOFTWARE0           = 0,
        IRQ_SOFTWARE1           = 1,
        IRQ_SOFTWARE2           = 2,
        IRQ_SOFTWARE3           = 3,
        IRQ_SOFTWARE4           = 4,
        IRQ_SOFTWARE5           = 5,
        IRQ_SOFTWARE6           = 6,
        IRQ_SOFTWARE7           = 7,
        IRQ_SOFTWARE8           = 8,
        IRQ_SOFTWARE9           = 9,
        IRQ_SOFTWARE10          = 10,
        IRQ_SOFTWARE11          = 11,
        IRQ_SOFTWARE12          = 12,
        IRQ_SOFTWARE13          = 13,
        IRQ_SOFTWARE14          = 14,
        IRQ_SOFTWARE15          = 15,
        IRQ_GLOBAL_TIMER        = 27,
        IRQ_NFIQ                = 28,
        IRQ_PRIVATE_TIMER       = 29,
        IRQ_AWDT                = 30,
        IRQ_NIRQ                = 31,
        IRQ_APU0                = 32,
        IRQ_APU1                = 33,
        IRQ_L2                  = 34,
        IRQ_OCM                 = 35,
        IRQ_PMU0                = 37,
        IRQ_PMU1                = 38,
        IRQ_XADC                = 39,
        IRQ_DEVC                = 40,
        IRQ_SWDT                = 41,
        IRQ_TTC0_0              = 42,
        IRQ_TTC0_1              = 43,
        IRQ_TTC0_2              = 44,
        IRQ_DMAC_ABORT          = 45,
        IRQ_DMAC0               = 46,
        IRQ_DMAC1               = 47,
        IRQ_DMAC2               = 48,
        IRQ_DMAC3               = 49,
        IRQ_SMC                 = 50,
        IRQ_QSPI                = 51,
        IRQ_GPIO                = 52,
        IRQ_USB0                = 53,
        IRQ_ETHERNET0           = 54,
        IRQ_ETHERNET0_WAKEUP    = 55,
        IRQ_SDIO0               = 56,
        IRQ_I2C0                = 57,
        IRQ_SPI0                = 58,
        IRQ_UART0               = 59,
        IRQ_CAN0                = 60,
        IRQ_PL0                 = 61,
        IRQ_PL1                 = 62,
        IRQ_PL2                 = 63,
        IRQ_PL3                 = 64,
        IRQ_PL4                 = 65,
        IRQ_PL5                 = 66,
        IRQ_PL6                 = 67,
        IRQ_PL7                 = 68,
        IRQ_TTC1_0              = 69,
        IRQ_TTC1_1              = 70,
        IRQ_TTC1_2              = 71,
        IRQ_DMAC4               = 72,
        IRQ_DMAC5               = 73,
        IRQ_DMAC6               = 74,
        IRQ_DMAC7               = 75,
        IRQ_USB1                = 76,
        IRQ_ETHERNET1           = 76,
        IRQ_ETHERNET1_WAKEUP    = 78,
        IRQ_SDIO1               = 79,
        IRQ_I2C1                = 80,
        IRQ_SPI1                = 81,
        IRQ_UART1               = 82,
        IRQ_CAN1                = 83,
        IRQ_PL8                 = 84,
        IRQ_PL9                 = 85,
        IRQ_PL10                = 86,
        IRQ_PL11                = 87,
        IRQ_PL12                = 88,
        IRQ_PL13                = 89,
        IRQ_PL14                = 90,
        IRQ_PL15                = 91,
        IRQ_PARITY              = 92,
    };

    // Interrupts
    static const unsigned int INTS = 93 + 1;
    static const unsigned int LAST_INT = INTS;
    static const unsigned int EXC_INT = 0; // Not mapped by IC. Exceptions are hard configured by SETUP.
    static const unsigned int HARD_INT = 16;
    static const unsigned int SOFT_INT = 0;
    enum {
        INT_TIMER       = IRQ_PRIVATE_TIMER,
        INT_USER_TIMER0 = IRQ_GLOBAL_TIMER,
        INT_USER_TIMER1 = 0,
        INT_USER_TIMER2 = 0,
        INT_USER_TIMER3 = 0,
        INT_USB0        = IRQ_USB0,
        INT_GPIOA       = IRQ_GPIO,
        INT_GPIOB       = IRQ_GPIO,
        INT_GPIOC       = IRQ_GPIO,
        INT_GPIOD       = IRQ_GPIO,
        INT_NIC0_RX     = IRQ_ETHERNET0,
        INT_NIC0_TX     = IRQ_ETHERNET0,
        INT_NIC0_ERR    = IRQ_ETHERNET0,
        INT_NIC0_TIMER  = 0,
        INT_FIRST_HARD  = HARD_INT,
        INT_LAST_HARD   = IRQ_PARITY,
        INT_RESCHEDULER = IRQ_SOFTWARE0
    };

    // GIC offsets from private peripheral space
    enum {
        GICCPU                      = 0x0100,
        GICDIST                     = 0x1000
    };

    // CPU Interface Registers offsets
    enum {                                      // Description                  Type    Value after reset
        ICCICR                      = 0x000,    // CPU Interface Control        r/w     0x00000000
        ICCPMR                      = 0x004,    // Interrupt Priority Mask      r/w     0x00000000
        ICCIAR                      = 0x00C,    // Interrupt Acknowledge        r/o     0x000003ff
        ICCEOIR                     = 0x010     // End Of Interrupt             w/o     -
    };

    // Useful bits in ICCICR
    enum {                                      // Description                  Type    Value after reset
        ITF_EN_S                    = 1 << 0,   // Enable secure signaling      r/w     0
        ITF_EN_NS                   = 1 << 1,   // Enable non-secure signaling  r/w     0
        ACK_CTL                     = 1 << 2    // Acknowledge control          r/w     0
    };

    // Distributor Registers offsets
    enum {                                      // Description                  Type    Value after reset
        ICDDCR                      = 0x000,    // Distributor Control          r/w     0
        ICDISER0                    = 0x100,    // Interrupt Set-Enable         r/w     0x0000ffff
        ICDISER1                    = 0x104,    // Interrupt Set-Enable         r/w     0x00000000
        ICDISER2                    = 0x108,    // Interrupt Set-Enable         r/w     0x00000000
        ICDISERn                    = 0x11c,    // Interrupt Set-Enable         r/w     0x00000000
        ICDICER0                    = 0x180,    // Interrupt Clear-Enable       r/w     0x0000ffff
        ICDICER1                    = 0x184,    // Interrupt Clear-Enable       r/w     0x00000000
        ICDICER2                    = 0x188,    // Interrupt Clear-Enable       r/w     0x00000000
        ICDICERn                    = 0x19c,    // Interrupt Clear-Enable       r/w     0x00000000
        ICDSGIR                     = 0xf00     // Software Generated Interrupt
    };

    // Useful bits in ICDDCR
    enum {                                      // Description                  Type    Value after reset
        DIST_EN_S                   = 1 << 0    // Enable secure interrupts     r/w     0
    };

public:
    GIC() {}

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static void enable() {
        gic_dist(ICDISER0) = ~0;
        gic_dist(ICDISER1) = ~0;
        gic_dist(ICDISER2) = ~0;
    }

    static void enable(int i) { gic_dist(ICDISER0 + (i/32)*4) = 1 << (i%32); }

    static void disable() {
        gic_dist(ICDICER0) = ~0;
        gic_dist(ICDICER1) = ~0;
        gic_dist(ICDICER1) = ~0;
    }

    static void disable(int i) { gic_dist(ICDICER0 + (i/32)*4) = 1 << (i%32); }

    static Interrupt_Id int_id() {
        Reg32 icciar = gic_cpu(ICCIAR) & INT_ID_MASK;

        // For every read of a valid interrupt id from the ICCIAR, the ISR must
        // perform a matching write to the ICCEOIR
        gic_cpu(ICCEOIR) = icciar;
        return icciar;
    }

    static void ipi(unsigned int cpu, const Interrupt_Id & id) {
        Reg32 target_list = 1 << cpu;
        gic_dist(ICDSGIR) = ((0 << 24) | (target_list << 16) | id); // FIXME:: 0 << 24????
    }

    static void init(void) {
        // Enable distributor
        gic_dist(ICDDCR) = DIST_EN_S;

        // Mask no interrupts
        gic_cpu(ICCPMR) = 0xf0;

        // Enable interrupts signaling by the CPU interfaces to the connected processors
        gic_cpu(ICCICR) = ACK_CTL | ITF_EN_NS | ITF_EN_S;
    }

protected:
    static volatile Reg32 & gic_cpu(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(BASE + GICCPU)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gic_dist(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(BASE + GICDIST)[o / sizeof(Reg32)]; }

    static void send_sgi(Reg32 interrupt_id, Reg32 target_list, Reg32 filter_list) {
        Reg32 aux = interrupt_id & 0x0F;
        target_list = target_list & 0x0F;
        filter_list = filter_list & 0x0F;
        aux = aux | (target_list << 16);
        aux = aux | (filter_list << 24);
        gic_dist(ICDSGIR) = aux;
    }

    // The versions bellow are dynamic and work on any A9, but are quite inefficient
//    static volatile Reg32 & gic_cpu(unsigned int offset) {
//        register Reg32 base asm("r0");
//        ASM("mrc p15, 4, %0, c15, c0, 0" : "=r"(base)); // get private peripheral space base into r0
//        return reinterpret_cast<volatile Reg32 *>(base + GICCPU)[offset / sizeof(Reg32)];
//    }
//    static volatile Reg32 & gic_dist(unsigned int offset) {
//        register Reg32 base asm("r0");
//        ASM("mrc p15, 4, %0, c15, c0, 0" : "=r"(base)); // get private peripheral space base into r0
//        return reinterpret_cast<volatile Reg32 *>(base + GICDIST)[offset / sizeof(Reg32)];
//    }

protected:
    static const unsigned int INT_ID_MASK = 0x3ff;
};

#else

// Broadcom BCM2835 IC
template<unsigned int BASE>
class BMC_IC: public IC_Common, private Machine_Model
{
private:
    typedef CPU::Reg32 Reg32;

public:
    // IRQs
    static const unsigned int IRQS = Machine_Model::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        // Interrupts not listed here should not be enable as they will interfere with the GPU operation

        //ENABLE_IRQ_1
        SYSTEM_TIMER_MATCH1           = 1,
        SYSTEM_TIMER_MATCH3           = 3,
        USB_CONTROLLER                = 9,
        UART_AUX_INT                  = 29,

        //ENABLE_IRQ_2
        I2C_SPI_SLV_INT               = 43,
        PWA0                          = 45,
        PWA1                          = 46,
        SMI                           = 48,
        GPIO_INT0                     = 49,
        GPIO_INT1                     = 50,
        GPIO_INT2                     = 51,
        GPIO_INT3                     = 52,
        I2C_INT                       = 53,
        SPI_INT                       = 54,
        PCM_INT                       = 55,
        UART_INT0                     = 57,

        //ENABLE_IRQ_BASIC
        ARM_TIMER_IRQ                 = 64,
        ARM_MAILBOX_IRQ               = 65,
        ARM_DOORBELL_0_IRQ            = 66,
        ARM_DOORBELL_1_IRQ            = 67,
        GPU_0_HALTED_IRQ              = 68,
        GPU_1_HALTED_IRQ              = 69,
        ACCESS_ERROR_TYPE_1_IRQ       = 70,
        ACCESS_ERROR_TYPE_0_IRQ       = 71,

        //MAILBOXES
        CORE0_MAILBOX0_IRQ            = 96,
        CORE0_MAILBOX1_IRQ            = 97,
        CORE0_MAILBOX2_IRQ            = 98,
        CORE0_MAILBOX3_IRQ            = 99,
        CORE1_MAILBOX0_IRQ            = 100,
        CORE1_MAILBOX1_IRQ            = 101,
        CORE1_MAILBOX2_IRQ            = 102,
        CORE1_MAILBOX3_IRQ            = 103,
        CORE2_MAILBOX0_IRQ            = 104,
        CORE2_MAILBOX1_IRQ            = 105,
        CORE2_MAILBOX2_IRQ            = 106,
        CORE2_MAILBOX3_IRQ            = 107,
        CORE3_MAILBOX0_IRQ            = 108,
        CORE3_MAILBOX1_IRQ            = 109,
        CORE3_MAILBOX2_IRQ            = 110,
        CORE3_MAILBOX3_IRQ            = 111
    };

    // Interrupts
    static const unsigned int INTS = 3 * 32 + 16 + 1;
    static const unsigned int LAST_INT = INTS;
    static const unsigned int HARD_INT = 0;
    static const unsigned int SOFT_INT = 0;
    enum {
        INT_TIMER       = SYSTEM_TIMER_MATCH1,
        INT_USER_TIMER0 = SYSTEM_TIMER_MATCH3,
        INT_USER_TIMER1 = 0,
        INT_USER_TIMER2 = 0,
        INT_USER_TIMER3 = 0,
        INT_USB0        = USB_CONTROLLER,
        INT_GPIOA       = GPIO_INT0,
        INT_GPIOB       = GPIO_INT1,
        INT_GPIOC       = GPIO_INT2,
        INT_GPIOD       = GPIO_INT3,
        INT_NIC0_RX     = 0,
        INT_NIC0_TX     = 0,
        INT_NIC0_ERR    = 0,
        INT_NIC0_TIMER  = 0,
        // INT_FIRST_HARD  = HARD_INT,
        // INT_LAST_HARD   = IRQ_PARITY,
        INT_RESCHEDULER = CORE0_MAILBOX0_IRQ,
        INT_RESCHEDULER0 = CORE0_MAILBOX0_IRQ,
        INT_RESCHEDULER1 = CORE1_MAILBOX0_IRQ,
        INT_RESCHEDULER2 = CORE2_MAILBOX0_IRQ,
        INT_RESCHEDULER3 = CORE3_MAILBOX0_IRQ
    };

    // BMC_CONTROL Registers
    enum {
        GPU_INT_ROUTING             = 0X00C,
        PMU_INT_ROUTING_SET         = 0X010,
        PMU_INT_ROUTING_CLR         = 0X014,
        LOCAL_INT_ROUTING           = 0X024, // Timer interrupt
        LOCAL_TIMER_CS              = 0X034, // Control/Status
        LOCAL_TIMER_WFLAGS          = 0X038, //

        CORE0_TIMER_INT_CTRL        = 0X040,
        CORE1_TIMER_INT_CTRL        = 0X044,
        CORE2_TIMER_INT_CTRL        = 0X048,
        CORE3_TIMER_INT_CTRL        = 0X04C,

        CORE0_MBOX_INT_CTRL         = 0X050,
        CORE1_MBOX_INT_CTRL         = 0X054,
        CORE2_MBOX_INT_CTRL         = 0X058,
        CORE3_MBOX_INT_CTRL         = 0X05C,

        CORE0_IRQ_SRC               = 0X060,
        CORE1_IRQ_SRC               = 0X064,
        CORE2_IRQ_SRC               = 0X068,
        CORE3_IRQ_SRC               = 0X06C,

        MBOX_WS_BASE                = 0X080, // Each CPU has 4 Mailboxes WRITE-SET   registers of 4 Bytes
        MBOX_WC_BASE                = 0X0C0  // Each CPU has 4 Mailboxes WRITE-CLEAR registers of 4 Bytes
    };

    // COREX_IRQ_SRC useful bits
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

    // IC Registers (as offsets to IC_BASE)
    enum {                                   // Type    Value after reset
        IRQ_BASIC_PENDING           = 0x000, // r       0x000
        IRQ_PENDING_1               = 0X004, // r       0x000
        IRQ_PENDING_2               = 0x008, // r/w     0x000
        FIQ_CONTROL                 = 0x00C, // r/w     0x000
        ENABLE_IRQS_1               = 0X010, // r/w     0x000
        ENABLE_IRQS_2               = 0x014, // r/w     0x000
        ENABLE_BASIC_IRQS           = 0X018, // r/w     0x000
        DISABLE_IRQS_1              = 0x01C, // r/w     0x000
        DISABLE_IRQS_2              = 0x020, // r/w     0x000
        DISABLE_BASIC_IRQS          = 0x024  // r/w     0x000
    };

    enum {
        ATISR                       = 0x0C,     // Interrupt Status ARM Timer
        STCLO                       = 0x04,     // Low COUNTER
        STC1                        = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC3                        = 0X18,     // Compare 3 - Value used to generate interrupt 3
        STCS                        = 0x0       // Control/Status System Timer
    };

public:
    BMC_IC() {}

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static void enable() {}

    static void enable(int i) {
        assert(i <= INTS);
        switch(i/32){
            case 0:
            case 1:
            case 2:
                ic_irq_enable((i/32)*4) |= 1 << (i%32);
                break;
            default:
                ic_mailbox_int((i%32)) |= 1 << i%4;
                break;
        }
    }

    static void disable() {}

    static void disable(int i) {
        assert(i < INTS);
        switch(i/32){
            case 0:
            case 1:
            case 2:
                ic_irq_disable((i/32)*4) |= 1 << (i%32);
                break;
            default:
                ic_mailbox_int((i%32)) &= ~(1 << i%4);
                break;
        }
    }

    static void mailbox_eoi(const Interrupt_Id & int_id) {
        unsigned int cpu;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(cpu) : : );
        cpu &= 0x3;
        ic_mailbox_wc(cpu*16) = 1 << 31;//ic_mailbox_ws(cpu*16 + (int_id%4)*4); // ACK
    }

    static Interrupt_Id int_id() {
        unsigned int cpu;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(cpu) : : );
        cpu &= 0x3;
        Reg32 mailbox_src = ic_mailbox_src(cpu*4);
        if(mailbox_src & 0x10) //if is a mailbox 0 interrupt    //ic_mailbox_wc(cpu*16) = ic_mailbox_ws(cpu*16); // ACK
            return CORE0_MAILBOX0_IRQ + cpu*4;
        else if (mailbox_src & 0x20) //if is a mailbox 1 interrupt
            return CORE0_MAILBOX1_IRQ + cpu*4;
        else if (mailbox_src & 0x40) //if is a mailbox 2 interrupt
            return CORE0_MAILBOX2_IRQ + cpu*4;
        else if (mailbox_src & 0x80) //if is a mailbox 3 interrupt
            return CORE0_MAILBOX3_IRQ + cpu*4;

        Reg32 basic_irqs_pending = ic_irq_pending(0);
        if (basic_irqs_pending) {
            if (basic_irqs_pending & 1) {
                //(volatile unsigned int *)(ARM_TIMER_BASE+ATISR) = 1; // ACK EOI
                return ARM_TIMER_IRQ;
            }
            // return 63 + basic_irqs_pending; // TODO
        }

        Reg32 irqs1_pending = ic_irq_pending(1*4);
        if (irqs1_pending) {
            if (irqs1_pending & 2) {
                //(volatile unsigned int *)(SYSTEM_TIMER_BASE) |= 1 << 1; //ACK EOI
                return SYSTEM_TIMER_MATCH1; // handler needs to reset counter compare register
            }
            if (irqs1_pending & 4) {
                //(volatile unsigned int *)(SYSTEM_TIMER_BASE) |= 1 << 3; //ACK EOI
                return SYSTEM_TIMER_MATCH3; // handler needs to reset counter compare register
            }
            return SYSTEM_TIMER_MATCH1; //TODO
        }
        Reg32 irqs2_pending = ic_irq_pending(2*4);
        return irqs2_pending;
    }

    static void ipi(unsigned int cpu, const Interrupt_Id & id) {
        ic_mailbox_ws(cpu*16) = 1 << 31;
    }

    static void init(void) {
        //mbox_init
        // volatile unsigned int *periph = (volatile unsigned int *)0x40000000;
        ic_mailbox_int(0*4) = 0xF; //0x50/sizeof(int) = 5*16/4 = 5 * 4 = 20
        ic_mailbox_int(1*4) = 0xF; //0x54/sizeof(int) = 5*16/4 + 4/4 = 5 * 4 +1 = 21
        ic_mailbox_int(2*4) = 0xF; //0x58/sizeof(int) = 5*16/4 + 8/4 = 5 * 4 +2 = 22
        ic_mailbox_int(3*4) = 0xF; //0x5C/sizeof(int) = 5*16/4 + 12/4 = 5 * 4 +3 = 23
    }

protected:
    static volatile Reg32 & ic_mailbox_src(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + CORE0_IRQ_SRC)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_mailbox_ws(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + MBOX_WS_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_mailbox_wc(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + MBOX_WC_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_mailbox_int(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + CORE0_MBOX_INT_CTRL)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_irq_enable(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(IC_BASE+ENABLE_IRQS_1)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_irq_disable(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IC_BASE+DISABLE_IRQS_1)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_irq_pending(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IC_BASE)[o / sizeof(Reg32)]; }

protected:
    static const unsigned int INT_ID_MASK = 0x3ff;
};

#endif

class IC: private GIC<Machine_Model::PPS_BASE>
{
    friend class Machine;
    friend Machine_Model;

private:
    typedef GIC<Machine_Model::PPS_BASE> Engine;
    typedef CPU::Reg32 Reg32;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;
    using Engine::INT_TIMER;
    using Engine::INT_USER_TIMER0;
    using Engine::INT_USER_TIMER1;
    using Engine::INT_USER_TIMER2;
    using Engine::INT_USER_TIMER3;
    using Engine::INT_GPIOA;
    using Engine::INT_GPIOB;
    using Engine::INT_GPIOC;
    using Engine::INT_GPIOD;
    using Engine::INT_USB0;
    using Engine::INT_NIC0_RX;
    using Engine::INT_NIC0_TX;
    using Engine::INT_NIC0_ERR;
    using Engine::INT_NIC0_TIMER;
    using Engine::INT_RESCHEDULER;
    using Engine::LAST_INT;

public:
    IC() {}

    static Interrupt_Handler int_vector(const Interrupt_Id & i) {
        assert(i < INTS);
        return _int_vector[i];
    }

    static void int_vector(const Interrupt_Id & i, const Interrupt_Handler & h) {
        db<IC>(TRC) << "IC::int_vector(int=" << i << ",h=" << reinterpret_cast<void *>(h) <<")" << endl;
        assert(i < INTS);
        _int_vector[i] = h;
    }

    static void enable() {
        db<IC>(TRC) << "IC::enable()" << endl;
        Engine::enable();
    }

    static void enable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::enable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::enable(i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        Engine::disable();
    }

    static void disable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::disable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::disable(i);
    }

    using Engine::irq2int;

    static void ipi(unsigned int cpu, const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::ipi(cpu=" << cpu << ",int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::ipi(cpu, i);
    }

    void undefined_instruction();
    void software_interrupt();
    void prefetch_abort();
    void data_abort();
    void reserved();
    void fiq();

private:
    static void dispatch(unsigned int i);
    static void eoi(unsigned int i);

    // Logical handlers
    static void int_not(const Interrupt_Id & i);
    static void hard_fault(const Interrupt_Id & i);

    // Physical handler
    static void entry();

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
    static Interrupt_Handler _eoi_vector[INTS];
};

__END_SYS

#endif
