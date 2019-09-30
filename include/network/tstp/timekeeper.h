// EPOS Trustful SpaceTime Protocol Timekeeper Declarations

#ifndef __tstp_timekeeper_h
#define __tstp_timekeeper_h

#include <machine/nic.h>
#include <smartdata.h>

#ifdef __tstp__

__BEGIN_SYS

class TSTP::Timekeeper: private SmartData, private Data_Observer<Buffer>
{
    friend class TSTP;

private:
    static const unsigned int MAX_DRIFT = 500000; // us
#if defined(__ieee802_15_4_H) && !defined(__common_only__)
    static const unsigned int NIC_TIMER_INTERRUPT_DELAY = IEEE802_15_4::SHR_SIZE * 1000000 / IEEE802_15_4::BYTE_RATE; // us FIXME: this has to come from NIC
#else
    static const unsigned int NIC_TIMER_INTERRUPT_DELAY = 0; // us
#endif

    typedef _NIC::Timer::Time_Stamp Time_Stamp;
    typedef _NIC::Timer::Offset Offset;

public:
    Timekeeper();
    ~Timekeeper();

    static Time_Stamp absolute(const Offset & offset); //{ return _location + coordinates; }
    static Offset relative(const Time_Stamp & ts);// { return coordinates - _location; }

    static Time now() { return _NIC::Timer::count2us(time_stamp()); }
    static bool synchronized() { return !sync_required(); }

private:
    void update(Data_Observed<Buffer> * obs, Buffer * buf);

    static void marshal(Buffer * buf);

    static Time_Stamp time_stamp() { return _NIC::Timer::read(); }
    static Time_Stamp sync_period() { return (long long int)(MAX_DRIFT * 1000000) / (_NIC::Timer::frequency() * _NIC::Timer::accuracy() / 1000000000LL); } // us
    static bool sync_required() { return (_next_sync == 0) || (time_stamp() >= (_next_sync - sync_period() / 2)); }
    static void keep_alive();

private:
    static volatile Time_Stamp _next_sync;
    static Function_Handler _life_keeper_handler;
    static Alarm * _life_keeper;
};

__END_SYS

#endif

#endif
