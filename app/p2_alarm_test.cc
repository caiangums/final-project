// Alarm Handlers TEST Program

#include <communicator.h>

using namespace EPOS;

OStream cout;

static const int DELAY_SECONDS = 1000000;

// Simple Alarm Delay that only waits
void alarm_delay() {

    cout << "  Alarm::delay() - start" << endl;

    Alarm::delay(2 * DELAY_SECONDS);

    cout << "  Alarm::delay() - end" << endl;
}

/* function handler to be used inside a Function_Handler()
 * call, that should be passed inside the Alarm() call 
 */
void function_handler() {
    cout << "    function_handler call" << endl;
}

// Simple Alarm with a function as a handler
void alarm_with_handler() {

    cout << "  Alarm() with handler - start" << endl;

    // Create the Function_Handler object, defined in handler.h
    Function_Handler handler(function_handler);

    cout << "  create with 1s of delay" << endl;

    // Create the Alarm object with time and the Function_Handler
    Alarm alarm(1 * DELAY_SECONDS, &handler);

    cout << "  create a delay to wait 3s" << endl;

    /* We need to wait in order to function_handler inside the 
     * Function_Handler object to be called
     */
    Alarm::delay(3 * DELAY_SECONDS);

    cout << "  Alarm() with handler - end" << endl;
}

// Alarm Delay that waits and execute the handler a couple of times
void alarm_with_recurrent_handler() {

    cout << "  Alarm() with recurrent handler - start" << endl;

    // Create the Function_Handler object, defined in handler.h
    Function_Handler handler(function_handler);

    cout << "  create with 1s of delay, 3 times" << endl;

    // Create the Alarm object with time and the Function_Handler
    Alarm alarm(1 * DELAY_SECONDS, &handler, 3);

    cout << "  create a delay(Alarm::delay()) to wait 5s" << endl;

    /* We need to wait in order to function_handler inside the 
     * Function_Handler object to be called
     */
    Alarm::delay(5 * DELAY_SECONDS);

    cout << "  Alarm() with recurrent handler - end" << endl;
}

/* Test_Object class to be passed as an argument to Functor_Handler,
 * eventually make some changes or even destroy the object.
 */
class Test_Object {
    public:
        Test_Object(int val): _val(val) {}
        ~Test_Object() {}

        // get
        int val() { return this->_val; }

        // set
        void val(int val) { this->_val = val; }

    private:
        int _val;
};

/* This is the function that is called by the Functor_Handler, passing
 * the Test_Object as parameter
 */
static void function_handler_to_functor(Test_Object * obj) {
    // Change something inside the object
    int v = obj->val() + 1;
    obj->val(v);
    cout << "    function_handler_to_functor call with val=(" << obj->val() << ")" << endl;
}

// Alarm with a Functor_Handler
void alarm_with_functor() {

    cout << "  Alarm() with Functor_Handler - start" << endl;

    // create the Test Object
    int initial_value = 0;
    Test_Object test_object(initial_value);

    // Create the Functor_Handler object, defined in handler.h
    Functor_Handler<Test_Object> handler(&function_handler_to_functor, &test_object);

    cout << "  create the alarm with 1s of delay, 3 times" << endl;

    // Create the Alarm object with time and the Function_Handler
    Alarm alarm(1 * DELAY_SECONDS, &handler, 3);

    cout << "  create a delay to wait 5s" << endl;

    /* We need to wait in order to function_handler_to_functor
     * to be called inside Functor_Handler, passing the Test_Object
     */
    Alarm::delay(5 * DELAY_SECONDS);

    cout << "  Alarm() with Functor_Handler - end" << endl;
}

int main()
{
    cout << "Alarm Test Program" << endl;

    alarm_delay();

    alarm_with_handler();

    alarm_with_recurrent_handler();

    alarm_with_functor();

    cout << "The end!" << endl;

    return 0;
}
