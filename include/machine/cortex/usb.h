// EPOS ARM Cortex USB Mediator Declarations

#ifndef __cortex_usb_h
#define __cortex_usb_h

#include <architecture/cpu.h>
#include <machine/ic.h>
#include <machine/usb.h>
#include __HEADER_MMOD(usb)

__BEGIN_SYS

class USB: private USB_Engine
{
    friend class Machine;

private:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Reg8 Reg8;

    static void reset();

    static void int_handler(const unsigned int & interrupt);
    static bool handle_ep0(const USB_2_0::Request::Device_Request & data);

    static const Descriptor::Device _device_descriptor;

    // Maximum packet sizes for each endpoint
    static const unsigned int _max_packet_ep0 = 32;
    static const unsigned int _max_packet_ep1 = 32;
    static const unsigned int _max_packet_ep2 = 64;
    static const unsigned int _max_packet_ep3 = 128;
    static const unsigned int _max_packet_ep4 = 256;
    static const unsigned int _max_packet_ep5 = 512;

    struct Full_Config
    {
        Descriptor::Configuration _configuration_descriptor;

        Descriptor::Interface _interface0_descriptor;
        CDC::Functional_Descriptor::Header _cdc_header_descriptor;
        CDC::Functional_Descriptor::Abstract_Control_Management _cdc_acm_descriptor;
        CDC::Functional_Descriptor::Union_Interface _cdc_ui_descriptor;
        CDC::Functional_Descriptor::Call_Management _cdc_cm_descriptor;
        Descriptor::Endpoint _endpoint0_descriptor;

        Descriptor::Interface _interface1_descriptor;
        Descriptor::Endpoint _endpoint1_descriptor;
        Descriptor::Endpoint _endpoint2_descriptor;

        const char& operator[](unsigned int idx) const { return (reinterpret_cast<const char*>(this))[idx]; };
    }__attribute__((packed));

    static const Full_Config _config;

    static const char * _send_buffer;
    static unsigned int _send_buffer_size;

    static void flush() { reg(CS0_CSIL) |= CSIL_INPKTRDY; }

    static volatile USB_2_0::STATE _state;
    static volatile bool _ready_to_put; //FIXME: wrong semantics
    static bool _ready_to_put_next;
    static bool _was_locked;

public:
    USB() { }

    static USB_2_0::STATE state() { return _state; }

    static char get();
    static void put(char c);
    static void put(const char * c, unsigned int size);

    static bool ready_to_get() {
        if(!configured())
            return false;
        lock();
        input();
        bool ret = reg(CSOL) & CSOL_OUTPKTRDY;
        unlock();
        return ret;
    }

    static bool ready_to_put() { return _ready_to_put; }

    static void disable();

    static unsigned int get(char * out, unsigned int max_size);

    static void eoi(const IC::Interrupt_Id & int_id) {
        // USB interrupt flags are cleared when read
        _cif |= reg(CIF);
        _iif |= reg(IIF);
        _oif |= reg(OIF);
    }

private:
    static bool configured() { return state() >= USB_2_0::STATE::CONFIGURED; }

    static void endpoint(int index) { reg(INDEX) = index; }
    static int endpoint() { return reg(INDEX); }
    static void control() { endpoint(0); }
    static void output() { endpoint(3); }
    static void input() { endpoint(4); }

    static void init();

    static void lock() {
        _was_locked = CPU::int_disabled();
        // Concerning the correctness of _was_locked if a rescheduling happens between these two lines:
        // 1) If _was_locked was true, no rescheduling happens. OK.
        // 2) If _was_locked was false and interrupts are still enabled when execution returns, we're OK.
        // 3) It seems like an error if _was_locked was false and execution comes back with interrupts disabled.
        // So _was_locked should be correct.
        CPU::int_disable();
    }
    static void unlock() {
        if(!_was_locked)
            CPU::int_enable();
    }


protected:
    static volatile Reg32 & reg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(USB_BASE + offset)); }

private:
    static Reg32 _cif;
    static Reg32 _iif;
    static Reg32 _oif;
};

__END_SYS

#endif
