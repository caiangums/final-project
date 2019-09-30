// EPOS Cortex-A UART Mediator Declarations

#ifndef __cortex_a_uart_h
#define __cortex_a_uart_h

#include <architecture/cpu.h>
#include <machine/uart.h>
#include __MODEL_H

__BEGIN_SYS

#ifdef __mmod_zynq__

class UART_Engine: protected Machine_Model
{
private:
    static const unsigned int UNITS = Traits<UART>::UNITS;
    static const unsigned int CLOCK = Traits<UART>::CLOCK;

public:
    UART_Engine(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
        : _base(reinterpret_cast<Log_Addr *>(unit ? UART1_BASE : UART0_BASE)) {
        assert(unit < UNITS);
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        // Configure the number of stop bits, data bits, and the parity
        Reg32 mode = reg(MODE_REG0) & ~0xff;

        mode |= stop_bits == 2 ? NBSTOP2 : NBSTOP1;
        mode |= data_bits == 8 ? CHRL8 : data_bits == 7 ? CHRL7 : CHRL6;
        mode |= parity == 2 ? PAREVEN : parity == 1 ? PARODD : PARNONE;
        reg(MODE_REG0) = mode;

        // Set the baud rate
        Reg32 br = CLOCK / (7 * baud_rate);
        reg(BAUD_RATE_DIVIDER_REG0) = 6;
        reg(BAUD_RATE_GEN_REG0) = br;

        // Set the receiver trigger level to 1
        reg(RCVR_FIFO_TRIGGER_LEVEL0) = 1;

        // Enable and reset RX and TX data paths
        reg(CONTROL_REG0) = RXRES | TXRES | RXEN | TXEN;
    }
    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        Reg32 mode = reg(MODE_REG0);

        *baud_rate = CLOCK/(7*reg(BAUD_RATE_GEN_REG0));
        *data_bits = (mode & 0x6) == CHRL8 ? 8 : (mode & 0x6) == CHRL7 ? 7 : 6;
        *parity = (mode & 0x38) == PAREVEN ? 2 : (mode & 0x38) == PARODD ? 1 : 0;
        *stop_bits = (mode & 0xC0) == NBSTOP2 ? 2 : 1;
    }

    unsigned char rxd() { return reg(TX_RX_FIFO0); }
    void txd(unsigned char ch) { reg(TX_RX_FIFO0) = ch; }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0) |= (receive ? INTRPT_RTRIG : 0) | (send ? INTRPT_TTRIG : 0);
        reg(INTRPT_DIS_REG0) &= ~(receive ? INTRPT_RTRIG : 0) & ~(send ? INTRPT_TTRIG : 0);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0) &= ~(receive ? INTRPT_RTRIG : 0) & ~(send ? INTRPT_TTRIG : 0);
        reg(INTRPT_DIS_REG0) |= (receive ? INTRPT_RTRIG : 0) | (send ? INTRPT_TTRIG : 0);
    }

    void reset() {
        unsigned int b, db, p, sb;
        config(&b, &db, &p, &sb);
        config(b, db, p, sb);
    }

    void loopback(bool flag) {
        Reg32 mode = reg(MODE_REG0) & ~0x300;

        if(flag)
            mode |= CHMODELB;
        else
            mode |= CHMODENORM;

        reg(MODE_REG0) = mode;
    }

    bool rxd_ok() { return reg(CHANNEL_STS_REG0) & STS_RTRIG; }
    bool txd_ok() { return !(reg(CHANNEL_STS_REG0) & STS_TFUL); }

    bool rxd_full();
    bool txd_empty();

private:
    volatile Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile Reg32*>(_base)[o / sizeof(Reg32)]; }

private:
    volatile Log_Addr * _base;
};

#else

__END_SYS

#include <machine/common/pl011.h>

__BEGIN_SYS

typedef PL011 UART_Engine;

#endif

class UART: private UART_Common, private UART_Engine
{
private:
    typedef UART_Engine Engine;

    static const unsigned int UNIT = Traits<UART>::DEF_UNIT;
    static const unsigned int BAUD_RATE = Traits<UART>::DEF_BAUD_RATE;
    static const unsigned int DATA_BITS = Traits<UART>::DEF_DATA_BITS;
    static const unsigned int PARITY = Traits<UART>::DEF_PARITY;
    static const unsigned int STOP_BITS = Traits<UART>::DEF_STOP_BITS;

public:
    UART(unsigned int unit = UNIT, unsigned int baud_rate = BAUD_RATE, unsigned int data_bits = DATA_BITS, unsigned int parity = PARITY, unsigned int stop_bits = STOP_BITS)
    : Engine((enable_uart(unit), unit), baud_rate, data_bits, parity, stop_bits), _unit(unit) {}

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        Engine::config(baud_rate, data_bits, parity, stop_bits);
    }
    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        Engine::config(*baud_rate, *data_bits, *parity, *stop_bits);
    }

    char get() { while(!rxd_ok()); return rxd(); }
    void put(char c) { while(!txd_ok()); txd(c); }

    void flush() { while(!txd_empty()); }

    bool ready_to_get() { return rxd_ok(); }
    bool ready_to_put() { return txd_ok(); }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        Engine::int_enable(receive, send, line, modem);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        Engine::int_disable(receive, send, line, modem);
    }

    void reset() { Engine::reset(); }
    void loopback(bool flag) { Engine::loopback(flag); }

    void power(const Power_Mode & mode) { power_uart(_unit, mode); }

private:
    unsigned int _unit;
};

__END_SYS

#endif
