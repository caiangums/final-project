// EPOS EPOSMote III (ARM Cortex-M3) MCU UART Mediator Declarations

#ifndef __model_uart_h
#define __model_uart_h

#include <machine/uart.h>
#include "sysctrl.h"
#include <machine/cortex/engines/pl011.h>
#include "../engines/cortex_m3/ioctrl.h"
#include "memory_map.h"

__BEGIN_SYS

class UART_Engine: public PL011
{
private:
    static const unsigned int UNITS = Traits<UART>::UNITS;

public:
    UART_Engine(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
    : PL011((IOCtrl<Memory_Map::IOC_BASE>::enable_uart(unit), Memory_Map::UART0_BASE + 0x1000 * unit), baud_rate, data_bits, parity, stop_bits), _unit(unit) {
        assert(unit < UNITS);
    }

    void power(const Power_Mode & mode) { SysCtrl<Memory_Map::SCR_BASE>::power_uart(_unit, mode); }

private:
    unsigned int _unit;
};

__END_SYS

#endif
