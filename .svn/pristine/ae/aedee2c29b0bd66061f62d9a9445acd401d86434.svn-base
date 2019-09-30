// EPOS Trustful SpaceTime Protocol Security Declarations

#ifndef __tstp_security_h
#define __tstp_security_h

#include <utility/poly1305.h>
#include <utility/diffie_hellman.h>
#include <machine/aes.h>
#include <smartdata.h>

#ifdef __tstp__

__BEGIN_SYS


class TSTP::Security: private SmartData, private Data_Observer<Buffer>
{
    friend class TSTP;

private:
    static const bool use_encryption = false;
    static const unsigned int KEY_SIZE = Traits<TSTP>::KEY_SIZE;
    static const unsigned int KEY_MANAGER_PERIOD = 10 * 1000 * 1000;
    static const unsigned long long KEY_EXPIRY = 1 * 60 * 1000 * 1000;
    static const unsigned long long POLY_TIME_WINDOW = KEY_EXPIRY / 2;

    typedef _SYS::AES<KEY_SIZE> _AES;
    typedef Diffie_Hellman<_AES> _DH;
    typedef Poly1305<_AES> _Poly1305;

public:
    typedef Array<unsigned char, 16> Node_Id;
    typedef Array<unsigned char, 16> Auth;
    typedef Array<unsigned char, 16> OTP;

    typedef _DH::Public_Key Public_Key;
    typedef _DH::Shared_Key Master_Secret;

    class Peer;
    typedef Simple_List<Peer> Peers;
    class Peer
    {
    public:
        Peer(const Node_Id & id, const Region & v): _id(id), _valid(v), _el(this), _auth_time(0) {
            _aes.encrypt(_id, _id, _auth);
        }

        void valid(const Region & r) { _valid = r; }
        const Region & valid() const { return _valid; }

        bool valid_deploy(const Space & where, const Time & when) {
            return _valid.contains(where, when);
        }

        bool valid_request(const Auth & auth, const Space & where, const Time & when) {
            return !memcmp(auth, _auth, sizeof(Auth)) && _valid.contains(where, when);
        }

        const Time & authentication_time() { return _auth_time; }

        Peers::Element * link() { return &_el; }

        const Master_Secret & master_secret() const { return _master_secret; }
        void master_secret(const Master_Secret & ms) {
            _master_secret = ms;
            _auth_time = TSTP::now();
        }

        const Auth & auth() const { return _auth; }
        const Node_Id & id() const { return _id; }

        friend Debug & operator<<(Debug & db, const Peer & p) {
            db << "{id=" << p._id << ",au=" << p._auth << ",v=" << p._valid << ",ms=" << p._master_secret << ",el=" << &p._el << "}";
            return db;
        }

    private:
        Node_Id _id;
        Auth _auth;
        Region _valid;
        Master_Secret _master_secret;
        Peers::Element _el;
        Time _auth_time;
    };

    class  Pending_Key;
    typedef Simple_List<Pending_Key> Pending_Keys;
    class Pending_Key
    {
    public:
        Pending_Key(const Public_Key & pk): _master_secret_calculated(false), _creation(TSTP::now()), _public_key(pk), _el(this) {}

        bool expired() { return TSTP::now() - _creation > KEY_EXPIRY; }

        const Master_Secret & master_secret() {
            if(_master_secret_calculated)
                return _master_secret;
            _master_secret = _dh.shared_key(_public_key);
            _master_secret_calculated = true;
            db<TSTP>(INF) << "TSTP::Security::Pending_Key: Master Secret set: " << _master_secret << endl;
            return _master_secret;
        }

        Pending_Keys::Element * link() { return &_el; };

        friend Debug & operator<<(Debug & db, const Pending_Key & p) {
            db << "{msc=" << p._master_secret_calculated << ",c=" << p._creation << ",pk=" << p._public_key << ",ms=" << p._master_secret << ",el=" << &p._el << "}";
            return db;
        }

    private:
        bool _master_secret_calculated;
        Time _creation;
        Public_Key _public_key;
        Master_Secret _master_secret;
        Pending_Keys::Element _el;
    };

    // Diffie-Hellman Request Security Bootstrap Control Message
    class DH_Request: public Control
    {
    public:
        DH_Request(const Region & d, const Public_Key & k)
        : Control(DH_REQUEST), _destination(d), _public_key(k) { }

        const Region & destination() const { return _destination; }
        void destination(const Region & d) { _destination = d; }

        const Public_Key & key() { return _public_key; }
        void key(const Public_Key & k) { _public_key = k; }

