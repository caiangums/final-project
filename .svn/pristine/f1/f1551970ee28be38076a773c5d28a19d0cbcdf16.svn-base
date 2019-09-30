// EPOS I2C Mediator Common package

#ifndef __i2c_h
#define __i2c_h

#include <system/config.h>

__BEGIN_SYS

class I2C_Common
{
protected:
    I2C_Common() {}

public:
    enum Role {
        MASTER,
        SLAVE,
    };
};

__END_SYS

#endif

#if defined(__I2C_H) && !defined(__common_only__)
#include __I2C_H
#endif
