// EPOS Network Component Initialization

#include <network.h>

__BEGIN_SYS

void Network_Common::init()
{
    db<Init, Network>(TRC) << "Network::init()" << endl;

    Initializer<0>::init();

#ifdef __ipv4__

    // If IP was initialized, initialize also the rest of the stack
    if(Traits<Network>::NETWORKS::Count<IP>::Result) {
        if(Traits<ICMP>::enabled)
            new (SYSTEM) ICMP;
        if(Traits<UDP>::enabled)
            new (SYSTEM) UDP;
        if(Traits<TCP>::enabled)
            new (SYSTEM) TCP;
    }

#endif

}

__END_SYS
