// EPOS Trustful Space-Time Protocol Implementation

#include <utility/math.h>
#include <utility/string.h>
#include <machine/nic.h>
#include <network/tstp/tstp.h>

#ifdef __tstp__

__BEGIN_SYS

// Class attributes
volatile TSTP::Timekeeper::Time_Stamp TSTP::Timekeeper::_next_sync;
Function_Handler TSTP::Timekeeper::_life_keeper_handler(&keep_alive);
Alarm * TSTP::Timekeeper::_life_keeper;

// Methods
TSTP::Timekeeper::~Timekeeper()
{
    db<TSTP>(TRC) << "TSTP::~Timekeeper()" << endl;

    detach(this);
}

void TSTP::Timekeeper::update(Data_Observed<Buffer> * obs, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    Header * header = buf->frame()->data<Header>();

    if(buf->is_microframe) {
        if(sync_required())
            buf->relevant = true;
    } else {
        buf->deadline = Router::destination(buf).t1;
        bool peer_closer_to_sink = buf->downlink ? (here() - sink() > header->last_hop() - sink()) : (buf->my_distance > buf->sender_distance);

        if(header->time_request()) {
            db<TSTP>(TRC) << "TSTP::Timekeeper::update: time_request received" << endl;
            // Respond to Time Request if able
            if(synchronized() && !peer_closer_to_sink) {
                db<TSTP>(TRC) << "TSTP::Timekeeper::update: responding to time request" << endl;
                keep_alive();
            }
        } else {
            if(peer_closer_to_sink) {
                Time_Stamp t0 = header->last_hop().t + _NIC::Timer::us2count(NIC_TIMER_INTERRUPT_DELAY);
                Time_Stamp t1 = buf->sfd_time_stamp;

                Offset adj = t0 - t1;
                _NIC::Timer::adjust(adj);

                _next_sync = time_stamp() + sync_period();
                _life_keeper->reset();

                db<TSTP>(TRC) << "TSTP::Timekeeper::update: adjusted timer offset by " << adj << endl;
                db<TSTP>(INF) << "TSTP::Timekeeper::update:now= " << now() << endl;
            }
        }
    }
}


// Class Methods
void TSTP::Timekeeper::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::marshal(buf=" << buf << ")" << endl;

    buf->frame()->data<Header>()->origin(now());
    buf->frame()->data<Header>()->time_request(sync_required());
    if((buf->frame()->data<Header>()->type() == CONTROL) && (buf->frame()->data<Control>()->subtype() == KEEP_ALIVE))
        buf->deadline = time_stamp() + sync_period();
    else
        buf->deadline = Router::destination(buf).t1; // deadline must be set after origin time for Security messages
}


void TSTP::Timekeeper::keep_alive()
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::keep_alive()" << endl;

    Buffer * buf = alloc(sizeof(Keep_Alive));
    new (buf->frame()->data<Keep_Alive>()) Keep_Alive;
    TSTP::send(buf);
}

__END_SYS

#endif
