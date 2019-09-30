// EPOS Trustful Space-Time Protocol Implementation

#include <utility/math.h>
#include <utility/string.h>
#include <machine/nic.h>
#include <network/tstp/tstp.h>

#ifdef __tstp__

__BEGIN_SYS

// Class attributes
TSTP::Security::_AES TSTP::Security::_aes;
TSTP::Security::_AES & TSTP::Security::_cipher = TSTP::Security::_aes;
TSTP::Security::Node_Id TSTP::Security::_id;
TSTP::Security::Auth TSTP::Security::_auth;
TSTP::Security::_DH TSTP::Security::_dh;
TSTP::Security::Pending_Keys TSTP::Security::_pending_keys;
TSTP::Security::Peers TSTP::Security::_pending_peers;
TSTP::Security::Peers TSTP::Security::_trusted_peers;
volatile bool TSTP::Security::_peers_lock;
Thread * TSTP::Security::_key_manager;
unsigned int TSTP::Security::_dh_requests_open;

// Methods
TSTP::Security::~Security()
{
    db<TSTP>(TRC) << "TSTP::~Security()" << endl;
    detach(this);
    if(_key_manager)
        delete _key_manager;
    while(Peers::Element * el = _trusted_peers.remove_head())
        delete el->object();
    while(Peers::Element * el = _pending_peers.remove_head())
        delete el->object();
    while(Pending_Keys::Element * el = _pending_keys.remove_head())
        delete el->object();
}

