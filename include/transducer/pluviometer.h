// EPOS Pluviometer Smart Transducer Declarations

#ifndef __pluviometer_h
#define __pluviometer_h

#include <machine/gpio.h>
#include <transducer.h>

__BEGIN_SYS

class Pluviometer: public Transducer<SmartData::Unit::I32 | SmartData::Unit::Length>, private GPIO::Observer
{
    friend Responsive_SmartData<Pluviometer>;

private:
    static const GPIO::Port GPIO_PORT = GPIO::B;
    static const GPIO::Pin GPIO_PIN = 4;
    static const TSC::Time_Stamp INTERRUPT_DEBOUNCE_TIME = 100000 * (TSC::FREQUENCY / 1000000); // 100 ms

public:
    static const unsigned int DEVS = 1;

    static const int ERROR = UNKNOWN;
    static const bool active = true;

    using Observer = Transducer::Observer;

public:
    Pluviometer(const Device_Id & dev = 0): _pulses(GPIO_PORT, GPIO_PIN, GPIO::IN, GPIO::UP, GPIO::FALLING) { assert(dev < DEVS); }

    virtual Value sense() { Value tmp = _pulse_count; _pulse_count = 0; return tmp; }

private:
    template<typename SD>
    void sense(SD * sd) { sd->_value = sense(); }

private:
    void update(GPIO::Observed * o) {
        TSC::Time_Stamp now = TSC::time_stamp();
        if((now - _last_interrupt) < INTERRUPT_DEBOUNCE_TIME)
            return;
        _last_interrupt = now;
        _pulse_count++;
        notify();
    }

private:
    GPIO _pulses;
    volatile unsigned int _pulse_count;
    volatile TSC::Time_Stamp _last_interrupt;
};

__END_SYS

#endif
