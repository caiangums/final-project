// EPOS Clerk Implementation

#include <clerk.h>

__BEGIN_SYS

// Clerk
#if defined(__PMU_H) && !defined(__common_only__)

bool Clerk<PMU>::_in_use[Traits<Build>::CPUS][CHANNELS];

#endif

// System_Monitor
Simple_List<Monitor> Monitor::_monitors[Traits<Build>::CPUS];

void Monitor::run()
{
    db<Monitor>(TRC) << "Monitor::run()" << endl;

    Simple_List<Monitor> * monitor = &_monitors[CPU::id()];
    for(List::Iterator it = monitor->begin(); it != monitor->end(); it++)
        it->object()->capture();
}

void Monitor::init()
{
//    if(Traits<Monitor>::CPU_MONITOR_TEMPERATURE) {
//        Clerk<Termometer> * temperature_clerk = new Clerk<Termometer>(TEMPERATURE);
//        _monitors.insert(new Clerk_Monitor<temperature_clerk, Traits<Monitor>::CPU_MONITOR_TEMPERATURE>);
//    }

    db<Monitor>(TRC) << "Monitor::init()" << endl;

    if(Traits<Monitor>::MONITOR_ELAPSED_TIME && (CPU::id() == 0)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring ELAPSED TIME at " << Traits<Monitor>::MONITOR_ELAPSED_TIME << " Hz" << endl;
        new Clerk<System>(ELAPSED_TIME, Traits<Monitor>::MONITOR_ELAPSED_TIME, true);
    }

    if(Traits<Monitor>::MONITOR_DEADLINE_MISS) {
        db<Monitor>(TRC) << "Monitor::init: monitoring DEADLINE MISS at " << Traits<Monitor>::MONITOR_DEADLINE_MISS << " Hz" << endl;
        new Clerk<System>(DEADLINE_MISS, Traits<Monitor>::MONITOR_DEADLINE_MISS, true);
    }

#if defined(__PAMU_H) && !defined(__common_only__)

    unsigned int used_channels = 0;

    if(Traits<Monitor>::MONITOR_CLOCK && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring CLOCK at " << Traits<Monitor>::MONITOR_CLOCK << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(CLOCK);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_CLOCK> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_CLOCK>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_DVS_CLOCK && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring DVS CLOCK at " << Traits<Monitor>::MONITOR_DVS_CLOCK << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(DVS_CLOCK);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_DVS_CLOCK> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_DVS_CLOCK>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_INSTRUCTION && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring INSTRUCTION at " << Traits<Monitor>::MONITOR_INSTRUCTION << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(INSTRUCTION);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_INSTRUCTION> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_INSTRUCTION>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_BRANCH && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring BRANCH at " << Traits<Monitor>::MONITOR_BRANCH << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(BRANCH);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_BRANCH> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_BRANCH>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_BRANCH_MISS && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring BRANCH MISS at " << Traits<Monitor>::MONITOR_BRANCH_MISS << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(BRANCH_MISS);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_BRANCH_MISS> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_BRANCH_MISS>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_L1_HIT && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring L1 HIT at " << Traits<Monitor>::MONITOR_L1_HIT << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(L1_HIT);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L1_HIT> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L1_HIT>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_L2_HIT && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring L2 HIT at " << Traits<Monitor>::MONITOR_L2_HIT << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(L2_HIT);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L2_HIT> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L2_HIT>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_L3_HIT && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring L3 HIT at " << Traits<Monitor>::MONITOR_L3_HIT << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(L3_HIT);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L3_HIT> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L3_HIT>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_LLC_HIT && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring LLC HIT at " << Traits<Monitor>::MONITOR_LLC_HIT << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(LLC_HIT);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_LLC_HIT> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_LLC_HIT>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_CACHE_HIT && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring CACHE HIT at " << Traits<Monitor>::MONITOR_CACHE_HIT << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(CACHE_HIT);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_CACHE_HIT> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_CACHE_HIT>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_L1_MISS && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring L1 MISS at " << Traits<Monitor>::MONITOR_L1_MISS << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(L1_MISS);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L1_MISS> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L1_MISS>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_L2_MISS && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring L2 MISS at " << Traits<Monitor>::MONITOR_L2_MISS << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(L2_MISS);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L2_MISS> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L2_MISS>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_L3_MISS && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring L3 MISS at " << Traits<Monitor>::MONITOR_L3_MISS << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(L3_MISS);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L3_MISS> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_L3_MISS>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_LLC_MISS && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring LLC MISS at " << Traits<Monitor>::MONITOR_LLC_MISS << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(LLC_MISS);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_LLC_MISS> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_LLC_MISS>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_CACHE_MISS && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring CACHE MISS at " << Traits<Monitor>::MONITOR_CACHE_MISS << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(CACHE_MISS);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_CACHE_MISS> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_CACHE_MISS>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(Traits<Monitor>::MONITOR_LLC_HITM && (used_channels < Clerk<PMU>::CHANNELS)) {
        db<Monitor>(TRC) << "Monitor::init: monitoring LLC HITM at " << Traits<Monitor>::MONITOR_LLC_HITM << " Hz" << endl;
        Clerk<PMU> * clerk = new Clerk<PMU>(LLC_HITM);
        Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_LLC_HITM> * monitor = new Clerk_Monitor<Clerk<PMU>, Traits<Monitor>::MONITOR_LLC_HITM>(clerk);
        _monitors[CPU::id()].insert(&monitor->_link);
        used_channels++;
    }

    if(used_channels >= Clerk<PMU>::CHANNELS)
        db<Monitor>(WRN) << "Monitor::init: some events not monitored because all PMU channels are busy!" << endl;

#endif

}

__END_SYS
