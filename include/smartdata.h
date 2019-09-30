// EPOS SmartData Declarations
//
// Smart Data encapsulates Transducers (i.e. sensors and actuators), local or remote, and bridges them with Network
// Transducers must be Observed objects, must implement either sense() or actuate(), and must define UNIT, NUM, and ERROR.

#ifndef __smartdata_h
#define __smartdata_h

#include <utility/geometry.h>
#include <utility/observer.h>
#include <utility/hash.h>
#include <utility/predictor.h>
#include <time.h>
#include <real-time.h>

__BEGIN_SYS

class SmartData
{
protected:
    static const unsigned int PAN = 10; // Nodes
    static const unsigned int LAN = 100; // Nodes
    static const unsigned int WAN = 10000; // Nodes
    static const unsigned int NODES = Traits<Build>::NODES;

public:
    // SI Unit defining the SmartData semantics (inspired by IEEE 1451 TEDs)
    class Unit
    {
    public:
        // Formats
        // Bit       31                                 16                                     0
        //         +--+----------------------------------+-------------------------------------+
        // Digital |0 | type                             | len                                 |
        //         +--+----------------------------------+-------------------------------------+

        // Bit       31   29   27     24     21     18     15     12      9      6      3      0
        //         +--+----+----+------+------+------+------+------+------+------+------+------+
        // SI      |1 |NUM |MOD |sr+4  |rad+4 |m+4   |kg+4  |s+4   |A+4   |K+4   |mol+4 |cd+4  |
        //         +--+----+----+------+------+------+------+------+------+------+------+------+
        // Bits     1   2    2     3      3      3      3      3      3      3      3      3


        // Valid values for field SI
        enum : unsigned long {
            SI      = 0U << 31, // The Unit is SI. Remaining bits are interpreted as specified here.
            DIGITAL = 1U << 31, // The Unit is plain digital data. Subsequent 15 bits designate the data type. Lower 16 bits are application-specific, usually a device selector.
            SID = DIGITAL
        };

        // Valid values for field NUM
        enum : unsigned long {
            I32 = 0 << 29, // Value is an integral int stored in the 32 last significant bits of a 32-bit big-endian integer.
            I64 = 1 << 29, // Value is an integral int stored in the 64 last significant bits of a 64-bit big-endian integer.
            F32 = 2 << 29, // Value is a real int stored as an IEEE 754 binary32 big-endian floating point.
            D64 = 3 << 29, // Value is a real int stored as an IEEE 754 binary64 big-endian double precision floating point.
            NUM = D64      // AND mask to select NUM bits
        };

        // Valid values for field MOD
        enum : unsigned long {
            DIR     = 0 << 27, // Unit is described by the product of SI base units raised to the powers recorded in the remaining fields.
            DIV     = 1 << 27, // Unit is U/U, where U is described by the product SI base units raised to the powers recorded in the remaining fields.
            LOG     = 2 << 27, // Unit is log_e(U), where U is described by the product of SI base units raised to the powers recorded in the remaining fields.
            LOG_DIV = 3 << 27, // Unit is log_e(U/U), where U is described by the product of SI base units raised to the powers recorded in the remaining fields.
            MOD     = LOG_DIV      // AND mask to select MOD bits
        };

        // Masks to select the SI units
        enum : unsigned long {
            SR      = 7 << 24,
            RAD     = 7 << 21,
            M       = 7 << 18,
            KG      = 7 << 15,
            S       = 7 << 12,
            A       = 7 <<  9,
            K       = 7 <<  6,
            MOL     = 7 <<  3,
            CD      = 7 <<  0
        };

        // Mask to select field LEN of Digital data
        enum {
            LEN     = (1 << 16) - 1
        };

        // Helper to create digital units
        template<unsigned int _TYPE, unsigned int _SUBTYPE, unsigned int _LEN>
        struct Digital_Unit
        {
            //            DIGITAL |            type              | length
            enum { UNIT = DIGITAL | _TYPE << 24 | _SUBTYPE << 16 | _LEN << 0 };

        private:
            // Compile-time verifications
            static const typename IF<(_TYPE    & (~((1 << 23) - 1))), void, bool>::Result Invalid_TYPE    = false;
            static const typename IF<(_SUBTYPE & (~((1 << 15) - 1))), void, bool>::Result Invalid_SUBTYPE = false;
            static const typename IF<(_LEN     & (~LEN)),             void, bool>::Result Invalid_LEN     = false;
        };

        // Helper to create SI units
        template<int _MOD, int _SR, int _RAD, int _M, int _KG, int _S, int _A, int _K, int _MOL, int _CD>
        struct SI_Unit
        {
            //            SI  |  MOD  |        sr       |         rad      |        m       |        kg       |        s       |        A      |        K      |       mol       |     cd
            enum { UNIT = SI  | _MOD  | (4 + _SR) << 24 | (4 + _RAD) << 21 | (4 + _M) << 18 | ( 4 +_KG) << 15 | (4 + _S) << 12 | (4 + _A) << 9 | (4 + _K) << 6 | (4 + _MOL) << 3 | (4 + _CD) };

        private:
            // Compile-time verifications
            static const typename IF<(_MOD       & (~MOD)), void, bool>::Result Invalid_MOD = false;
            static const typename IF<((_SR  + 4) &  (~7u)), void, bool>::Result Invalid_SR  = false;
            static const typename IF<((_RAD + 4) &  (~7u)), void, bool>::Result Invalid_RAD = false;
            static const typename IF<((_M   + 4) &  (~7u)), void, bool>::Result Invalid_M   = false;
            static const typename IF<((_KG  + 4) &  (~7u)), void, bool>::Result Invalid_KG  = false;
            static const typename IF<((_S   + 4) &  (~7u)), void, bool>::Result Invalid_S   = false;
            static const typename IF<((_A   + 4) &  (~7u)), void, bool>::Result Invalid_A   = false;
            static const typename IF<((_K   + 4) &  (~7u)), void, bool>::Result Invalid_K   = false;
            static const typename IF<((_MOL + 4) &  (~7u)), void, bool>::Result Invalid_MOL = false;
            static const typename IF<((_CD  + 4) &  (~7u)), void, bool>::Result Invalid_CD  = false;
        };

        // Typical SI Quantities
        enum Quantity {
            //                                mod,     sr,    rad,      m,     kg,      s,      A,      K,    mol,     cd
            Acceleration            = SI_Unit<DIR,     +0,     +0,     +1,     +0,     -2,     +0,     +0,     +0,     +0>::UNIT,
            Amount_of_Substance     = SI_Unit<DIR,     +0,     +0,     +0,     +0,     +0,     +0,     +0,     +1,     +0>::UNIT,
            Angular_Speed           = SI_Unit<DIR,     +0,     +1,     +0,     +0,     -1,     +0,     +0,     +0,     +0>::UNIT,
            Area                    = SI_Unit<DIR,     +0,     +0,     +2,     +0,     +0,     +0,     +0,     +0,     +0>::UNIT,
            Current                 = SI_Unit<DIR,     +0,     +0,     +0,     +0,     +0,     +1,     +0,     +0,     +0>::UNIT,
            Electric_Current        = Current,
            Force                   = SI_Unit<DIR,     +0,     +0,     +1,     +1,     -2,     +0,     +0,     +0,     +0>::UNIT,
            Humidity                = SI_Unit<DIR,     +0,     +0,     -3,     +1,     +0,     +0,     +0,     +0,     +0>::UNIT,
            Length                  = SI_Unit<DIR,     +0,     +0,     +1,     +0,     +0,     +0,     +0,     +0,     +0>::UNIT,
            Luminous_Intensity      = SI_Unit<DIR,     +0,     +0,     +0,     +0,     +0,     +0,     +0,     +0,     +1>::UNIT,
            Mass                    = SI_Unit<DIR,     +0,     +0,     +0,     +1,     +0,     +0,     +0,     +0,     +0>::UNIT,
            Power                   = SI_Unit<DIR,     +0,     +0,     +2,     +1,     -3,     +0,     +0,     +0,     +0>::UNIT,
            Speed                   = SI_Unit<DIR,     +0,     +0,     +1,     +0,     -1,     +0,     +0,     +0,     +0>::UNIT,
            Sound_Intensity         = SI_Unit<DIR,     +0,     +0,     +0,     +1,     -3,     +0,     +0,     +0,     +0>::UNIT,
            Temperature             = SI_Unit<DIR,     +0,     +0,     +0,     +0,     +0,     +0,     +1,     +0,     +0>::UNIT,
            Time                    = SI_Unit<DIR,     +0,     +0,     +0,     +0,     +1,     +0,     +0,     +0,     +0>::UNIT,
            Velocity                = Speed,
            Volume                  = SI_Unit<DIR,     +0,     +0,     +3,     +0,     +0,     +0,     +0,     +0,     +0>::UNIT,
            Voltage                 = SI_Unit<DIR,     +0,     +0,     +2,     +1,     -3,     -1,     +0,     +0,     +0>::UNIT,
            Water_Flow              = SI_Unit<DIR,     +0,     +0,     +3,     +0,     -1,     +0,     +0,     +0,     +0>::UNIT,