        friend Debug & operator<<(Debug & db, const DH_Request & m) {
            db << reinterpret_cast<const Control &>(m) << ",d=" << m._destination << ",k=" << m._public_key;
            return db;
        }

    private:
        Region _destination;
        Public_Key _public_key;
    };

    // Diffie-Hellman Response Security Bootstrap Control Message
    class DH_Response: public Control
    {
    public:
        DH_Response(const Public_Key & k)
        : Control(DH_RESPONSE), _public_key(k) {}

        Public_Key key() { return _public_key; }
        void key(const Public_Key & k) { _public_key = k; }

        friend Debug & operator<<(Debug & db, const DH_Response & m) {
            db << reinterpret_cast<const Control &>(m) << ",k=" << m._public_key;
            return db;
        }

    private:
        Public_Key _public_key;
    };

    // Authentication Request Security Bootstrap Control Message
    class Auth_Request: public Control
    {
    public:
        Auth_Request(const Auth & a, const OTP & o)
        : Control(AUTH_REQUEST), _auth(a), _otp(o) { }

        const Auth & auth() const { return _auth; }
        void auth(const Auth & a) { _auth = a; }

        const OTP & otp() const { return _otp; }
        void otp(const OTP & o) { _otp = o; }

        friend Debug & operator<<(Debug & db, const Auth_Request & m) {
            db << reinterpret_cast<const Control &>(m) << ",a=" << m._auth << ",o=" << m._otp;
            return db;
        }

    private:
        Auth _auth;
        OTP _otp;
    };

    // Authentication Granted Security Bootstrap Control Message
    class Auth_Granted: public Control
    {
    public:
        Auth_Granted(const Region & d, const Auth & a)
        : Control(AUTH_GRANTED), _destination(d), _auth(a) {}

        const Region & destination() const { return _destination; }
        void destination(const Region & d) { _destination = d; }

        const Auth & auth() const { return _auth; }
        void auth(const Auth & a) { _auth = a; }

        friend Debug & operator<<(Debug & db, const Auth_Granted & m) {
            db << reinterpret_cast<const Control &>(m) << ",d=" << m._destination << ",a=" << m._auth;
            return db;
        }

    private:
        Region _destination;
        Auth _auth;
     };

    // Report Control Message
    class Report: public Control
    {
    public:
        Report(const Unit & unit, const Error & error = 0, bool epoch_request = false)
        : Control(REPORT, error) {}

        friend Debug & operator<<(Debug & db, const Report & r) {
//            db << reinterpret_cast<const Control &>(r) << ",u=" << r._unit << ",e=" << r._error << ",r=" << r._epoch_request;
            return db;
        }
    };

public:
    Security();
    ~Security();

    static void add_peer(const unsigned char * peer_id, unsigned int id_len, const Region & valid_region) {
        Node_Id id(peer_id, id_len);
        Peer * peer = new (SYSTEM) Peer(id, valid_region);
        _pending_peers.insert(peer->link());
        if(!_key_manager)
            _key_manager = new (SYSTEM) Thread(&key_manager);
    }

    static Time deadline(const Time & origin) {
        return origin + min(static_cast<Time>(KEY_MANAGER_PERIOD), KEY_EXPIRY) / 2;
    }

private:
    void update(Data_Observed<Buffer> * obs, Buffer * buf);

    static void marshal(Buffer * buf);

    static void pack(unsigned char * msg, const Peer * peer) {
        Time t = TSTP::now() / POLY_TIME_WINDOW;

        const unsigned char * ms = reinterpret_cast<const unsigned char *>(&peer->master_secret());
        const unsigned char * id = reinterpret_cast<const unsigned char *>(&peer->id());

        unsigned char nonce[16];
        memset(nonce, 0, 16);
        memcpy(nonce, &t, sizeof(Time) < 16u ? sizeof(Time) : 16u);

        _Poly1305 poly(id, ms);

        poly.stamp(&msg[sizeof(Master_Secret)], nonce, reinterpret_cast<const unsigned char *>(msg), sizeof(Master_Secret));

        if(use_encryption) {
            // mi = ms ^ _id
            static const unsigned int MI_SIZE = sizeof(Node_Id) > sizeof(Master_Secret) ? sizeof(Node_Id) : sizeof(Master_Secret);
            unsigned char mi[MI_SIZE];
            unsigned int i;
            for(i = 0; (i < sizeof(Node_Id)) && (i < sizeof(Master_Secret)); i++)
                mi[i] = id[i] ^ ms[i];
            for(; i < sizeof(Node_Id); i++)
                mi[i] = id[i];
            for(; i < sizeof(Master_Secret); i++)
                mi[i] = ms[i];

            OTP key;
            poly.stamp(key, nonce, mi, MI_SIZE);
            _aes.encrypt(msg, key, msg);
        }
    }

