// EPOS EPOSMote III (ARM Cortex-M3) MCU GPIO Mediator Declarations

#ifndef __model_gpio_h
#define __model_gpio_h

#include <machine/gpio.h>
#include "sysctrl.h"
#include <machine/cortex/engines/pl061.h>
#include "../engines/cortex_m3/scb.h"
#include "memory_map.h"

__BEGIN_SYS

class GPIO_Engine: private GPIO_Common, public PL061
{
private:
    static const bool supports_power_up = true;

public:
    GPIO_Engine(char port, unsigned int pin, const Direction & dir, const Pull & pull = UP, const Edge & int_edge = NONE)
    : PL061(pin, dir, pull), _port(port), _pin(pin), _pin_bit(1 << pin), _data(&gpio(_port, _pin_bit << 2)) {
        assert(port < GPIO_PORTS);
        clear_interrupts();
        if(int_edge != NONE) {
            _devices[_port][_pin] = this;
            int_enable(int_edge);
        }
    }
    ~GPIO_Engine() {
        int_disable();
        _devices[_port][_pin] = 0;
    }

    bool get() const {
        assert(_direction == IN || _direction == INOUT);
        return *_data;
    }

    void set(bool value = true) {
        assert(_direction == INOUT || _direction == OUT);
        if(_direction == INOUT) {
            gpio(_port, DIR) |= _pin_bit;
            *_data = 0xff * value;
            gpio(_port, DIR) &= ~_pin_bit;
        } else
            *_data = 0xff * value;
    }

    void clear() { set(false); }

    void direction(const Direction & dir) {
        _direction = dir;
        switch(dir) {
            case OUT:
                gpio(_port, DIR) |= _pin_bit;
                break;
            case IN:
            case INOUT:
                gpio(_port, DIR) &= ~_pin_bit;
                break;
        }
    }

    void pull(const Pull & p) {
        switch(p) {
            case UP:
                gpio_pull_up(_port, _pin);
                break;
            case DOWN:
                gpio_pull_down(_port, _pin);
                break;
            case FLOATING:
                gpio_floating(_port, _pin);
                break;
        }
    }

    void int_enable() { gpio(_port, IM) |= _pin_bit; }
    void int_enable(const Level & level, bool power_up = false, const Level & power_up_level = HIGH);
    void int_enable(const Edge & edge, bool power_up = false, const Edge & power_up_edge = RISING);
    void int_disable() { gpio(_port, IM) &= ~_pin_bit; }

    static void eoi(const IC::Interrupt_Id & i);



    static void wake_up_on(const WAKE_UP_EVENT & e) {
        scs(IWE) = e;
    }

private:
    void clear_interrupt() {
        gpio(_port, ICR) = _pin_bit;
        gpio(_port, IRQ_DETECT_ACK) &= ~(_pin_bit << (8 * _port));
    }

    static void int_handler(const IC::Interrupt_Id & i);

private:
    Port _port;
    Pin _pin;
    unsigned int _pin_bit;
    Direction _direction;
    volatile Reg32 * _data;

    static GPIO * _devices[GPIO_PORTS][8];
    static unsigned char _mis[GPIO_PORTS];
    static unsigned int _irq_detect_ack[GPIO_PORTS];
};

__END_SYS

#endif