            Ratio                   = SI_Unit<LOG_DIV, -4,     -4,     -4,     -4,     -4,     -4,     -4,     -4,     -4>::UNIT, // not an SI unit
            PPM                     = SI_Unit<LOG_DIV, -4,     -4,     -4,     -4,     -4,     -4,     -4,     -4,     -3>::UNIT, // not an SI unit
            PPB                     = SI_Unit<LOG_DIV, -4,     -4,     -4,     -4,     -4,     -4,     -4,     -4,     -2>::UNIT, // not an SI unit
            Antigravity             = SI_Unit<LOG_DIV, +3,     +3,     +3,     +3,     +3,     +3,     +3,     +3,     +3>::UNIT, // for Dummy_Transducer :-)
        };

        // Digital Units
        enum Custom_Units {
            //                                         type, subtype,   length
            // Switches
            Switch                  = Digital_Unit<       0,       0,        1>::UNIT,
            On_Off                  = Switch,

            // RFIDs and SmartCartds
            RFID32                  = Digital_Unit<       1,       0,        4>::UNIT,

            // Audio and Video (from RTP)                                       A/V         Hz        Ch       Ref
            PCMU                    = Digital_Unit<       2,       0,        0>::UNIT, // A         8000        1       [RFC3551]
            GSM                     = Digital_Unit<       2,       3,        0>::UNIT, // A         8000        1       [RFC3551]
            G723                    = Digital_Unit<       2,       4,        0>::UNIT, // A         8000        1       [Vineet_Kumar][RFC3551]
            DVI4_8                  = Digital_Unit<       2,       5,        0>::UNIT, // A         8000        1       [RFC3551]
            DVI4_16                 = Digital_Unit<       2,       6,        0>::UNIT, // A        16000        1       [RFC3551]
            LPC                     = Digital_Unit<       2,       7,        0>::UNIT, // A         8000        1       [RFC3551]
            PCMA                    = Digital_Unit<       2,       8,        0>::UNIT, // A         8000        1       [RFC3551]
            G722                    = Digital_Unit<       2,       9,        0>::UNIT, // A         8000        1       [RFC3551]
            L16_2                   = Digital_Unit<       2,      10,        0>::UNIT, // A        44100        2       [RFC3551]
            L16_1                   = Digital_Unit<       2,      11,        0>::UNIT, // A        44100        1       [RFC3551]
            QCELP                   = Digital_Unit<       2,      12,        0>::UNIT, // A         8000        1       [RFC3551]
            CN                      = Digital_Unit<       2,      13,        0>::UNIT, // A         8000        1       [RFC3389]
            MPA                     = Digital_Unit<       2,      14,        0>::UNIT, // A        90000                [RFC3551][RFC2250]
            G728                    = Digital_Unit<       2,      15,        0>::UNIT, // A         8000        1       [RFC3551]
            DVI4_11                 = Digital_Unit<       2,      16,        0>::UNIT, // A        11025        1       [Joseph_Di_Pol]
            DVI4_22                 = Digital_Unit<       2,      17,        0>::UNIT, // A        22050        1       [Joseph_Di_Pol]
            G729                    = Digital_Unit<       2,      18,        0>::UNIT, // A         8000        1       [RFC3551]
            CelB                    = Digital_Unit<       2,      25,        0>::UNIT, // V        90000                [RFC2029]
            JPEG                    = Digital_Unit<       2,      26,        0>::UNIT, // V        90000                [RFC2435]
            nv                      = Digital_Unit<       2,      28,        0>::UNIT, // V        90000                [RFC3551]
            H261                    = Digital_Unit<       2,      31,        0>::UNIT, // V        90000                [RFC4587]
            MPV                     = Digital_Unit<       2,      32,        0>::UNIT, // V        90000                [RFC2250]
            MP2T                    = Digital_Unit<       2,      33,        0>::UNIT, // AV       90000                [RFC2250]
            H263                    = Digital_Unit<       2,      34,        0>::UNIT, // V        90000                [Chunrong_Zhu]
        };

        // SI Factors
        typedef char Factor;
        enum {
         // Name           Code         Symbol    Factor
            ATTO        = (8 - 8), //     a       0.000000000000000001
            FEMTO       = (8 - 7), //     f       0.000000000000001
            PICO        = (8 - 6), //     p       0.000000000001
            NANO        = (8 - 5), //     n       0.000000001
            MICRO       = (8 - 4), //     μ       0.000001
            MILI        = (8 - 3), //     m       0.001
            CENTI       = (8 - 2), //     c       0.01
            DECI        = (8 - 1), //     d       0.1
            NONE        = (8    ), //     -       1
            DECA        = (8 + 1), //     da      10
            HECTO       = (8 + 2), //     h       100
            KILO        = (8 + 3), //     k       1000
            MEGA        = (8 + 4), //     M       1000000
            GIGA        = (8 + 5), //     G       1000000000
            TERA        = (8 + 6), //     T       1000000000000
            PETA        = (8 + 7)  //     P       1000000000000000
        };

        template<unsigned long UNIT>
        struct Get
        {
            typedef typename IF<((UNIT & SID) == SI)      && ((UNIT & NUM) == I32), long int,
                    typename IF<((UNIT & SID) == SI)      && ((UNIT & NUM) == I64), long long int,
                    typename IF<((UNIT & SID) == SI)      && ((UNIT & NUM) == F32), float,
                    typename IF<((UNIT & SID) == SI)      && ((UNIT & NUM) == D64), double,
                    typename IF<((UNIT & SID) == DIGITAL) && ((UNIT & LEN) == 1),   char,
                    typename IF<((UNIT & SID) == DIGITAL) && ((UNIT & LEN) == 2),   short,
                    typename IF<((UNIT & SID) == DIGITAL) && ((UNIT & LEN) == 4),   long,
                    typename IF<((UNIT & SID) == DIGITAL) && ((UNIT & LEN) > 0),    char[UNIT & LEN],
                    void>::Result>::Result>::Result>::Result>::Result>::Result>::Result>::Result Type;
        };

        template<typename T>
        struct GET;

        template<unsigned long U>
        struct Wrap { enum { UNIT = U }; };

    public:
        Unit(): _unit(0) {}
        Unit(unsigned long u) { _unit = u; }

