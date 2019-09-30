// EPOS Trustful SpaceTime Protocol Router Declarations

#ifndef __tstp_router_h
#define __tstp_router_h

#include <smartdata.h>

#ifdef __tstp__

__BEGIN_SYS

class TSTP::Router: private SmartData, private Data_Observer<Buffer>
{
    friend class TSTP;

private:
    static const bool forwarder = true;
    static const bool drop_expired = true;

    static const unsigned int RANGE = Traits<TSTP>::RADIO_RANGE;

public:
    Router();
    ~Router();

    bool synchronized();

    static Region destination(Buffer * buf);

private:
    void update(Data_Observed<Buffer> * obs, Buffer * buf);

    bool forward(Buffer * buf) {
        // Not a forwarder
        if(!forwarder)
            return false;

        if(buf->my_distance >= buf->sender_distance) {
            if(!buf->destined_to_me) { // Message comes from node closer to the destination
                return false;
            } else {
                if(buf->frame()->data<Header>()->type() == INTEREST) {
                    return false;
                }
            }
        }

        // Do not forward messages that come from too far away, to avoid radio range asymmetry
        Space::Distance d = here() - buf->frame()->data<Header>()->last_hop();
        if(d > RANGE)
            return false;

        Time expiry = buf->deadline;

        if(expiry == Metadata::INFINITE) // Message will not expire
            return true;
        else if (expiry <= now()) // Expired message
            return !drop_expired;

        Time best_case_delivery_time = (buf->my_distance + RANGE - 1) / RANGE * buf->period;
        Time relative_expiry = expiry - now();

        // Message will expire soon
        if(best_case_delivery_time > relative_expiry)
            return false;

        buf->deadline -= best_case_delivery_time;

        return true;
    }

    // Apply distance routing metric
    static void offset(Buffer * buf) {
        if(buf->is_new)
            buf->offset *= 1 + (buf->my_distance % RANGE);
        else
            // forward() guarantees that my_distance < sender_distance
            buf->offset *= RANGE + buf->my_distance - buf->sender_distance;
        buf->offset /= RANGE;
    }

    static void marshal(Buffer * buf);
};

__END_SYS

#endif

#endif
