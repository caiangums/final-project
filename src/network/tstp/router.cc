// EPOS Trustful Space-Time Protocol Router Implementation

#include <utility/math.h>
#include <utility/string.h>
#include <machine/nic.h>
#include <network/tstp/tstp.h>

#ifdef __tstp__

__BEGIN_SYS

// Class attributes

// Methods
TSTP::Router::~Router()
{
    db<TSTP>(TRC) << "TSTP::~Router()" << endl;
    detach(this);
}

void TSTP::Router::update(Data_Observed<Buffer> * obs, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Router::update(obs=" << obs << ",buf=" << buf << ")" << endl;
    if(buf->is_microframe) {
        if(!buf->relevant)
            buf->relevant = forwarder && (buf->my_distance < buf->sender_distance);
    } else {
        Header * header = buf->frame()->data<Header>();
        // Keep Alive messages are never forwarded
        if((header->type() == CONTROL) && (buf->frame()->data<Control>()->subtype() == KEEP_ALIVE))
            buf->destined_to_me = false;
        else {
            Region dst = destination(buf);
            buf->destined_to_me = ((header->origin() != TSTP::here()) && (dst.contains(TSTP::here(), dst.t0)));

            if(forward(buf)) {

                // Forward or ACK the message

                Buffer * send_buf = TSTP::alloc(buf->size());

                // Copy frame contents
                memcpy(send_buf->frame(), buf->frame(), buf->size());

                // Copy Buffer Metainformation
                send_buf->size(buf->size());
                send_buf->id = buf->id;
                send_buf->destined_to_me = buf->destined_to_me;
                send_buf->downlink = buf->downlink;
                send_buf->deadline = buf->deadline;
                send_buf->my_distance = buf->my_distance;
                send_buf->sender_distance = buf->sender_distance;
                send_buf->is_new = false;
                send_buf->is_microframe = false;
                send_buf->random_backoff_exponent = 0;

                // Calculate offset
                offset(send_buf);

                // Adjust Last Hop location
                Header * header = send_buf->frame()->data<Header>();
                header->last_hop(TSTP::here());
                send_buf->sender_distance = send_buf->my_distance;

                header->location_confidence(_locator->confidence());
                header->time_request(TSTP::Timekeeper::sync_required());

                send_buf->hint = send_buf->my_distance;

                TSTP::_nic->send(send_buf);
            }
        }
    }
}


// Class Methods
void TSTP::Router::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Router::marshal(buf=" << buf << ")" << endl;
    TSTP::Region dest = destination(buf);
    buf->downlink = dest.center != TSTP::sink();
    buf->destined_to_me = (buf->frame()->data<Header>()->origin() != TSTP::here()) && (dest.contains(TSTP::here(), TSTP::now()));
    buf->hint = buf->my_distance;

    offset(buf);
}


TSTP::Region TSTP::Router::destination(Buffer * buf)
{
    switch(buf->frame()->data<Header>()->type()) {
        case INTEREST:
            return buf->frame()->data<Interest>()->region();
        case RESPONSE:
            return Region(sink(), 0, buf->frame()->data<Response>()->origin(), buf->frame()->data<Response>()->expiry());
        case COMMAND:
            return buf->frame()->data<Command>()->region();
        case CONTROL:
            switch(buf->frame()->data<Control>()->subtype()) {
                default:
                case DH_RESPONSE:
                case AUTH_REQUEST: {
                    Time origin = buf->frame()->data<Header>()->origin();
                    Time deadline = Security::deadline(origin);
                    return Region(sink(), 0, origin, deadline);
                }
                case DH_REQUEST: {
                    Time origin = buf->frame()->data<Header>()->origin();
                    Time deadline = Security::deadline(origin);
                    return Region(buf->frame()->data<Security::DH_Request>()->destination().center, buf->frame()->data<Security::DH_Request>()->destination().radius, origin, deadline);
                }
                case AUTH_GRANTED: {
                    Time origin = buf->frame()->data<Header>()->origin();
                    Time deadline = Security::deadline(origin);
                    return Region(buf->frame()->data<Security::Auth_Granted>()->destination().center, buf->frame()->data<Security::Auth_Granted>()->destination().radius, origin, deadline);
                }
                case REPORT: {
                    return Region(sink(), 0, buf->frame()->data<Control>()->origin(), -1/*TODO*/);
                }
                case KEEP_ALIVE: {
                    while(true) {
                        Space fake(here().x + (Random::random() % (RANGE / 3)), here().y + (Random::random() % (RANGE / 3)), (here().z + Random::random() % (RANGE / 3)));
                        if(fake != here())
                            return Region(fake, 0, 0, -1); // Should never be destined_to_me
                    }
                }
                case EPOCH: {
                    return buf->frame()->data<Epoch>()->destination();
                }
//                case MODEL: {
//                    return Region(sink(), 0, buf->frame()->data<Model>()->origin(), buf->frame()->data<Model>()->expiry());
//                }
            }
        default:
            db<TSTP>(WRN) << "TSTP::Locator::destination(): invalid frame type " << buf->frame()->data<Header>()->type() << endl;
            return Region(here(), 0, now() - 2, now() - 1);
    }
}

__END_SYS

#endif