        operator unsigned long() const { return _unit; }

        unsigned int value_size() const {
            return (_unit & SI) && ((_unit & NUM) == I32) ? sizeof(long int)
                : (_unit & SI) && ((_unit & NUM) == I64) ? sizeof(long long int)
                    : (_unit & SI) && ((_unit & NUM) == F32) ? sizeof(float)
                        : (_unit & SI) && ((_unit & NUM) == D64) ? sizeof(double)
                            : !(_unit & SI) ? _unit & LEN : 0;
        }

        int sr()  const { return ((_unit & SR)  >> 24) - 4 ; }
        int rad() const { return ((_unit & RAD) >> 21) - 4 ; }
        int m()   const { return ((_unit & M)   >> 18) - 4 ; }
        int kg()  const { return ((_unit & KG)  >> 15) - 4 ; }
        int s()   const { return ((_unit & S)   >> 12) - 4 ; }
        int a()   const { return ((_unit & A)   >>  9) - 4 ; }
        int k()   const { return ((_unit & K)   >>  6) - 4 ; }
        int mol() const { return ((_unit & MOL) >>  3) - 4 ; }
        int cd()  const { return ((_unit & CD)  >>  0) - 4 ; }

        friend Debug & operator<<(Debug & db, const Unit & u) {
            if(u & SI) {
                db << "{SI";
                switch(u & MOD) {
                case DIR: break;
                case DIV: db << "[U/U]"; break;
                case LOG: db << "[log(U)]"; break;
                case LOG_DIV: db << "[log(U/U)]";
                };
                switch(u & NUM) {
                case I32: db << ":I32"; break;
                case I64: db << ":I64"; break;
                case F32: db << ":F32"; break;
                case D64: db << ":D64";
                }
                db << ':';
                if(u.sr())
                    db << "sr^" << u.sr() << '.';
                if(u.rad())
                    db << "rad^" << u.rad() << '.';
                if(u.m())
                    db << "m^" << u.m() << '.';
                if(u.kg())
                    db << "kg^" << u.kg() << '.';
                if(u.s())
                    db << "s^" << u.s() << '.';
                if(u.a())
                    db << "A^" << u.a() << '.';
                if(u.k())
                    db << "K^" << u.k() << '.';
                if(u.mol())
                    db << "mol^" << u.mol() << '.';
                if(u.cd())
                    db << "cdr^" << u.cd() << '.';
                db << '\b';
            } else
                db << "{D:" << "l=" <<  (u >> 16);
            db << "}";
            return db;
        }

    private:
        unsigned long _unit;
    } __attribute__((packed));

    // Numeric value (either integer32, integer64, float32, double64 according to Unit::NUM)
    template<unsigned long UNIT>
    class Value
    {
        typedef typename Unit::Get<UNIT>::Type Type;

    public:
        Value() {}
        Value(const Type & v): _value(v) {}

        operator Type & () { return _value; }

    private:
        Type _value;
    } __attribute__((packed));

    // Precision or Error expressed as 10^Error
    typedef char Error;
    typedef Error Precision;
    enum : signed char {
        ANY = 0,
        UNKNOWN = -1
    };

    // Space (expressed in m from the center of the Earth; compressible by communication protocols)
    // Scale for geographic Space used by communication protocols (applications always get CM_32)
    enum Scale {
        CMx50_8  = 0,
        CM_16    = 1,
        CMx25_16 = 2,
        CM_32    = 3
    };
    static const Scale SCALE = (NODES <= PAN) ? CMx50_8 : (NODES <= LAN) ? CM_16 : (NODES <= WAN) ? CMx25_16 : CM_32;
    template<Scale S>
    struct Select_Padding {
        static const unsigned int PADDING = (S == CMx50_8)? 8 : ((S == CM_16) | (S == CMx25_16)) ? 16 : 0;
    };

    // Spatial (Geographic) Global_Space
    template<Scale S>
    struct _Space: public Point<typename SWITCH<S, CASE<CMx50_8, char, CASE<CM_16, short, CASE<CMx25_16, short, CASE<CM_32, long>>>>>::Result, 3>, private Padding<Select_Padding<S>::PADDING>
    {
        typedef typename SWITCH<S, CASE<CMx50_8, char, CASE<CM_16, short, CASE<CMx25_16, short, CASE<CM_32, long>>>>>::Result Number;
        typedef typename SWITCH<S, CASE<CMx50_8, unsigned char, CASE<CM_16, unsigned short, CASE<CMx25_16, unsigned short, CASE<CM_32, unsigned long>>>>>::Result Unsigned_Number;

        static const Number UNKNOWN = 1 << (sizeof(Number) * 8 - 1);

        _Space(const Number & x = 0, const Number & y = 0, const Number & z = 0): Point<Number, 3>(x, y, z) {}
        _Space(const Point<Number, 3> & p): Point<Number, 3>(p) {}

        operator _Space<CM_32>() const;
    } __attribute__((packed));
    typedef _Space<SCALE> Space;
    typedef _Space<CM_32> Global_Space;

    // Time (expressed in us)
    typedef unsigned long long Time;
    typedef long Time_Offset;
    struct Time_Interval
    {
        Time_Interval(const Time & begin, const Time & end): t0(begin), t1(end) {}

        bool contains(const Time & t) const { return (t >= t0) && (t <= t1); }

        Time t0;
        Time t1;
    } __attribute__((packed));

    // Space-Time (not exactly SpaceTime, because this is not a Minkowski space)
    template<Scale S>
    struct _Spacetime: public _Space<S>
    {
        typedef typename _Space<S>::Number Number;

        _Spacetime(const Number & x = 0, const Number & y = 0, const Number & z = 0, const Time & _t = 0): _Space<S>(x, y, z), t(_t) {}
        _Spacetime(const _Space<S> & s, const Time & _t): _Space<S>(s), t(_t) {}

        _Spacetime & operator=(const _Space<S> & s) {this->x = s.x; this->y = s.y; this->z = s.z; return *this;}
        _Spacetime & operator=(const Time & _t) {this->t = _t; return *this;}

        _Spacetime & operator+(const _Space<S> & s) {this->x += s.x; this->y += s.y; this->z += s.z; return *this;}
        _Spacetime & operator+(const Time & t) {this->t += t; return *this;}
        _Spacetime & operator+(const Time_Offset & to) {this->t += to; return *this;}

        operator _Space<S>() const { return const_cast<const _Space<S> &>(*this); }
        operator Time() const { return const_cast<const Time &>(t); }

        friend Debug & operator<<(Debug & db, const _Spacetime & st) {
            db << "{" << reinterpret_cast<const _Space<S> &>(st) << ",t=" << st.t << "}";
            return db;
        }

        Time t;
    } __attribute__((packed));
    typedef _Spacetime<SCALE> Spacetime;

    // Spatial Region in a time interval
    template<Scale S>
    struct _Region: public Sphere<typename _Space<S>::Number, typename _Space<S>::Unsigned_Number>, public Time_Interval // no need for padding: 3x8+8 | 3x16+16 | 3x32+32
    {
        typedef Sphere<typename _Space<S>::Number, typename _Space<S>::Unsigned_Number> _Sphere;
        using typename _Sphere::Center;
        using typename _Sphere::Radius;

        _Region(const Center & c, const Radius & r, const Time & t0, const Time & t1)
        : _Sphere(c, r), Time_Interval(t0, t1) {}
        _Region(const _Spacetime<S> & st, const Radius & r, const Time & t1)
        : _Sphere(st, r), Time_Interval(st, t1) {}

        operator _Spacetime<S>() const { return _Spacetime<S>(this->center, t0); }

        bool operator==(const _Region & r) const { return (*this == r); }
        bool operator!=(const _Region & r) const { return !(*this == r); }

