// EPOS Keypad Smart Transducer Declarations

#ifndef __keypad_h
#define __keypad_h

#include <machine/keyboard.h>
#include <transducer.h>

__BEGIN_SYS

class Keypad: public Transducer<SmartData::Unit::Switch>, private Keyboard::Observer
{
public:
    static const unsigned int DEVS = 1;
    static const int ERROR = 0;
    static const bool active = true;

    using Observer = Transducer::Observer;

public:
    Keypad(const Device_Id & dev = 0): _key(0) {
        assert(dev < DEVS);
        Keyboard::attach(this);
    }

    virtual Value sense() { return _key; }

private:
    void update(Keyboard::Observed * o) {
        _key = _keyboard.get();
        notify();
    }

private:
    Keyboard _keyboard; // Keyboard must be a real one (not a serial emulation, which does not trigger interrupts)
    volatile Value _key;
};

__END_SYS

#endif
