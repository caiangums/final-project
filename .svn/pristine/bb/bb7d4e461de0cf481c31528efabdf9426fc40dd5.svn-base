// EPOS Smart Transducer Declarations

#ifndef __tranducer_h
#define __tranducer_h

#include <machine.h>
#include <smartdata.h>

__BEGIN_SYS

template<unsigned long _UNIT>
class Transducer: public SmartData, public Observed
{
public:
    static const unsigned int UNIT = _UNIT;

    enum : unsigned int { SENSOR = 1 << 0, ACTUATOR = 1 << 1 };
    typedef unsigned int Type;
    static const Type TYPE = SENSOR | ACTUATOR;

    typedef typename Unit::Get<UNIT>::Type Value;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

protected:
    Transducer() {}

public:
    virtual ~Transducer() {}

    virtual Value sense() = 0;
    virtual void actuate(const Value & value) {}

    Power_Mode power() { return Power_Mode::FULL; }
    void power(const Power_Mode & mode) {}
};

class Dummy_Transducer: public Transducer<SmartData::Unit::Antigravity>
{
    friend Responsive_SmartData<Dummy_Transducer>;

public:
    static const bool active = false;
    static const Error ERROR = 0;
    static const Type TYPE = SENSOR | ACTUATOR;

public:
    Dummy_Transducer(const Device_Id & dev): _value(0) {}

    virtual Value sense() { return _value++; }
    virtual void actuate(const Value & value) { _value = value; }

private:
    Value _value;
};

typedef Responsive_SmartData<Dummy_Transducer> Antigravity;
typedef Interested_SmartData<Dummy_Transducer::Unit::Wrap<Dummy_Transducer::UNIT>> Antigravity_Proxy;

__END_SYS

#ifdef __CO2_SENSOR_H
#include __CO2_SENSOR_H
__IN_SYS(typedef Responsive_SmartData<CO2_Sensor> CO2_Concentration;)
__IN_SYS(typedef Interested_SmartData<CO2_Sensor::Unit::Wrap<CO2_Sensor::UNIT>> CO2_Concentration_Proxy;)
#endif

#ifdef __PLUVIOMETER_H
#include __PLUVIOMETER_H
__IN_SYS(typedef Responsive_SmartData<Pluviometer> Precipitation;)
__IN_SYS(typedef Interested_SmartData<Pluviometer::Unit::Wrap<Pluviometer::UNIT>> Precipitation_Proxy;)
#endif

#ifdef __PRESSURE_SENSOR_H
#include __PRESSURE_SENSOR_H
__IN_SYS(typedef Responsive_SmartData<Pressure_Sensor> Atmospheric_Pressure;)
__IN_SYS(typedef Interested_SmartData<Pressure_Sensor::Unit::Wrap<Pressure_Sensor::UNIT>> Atmospheric_Pressure_Proxy;)
#endif

#ifdef __KEYPAD_H
#include __KEYPAD_H
__IN_SYS(typedef Responsive_SmartData<Keypad> Smart_Key;)
__IN_SYS(typedef Interested_SmartData<Keypad::Unit::Wrap<Keypad::UNIT>> Smart_Key_Proxy;)
#endif

#endif