        bool contains(const _Spacetime<S> & st) const {
            return Time_Interval::contains(st) && _Sphere::contains(st);
        }
        bool contains(const _Sphere & s, const Time & t) const {
            return Time_Interval::contains(t) && _Sphere::contains(s.center);
        }

        friend Debug & operator<<(Debug & db, const _Region & r) {
            db << "{" << reinterpret_cast<const _Sphere &>(r) << ",t0=" << r.t0 << ",t1=" << r.t1 << "}";
            return db;
        }
    } __attribute__((packed));
    typedef _Region<SCALE> Region;

    // Device enumerator (do differentiate two SmartData at the same (unit, x, y, z, t))
    typedef unsigned long Device_Id;
    enum : unsigned long {
        DEFAULT = 0,
        UNIQUE = DEFAULT
    };

    // Versions (for future extensions)
    typedef unsigned char Version;
    enum : unsigned char {
        V0      = 0,
        FIXED   = V0,
        V1      = 4,
        MOBILE  = V1
    };

    // SmartData Modes
    typedef unsigned char Mode;
    enum : unsigned char {
        // Responsive modes
        PRIVATE    = 0, // Local access only
        ADVERTISED = 1, // Advertised to the network (can bind to interests)
        COMMANDED  = 2, // Commanded via the network (can bind to interests and be commanded)

        // Interested modes
        SINGLE     = 0, // Only one response is desired for each interest (desired, but multiple responses are still possible)
        ALL        = 1, // All possible responses (e.g. from different sensors) are desired
        DELETE     = 2, // Revoke an interest

        // Data modes
        INTERESTED =  1 << 3,
        RESPONSIVE =  1 << 4,
        CONTROLLER =  1 << 5,
        CUMULATIVE =  1 << 6,
        PREDICTIVE =  1 << 7
    };

    // Message Types
    typedef unsigned char Type;
    enum : unsigned char {
        INTEREST  = 0,
        RESPONSE  = 1,
        COMMAND   = 2,
        CONTROL   = 3
    };

    // Control Message Subtypes
    typedef unsigned char Subtype;
    enum : unsigned char {
        NONE            = 0,

        // Response
        ADVERTISEMENT   = 1,

        // Interest
        ANNOUNCEMENT    = 1,

        // Control
        DH_REQUEST      = 1,
        DH_RESPONSE,
        AUTH_REQUEST,
        AUTH_GRANTED,
        REPORT,
        KEEP_ALIVE,
        EPOCH,
        MODEL
    };

    // Message Header
    template<Scale S>
    class _Header
    {
        // Bit 0   3    5  6
        //     +---+----+--+---+----+----+----+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ----+
        //     |ver|type|tr|scl|subt|mode| lc |   o.x   |   o.y   |   o.z   |   pad   |   o.t   |  lh.x   |  lh.y   |  lh.z   |   pad   |   lh.t   |
        //     +---------------+----+----+----+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ----+
        // Bits        8         8    8    8    8/16/32   8/16/32   8/16/32   8/16/0       64     8/16/32   8/16/32   8/16/32   8/16/0       64
        //
        // ver[3]:      0->fixed        4->mobile
        // type[2]:     0->interest     1->response     2->command      3->control
        // tr[1]:       0->no request   1->time request
        // scale[2]:    0->8-bit,cm.50  1->16-bit,cm    2->16-bit,cm.25 3->32b,cm
        // subtype:
        // mode:
        //  Responsive: 0->private      1->advertised   2->commanded
        //  Interested: 0->one sample   1->all samples  2->revoke
        // loc con:     0-100
        // origin:      variable size Spacetime
        // last hop:    variable size Spacetime

    public:
        _Header() {}
        _Header(const Subtype & st): _subtype(st) {}
        _Header(const Subtype & st, const Error & error): _subtype(st), _error(error) {}
        _Header(const Unit & u, const Device_Id & d, const Type & t, const Subtype & st = NONE, const Mode & m = NONE, bool tr = false, const Percent & c = 0, const Spacetime & lh = 0, const Version & v = V0)
        : _config((S & 0x03) << 6 | tr << 5 | (t & 0x03) << 3 | (v & 0x07)), _subtype(st), _mode(m), _location_confidence(c), _unit(u), _device(d), _last_hop(lh) {}
        _Header(const Spacetime & o, const Unit & u, const Device_Id & d, const Type & t, const Subtype & st = NONE, const Mode & m = NONE, bool tr = false, const Percent & c = 0, const Spacetime & lh = 0, const Version & v = V0)
        : _config((S & 0x03) << 6 | tr << 5 | (t & 0x03) << 3 | (v & 0x07)), _subtype(st), _mode(m), _location_confidence(c), _origin(o), _unit(u), _device(d), _last_hop(lh) {}

        Version version() const { return static_cast<Version>(_config & 0x07); }
        bool fixed() const { return !(_config & 0x01); }
        bool mobile() const { return (_config & 0x01); }
        void version(const Version & v) { _config = (_config & 0xf8) | (v & 0x07); }

        Type type() const { return static_cast<Type>((_config >> 3) & 0x03); }
        void type(const Type & t) { _config = (_config & 0xe7) | ((t & 0x03) << 3); }

        const Subtype & subtype() const { return const_cast<const Subtype &>(_subtype); }
        void subtype(const Subtype & st) { _subtype = st; }

        bool time_request() const { return (_config >> 5) & 0x01; }
        void time_request(bool tr) { _config = (_config & 0xdf) | (tr << 5); }

        Scale scale() const { return static_cast<Scale>((_config >> 6) & 0x03); }
        void scale(const Scale & s) { _config = (_config & 0x3f) | (s & 0x03) << 6; }

        const Percent & location_confidence() const { return _location_confidence; }
        void location_confidence(const Percent & c) { _location_confidence = c; }

        const Unit & unit() const { return _unit; }
        void unit(const Unit & u) { _unit = u; }
        const Device_Id & device() const { return const_cast<const Device_Id &>(_device); }
        void device(const Device_Id & u) { _device = u; }

        const Spacetime & origin() const { return _origin; }
        const Space & space() const { return reinterpret_cast<const Space &>(_origin); }
        const Time & time() const { return reinterpret_cast<const Time &>(_origin); }
        void origin(const Spacetime & o) { _origin = o; }
        void origin(const Space & s) { _origin = s; }
        void origin(const Time & t) { _origin = t; }

        const Spacetime & last_hop() const { return _last_hop; }
        void last_hop(const Spacetime & lh) { _last_hop = lh; }
        void last_hop(const Space & s) { _last_hop = s; }
        void last_hop(const Time & t) { _last_hop = t; }

        const Mode & mode() const { return const_cast<const Mode &>(_mode); }
        void mode(const Mode & m) { _mode = m; }

        unsigned int size() const {
            unsigned int tmp;
            switch(type()) {
            case INTEREST: tmp = sizeof(Interest) + _unit.value_size(); break;
            case RESPONSE: tmp = sizeof(Response) + _unit.value_size(); break;
            case COMMAND:  tmp = sizeof(Command)  + _unit.value_size(); break;
            case CONTROL:  tmp = sizeof(Control)  + _unit.value_size(); break;
            }
            return tmp;
        }

        friend Debug & operator<<(Debug & db, const _Header & h) {
            db << "{v=" << h.version() - V0 << ",t=" << ((h.type() == INTEREST) ? 'I' :  (h.type() == RESPONSE) ? 'R' : (h.type() == COMMAND) ? 'C' : 'T') << ",tr=" << h.time_request() << ",sc=" << h.scale()
               << ",st=" << h._subtype << ",m=" << h._mode << ",lc=" << h._location_confidence << ",o=" << h._origin << ",u=" << h._unit << ",d=" << h._device << ",lh=" << h._last_hop << "}";
            return db;
        }

