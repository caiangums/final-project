// EPOS Application Scaffold and Application Component Implementation

#include <system.h>
#include <network.h>
#include <clerk.h>

__BEGIN_SYS

// Application class attributes
char Application::_preheap[];
Heap * Application::_heap;

__END_SYS

__BEGIN_API

// Global objects
__USING_UTIL
OStream cout;
OStream cerr;

__END_API

extern "C" {
    void __pre_main() {
        if(_SYS::Traits<_SYS::Monitor>::enabled)
            _SYS::Monitor::init();

        if(_SYS::Traits<_SYS::Network>::enabled)
            _SYS::Network_Common::init();
    }
}
