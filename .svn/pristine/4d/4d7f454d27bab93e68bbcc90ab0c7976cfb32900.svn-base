// EPOS Network Common Package

#ifndef __network_common_h
#define __network_common_h

#include <system/config.h>


extern "C" { void __pre_main(); }

__BEGIN_SYS

class Network_Common
{
     friend void ::__pre_main();

public:
    template<int unit = 0>
    struct Initializer
    {
        typedef typename Traits<Network>::NETWORKS::template Get<unit>::Result NET;

        static void init() {
            if(Traits<NET>::enabled)
                NET::init(unit);

            Initializer<unit + 1>::init();
        };
    };

protected:
    Network_Common() {}

private:
    static void init();
};

template<>
struct Network_Common::Initializer<Traits<Network>::NETWORKS::Length>
{
    static void init() {};
};


__END_SYS

#endif
