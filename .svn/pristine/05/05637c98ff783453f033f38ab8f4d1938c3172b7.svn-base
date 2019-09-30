// EPOS Trustful Space-Time Protocol Locator Implementation

#include <machine/nic.h>
#include <network/tstp/tstp.h>

#ifdef __tstp__

__BEGIN_SYS

// Class attributes
TSTP::Global_Space TSTP::Locator::_absolute_location;
TSTP::Locator::Engine TSTP::Locator::_engine;

// Methods
TSTP::Locator::~Locator()
{
    db<TSTP>(TRC) << "TSTP::~Locator()" << endl;

    detach(this);
}

void TSTP::Locator::update(Data_Observed<Buffer> * obs, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Locator::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    if(buf->is_microframe) {
        buf->sender_distance = buf->hint; // This would fit better in the Router, but Timekeeper uses this info
        if(!_engine.synchronized())
            buf->relevant = true;
        else if(!buf->downlink)
            buf->my_distance = here() - TSTP::sink();
    } else {
        Global_Space dst = Space(Router::destination(buf).center);
        buf->sender_distance = buf->frame()->data<Header>()->last_hop() - dst;
        Header * h = buf->frame()->data<Header>();
        _engine.learn(h->last_hop(), h->location_confidence(), buf->rssi);

        buf->my_distance = here() - dst;
        buf->downlink = dst != TSTP::sink(); // This would fit better in the Router, but Timekeeper uses this info

        // Respond to Keep Alive if sender is low on location confidence
        if(_engine.synchronized()) {
            Header * header = buf->frame()->data<Header>();
            if(header->type() == CONTROL) {
                Control * control = buf->frame()->data<Control>();
                if((control->subtype() == KEEP_ALIVE) && !_engine.neigbor_synchronized(header->location_confidence()))
                    Timekeeper::keep_alive();
            }
        }
    }
}


// Class Methods
void TSTP::Locator::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Locator::marshal(buf=" << buf << ")" << endl;

    Space dst = Space(Router::destination(buf).center);
    buf->my_distance = here() - dst;
    if(buf->is_new)
        buf->sender_distance = buf->my_distance;
    buf->downlink = dst != sink(); // This would fit better in the Router, but Timekeeper uses this info
    buf->frame()->data<Header>()->location_confidence(Locator::confidence());
    buf->frame()->data<Header>()->origin(here());
    buf->frame()->data<Header>()->last_hop(here());
}

__END_SYS

#endif