    static bool unpack(const Peer * peer, unsigned char * msg, const unsigned char * mac, Time reception_time) {
        unsigned char original_msg[sizeof(Master_Secret)];
        memcpy(original_msg, msg, sizeof(Master_Secret));

        const unsigned char * ms = reinterpret_cast<const unsigned char *>(&peer->master_secret());
        const unsigned char * id = reinterpret_cast<const unsigned char *>(&peer->id());

        // mi = ms ^ _id
        static const unsigned int MI_SIZE = sizeof(Node_Id) > sizeof(Master_Secret) ? sizeof(Node_Id) : sizeof(Master_Secret);
        unsigned char mi[MI_SIZE];
        unsigned int i;
        for(i = 0; (i < sizeof(Node_Id)) && (i < sizeof(Master_Secret)); i++)
            mi[i] = id[i] ^ ms[i];
        for(; i < sizeof(Node_Id); i++)
            mi[i] = id[i];
        for(; i < sizeof(Master_Secret); i++)
            mi[i] = ms[i];

        reception_time /= POLY_TIME_WINDOW;

        OTP key;
        unsigned char nonce[16];
        _Poly1305 poly(id, ms);

        memset(nonce, 0, 16);
        memcpy(nonce, &reception_time, sizeof(Time) < 16u ? sizeof(Time) : 16u);
        poly.stamp(key, nonce, mi, MI_SIZE);
        if(use_encryption)
            _aes.decrypt(original_msg, key, msg);
        if(poly.verify(mac, nonce, msg, sizeof(Master_Secret)))
            return true;

        reception_time--;
        memset(nonce, 0, 16);
        memcpy(nonce, &reception_time, sizeof(Time) < 16u ? sizeof(Time) : 16u);
        poly.stamp(key, nonce, mi, MI_SIZE);
        if(use_encryption)
            _aes.decrypt(original_msg, key, msg);
        if(poly.verify(mac, nonce, msg, sizeof(Master_Secret)))
            return true;

        reception_time += 2;
        memset(nonce, 0, 16);
        memcpy(nonce, &reception_time, sizeof(Time) < 16u ? sizeof(Time) : 16u);
        poly.stamp(key, nonce, mi, MI_SIZE);
        if(use_encryption)
            _aes.decrypt(original_msg, key, msg);
        if(poly.verify(mac, nonce, msg, sizeof(Master_Secret)))
            return true;

        memcpy(msg, original_msg, sizeof(Master_Secret));
        return false;
    }

    // TODO: remove?
    static void encrypt(const unsigned char * msg, const Peer * peer, unsigned char * out) {
        OTP key = otp(peer->master_secret(), peer->id());
        _aes.encrypt(msg, key, out);
    }

    static OTP otp(const Master_Secret & master_secret, const Node_Id & id) {
        const unsigned char * ms = reinterpret_cast<const unsigned char *>(&master_secret);

        // mi = ms ^ _id
        static const unsigned int MI_SIZE = sizeof(Node_Id) > sizeof(Master_Secret) ? sizeof(Node_Id) : sizeof(Master_Secret);
        unsigned char mi[MI_SIZE];
        unsigned int i;
        for(i = 0; (i < sizeof(Node_Id)) && (i < sizeof(Master_Secret)); i++)
            mi[i] = id[i] ^ ms[i];
        for(; i < sizeof(Node_Id); i++)
            mi[i] = id[i];
        for(; i < sizeof(Master_Secret); i++)
            mi[i] = ms[i];

        Time t = TSTP::now() / POLY_TIME_WINDOW;

        unsigned char nonce[16];
        memset(nonce, 0, 16);
        memcpy(nonce, &t, sizeof(Time) < 16u ? sizeof(Time) : 16u);

        OTP out;
        _Poly1305(id, ms).stamp(out, nonce, mi, MI_SIZE);
        return out;
    }