void TSTP::Security::update(Data_Observed<Buffer> * obs, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Security::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    if(!buf->is_microframe && buf->destined_to_me) {

        switch(buf->frame()->data<Header>()->type()) {

            case CONTROL: {
                db<TSTP>(TRC) << "TSTP::Security::update(): Control message received" << endl;
                switch(buf->frame()->data<Control>()->subtype()) {

                    case DH_REQUEST: {
                        if(TSTP::here() != TSTP::sink()) {
                            DH_Request * dh_req = buf->frame()->data<DH_Request>();
                            db<TSTP>(INF) << "TSTP::Security::update(): DH_Request message received: " << *dh_req << endl;

                            //while(CPU::tsl(_peers_lock));
                            //CPU::int_disable();
                            bool valid_peer = false;
                            for(Peers::Element * el = _pending_peers.head(); el; el = el->next())
                                if(el->object()->valid_deploy(dh_req->origin(), TSTP::now())) {
                                    valid_peer = true;
                                    break;
                                }
                            if(!valid_peer)
                                for(Peers::Element * el = _trusted_peers.head(); el; el = el->next())
                                    if(el->object()->valid_deploy(dh_req->origin(), TSTP::now())) {
                                        valid_peer = true;
                                        _trusted_peers.remove(el);
                                        _pending_peers.insert(el);
                                        break;
                                    }
                            //_peers_lock = false;
                            //CPU::int_enable();

                            if(valid_peer) {
                                db<TSTP>(TRC) << "TSTP::Security::update(): Sending DH_Response" << endl;
                                // Respond to Diffie-Hellman request
                                Buffer * resp = TSTP::alloc(sizeof(DH_Response));
                                new (resp->frame()) DH_Response(_dh.public_key());
                                TSTP::marshal(resp);
                                TSTP::_nic->send(resp);

                                // Calculate Master Secret
                                Pending_Key * pk = new (SYSTEM) Pending_Key(buf->frame()->data<DH_Request>()->key());
                                Master_Secret ms = pk->master_secret();
                                //while(CPU::tsl(_peers_lock));
                                //CPU::int_disable();
                                _pending_keys.insert(pk->link());
                                //_peers_lock = false;
                                //CPU::int_enable();

                                db<TSTP>(TRC) << "TSTP::Security::update(): Sending Auth_Request" << endl;
                                // Send Authentication Request
                                resp = TSTP::alloc(sizeof(Auth_Request));
                                new (resp->frame()) Auth_Request(_auth, otp(ms, _id));
                                TSTP::marshal(resp);
                                TSTP::_nic->send(resp);
                                db<TSTP>(TRC) << "Sent" << endl;
                            }
                        }
                    } break;

                    case DH_RESPONSE: {
                        if(_dh_requests_open) {
                            DH_Response * dh_resp = buf->frame()->data<DH_Response>();
                            db<TSTP>(INF) << "TSTP::Security::update(): DH_Response message received: " << *dh_resp << endl;

                            //CPU::int_disable();
                            bool valid_peer = false;
                            for(Peers::Element * el = _pending_peers.head(); el; el = el->next())
                                if(el->object()->valid_deploy(dh_resp->origin(), TSTP::now())) {
                                    valid_peer = true;
                                    db<TSTP>(TRC) << "Valid peer found: " << *el->object() << endl;
                                    break;
                                }

                            if(valid_peer) {
                                _dh_requests_open--;
                                Pending_Key * pk = new (SYSTEM) Pending_Key(buf->frame()->data<DH_Response>()->key());
                                _pending_keys.insert(pk->link());
                                db<TSTP>(INF) << "TSTP::Security::update(): Inserting new Pending Key: " << *pk << endl;
                            }
                            //CPU::int_enable();
                        }
                    } break;

                    case AUTH_REQUEST: {

                        Auth_Request * auth_req = buf->frame()->data<Auth_Request>();
                        db<TSTP>(INF) << "TSTP::Security::update(): Auth_Request message received: " << *auth_req << endl;

                        //CPU::int_disable();
                        Peer * auth_peer = 0;
                        for(Peers::Element * el = _pending_peers.head(); el; el = el->next()) {
                            Peer * peer = el->object();

                            if(peer->valid_request(auth_req->auth(), auth_req->origin(), TSTP::now())) {
                                for(Pending_Keys::Element * pk_el = _pending_keys.head(); pk_el; pk_el = pk_el->next()) {
                                    Pending_Key * pk = pk_el->object();
                                    if(verify_auth_request(pk->master_secret(), peer->id(), auth_req->otp())) {
                                        peer->master_secret(pk->master_secret());
                                        _pending_peers.remove(el);
                                        _trusted_peers.insert(el);
                                        auth_peer = peer;

                                        _pending_keys.remove(pk_el);
                                        delete pk_el->object();

                                        break;
                                    }
                                }
                                if(auth_peer)
                                    break;
                            }
                        }
                        //CPU::int_enable();

                        if(auth_peer) {
                            Auth encrypted_auth;
                            encrypt(auth_peer->auth(), auth_peer, encrypted_auth);

                            Buffer * resp = TSTP::alloc(sizeof(Auth_Granted));
                            new (resp->frame()) Auth_Granted(auth_peer->valid(), encrypted_auth);
                            TSTP::marshal(resp);
                            db<TSTP>(INF) << "TSTP::Security: Sending Auth_Granted message " << resp->frame()->data<Auth_Granted>() << endl;
                            TSTP::_nic->send(resp);
                        } else
                            db<TSTP>(WRN) << "TSTP::Security::update(): No peer found" << endl;
                    } break;

                    case AUTH_GRANTED: {

                        if(TSTP::here() != TSTP::sink()) {
                            Auth_Granted * auth_grant = buf->frame()->data<Auth_Granted>();
                            db<TSTP>(INF) << "TSTP::Security::update(): Auth_Granted message received: " << *auth_grant << endl;
                            //CPU::int_disable();
                            bool auth_peer = false;
                            for(Peers::Element * el = _pending_peers.head(); el; el = el->next()) {
                                Peer * peer = el->object();
                                for(Pending_Keys::Element * pk_el = _pending_keys.head(); pk_el; pk_el = pk_el->next()) {
                                    Pending_Key * pk = pk_el->object();
                                    Auth decrypted_auth;
                                    OTP key = otp(pk->master_secret(), peer->id());
                                    _cipher.decrypt(auth_grant->auth(), key, decrypted_auth);
                                    if(decrypted_auth == _auth) {
                                        peer->master_secret(pk->master_secret());
                                        _pending_peers.remove(el);
                                        _trusted_peers.insert(el);
                                        auth_peer = true;

                                        _pending_keys.remove(pk_el);
                                        delete pk_el->object();

                                        break;
                                    }
                                }
                                if(auth_peer)
                                    break;
                            }
                            //CPU::int_enable();
                        }
                    } break;

                    case MODEL: {
                        buf->trusted = true;
                    } break;

                    default: break;
                }
            }
            case RESPONSE: {
                db<TSTP>(INF) << "TSTP::Security::update(): Response message received from " << buf->frame()->data<Header>()->origin() << endl;
//                Response * resp = buf->frame()->data<Response>();
                Time reception_time = _NIC::Timer::count2us(buf->sfd_time_stamp);
                for(Peers::Element * el = _trusted_peers.head(); el; el = el->next()) {
                    if(el->object()->valid_deploy(buf->frame()->data<Header>()->origin(), TSTP::now())) {
                        unsigned char * data = buf->frame()->data<unsigned char>();
                        if(unpack(el->object(), data, &data[sizeof(Master_Secret)], reception_time)) {
                            buf->trusted = true;
                            break;
                        } else {
                            db<TSTP>(WRN) << "TSTP::Security: Unpack failed" << endl;
                        }
                    }
                }
                buf->trusted = true; // TODO
            } break;
            case INTEREST: {
                buf->trusted = true;
            } break;
            default: {
                buf->trusted = true; // TODO
                break;
            }
        }
    }
}

void TSTP::Security::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Security::marshal(buf=" << buf << ")" << endl;
    if(buf->frame()->data<Header>()->type() == TSTP::RESPONSE) {
        Peer * peer = 0;
        for(Peers::Element * el = _trusted_peers.head(); el; el = el->next())
            if(el->object()->valid_deploy(Router::destination(buf).center, TSTP::now())) {
                peer = el->object();
                break;
            }
        if(!peer)
            return;

        // Pad data to the size of the key
        unsigned int data_size = buf->size() - (sizeof(Response) - (MTU - sizeof(Unit) - sizeof(int) - sizeof(Time) - sizeof(CRC)));
//        Response * response = buf->frame()->data<Response>();
        unsigned char * data = buf->frame()->data<unsigned char>();
        buf->size(sizeof(Response) - (MTU - sizeof(Unit) - sizeof(int) - sizeof(Time) - sizeof(CRC)) + sizeof(Master_Secret));
        for(unsigned int i = data_size; i < sizeof(Master_Secret); i++)
            data[i] = 0;

        pack(data, peer);
        buf->trusted = true;
    } else
        buf->trusted = true;
}

__END_SYS

#endif
