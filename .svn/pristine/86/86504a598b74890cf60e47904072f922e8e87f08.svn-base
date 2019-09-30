// EPOS PC Scratchpad Memory Mediator Declarations

#ifndef __pc_scratchpad_h
#define __pc_scratchpad_h

#define __common_only__
#include <machine/scratchpad.h>
#undef __common_only__

__BEGIN_SYS

class Scratchpad: public Scratchpad_Base
{
    friend class Machine;

private:
    static const unsigned int ADDRESS = Traits<Scratchpad>::ADDRESS;
    static const unsigned int SIZE = Traits<Scratchpad>::SIZE;

public:
    Scratchpad() {}

private:
    static void init();
};

__END_SYS

#endif
