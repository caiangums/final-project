// EPOS ARM Cortex GPIO Mediator Declarations

#ifndef __cortex_gpio_h
#define __cortex_gpio_h

#include <architecture/cpu.h>
#include <machine/gpio.h>
#include <machine/ic.h>
#include __HEADER_MMOD(gpio)
#include <utility/observer.h>

__BEGIN_SYS

class GPIO: public GPIO_Engine, public Observed
{
    friend class Machine;
    friend class PWM;

private:
    typedef GPIO_Engine Engine;

public:
    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

public:
    GPIO(char port, const Pin & pin, const Direction & dir, const Pull & pull = UP, const Edge & int_edge = NONE)
    : Engine(port, pin, dir, pull, int_edge) {}
    virtual ~GPIO() {}

    using Engine::get;
    using Engine::set;
    using Engine::clear;
    using Engine::direction;
    using Engine::pull;
    using Engine::int_enable;
    using Engine::int_disable;

private:
    using Engine::init;
};

__END_SYS

#endif