    protected:
        unsigned char _config;
        union {
            Subtype _subtype;
            Precision _precision;
        };
        union {
            Mode _mode;
            Error _error;
        };
        Percent _location_confidence;
        Spacetime _origin;
        Unit _unit;
        Device_Id _device;
        Spacetime _last_hop;
    } __attribute__((packed));
    typedef _Header<SCALE> Header;

    // Interests for SmartData (used by SmartData encapsulating remote sources)
    class Interest: public Header
    {
    public:
        Interest(const Region & region, const Unit & unit, const Device_Id & device, const Mode & mode, const Precision & precision, const Time_Offset & expiry, const Time_Offset & period)
        : Header(unit, device, INTEREST, NONE, mode, precision), _region(region), _expiry(expiry), _period(period) {}

        Precision precision() const { return static_cast<Precision>(_subtype); }
        const Region & region() const { return _region; }
        Time_Offset expiry() const { return _expiry; }
        Time_Offset period() const { return _period; }

        template<typename T>
        const T & value() const { return *reinterpret_cast<const T *>(&_data); }
        template<typename T>
        void value(const T & v) { *reinterpret_cast<T *>(&_data) = v; }

        unsigned int data_size() const { return _unit.value_size(); };

        bool time_triggered() { return _period; }
        bool event_driven() { return !time_triggered(); }

        friend Debug & operator<<(Debug & db, const Interest & m) {
            db << "[INT" << ((m._mode == ALL) ? ".A" : ".S") << "]:{h=" << reinterpret_cast<const Header &>(m) << ",r=" << m._region << ",x=" << m._expiry << ",p=" << m._period << "}";
            return db;
        }

    private:
        Region _region;
        Time_Offset _expiry;
        Time_Offset _period;
        char _data[]; // must be manually allocated (adds 0 bytes to sizeof; can overlap)
    } __attribute__((packed));

    // Responses to SmartData Interests (used by SmartData encapsulating local sources, usually sensors)
    class Response: public Header
    {
    public:
        Response() {}
        Response(const Spacetime & origin, const Unit & unit, const Device_Id & device, const Mode & mode, const Error & error, const Percent & confidence, const Time_Offset & expiry)
        : Header(origin, unit, device, RESPONSE, NONE, mode, error), _confidence(confidence), _expiry(expiry) {}

        Error error() const { return _error; }
        const Percent & confidence() const { return _confidence; }
        Time_Offset expiry() const { return _expiry; }

        template<typename T>
        const T & value() const { return *reinterpret_cast<const T *>(&_data); }
        template<typename T>
        void value(const T & v) { *reinterpret_cast<T *>(&_data) = v; }

        unsigned int data_size() const { return _unit.value_size(); };

        friend Debug & operator<<(Debug & db, const Response & m) {
            db << "[RES]:{h=" << reinterpret_cast<const Header &>(m) << ",c=" << int(m._confidence) << ",x=" << m._expiry << "}";
            return db;
        }

    private:
        Percent _confidence;
        Time_Offset _expiry;
        char _data[]; // must be manually allocated (adds 0 bytes to sizeof; can overlap)
    } __attribute__((packed));

    // Commands to SmartData (e.g. actuation)
    class Command: public Header
    {
    public:
        Command(const Region & region, const Unit & unit, const Device_Id & device, const Mode & mode, const Precision & precision, const Time_Offset & expiry, const Time_Offset & period)
        : Header(region, unit, device, COMMAND, mode, precision), _radius(region.radius), _t1(region.t1), _expiry(expiry), _period(period) {}

        Region region() const { return Region(_origin, _radius, _t1); }
        Time_Offset expiry() const { return _expiry; }
        Time_Offset period() const { return _period; }

        template<typename T>
        const T & value() const { return *reinterpret_cast<const T *>(&_data); }
        template<typename T>
        void value(const T & v) { *reinterpret_cast<T *>(&_data) = v; }

        unsigned int data_size() const { return _unit.value_size(); };

        friend Debug & operator<<(Debug & db, const Command & m) {
            db << "[CMD]:{h=" << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",m=" << ((m._mode == ALL) ? 'A' : 'S') << ",x=" << m._expiry << ",re=" << m.region() << ",p=" << m._period << "}";
            return db;
        }

    private:
        Region::Radius _radius;
        Time _t1;
        Time_Offset _expiry;
        Time_Offset _period;
        char _data[]; // must be manually allocated (adds 0 bytes to sizeof; can overlap)
    } __attribute__((packed));

    // Control Messages
    class Control: public Header
    {
    protected:
        Control(const Subtype & subtype): Header(subtype) {}
        Control(const Subtype & subtype, const Error & error): Header(subtype, error) {}
        Control(const Region & region, const Unit & unit, const Device_Id & device, const Mode & mode, const Subtype & subtype)
        : Header(region, unit, device, CONTROL, mode, subtype), _radius(region.radius), _t1(region.t1) {}
        Control(const Spacetime & origin, const Unit & unit, const Device_Id & device, const Mode & mode, const Subtype & subtype)
        : Header(origin, unit, device, CONTROL, subtype, mode), _radius(0), _t1(0) {}

        Region region() const { return Region(_origin, _radius, _t1); }

        friend Debug & operator<<(Debug & db, const Control & m) {
            db << "[CTL.";
            switch(m._subtype) {
            case DH_REQUEST: db << "DHQ"; break;
            case DH_RESPONSE: db << "DHP"; break;
            case AUTH_REQUEST: db << "AUQ"; break;
            case AUTH_GRANTED: db << "AUP"; break;
            case REPORT: db << "R"; break;
            case KEEP_ALIVE: db << "T"; break;
            case EPOCH: db << "E"; break;
            case MODEL: db << "M"; break;
            }
            db << "]:{h=" << reinterpret_cast<const Header &>(m) << "}";
            return db;
        }

    protected:
        Region::Radius _radius;
        Time _t1;
    } __attribute__((packed));


    // SmartData observer/d conditioned to Unit
    template<typename Network>
    using Observer = Data_Observer<typename Network::Buffer, Unit>;
    template<typename Network>
    using Observed = Data_Observed<typename Network::Buffer, Unit>;


    // A SmartData series as stored in a database
    struct DB_Series {
        unsigned char type;
        unsigned long unit;
        long x;
        long y;
        long z;
        long device;
        unsigned long r;
        unsigned long long t0;
        unsigned long long t1;

        friend Debug & operator<<(Debug & os, const DB_Series & s) {
            os << "{t=" << s.type << ",u=" << s.unit << ",s=(" << s.x << "," << s.y << "," << s.z << "):" << s.device << "+" << s.r << ",t=[" << s.t0 << "," << s.t1 << "]}";
            return os;
        }
    };

    // A data-point as stored in a SmartData series database
    struct DB_Record {
        unsigned char type;
        unsigned long unit;
        double value;
        unsigned char error;
        unsigned char confidence;
        long x;
        long y;
        long z;
        long device;
        unsigned long long t;

        friend Debug & operator<<(Debug & os, const DB_Record & d) {
            os << "{t=" << d.type << ",u=" << d.unit << ",d=" << float(d.value) << ",e=" << d.error << ",s=(" << d.x << "," << d.y << "," << d.z << "):d=" << d.device << ",t=" << d.t << "}";
            return os;
        }
    };
};

template<> struct SmartData::Unit::GET<long int>      { enum { NUM = I32 }; };
template<> struct SmartData::Unit::GET<long long int> { enum { NUM = I64 }; };
template<> struct SmartData::Unit::GET<float>         { enum { NUM = F32 }; };
template<> struct SmartData::Unit::GET<double>        { enum { NUM = D64 }; };


