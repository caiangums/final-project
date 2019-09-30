// EPOS Analog to Digital Converter (ADC) Mediator Common Package

#ifndef __adc_h
#define __adc_h

#include <system/config.h>

__BEGIN_SYS

class ADC_Common
{
protected:
    ADC_Common() {}

public:
    template <typename T>
    unsigned short read();
};

__END_SYS

#endif

#if defined(__ADC_H) && !defined(__common_only__)
#include __ADC_H
#endif
