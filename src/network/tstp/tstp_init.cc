// EPOS Trustful SpaceTime Protocol Initialization

#include <machine/nic.h>
#include <network/tstp/tstp.h>
#include <system.h>
#include <time.h>

#ifdef __tstp__

__BEGIN_SYS

TSTP::Locator::Locator()
{
    db<TSTP>(TRC) << "TSTP::Locator()" << endl;

    System_Info::Boot_Map * bm = &System::info()->bm;
    if(bm->space_x != Space::UNKNOWN) {
        _engine.here(Space(bm->space_x, bm->space_y, bm->space_z));
        _engine.confidence(100);
    } else {
        _engine.here(Space(Space::UNKNOWN, Space::UNKNOWN, Space::UNKNOWN));
        _engine.confidence(0);
    }
    attach(this);

    db<TSTP>(INF) << "TSTP::Locator::here=" << here() << endl;

    // Wait for spatial localization
    while(confidence() < 80)
        Thread::self()->yield();

    // _absolute_location is initialized later through an Epoch message
}

TSTP::Timekeeper::Timekeeper()
{
    db<TSTP>(TRC) << "TSTP::Timekeeper()" << endl;
    db<TSTP>(INF) << "TSTP::Timekeeper: timer frequency = " << _NIC::Timer::frequency() << " Hz" << endl;
    db<TSTP>(INF) << "TSTP::Timekeeper: timer accuracy = " << _NIC::Timer::accuracy() << " ppb" << endl;
    db<TSTP>(INF) << "TSTP::Timekeeper: maximum drift = " << MAX_DRIFT << " us" << endl;
    db<TSTP>(INF) << "TSTP::Timekeeper: sync period = " << sync_period() << " us" << endl;

    if(here() == sink())
        _next_sync = -1ull; // Just so that the sink will always have synchronized() return true

    attach(this);

    if(here() != sink()) {
        keep_alive();
        _life_keeper = new (SYSTEM) Alarm(sync_period(), &_life_keeper_handler, Alarm::INFINITE);

        // Wait for time synchronization
        while(sync_required())
            Thread::self()->yield();
    }
}

TSTP::Router::Router()
{
    db<TSTP>(TRC) << "TSTP::Router()" << endl;

    attach(this);
}

TSTP::Security::Security()
{
    db<TSTP>(TRC) << "TSTP::Security()" << endl;

    new (&_id) Node_Id(Machine::uuid(), sizeof(UUID));

    db<TSTP>(INF) << "Node ID: " << _id << endl;

    assert(AES::KEY_SIZE == sizeof(Node_Id));
    _aes.encrypt(_id, _id, _auth);

    attach(this);

    /*
    // Guto: what is this?
    if((TSTP::here() != TSTP::sink()) && (!Traits<Radio>::promiscuous)) {
        Peer * peer = new (SYSTEM) Peer(_id, Region(TSTP::sink(), 0, 0, -1));
        _pending_peers.insert(peer->link());

        // Wait for key establishment
        while(_trusted_peers.size() == 0)
            Thread::self()->yield();
    }
    */
}


template<unsigned int UNIT>
TSTP::TSTP(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::TSTP(u=" << unit << ")" << endl;

    _nic = Traits<Traits<TSTP>::NIC_Family>::DEVICES::Get<UNIT>::Result::get(unit);
    _nic->attach(this, PROTO_TSTP);

    _locator = new (SYSTEM) Locator;
    _timekeeper = new (SYSTEM) Timekeeper;
    _router = new (SYSTEM) Router;
//    _security = new (SYSTEM) Security;
}


void TSTP::init(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::init(u=" << unit << ")" << endl;

    new (SYSTEM) TSTP(unit);
}

__END_SYS

#endif