template<> inline SmartData::_Space<SmartData::CMx50_8>::operator  SmartData::_Space<CM_32>() const { return _Space<CM_32>(Point<Number, 3>::x * 50, Point<Number, 3>::y * 50, Point<Number, 3>::z * 50); }
template<> inline SmartData::_Space<SmartData::CM_16>::operator    SmartData::_Space<CM_32>() const { return _Space<CM_32>(Point<Number, 3>::x,      Point<Number, 3>::y,      Point<Number, 3>::z); }
template<> inline SmartData::_Space<SmartData::CMx25_16>::operator SmartData::_Space<CM_32>() const { return _Space<CM_32>(Point<Number, 3>::x * 25, Point<Number, 3>::y * 25, Point<Number, 3>::z * 25); }
template<> inline SmartData::_Space<SmartData::CM_32>::operator    SmartData::_Space<CM_32>() const { return _Space<CM_32>(Point<Number, 3>::x,      Point<Number, 3>::y,      Point<Number, 3>::z); }


// Local data source, possibly advertised to or commanded through the network
template<typename Transducer, typename Network>
class Responsive_SmartData: public SmartData, public Observed, private Network::Observer, private Transducer::Observer
{
    friend Transducer;

public:
    static const unsigned long UNIT = Transducer::UNIT;
    static const unsigned int ERROR = Transducer::ERROR;
    static const bool predictive = (Traits<SmartData>::PREDICTOR != Traits<SmartData>::NONE);
    static const bool active = Transducer::active;

    typedef typename Unit::Get<UNIT>::Type Value;

private:
    typedef typename Network::Buffer Buffer;
    typedef typename Network::Locator Locator;
    typedef typename Network::Timekeeper Timekeeper;
    typedef typename Select_Predictor<Traits<SmartData>::PREDICTOR>::template Predictor<Time, Value> Predictor;

    class Binding;
    typedef Simple_List<Binding> Interesteds;
    typedef Simple_List<SmartData> Responsives;

    class Binding
    {
    private:
        typedef typename Simple_List<Binding>::Element Element;

    public:
        Binding(const Interest & interest)
        : _region(interest.region()), _mode(interest.mode()), _precision(interest.precision()), _expiry(interest.expiry()), _period(interest.period()), _link(this) {}

        const Region & region() const { return _region; }
        const Mode & mode() const { return _mode; }
        const Precision & precision() const { return _precision; }
        const Time_Offset & expiry() const { return _expiry; }
        const Time_Offset & period() const { return _period; }

        Element * link() { return &_link; }

    private:
        Region _region;
        Mode _mode;
        Precision _precision;
        Time_Offset _expiry;
        Time_Offset _period;

        Element _link;
    };

    enum Operation {
        ADVERTISE,
        CONCEAL,
        RESPOND,
        CONTROL
    };

public:
    Responsive_SmartData(const Device_Id & dev, const Time_Offset & expiry, const Mode & mode = PRIVATE)
    : _mode(mode), _origin(Locator::here(), Timekeeper::now()), _device(dev), _value(0), _error(ERROR), _confidence(0), _expiry(expiry),
     _transducer(new (SYSTEM) Transducer(dev)), _predictor(predictive ? new (SYSTEM) Predictor(typename Predictor::Configuration(), false) : 0), _thread(0), _link(this) {
        db<SmartData>(TRC) << "SmartData[R](d=" << dev << ",x=" << expiry << ",m=" << mode << ")=>" << this << endl;
        if(active)
            _transducer->attach(this);
        else
            if(Transducer::TYPE & Transducer::SENSOR) {
                _value = _transducer->sense();
                _origin = Timekeeper::now();
            }
        db<SmartData>(TRC) << "SmartData[R]::this=" << this << "=>" << *this << endl;
        if(_mode & ADVERTISED) {
            _responsives.insert(&_link);
            Network::attach(this, UNIT);
            process(ADVERTISE);
        }
    }

    virtual ~Responsive_SmartData() {
        db<SmartData>(TRC) << "~SmartData[R](this=" << this << ")" << endl;
        process(CONCEAL);
        Network::detach(this, UNIT);
        _responsives.remove(&_link);
        if(_thread)
            delete _thread;
    }

    const Unit unit() const { return UNIT; }
    const Error & error() const { return _error; }
    const Percent & confidence() const { return _confidence; }

    Spacetime origin() const { return Spacetime(space(), time()); }
    Space space() const { return Locator::absolute(_origin); }
    Time time() const { return Timekeeper::absolute(_origin); }

    const Mode & mode() const { return _mode; }

    Time_Offset expiry() const { return _expiry; }
    bool expired() const { return Timekeeper::now() > (_origin + _expiry); }

    operator Value() {
        db<SmartData>(TRC) << "SmartData[R]::operator Value(this=" << this << ")" << endl;

        Value tmp = _value;

        if(Transducer::TYPE & Transducer::SENSOR) {
            if(expired()) {
                if(!active) {
                    _value = _transducer->sense();
                    _origin = Timekeeper::now();
                } else {
                    // Active transducer should have called update() timely
                    db<SmartData>(WRN) << "SmartData[R]::value(this=" << this << ",t=" <<_origin.t + _expiry << ",v=" << _value << ") => expired!" << endl;
                }
            }
        } else
            db<SmartData>(WRN) << "SmartData[R]::value() called for actuation-only transducer!" << endl;

        if(_mode & CUMULATIVE)
            _value = 0;

        return tmp;
    }

    SmartData & operator=(const Value & v) {
        db<SmartData>(TRC) << "SmartData[R]::operator=(v=" << v << ")" << endl;

        if(Transducer::TYPE & Transducer::ACTUATOR) {
            _transducer->actuate(v);
            _value = _transducer->sense();
            if(!_thread && !_interesteds.empty())
                process(RESPOND);
        } else
            db<SmartData>(WRN) << "SmartData[R]::operator= called for sensing-only transducer!" << endl;

        return *this;
    }

    const Power_Mode & power() const { return Transducer::power(); }
    void power(const Power_Mode & mode) const { Transducer::power(mode); }

    DB_Record db_record() {
        DB_Record record;
        record.type = FIXED;
        record.unit = UNIT;
        record.value = _value;
        record.error = _error;
        record.confidence = _confidence;
        Space s = space();
        record.x = s.x;
        record.y = s.y;
        record.z = s.z;
        record.t = time();
        record.device = _device;
        return record;
    }

    static Space here() { return Locator::here(); } // Scale conversion done by SmartData::_Space specializations
    static Time now() { return Timekeeper::now(); }

    friend Debug & operator<<(Debug & db, const Responsive_SmartData & d) {
        db << "{R" << ((d._thread) ? ".TT" : ".ED");
        switch(d._mode) {
        case FIXED:     db << ".ST"; break;
        case MOBILE:     db << ".MB"; break;
        case ADVERTISED: db << ".AD"; break;
        case COMMANDED:  db << ".CM"; break;
        case CUMULATIVE: db << ".SM"; break;
        case PREDICTIVE: db << ".PR"; break;
        }
        if(d._thread)
            db << "]:p=" << d._thread->period();
        db << ":u=" << d.unit() << ",d=" << d._device << ",o=" << d._origin << ",v=" << d._value << ",e=" << int(d._error) << ",c=" << d._confidence << ",x=" << d._expiry << "}";
        return db;
    }

private:
    void process(const Operation & op) {
        db<SmartData>(TRC) << "SmartData[R]::process(op=" << ((op == ADVERTISE) ? "ADV" : (op == CONCEAL) ? "CON" : (op == RESPOND) ? "RES" : "CTL") << ")" << endl;

        Buffer * buffer = Network::alloc(sizeof(Response) + sizeof(Value));
        Header * header = buffer->frame()->template data<Header>();
        Response * response = new (header) Response(_origin, UNIT, _device, _mode, _error, _confidence, _expiry);

        switch(op) {
        case ADVERTISE:
            response->subtype(ADVERTISEMENT);
            break;
        case RESPOND:
            response->value<Value>(_value);
            break;
        }

        db<SmartData>(INF) << "SmartData[R]::process:msg=" << *response << endl;

        Network::send(buffer);
    }

