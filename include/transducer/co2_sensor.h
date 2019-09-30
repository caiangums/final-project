// EPOS CO2 Smart Transducer Declarations

#ifndef __co2_sensor_h
#define __co2_sensor_h

#include <machine/uart.h>
#include <machine/common/cm1101.h>
#include <transducer.h>

__BEGIN_SYS

// Cubic Nondispersive Infrared (NDIR) Carbon Dioxide Single Beam Sensor CM1101
class CO2_Sensor: public Transducer<SmartData::Unit::I32 | SmartData::Unit::PPM>, private CM1101
{
private:
    static const unsigned int UART_UNIT = 0;
    static const unsigned int UART_BAUD_RATE = 9600;
    static const unsigned int UART_DATA_BITS = 8;
    static const unsigned int UART_PARITY = 0;
    static const unsigned int UART_STOP_BITS = UART_Common::NONE;

public:
    static const unsigned int DEVS = 1;

    static const int ERROR = 50; // PPM
    static const bool active = false;

public:
    CO2_Sensor(unsigned int dev = 0): CM1101(&_uart), _uart(UART_UNIT, UART_BAUD_RATE, UART_DATA_BITS, UART_PARITY, UART_STOP_BITS) { assert(dev < DEVS); }

    Value sense() { return co2(); }

    template<typename SD>
    void sense(unsigned int dev, SD * sd) {
        sd->_value = co2();
    }

private:
    UART _uart;
};

//template<>
//void CO2_Sensor::sense<SmartData<SmartData::Unit::Temperature>>(unsigned int dev, SmartData<Unit::I32 | Unit::Temperature> * sd) {
//    sd->_value = temperature();
//}
//void sense(unsigned int dev, SmartData<Unit::I32 | Unit::Ratio> * sd) {
//    sd->_value = humidity();
//}

__END_SYS

#endif
