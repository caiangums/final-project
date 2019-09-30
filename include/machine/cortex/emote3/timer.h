// EPOS EPOSMote III (ARM Cortex-M3) MCU Timer Mediator Declarations

#ifndef __model_timer_h
#define __model_timer_h

#include <machine/timer.h>
#include <machine/cortex/engines/cortex_m3/systick.h>
#include <machine/cortex/engines/cortex_m3/gptm.h>
#include "memory_map.h"

__BEGIN_SYS

typedef SysTick<Memory_Map::SCS_BASE> System_Timer_Engine;
typedef GPTM<Memory_Map::TIMER0_BASE, 0x1000, Traits<Timer>::UNITS> User_Timer_Engine;

__END_SYS

#endif