    void update(Data_Observed<Buffer, Unit> * obs, const Unit & unit, Buffer * buffer) {
        Header * header = buffer->frame()->template data<Header>();

        db<SmartData>(TRC) << "SmartData[R]::update(obs=" << obs << ",unit=" << unit << ",buf=" << buffer << ")" << endl;

        switch(header->type()) {
        case INTEREST: {
            Interest * interest = reinterpret_cast<Interest *>(header);
            db<SmartData>(INF) << "SmartData[R]::update:msg=" << *interest << endl;

            if(_mode & ADVERTISED) {
                if(interest->mode() == DELETE)
                    unbind(interest);
                else
                    bind(interest);
                if(!_interesteds.empty()) {
                    if(!active) {
                        _value = _transducer->sense();
                        _origin = Timekeeper::now();
                    }
                    db<SmartData>(INF) << "SmartData[R]::value:n=" << Timekeeper::now() << ",t=" << _origin.t << ",t+e=" << _origin.t + _expiry << ")" << endl;
                    process(RESPOND);
                }
            } else
                db<SmartData>(INF) << "SmartData[R]::update: not advertised!" << endl;
        } break;
        case RESPONSE: {
            Response * response = reinterpret_cast<Response *>(header);
            db<SmartData>(INF) << "SmartData[R]::update:msg=" << *response << endl;
            db<SmartData>(INF) << "SmartData[R]::update: not interested!" << endl;
        } break;
        case COMMAND: {
            Command * command = reinterpret_cast<Command *>(header);
            db<SmartData>(INF) << "SmartData[R]::update:msg=" << *command << endl;

            if(_mode & COMMANDED) {
                _transducer->actuate(command->template value<Value>());
                _value = _transducer->sense();
            } else
                db<SmartData>(INF) << "SmartData[R]::update: not commanded!" << endl;
        } break;
        case CONTROL: {
            Control * control = reinterpret_cast<Control *>(header);
            db<SmartData>(INF) << "SmartData[R]::update:msg=" << *control << endl;

            switch(header->subtype()) {
            //            case Network::MODEL: {
            //                Network::Model * model = reinterpret_cast<Network::Model *>(packet);
            //                if(model->model<Model_Common>()->type() == Predictor::Model::TYPE) {
            //                    if(_predictor){
            //                        _origin = model->time();
            //                        _error = model->error();
            //                        _coordinates = model->origin();
            //                        _predictor->update(*model->model<typename Predictor::Model>(), false);
            //                        _value = _predictor->predict(_origin.t);
            //                        notify();
            //                    }
            //                }
            //            } break;
            //            default:
            //                break;
            }
        } break;
        }
    }

    // Event-driven update
    void update(typename Transducer::Observed * obs) {
        _origin = Timekeeper::now();
        _value = _transducer->sense();
        db<SmartData>(TRC) << "SmartData[R]::update(this=" << this << ",x=" << _expiry << ")=>" << _value << endl;
        notify();
        if(!_thread && !_interesteds.empty())
            process(RESPOND);
    }

    bool bind(Interest * interest) {
        db<SmartData>(TRC) << "SmartData[R]::bind(int=" << interest << ")" << endl;

        bool bound = false;
        typename Interesteds::Iterator i = _interesteds.begin();
        for(; i != _interesteds.end(); i++)
            if((interest->device() == _device) && i->object()->region().contains(interest->region()))
                break;

        if(i == _interesteds.end()) {
            Binding * binding = new (SYSTEM) Binding(*interest);
            _interesteds.insert(binding->link());
            if(interest->period()) {
                if(!_thread)
                    _thread = new (SYSTEM) Periodic_Thread(interest->period(), &updater, _device, interest->expiry(), this);
                else
                    if(interest->period() != _thread->period())
                        _thread->period(interest->period()); // FIXME: MDC(_thread->period(), interest->period())
            }
            bound = true;
        }

        return bound;
    }

    bool unbind(Interest * interest) {
        bool bound = true;

        typename Interesteds::Iterator i = _interesteds.begin();
        for(; i != _interesteds.end(); i++)
            if((interest->device() == _device) && (i->object()->region() == interest->region()))
                break;

        if(i != _interesteds.end()) {
            _interesteds.remove(i);
            delete i->object();
            if(_interesteds.empty()) {
                if(_thread) {
                    delete _thread;
                    _thread = 0;
                }
                if(_predictor){
                    delete _predictor;
                    _predictor = 0;
                }
                bound = false;
            }
        }

        return bound;
    }


    // Time-triggered updater
    static int updater(unsigned int device, Time_Offset expiry, Responsive_SmartData * sd) {
        db<SmartData>(TRC) << "SmartData[R]::updater(d=" << device << ",x=" << expiry << ",sd=" << sd << ")" << endl;
        while(true) {
            sd->_value = sd->_transducer->sense();
            sd->_origin = Timekeeper::now();
            sd->process(RESPOND);
            Periodic_Thread::wait_next();
        }
        return 0;
    }

private:
    Mode _mode;
    Spacetime _origin;
    unsigned int _device;
    Value _value;
    Error _error;
    Percent _confidence;
    Time _expiry;

    Transducer * _transducer;
    Predictor * _predictor;
    Periodic_Thread * _thread;

    typename Simple_List<SmartData>::Element _link;

    static Interesteds _interesteds;
    static Responsives _responsives;
};


// SmartData encapsulating remote transducers
template<typename _Unit, typename Network>
class Interested_SmartData: public SmartData, public Observed, private Network::Observer
{
public:
    static const unsigned long UNIT = _Unit::UNIT;
    static const bool predictive = (Traits<SmartData>::PREDICTOR != Traits<SmartData>::NONE);

    typedef typename Unit::Get<UNIT>::Type Value;
    typedef SmartData::Observed<Network> Observed;
    typedef SmartData::Observer<Network> Observer;
    using SmartData::Header;
    using SmartData::Interest;

private:
    typedef typename Network::Buffer Buffer;
    typedef typename Network::Locator Locator;
    typedef typename Network::Timekeeper Timekeeper;
    typedef typename Select_Predictor<Traits<SmartData>::PREDICTOR>::template Predictor<Time, Value> Predictor;

    typedef Simple_List<SmartData> Interests;

    enum Operation {
        ANNOUNCE,
        SUPPRESS,
        COMMAND,
        CONTROL
    };

public:
    Interested_SmartData(const Region & region, const Time_Offset & expiry, const Time_Offset & period = 0, const Mode & mode = SINGLE, const Precision & precision = 0, const Device_Id & device = UNIQUE)
    : _mode(mode), _region(region), _device(device), _precision(precision), _expiry(expiry), _period(period), _value(0), _predictor((predictive && (mode & PREDICTIVE)) ? new (SYSTEM) Predictor : 0), _link(this) {
        db<SmartData>(TRC) << "SmartData[I](r=" << region << ",d=" << device << ",x=" << expiry << ",m=" << mode << ",e=" << int(precision) << ",p=" << period << ")=>" << this << endl;
        _interests.insert(&_link);
        Network::attach(this, UNIT);
        process(ANNOUNCE);
        db<SmartData>(INF) << "SmartData[I]::this=" << this << "=>" << *this << endl;
    }

