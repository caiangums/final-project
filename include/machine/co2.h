// EPOS CO2 Sensor Mediator Common package

#ifndef __co2_h
#define __co2_h

#include <machine/transducer.h>

__BEGIN_SYS

class CO2_Common
{
protected:
    CO2_Common() {}
};

__END_SYS

#if defined(__CO2_H) && defined(__CO2_CM1101)

#include <machine/uart.h>
#include <machine/common/cm1101.h>
typedef CM1101 CO2;

#endif

#endif
