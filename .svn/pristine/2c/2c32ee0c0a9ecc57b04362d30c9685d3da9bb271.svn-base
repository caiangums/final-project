// EPOS Trustful Space-Time Protocol Implementation

#include <utility/math.h>
#include <utility/string.h>
#include <machine/nic.h>
#include <network/tstp/tstp.h>

#ifdef __tstp__

__BEGIN_SYS

// Class attributes
TSTP::Router * TSTP::_router;
TSTP::Locator * TSTP::_locator;
TSTP::Timekeeper * TSTP::_timekeeper;
TSTP::Security * TSTP::_security;

TSTP::Time TSTP::_epoch;

TSTP::_NIC * TSTP::_nic;
Data_Observed<TSTP::Buffer> TSTP::_parts;
Data_Observed<TSTP::Buffer, TSTP::Unit> TSTP::_clients;


// Methods
TSTP::~TSTP()
{
    db<TSTP>(TRC) << "TSTP::~TSTP()" << endl;
    _nic->detach(this, 0);
}

void TSTP::update(NIC_Family::Observed * obs, const Protocol & prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::update(nic=" << obs << ",prot=" << hex << prot << ",buf=" << buf << ")" << endl;

    Packet * packet = buf->frame()->data<Packet>();
    db<TSTP>(TRC) << "TSTP::update:packet=" << *packet << endl;

    _parts.notify(buf);
    _clients.notify(packet->header()->unit(), buf);
//    if(buf->is_microframe || !buf->trusted)
//        return;
//
//    Packet * packet = buf->frame()->data<Packet>();
//
//    bool is_model = (packet->type() == CONTROL && buf->frame()->data<Control>()->subtype() == MODEL);
//
//    if(is_model && buf->frame()->data<Control>()->origin() != here()) {
//        Model * model = reinterpret_cast<Model *>(packet);
//        if(model->time() < now()) {
//            // Check region inclusion and notify interested observers
//            Interests::List * list = _interested[model->unit()];
//            if(list && !list->empty()) {
//                for(Interests::Element * el = list->head(); el; el = el->next()) {
//                    Interested * interested = el->object();
//                    if(interested->region().contains(model->origin(), model->time()))
//                        notify(interested, buf);
//                }
//            } else {
//                Responsives::List * list_responsive = _responsives[model->unit()];
//                if(list_responsive && !list_responsive->empty()) {
//                    for(Responsives::Element * el = list_responsive->head(); el; el = el->next()) {
//                        Responsive * responsive = el->object();
//                        if(responsive->interest() && responsive->model_listener() && responsive->interest()->region().contains(model->origin(), model->time())){
//                            notify(responsive, buf);
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    if(packet->time() > now() || !buf->destined_to_me)
//        return;
//
//    switch(packet->type()) {
//    case INTEREST: {
//        Interest * interest = reinterpret_cast<Interest *>(packet);
//        db<TSTP>(INF) << "TSTP::update:interest=" << interest << " => " << *interest << endl;
//        // Check for local capability to respond and notify interested observers
//        Responsives::List * list = _responsives[interest->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
//        if(list)
//            for(Responsives::Element * el = list->head(); el; el = el->next()) {
//                Responsive * responsive = el->object();
//                if((now() < interest->region().t1) && interest->region().contains(responsive->origin(), interest->region().t1))
//                    notify(responsive, buf);
//            }
//    } break;
//    case RESPONSE: {
//        Response * response = reinterpret_cast<Response *>(packet);
//        db<TSTP>(INF) << "TSTP::update:response=" << response << " => " << *response << endl;
//        if(response->time() < now()) {
//            // Check region inclusion and notify interested observers
//            Interests::List * list = _interested[response->unit()];
//            if(list)
//                for(Interests::Element * el = list->head(); el; el = el->next()) {
//                    Interested * interested = el->object();
//                    if(interested->region().contains(response->origin(), response->time()))
//                        notify(interested, buf);
//                }
//        }
//    } break;
//    case COMMAND: {
//        Command * command = reinterpret_cast<Command *>(packet);
//        db<TSTP>(INF) << "TSTP::update:command=" << command << " => " << *command << endl;
//        // Check for local capability to respond and notify interested observers
//        Responsives::List * list = _responsives[command->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
//        if(list)
//            for(Responsives::Element * el = list->head(); el; el = el->next()) {
//                Responsive * responsive = el->object();
//                if(command->region().contains(responsive->origin(), now()))
//                    notify(responsive, buf);
//            }
//    } break;
//    case CONTROL: {
//        switch(buf->frame()->data<Control>()->subtype()) {
//            case DH_REQUEST:
//                db<TSTP>(INF) << "TSTP::update: DH_Request: " << *buf->frame()->data<DH_Request>() << endl;
//                break;
//            case DH_RESPONSE:
//                db<TSTP>(INF) << "TSTP::update: DH_Response: " << *buf->frame()->data<DH_Response>() << endl;
//                break;
//            case AUTH_REQUEST:
//                db<TSTP>(INF) << "TSTP::update: Auth_Request: " << *buf->frame()->data<Auth_Request>() << endl;
//                break;
//            case AUTH_GRANTED:
//                db<TSTP>(INF) << "TSTP::update: Auth_Granted: " << *buf->frame()->data<Auth_Granted>() << endl;
//                break;
//            case REPORT: {
//                db<TSTP>(INF) << "TSTP::update: Report: " << *buf->frame()->data<Report>() << endl;
//                Report * report = reinterpret_cast<Report *>(packet);
//                if(report->time() < now()) {
//                    // Check region inclusion and advertise interested observers
//                    Interests::List * list = _interested[report->unit()];
//                    if(list)
//                        for(Interests::Element * el = list->head(); el; el = el->next()) {
//                            Interested * interested = el->object();
//                            if(interested->region().contains(report->origin(), report->time()))
//                                interested->advertise();
//                        }
//                    if(report->epoch_request() && (here() == sink())) {
//                        db<TSTP>(TRC) << "TSTP::update: responding to Epoch request" << endl;
//                        Buffer * buf = alloc(sizeof(Epoch));
//                        Epoch * epoch = new (buf->frame()->data<Epoch>()) Epoch(Region(report->origin(), 0, now(), now() + 10000000)); // TODO: what's the expiry?
//                        marshal(buf);
//                        db<TSTP>(INF) << "TSTP::update:epoch = " << epoch << " => " << (*epoch) << endl;
//                        _nic->send(buf);
//                    }
//                }
//            } break;
//            case KEEP_ALIVE:
//                db<TSTP>(INF) << "TSTP::update: Keep_Alive: " << *buf->frame()->data<Keep_Alive>() << endl;
//                break;
//            case EPOCH: {
//                db<TSTP>(INF) << "TSTP::update: Epoch: " << *buf->frame()->data<Epoch>() << endl;
//                Epoch * epoch = reinterpret_cast<Epoch *>(packet);
//                if(here() != sink()) {
//                    _global_coordinates = epoch->coordinates();
//                    _epoch = epoch->epoch();
//                    db<TSTP>(INF) << "TSTP::update: Epoch: adjusted epoch Space-Time to: " << _global_coordinates << ", " << _epoch << endl;
//                }
//            } break;
//            default:
//                db<TSTP>(WRN) << "TSTP::update: Unrecognized Control subtype: " << buf->frame()->data<Control>()->subtype() << endl;
//                break;
//        }
//    } break;
//    case MODEL: {
//        // Already treated
//    } break;
//    default:
//        db<TSTP>(WRN) << "TSTP::update: Unrecognized packet type: " << packet->type() << endl;
//        break;
//    }
}

// Class methods
void TSTP::marshal(Buffer * buf) {
    Locator::marshal(buf);
    Timekeeper::marshal(buf);
    Router::marshal(buf);
    Security::marshal(buf);

    Packet * packet = buf->frame()->data<Packet>();
    db<TSTP>(TRC) << "TSTP::marshal:packet=" << *packet << endl;
}

__END_SYS

#endif
