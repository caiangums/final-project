// EPOS DIRP Protocol Implementation

#include <network/ipv4/dirp.h>
#include <system.h>
#include <time.h>

#ifdef __ipv4__

__BEGIN_SYS

// Class attributes
DIRP::Observed DIRP::_observed;
DIRP* DIRP::_networks[];

static const int DELAY_SECONDS = 1000000;

/* Alarm_Object class to be passed as an argument to Functor_Handler of Alarm,
 * eventually make some changes or even destroy the object.
 */
class Alarm_Object {
    public:
        Alarm_Object(const void* data, int retries):
            _retries(retries), _data(data) {}
        ~Alarm_Object() {}

        // get
        int retries() { return this->_retries; }

        // set
        void retries(int retries) { this->_retries = retries; }

    private:
        int _retries;
        const void* _data;

};

/* This is the function that is called by the Functor_Handler, passing
 * the alarm_object as parameter
 */
void function_handler_to_functor(Alarm_Object * obj) {
    // Change something inside the object
    int v = obj->retries() + 1;
    obj->retries(v);
    db<DIRP>(ERR) << "    function_handler_to_functor call with retries=(" << obj->retries() << ")" << endl;
}

// Alarm with a Functor_Handler
Alarm* alarm_with_functor(const void * data) {
    db<DIRP>(ERR) << "  Alarm() with Functor_Handler - start" << endl;

    // create the Test Object
    int retries = Traits<Network>::RETRIES;
    Alarm_Object alarm_object(data, retries);

    // Create the Functor_Handler object, defined in handler.h
    Functor_Handler<Alarm_Object> handler(&function_handler_to_functor, &alarm_object);

    db<DIRP>(ERR) << "  create the alarm with 1s of delay, 3 times" << endl;

    // Create the Alarm object with time and the Function_Handler
    Alarm* alarm = new Alarm(DELAY_SECONDS * (int)Traits<Network>::TIMEOUT/3, &handler, retries);

    db<DIRP>(ERR) << "  Alarm() with Functor_Handler - end" << endl;
    return alarm;
}

// TODOr use abstractions like Packet and Header
int DIRP::send(const Address::Local & from, const Address & to, const void * data, unsigned int size) {
    // Get singleton DIRP
    DIRP * dirp = DIRP::get_by_nic(0);

    unsigned int port = to.port();
    Ethernet::Address mac_addr = dirp->address().mac();
    db<DIRP>(WRN) << "SENDING: MAC "<< mac_addr << endl;

    unsigned int port_size = 4; //sizeof(to.port());
    unsigned int mac_size = 6; //sizeof(Ethernet::Address);
    unsigned int ack_size = sizeof(Code);

    // our packet is the data to be sent plus the destin port (an unsigned int, 4 bytes) in the front
    char packet[size + port_size + mac_size + ack_size];

    Code noack = Code::NOTHING;

    memcpy(packet, &port, port_size);  // add port
    memcpy(&packet[port_size], &mac_addr, mac_size);  // add MAC after port
    memcpy(&packet[port_size + mac_size], &noack, ack_size);  // add ACK after MAC
    memcpy(&packet[port_size + mac_size + ack_size], data, size); // add data after ACK
    dirp->nic()->send(to.mac(), dirp->PROTOCOL, reinterpret_cast<void *>(packet), sizeof(packet));

    // creates handler to resend message
    int retries = Traits<Network>::RETRIES;
    DIRP::DIRP_Sender dirp_sender(to, dirp, reinterpret_cast<void *>(packet), sizeof(packet));
    dirp->_handler = new Functor_Handler<DIRP::DIRP_Sender>(&(DIRP::retry_send), &dirp_sender);

    // creates alarm that resend the message in case of no ACK received
    dirp->_alarm = new Alarm((DELAY_SECONDS * (int)Traits<Network>::TIMEOUT/3), dirp->_handler, retries);

    // wait for ACK
    Buffer* buf = _observed.notified(from);

    // cleanup alarm and handler
    delete dirp->_alarm;
    delete dirp->_handler;

    /*
    * Pode chegar nesse ponto em dois casos:
    * Quando o alarme der Timeout (caso em que buffer é nulo)
    * Quando chegar um ACK, e o update() libera normalmente.
    */
    if (buf == nullptr) {
        return -1;
    }
    //delete retries;
    return size;
}

// TODOr use abstractions like Packet and Header
int DIRP::receive(Buffer * buf, void * d, unsigned int s) {
    DIRP* dirp = get_by_nic(0);

    char * data = buf->frame()->data<char>();

    unsigned int port_size = 4;//sizeof(Address::Local);
    unsigned int mac_size = 6;//sizeof(Ethernet::Address);
    unsigned int ack_size = sizeof(Code);
    unsigned int header_size = port_size + mac_size + ack_size;

    // só chega aqui quando não é ACK
    /*Code isack;
    memcpy(&isack, &data[port_size+mac_size], ack_size);
    if (isack != Code::ACK) {*/
        // get port
        char port[port_size];
        memcpy(port, data, port_size);
        // get mac address
        Ethernet::Address mac_addr;
        memcpy(&mac_addr, &data[port_size], mac_size);  // n bytes is the port length

        char packet[port_size + mac_size + ack_size];
        Code ack = Code::ACK;

        memcpy(packet, &port, port_size);  // add port
        memcpy(&packet[port_size], &mac_addr, mac_size);  // add MAC after port
        memcpy(&packet[port_size + mac_size], &ack, ack_size);  // add ACK after MAC
        dirp->nic()->send(mac_addr, dirp->PROTOCOL, reinterpret_cast<void *>(packet), sizeof(packet));
    //}

    memcpy(d, &(data[header_size]), s);  // n bytes is the port length
    buf->nic()->free(buf);

    return s;
}

// TODOr use abstractions like Packet and Header
void DIRP::update(Observed* obs, const Protocol& prot, Buffer* buf) {
    // TODOr 5 is: DATA_SIZE from main()
    unsigned int size = 5 + 4 + 4 + 6;
    char data[size];

    memcpy(data, buf->frame()->data<char>(), size);

    // TODOr: remember that if we are getting 1 byte here, the DIRP port is limited to 127.
    // the first byte of data represents an unsigned int, not a char
    unsigned int port = (unsigned int)((unsigned char)(data[0]));

    buf->nic(_nic);
    if(!notify(port, buf))
        buf->nic()->free(buf);
}

__END_SYS

#endif

