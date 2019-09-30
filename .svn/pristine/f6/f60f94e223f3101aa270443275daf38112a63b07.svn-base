// EPOS Trustful SpaceTime Protocol Locator Declarations

#ifndef __tstp_locator_h
#define __tstp_locator_h

#include <network/hecops.h>
#include <smartdata.h>

#ifdef __tstp__

__BEGIN_SYS

class TSTP::Locator: private SmartData, private Data_Observer<Buffer>
{
    friend class TSTP;

private:
    typedef HeCoPS<Space, 3> Engine;

public:
    Locator();
    ~Locator();

    static const Space & here() { return _engine.here(); }
    static const Percent & confidence() { return _engine.confidence(); }

    static Global_Space absolute(const Global_Space & gs) { return _absolute_location + gs; }
    static Global_Space relative(const Global_Space & gs) { return gs - _absolute_location; }

private:
    void update(Data_Observed<Buffer> * obs, Buffer * buf);

    static void marshal(Buffer * buf);

private:
    static Global_Space _absolute_location;
    static Engine _engine;
};

__END_SYS

#endif

#endif
