#include <utility/observer.h>
#include <network/tstp/tstp.h>
#include <transducer.h>
#include <smartdata.h>

static const unsigned int ITERATIONS = 10;

using namespace EPOS;

OStream cout;

void controller(const Antigravity & in1, const Antigravity & in2, Antigravity & out) {}

int sink();
int node();

int main()
{
    cout << "SmartData Test" << endl;

    cout << "Sizes:" << endl;
    cout << "  SmartData::Unit:           " << sizeof(SmartData::Unit) << endl;
    cout << "  SmartData::Value<SI|I32>:  " << sizeof(SmartData::Value<SmartData::Unit::SI | SmartData::Unit::I32>) << endl;
    cout << "  SmartData::Value<SI|I64>:  " << sizeof(SmartData::Value<SmartData::Unit::SI | SmartData::Unit::I64>) << endl;
    cout << "  SmartData::Value<SI|F32>:  " << sizeof(SmartData::Value<SmartData::Unit::SI | SmartData::Unit::F32>) << endl;
    cout << "  SmartData::Value<SI|D64>:  " << sizeof(SmartData::Value<SmartData::Unit::SI | SmartData::Unit::D64>) << endl;
    cout << "  SmartData::Spacetime:      " << sizeof(SmartData::Spacetime) << endl;
    cout << "  SmartData::Space:          " << sizeof(SmartData::Space) << endl;
    cout << "  SmartData::Global_Space:   " << sizeof(SmartData::Global_Space) << endl;
    cout << "  SmartData::Time:           " << sizeof(SmartData::Time) << endl;
    cout << "  SmartData::Time_Offset:    " << sizeof(SmartData::Time_Offset) << endl;
    cout << "  SmartData::Region:         " << sizeof(SmartData::Region) << endl;
    cout << "  SmartData::Header:         " << sizeof(SmartData::Header) << endl;
    cout << "  SmartData::Interest:       " << sizeof(SmartData::Interest) << endl;
    cout << "  SmartData::Response:       " << sizeof(SmartData::Response) << endl;
    cout << "  SmartData::Command:        " << sizeof(SmartData::Command) << endl;
    cout << "  SmartData::Control:        " << sizeof(SmartData::Control) << endl;
    cout << "  TSTP::Header:              " << sizeof(TSTP::Header) << endl;
    cout << "  TSTP::Packet:              " << sizeof(TSTP::Packet) << endl;

    if(TSTP::here() == TSTP::sink())
        sink();
    else
        node();

    //    CAntigravity ctl(&controller, a, b, c);

    cout << "Bye!" << endl;

    return 0;
}

int sink()
{
    cout << "I'm the sink!" << endl;

    Antigravity_Proxy a(Antigravity::Region((0, 0, 0), 100, Antigravity::now(), Antigravity::now() + ITERATIONS * 1000000), 1000000, 1000000);
//    Smart_Key_Proxy d(Smart_Key::Region((0, 0, 0), 100, Smart_Key::now(), Smart_Key::now()+10000000), 10000000);

    cout << "My coordinates are " << a.here() << endl;
    cout << "The time now is " << a.now() << endl;

    cout << "I'm interested on " << a.unit() << endl;
    cout << "I'll wait for data of this kind for " << ITERATIONS << " seconds..." << endl;
    for(int i = 0; i < ITERATIONS + 5; i++) {
        Delay(1000000);
        cout << "a=" << a << endl;
    }
    cout << "done!" << endl;
}

int node()
{
    cout << "I'm a node!" << endl;

    Antigravity a(0, 1000000, Antigravity::ADVERTISED);
//    Antigravity b(0, 10000000, Antigravity::ADVERTISED);
//    Antigravity c(0, 1000000, Antigravity::COMMANDED);

//    Smart_Key d(0, 500000, Smart_Key::PRIVATE);

    cout << "My coordinates are " << a.here() << endl;
    cout << "The time now is " << a.now() << endl;

//    a = 1;
//    b = 1;
//    c = 1;

    cout << "I have three sensors that measure " << a.unit() << endl;
    cout << "OMG that's ANTI-GRAVITY!!!" << endl;
    cout << "I'll update data of this kind for " << ITERATIONS << " seconds..." << endl;

    for(int i = 0; i < ITERATIONS; i++) {
        Delay(1000000);
//        a = i;
//        b = i * 2;
//        c = i * 3;
//        db<TSTP>(TRC) << "a=" << a << endl;
        cout << "a=" << a << endl;
//        cout << "b=" << b << endl;
//        db<TSTP>(TRC) << "c=" << c << endl;
    }
    cout << "done!" << endl;
}
