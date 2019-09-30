// EPOS Pressure Smart Transducer Declarations

#ifndef __pressure_sensor_h
#define __pressure_sensor_h

#include <machine/gpio.h>
#include <machine/adc.h>
#include <transducer.h>

__BEGIN_SYS

// Keller Capacitive Transmitters Series 46 X / 46 X Ei (http://www.keller-druck.com/home_e/paprod_e/46x_e.asp)
class Keller_46X: public Transducer<SmartData::Unit::I32 | SmartData::Unit::Length>
{
private:
    static const GPIO::Port GPIO_PORT = GPIO::B;
    static const GPIO::Pin GPIO_PIN = 0;
    static const ADC::Channel ADC_CHANNEL = ADC::SINGLE_ENDED_ADC2;

public:
    static const unsigned int DEVS = 1;

    static const int ERROR = UNKNOWN;
    static const bool active = false;

public:
    Keller_46X(unsigned int dev = 0): _relay(GPIO_PORT, GPIO_PIN, GPIO::OUT), _adc(ADC_CHANNEL) {
        assert(dev < DEVS);
        _relay.clear();
    }

    Value sense() {
        _relay.set();
        Machine::delay(2000000);
        Value tmp = _adc.read();
        _relay.clear();
        return tmp;
    }

    template<typename SD>
    void sense(unsigned int dev, SD * sd) { sd->_value = sense(); }

private:
    GPIO _relay;
    ADC _adc;
};

typedef Keller_46X Pressure_Sensor;


__END_SYS

#endif