    static bool verify_auth_request(const Master_Secret & master_secret, const Node_Id & id, const OTP & otp) {
        const unsigned char * ms = reinterpret_cast<const unsigned char *>(&master_secret);

        // mi = ms ^ _id
        static const unsigned int MI_SIZE = sizeof(Node_Id) > sizeof(Master_Secret) ? sizeof(Node_Id) : sizeof(Master_Secret);
        unsigned char mi[MI_SIZE];
        unsigned int i;
        for(i = 0; (i < sizeof(Node_Id)) && (i < sizeof(Master_Secret)); i++)
            mi[i] = id[i] ^ ms[i];
        for(; i < sizeof(Node_Id); i++)
            mi[i] = id[i];
        for(; i < sizeof(Master_Secret); i++)
            mi[i] = ms[i];

        unsigned char nonce[16];
        Time t = TSTP::now() / POLY_TIME_WINDOW;

        _Poly1305 poly(id, ms);

        memset(nonce, 0, 16);
        memcpy(nonce, &t, sizeof(Time) < 16u ? sizeof(Time) : 16u);
        if(poly.verify(otp, nonce, mi, MI_SIZE))
            return true;

        t--;
        memset(nonce, 0, 16);
        memcpy(nonce, &t, sizeof(Time) < 16u ? sizeof(Time) : 16u);
        if(poly.verify(otp, nonce, mi, MI_SIZE))
            return true;

        t += 2;
        memset(nonce, 0, 16);
        memcpy(nonce, &t, sizeof(Time) < 16u ? sizeof(Time) : 16u);
        if(poly.verify(otp, nonce, mi, MI_SIZE))
            return true;

        return false;
    }

    static int key_manager() {

        Peers::Element * last_dh_request = 0;

        while(true) {
            Alarm::delay(KEY_MANAGER_PERIOD);

            db<TSTP>(TRC) << "TSTP::Security::key_manager()" << endl;
            CPU::int_disable();
            //while(CPU::tsl(_peers_lock));

            // Cleanup expired pending keys
            Pending_Keys::Element * next_key;
            for(Pending_Keys::Element * el = _pending_keys.head(); el; el = next_key) {
                next_key = el->next();
                Pending_Key * p = el->object();
                if(p->expired()) {
                    _pending_keys.remove(el);
                    delete p;
                    db<TSTP>(INF) << "TSTP::Security::key_manager(): removed pending key" << endl;
                }
            }

            // Cleanup expired peers
            Peers::Element * next;
            for(Peers::Element * el = _trusted_peers.head(); el; el = next) {
                next = el->next();
                Peer * p = el->object();
                if(!p->valid_deploy(p->valid().center, TSTP::now())) {
                    _trusted_peers.remove(el);
                    delete p;
                    db<TSTP>(INF) << "TSTP::Security::key_manager(): permanently removed trusted peer" << endl;
                }
            }
            for(Peers::Element * el = _pending_peers.head(); el; el = next) {
                next = el->next();
                Peer * p = el->object();
                if(!p->valid_deploy(p->valid().center, TSTP::now())) {
                    _pending_peers.remove(el);
                    delete p;
                    db<TSTP>(INF) << "TSTP::Security::key_manager(): permanently removed pending peer" << endl;
                }
            }

            // Cleanup expired established keys
            for(Peers::Element * el = _trusted_peers.head(); el; el = next) {
                next = el->next();
                Peer * p = el->object();
                if(TSTP::now() - p->authentication_time() > KEY_EXPIRY) {
                    _trusted_peers.remove(el);
                    _pending_peers.insert(el);
                    db<TSTP>(INF) << "TSTP::Security::key_manager(): trusted peer's key expired" << endl;
                }
            }

            // Send DH Request to at most one peer
            Peers::Element * el;
            if(last_dh_request && last_dh_request->next())
                el = last_dh_request->next();
            else
                el = _pending_peers.head();

            last_dh_request = 0;

            for(; el; el = el->next()) {
                Peer * p = el->object();
                if(p->valid_deploy(p->valid().center, TSTP::now())) {
                    last_dh_request = el;
                    Buffer * buf = alloc(sizeof(DH_Request));
//                    new (buf->frame()->data<DH_Request>()) DH_Request(Region::Space(p->valid().center, p->valid().radius), _dh.public_key());
                    marshal(buf);
                    _dh_requests_open++;
                    TSTP::_nic->send(buf);
                    db<TSTP>(INF) << "TSTP::Security::key_manager(): Sent DH_Request: "  << *buf->frame()->data<DH_Request>() << endl;
                    break;
                }
            }

            //_peers_lock = false;
            CPU::int_enable();
        }

        return 0;
    }

private:
    static Node_Id _id;
    static Auth _auth;

    static Thread * _key_manager;
    static Pending_Keys _pending_keys;
    static volatile bool _peers_lock;
    static Peers _pending_peers;
    static Peers _trusted_peers;
    static unsigned int _dh_requests_open;

    static _AES _aes;
    static _AES & _cipher;
    static _DH _dh;
};

__END_SYS

#endif

#endif