    virtual ~Interested_SmartData() {
        db<SmartData>(TRC) << "~SmartData[I](this=" << this << ")" << endl;
        process(SUPPRESS);
        Network::detach(this, UNIT);
        _interests.remove(&_link);
    }

    const Unit unit() const { return UNIT; }
    const Error & error() const { return _response.error(); }
    const Percent & confidence() const { return _response.confidence; }

    Spacetime origin() const { return Spacetime(space(), time()); }
    Space space() const { return Locator::absolute(_response.origin); }
    Time time() const { return Timekeeper::absolute(_response.origin); }

    const Mode & mode() const { return _mode; }
    const Precision & precision() const { return _precision; }

    Time_Offset expiry() const { return _response.expiry; }
    bool expired() const { return Timekeeper::now() > (_response.time() + _expiry); }

    operator Value & () {
        if(expired()) {
            if(predictive)
                _value = _predictor->predict(Timekeeper::now());
            else
                // Remote data sources should have sent messages timely triggering update()
                db<SmartData>(WRN) << "SmartData[I]::value(this=" << this << ",t=" <<_response.time() + _expiry << ",v=" << _value << ") => expired!" << endl;
        }
        return _value;
    }

    SmartData & operator=(const Value & v) {
        process(COMMAND, v);
        return *this;
    }

//    const Power_Mode & power() const { return Transducer::power(); } distributed power management coming soon!
//    void power(const Power_Mode & mode) const { Transducer::power(mode); }

    DB_Record db_record() {
        DB_Record record;
        record.type = FIXED;
        record.unit = UNIT;
        record.value = _value;
        record.error = _response.error();
        record.confidence = _response.confidence;
        Space s = origin();
        record.x = s.x;
        record.y = s.y;
        record.z = s.z;
        record.t = time();
        record.device = _response.device;
        return record;
    }

    DB_Series db_series(){
        DB_Series series;
        series.type = FIXED;
        series.unit = UNIT;
        Space c = Locator::absolute(_region.center);
        series.x = c.x;
        series.y = c.y;
        series.z = c.z;
        series.r = _region.radius;
        series.t0 = Timekeeper::absolute(_region.t0);
        series.t1 = Timekeeper::absolute(_region.t1);
        return series;
    }

    static Space here() { return Locator::here(); } // Scale conversion done by SmartData::_Space specializations
    static Time now() { return Timekeeper::now(); }

    friend Debug & operator<<(Debug & db, const Interested_SmartData & d) {
        db << "{I" << ((d._period) ? ".TT" : ".ED");
        switch(d._mode) {
        case FIXED:      db << ".ST"; break;
        case MOBILE:     db << ".MB"; break;
        case ADVERTISED: db << ".AD"; break;
        case COMMANDED:  db << ".CM"; break;
        case CUMULATIVE: db << ".SM"; break;
        case PREDICTIVE: db << ".PR"; break;
        }
        db << ",r=" << d._region << ",d=" << d._device << ",e=" << int(d._precision) << ",x=" << d._expiry;
        if(d._period)
            db << ",p=" << d._period;
        db << ",resp={o=" << d._response.origin() << ",d=" << d._response.device() << ",v=" << d._value << ",e=" << int(d._response.error()) << ",c=" << d._response.confidence() << ",x=" << d._response.expiry() << "}}";
        return db;
    }

private:
    void process(const Operation & op, const Value & v = 0) {
        db<SmartData>(TRC) << "SmartData[I]::process(op=" << ((op == ANNOUNCE) ? "ANN" : (op == SUPPRESS) ? "SUP" : (op == COMMAND) ? "COM" : "CTL") << ",v=" << v << ")" << endl;

        Buffer * buffer = Network::alloc(sizeof(Interest) + sizeof(Value));
        Header * header = buffer->frame()->template data<Header>();
        Interest * interest = new (header) Interest(_region, UNIT, _device, _mode, _precision, _expiry, _period);

        switch(op) {
        case ANNOUNCE:
            interest->subtype(ANNOUNCEMENT);
            break;
        case SUPPRESS:
            interest->mode(DELETE);
            break;
        case COMMAND:
            interest->value<Value>(v);
            break;
        }

        db<SmartData>(INF) << "SmartData[I]::process:msg=" << *interest << endl;

        Network::send(buffer);
    }

    void update(Observed * obs, const Unit & unit, Buffer * buffer) {
        Header * header = buffer->frame()->template data<Header>();

        db<SmartData>(TRC) << "SmartData[I]::update(obs=" << obs << ",unit=" << unit << ",buf=" << buffer << ")" << endl;

        switch(header->type()) {
        case INTEREST: {
            Interest * interest = reinterpret_cast<Interest *>(header);
            db<SmartData>(INF) << "SmartData[I]::update:msg=" << *interest << endl;
            db<SmartData>(WRN) << "SmartData[I]::update: not advertised!" << endl;
        } break;
        case RESPONSE: {
            Response * response = reinterpret_cast<Response *>(header);
            db<SmartData>(INF) << "SmartData[I]::update:msg=" << *response << endl;
            if((response->unit() == UNIT) && _region.contains(response->origin())) {
                if(response->subtype() == ADVERTISEMENT)
                    process(ANNOUNCE);
                else {
                    _response = *response;
                    if(_mode & CUMULATIVE)
                        _value += response->template value<Value>();
                    else
                        _value = response->template value<Value>();
                    notify();
                }
            } else
                db<SmartData>(INF) << "SmartData[I]::update: not interested!" << endl;
        } break;
        case COMMAND: {
            Command * command = reinterpret_cast<Command *>(header);
            db<SmartData>(INF) << "SmartData[I]::update:msg=" << *command << endl;
            db<SmartData>(WRN) << "SmartData[I]::update: not commanded!" << endl;
        } break;
        case CONTROL: {
            Control * control = reinterpret_cast<Control *>(header);
            db<SmartData>(INF) << "SmartData[I]::update:msg=" << *control << endl;

            switch(header->subtype()) {
            //            case Network::MODEL: {
            //                Network::Model * model = reinterpret_cast<Network::Model *>(packet);
            //                if(model->model<Model_Common>()->type() == Predictor::Model::TYPE) {
            //                    if(_predictor){
            //                        _origin = model->time();
            //                        _error = model->error();
            //                        _coordinates = model->origin();
            //                        _predictor->update(*model->model<typename Predictor::Model>(), false);
            //                        _value = _predictor->predict(_origin.t);
            //                        notify();
            //                    }
            //                }
            //            } break;
            //            default:
            //                break;
            }
        } break;
        }
    }

private:
    // Interested attributes
    Mode _mode;
    Region _region;
    unsigned int _device;
    Precision _precision;
    Time_Offset _expiry;
    Time_Offset _period;
    Predictor * _predictor;
    typename Simple_List<SmartData>::Element _link;

    // Last response attributes
    Value _value;
    Response _response;

    static Interests _interests;
};


// SmartData encapsulating controllers
template<typename Unit, typename Network>
class Controller_SmartData: public SmartData, public Observed
{
private:
public:
    template<typename Controller, typename ... SD>
    Controller_SmartData(Controller controller, SD ... sd);

    virtual ~Controller_SmartData();
};

template<typename Transducer, typename Network>
typename Responsive_SmartData<Transducer, Network>::Interesteds Responsive_SmartData<Transducer, Network>::_interesteds;
template<typename Transducer, typename Network>
typename Responsive_SmartData<Transducer, Network>::Responsives Responsive_SmartData<Transducer, Network>::_responsives;

template<typename Unit, typename Network>
typename Interested_SmartData<Unit, Network>::Interests Interested_SmartData<Unit, Network>::_interests;

__END_SYS

#endif
