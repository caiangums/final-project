// EPOS EPOSMote III (ARM Cortex-M3) MCU GPIO Mediator Declarations

#ifndef __model_gpio_h
#define __model_gpio_h

#include "sysctrl.h"
#include <machine/cortex/engines/cortex_m3/scb.h>
#include <machine/cortex/engines/pl061.h>
#include "memory_map.h"

__BEGIN_SYS

class GPIO_Engine: public GPIO_Common
{
protected:
    static const unsigned int PORTS = Traits<GPIO>::UNITS;
    static const bool supports_power_up = true;

public:
    GPIO_Engine(unsigned int port, const Pin & pin, const Direction & dir, const Pull & p, const Edge & int_edge)
    : _port(port), _pin(pin), _pin_mask(1 << pin) {
        assert(port < PORTS);
        power(FULL);
        _gpio = _gpios[port];
        _gpio->select_pin_function(_pin_mask, PL061::FUN_GPIO);
        pull(p);
        direction(dir);
        if(int_edge != NONE) {
            _gpio->clear_interrupts(_pin_mask);
            _handlers[_port][_pin] = this;
            int_enable(int_edge);
        }
    }
    ~GPIO_Engine() {
        int_disable();
        _handlers[_port][_pin] = 0;
    }

    bool get() const {
        assert(_direction == IN || _direction == INOUT);
        return _gpio->get(_pin_mask);
    }

    void set(bool value = true) {
        assert(_direction == INOUT || _direction == OUT);
        if(_direction == INOUT)
            _gpio->direction(_pin_mask, OUT); // temporarily, so don't set _direction
        _gpio->set(_pin_mask, value);
        if(_direction == INOUT)
            _gpio->direction(_pin_mask, IN);
    }

    void clear() { _gpio->clear(_pin_mask); }

    void direction(const Direction & dir) {
        _direction = dir;
        _gpio->direction(_pin_mask, dir);
    }

    void pull(const Pull & p) { _gpio->pull(p, _pin_mask); }

    void int_enable() { _gpio->int_enable(_pin_mask); }
    void int_enable(const Level & level, bool power_up = false, const Level & power_up_level = HIGH);
    void int_enable(const Edge & edge, bool power_up = false, const Edge & power_up_edge = RISING);
    void int_disable() { _gpio->int_disable(_pin_mask); }

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
            // TODO: clock_gpio(mode)
//            scr(RCGC2) |= 1 << unit;                   // Activate port "unit" clock
            break;
        case OFF:
//            scr(RCGC2) &= ~(1 << unit);                // Deactivate port "unit" clock
            break;
        }
    }

//    static void wake_up_on(const WAKE_UP_EVENT & e) {
//        scs(IWE) = e;
//    }

protected:
    static void int_handler(const IC::Interrupt_Id & i);
    static void eoi(const IC::Interrupt_Id & i);

    static void init();

private:
    Port _port;
    Pin _pin;
    Pin _pin_mask;
    Direction _direction;
    PL061 * _gpio;

    static GPIO_Engine * _handlers[PORTS][8];
    static unsigned char _mis[PORTS];
    static unsigned int _irq_detect_ack[PORTS];

    PL061 * _gpios[PORTS];
};

__END_SYS

#endif
