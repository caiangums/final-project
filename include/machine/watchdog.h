// EPOS Watchdog Mediator Common Package

#ifndef __watchdog_h
#define __watchdog_h

#include <utility/handler.h>
#include <machine/machine.h>

__BEGIN_SYS

class Watchdog_Common
{
protected:
    Watchdog_Common() {}

public:
    static void enable();
    static void disable();
    static void kick();
};

__END_SYS

#endif

#if defined(__WATCHDOG_H) && !defined(__common_only__)
#include __WATCHDOG_H
#endif
