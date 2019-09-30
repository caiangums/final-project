// EPOS LM3S811 (ARM Cortex-M3) MCU UART Mediator Declarations

#ifndef __model_uart_h
#define __model_uart_h

#define __common_only__
#include <machine/uart.h>
#undef __common_only__
#include <machine/cortex/engines/pl011.h>
#include <machine/cortex/engines/pl061.h>
#include "memory_map.h"

__BEGIN_SYS

class UART_Engine: public UART_Common
{
private:
    static const unsigned int UNITS = Traits<UART>::UNITS;

public:
    UART_Engine(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        assert(unit < UNITS);
        _unit = unit;
        power(FULL);  // physically enable the UART in SysCtrl before configuring it
// TODO: move this to eMote. LM does not have IOC!
//        if(unit == 0) {   // pins A[1:0] are multiplexed between GPIOA and UART0; select UART
//            IOCtrl * ioc = new(reinterpret_cast<void *>(Memory_Map::IOC_BASE)) IOCtrl;
//            ioc->enable_uart0();
//            PL061 * pl061 = new(reinterpret_cast<void *>(Memory_Map::GPIOA_BASE)) PL061;
//            pl061->select_alternate_function(3);
//        }
        _pl011 = new(reinterpret_cast<void *>(Memory_Map::UART0_BASE + 0x1000 * unit)) PL011;
        _pl011->config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        _pl011->config(baud_rate, data_bits, parity, stop_bits);
    }
    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        _pl011->config(baud_rate, data_bits, parity, stop_bits);
    }

    char rxd() { return _pl011->rxd(); }
    void txd(char c) { return _pl011->txd(c); }

    bool rxd_ok() { return _pl011->rxd_ok(); }
    bool txd_ok() { return _pl011->txd_ok(); }

    bool rxd_full() { return _pl011->rxd_full(); }
    bool txd_empty() { return _pl011->txd_empty(); }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        _pl011->int_enable(receive, send, line, modem);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        _pl011->int_disable(receive, send, line, modem);
    }

    void reset() { _pl011->reset(); }
    void loopback(bool flag) { _pl011->loopback(flag); }

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
            // TODO: clock_uart(mode)
            //            scr(RCGCUART) |= 1 << _unit;                 // Enable clock for UART "unit" while in Running mode
            //            scr(SCGCUART) |= 1 << _unit;                 // Enable clock for UART "unit" while in Sleep mode
            //            scr(DCGCUART) |= 1 << _unit;                 // Enable clock for UART "unit" while in Deep Sleep mode
            break;
        case OFF:
            //            scr(RCGCUART) &= ~(1 << _unit);              // Deactivate UART "unit" clock
            //            scr(SCGCUART) &= ~(1 << _unit);              // Deactivate UART "unit" clock
            //            scr(DCGCUART) &= ~(1 << _unit);              // Deactivate UART "unit" clock
            break;
        }
    }

    static void init() {}

private:
    unsigned int _unit;
    PL011 * _pl011;
};

__END_SYS

#endif
