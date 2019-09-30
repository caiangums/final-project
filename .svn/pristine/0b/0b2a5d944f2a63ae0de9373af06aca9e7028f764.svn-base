// EPOS Machine Mediator

#ifndef __clerk_h
#define __clerk_h

#include <architecture.h>
#include <machine.h>
#include <system.h>
#include <time.h>
#include <transducer.h>
#include <process.h>

extern "C" { void __pre_main(); }

__BEGIN_SYS

class Monitor
{
    friend void ::__pre_main(); // for init()
    friend class Thread;        // for init()

protected:
    typedef TSC::Time_Stamp Time_Stamp;
    typedef TSC::Hertz Hertz;
    typedef Simple_List<Monitor> List;

    // Anomaly detection parameters
    static const unsigned int STD_DEV_ACCEPTED_DRIFT = 2;         // +/- 2 * std_dev
    static const unsigned int AVERAGE_ACCEPTED_DRIFT = 2;         // +/- 2 * average
    static const unsigned int TIME_ACCEPTED_DRIFT = 2;            // ts difference between captures +/- 2 * frequency
    static const unsigned int MINIMUN_SNAPSHOTS_TO_VALIDATE = 20; // start verification if #SNAPSHOTS greater than this value

public:
    Monitor(): _captures(0), _t0(TSC::time_stamp()) {}
    virtual ~Monitor() {}

    virtual void capture() = 0;
    virtual OStream & operator<<(OStream & os) = 0;

    static void run();

    static void process_batch() {
        db<Monitor>(TRC) << "Monitor::process_batch()" << endl;

        unsigned int i;
        for(unsigned int n = 0; n < CPU::cores(); n++) {
            i = 0;
            db<Monitor>(WRN) << "CPU" << n << endl;
            for(List::Iterator it = _monitors[n].begin(); it != _monitors[n].end(); it++) {
                db<Monitor>(WRN) << "TS," << i << endl;
                db<Monitor>(WRN) << it->object();
                i++;
            }
        }
    }

protected:
    Time_Stamp time_stamp() { return TSC::time_stamp() - _t0; }

private:
    static void init();

protected:
    unsigned int _captures;
    Time_Stamp _t0;

private:
    static Simple_List<Monitor> _monitors[Traits<Build>::CPUS];
};


template<typename Clerk>
class Clerk_Monitor: public Monitor
{
    friend class Monitor;

private:
//    static const unsigned int PERIOD = (FREQUENCY > 0) ? 1000000 / FREQUENCY : -1UL;

public:
    typedef typename Clerk::Data Data;

    struct Snapshot
    {
        Time_Stamp ts;
        Data data;
    };

public:
    Clerk_Monitor(Clerk * clerk, const Hertz & frequency): _clerk(clerk), _frequency(frequency), _period((frequency > 0) ? 1000000 / frequency : -1UL), _last_capture(0), _average(0), _link(this) {
        db<Clerk>(TRC) << "Clerk_Monitor(clerk=" << clerk << ") => " << this << ")" << endl;
        _snapshots = Traits<Build>::EXPECTED_SIMULATION_TIME * frequency;
        // if((_snapshots * sizeof(Snapshot)) > Traits<Monitor>::MAX_BUFFER_SIZE)
        //     _snapshots = Traits<Monitor>::MAX_BUFFER_SIZE * sizeof(Snapshot);
        _buffer = new (SYSTEM) Snapshot[_snapshots];

        _monitors[CPU::id()].insert(&_link);
    }
    ~Clerk_Monitor() {
        _monitors[CPU::id()].remove(&_link);
        delete _buffer;
    }

    Snapshot & operator[](unsigned int i) const { return (i < _captures) ? _buffer[i] : _buffer[_captures]; }

    unsigned int captures() { return _captures; }

    void capture() {
        Time_Stamp ts = time_stamp();
        if(_captures < _snapshots && ((ts - _last_capture) > _period)) {
            _buffer[_captures].ts = ts;
            _buffer[_captures].data = _clerk->read();
            _average = (_average * _captures + _buffer[_captures].data) / (_captures + 1);
            _captures++;
            _last_capture = ts;
        }
    }

    // Validate a snapshot against the collected time series
    bool validate(const Snapshot & s) {
        if(_captures > MINIMUN_SNAPSHOTS_TO_VALIDATE) {
            // Statistic validation
            // Drift is a deviation from a predicted value, a statistical error, that
            // can be measured as standard deviation from the predicted values (v - predicted_v) ^ 2
            // The statistical predictor has a default acceptable value that can be adjusted.
            // If the prediction is the average, the accepted drift may be on 'x' times average...
            // need to always update model (average is updated every capture)
            // This detector is used on level.c, spectrum.c,

            // First verification
            // On a gateway scenario... anomaly detection by delay 'x' times bigger than average delay or expected delay
            // this detector is used on loss.c

            // Second verification

            // Percentage of high (or low) values...
            // if the buffer is filled up, check the number of SNAPSHOTS with value equal to the maximum or equal to 0
            // if the number of repetitions of the max value is bigger than a quarter of the total number of elements
            // there is an anomaly
            // This detector is used on clip.c
            // not implemented (not sure if this is a valid verification here), maybe set a base value (everything greater or lower would be an anomaly)
            // at current event begin, if previous event not succeed, there is an anomaly
            // this detector is used on processtrace.c
            // not implemented (on gateway scenarios, would be great)
            // using dynamic std_dev calc.
            // Welford's algorithm computes the sample variance incrementally.
            // https://stackoverflow.com/questions/5543651/computing-standard-deviation-in-a-stream
            // for each of the captures:
            // delta = cur_value - mean;
            // mean += delta / SNAPSHOTS;
            // M2 += delta * (cur_value - mean);
            // var = M2 / SNAPSHOTS;
            // std = squareRoot(var);
            // with a pre-defined hard-limit and soft limit
            // if curr_value out of (mean +/- std * (hard-limit or limit)): anomaly
            // this detector is used on spike.c

            // Third verification
            // Voter.c analysis the amount of anomalies on a channel (here Clerk_Monitor)
            // if this is greater than number of detectors / 5 (here, detectors are Clerks)

            unsigned int drift;
            unsigned int time_deviation;
            unsigned int delta;
            Data std;

            // first verification
            drift = (s.data - _average);
            drift *= drift;
            if(drift > AVERAGE_ACCEPTED_DRIFT * _average)
                return false;

            // second verification
            if((s.ts - _frequency) < 0)
                time_deviation = -1 * (s.ts - _frequency);
            else
                time_deviation = s.ts - _frequency;
            if(time_deviation > TIME_ACCEPTED_DRIFT * _frequency)
                return false;

            // third verification
            delta = s.data - _average;
            std = delta*(_average-(delta/_captures));
            std /= _captures;
            std = squareRoot(std);
            if((s.data > (_average + std * STD_DEV_ACCEPTED_DRIFT)) || (s.data < (_average - std * STD_DEV_ACCEPTED_DRIFT)))
                return false;
        }
        return true;
    }

    OStream & operator<<(OStream & os) {
        for(unsigned int i = 0; i < _captures; i++)
            os << _buffer[i].ts << "," << _buffer[i].data << endl;
        return os;
    }

private:
    Clerk * _clerk;
    Hertz _frequency;
    Time_Stamp _period;
    Time_Stamp _last_capture;
    Data _average;
    unsigned int _snapshots;
    Snapshot * _buffer;
    List::Element _link;
};


// Transducer Clerk
enum Transducer_Clerk_Events {
    TEMPERATURE
};

template<typename T>
class Clerk: private T
{
public:
    typedef typename T::Value Data;
    typedef Monitor::Hertz Hertz;

public:
    Clerk(const Transducer_Clerk_Events & event, unsigned int dev, const Hertz frequency = 0, bool monitored = false): _dev(dev), _monitor(monitored ? new (SYSTEM) Clerk_Monitor<Clerk>(this, frequency) : 0) {}
    ~Clerk() {}

    Data read() { return T::sense(_dev); }

    void start() {}
    void stop() {}
    void reset() {}

private:
    unsigned int _dev;
    Clerk_Monitor<Clerk> * _monitor;
};


// System Clerk
enum System_Clerk_Event {
    ELAPSED_TIME,
    DEADLINE_MISS,
    RUNNING_THREAD_ID,
    SYSTEM_CLOCK,
    CPU_TEMPERATURE,
    CPU_VOLTAGE
};

template<>
class Clerk<System>
{
public:
    typedef int Data;
    typedef TSC::Hertz Hertz;

public:
    Clerk(const System_Clerk_Event & event, const Hertz frequency = 0, bool monitored = false): _event(event), _monitor(monitored ? new (SYSTEM) Clerk_Monitor<Clerk>(this, frequency) : 0) {}
    ~Clerk() {}

    Data read() {
        switch(_event) {
        case ELAPSED_TIME:
            return Alarm::elapsed();
        case DEADLINE_MISS:
            return 0;
        default:
            return 0;
        }
    }

    void start() {}
    void stop() {}
    void reset() {}

private:
    System_Clerk_Event _event;
    Clerk_Monitor<Clerk> * _monitor;
};


#if defined(__PMU_H) && !defined(__common_only__)

// PMU Clerk
enum PMU_Clerk_Event {
    CLOCK                               = PMU_Common::CLOCK,
    DVS_CLOCK                           = PMU_Common::DVS_CLOCK,
    INSTRUCTION                         = PMU_Common::INSTRUCTION,
    BRANCH                              = PMU_Common::BRANCH,
    IMMEDIATE_BRANCH                    = PMU_Common::IMMEDIATE_BRANCH,
    UNALIGNED_LOAD_STORE                = PMU_Common::UNALIGNED_LOAD_STORE,
    BRANCH_MISS                         = PMU_Common::BRANCH_MISS,
    L1_HIT                              = PMU_Common::L1_HIT,
    L2_HIT                              = PMU_Common::L2_HIT,
    L3_HIT                              = PMU_Common::L3_HIT,
    LLC_HIT                             = PMU_Common::LLC_HIT,
    CACHE_HIT                           = PMU_Common::CACHE_HIT,
    L1_MISS                             = PMU_Common::L1_MISS,
    L2_MISS                             = PMU_Common::L2_MISS,
    L3_MISS                             = PMU_Common::L3_MISS,
    LLC_MISS                            = PMU_Common::LLC_MISS,
    CACHE_MISS                          = PMU_Common::CACHE_MISS,
    LLC_HITM                            = PMU_Common::LLC_HITM,
    DCACHE_STALL                        = PMU_Common::DCACHE_STALL,
    L1I_MISS                            = PMU_Common::L1I_MISS,
    L1I_TLB_MISS                        = PMU_Common::L1I_TLB_MISS,
    L1D_TLB_MISS                        = PMU_Common::L1D_TLB_MISS,
    EXCEPTION_TAKEN                     = PMU_Common::EXCEPTION_TAKEN,
    PREDICTABLE_BRANCH_EXECUTED         = PMU_Common::PREDICTABLE_BRANCH_EXECUTED,
    DATA_MEMORY_ACCESS                  = PMU_Common::DATA_MEMORY_ACCESS,
    L1I_HIT                             = PMU_Common::L1I_HIT,
    L1D_WRITEBACK                       = PMU_Common::L1D_WRITEBACK,
    L2D_WRITEBACK                       = PMU_Common::L2D_WRITEBACK,
    BUS_ACCESS                          = PMU_Common::BUS_ACCESS,
    CHAIN                               = PMU_Common::CHAIN,
    LOCAL_MEMORY_ERROR                  = PMU_Common::LOCAL_MEMORY_ERROR,
    INSTRUCTION_SPECULATIVELY_EXECUTED  = PMU_Common::INSTRUCTION_SPECULATIVELY_EXECUTED,
    BUS_CYCLE                           = PMU_Common::BUS_CYCLE,
    JAZELLE_BACKWARDS_BRANCHES_EXECUTED = PMU_Common::JAZELLE_BACKWARDS_BRANCHES_EXECUTED,
    COHERENT_LINEFILL_MISS              = PMU_Common::COHERENT_LINEFILL_MISS,
    COHERENT_LINEFILL_HIT               = PMU_Common::COHERENT_LINEFILL_HIT,
    ICACHE_DEPENDENT_STALL_CYCLES       = PMU_Common::ICACHE_DEPENDENT_STALL_CYCLES,
    DCACHE_DEPENDENT_STALL_CYCLES       = PMU_Common::DCACHE_DEPENDENT_STALL_CYCLES,
    MAIN_TLB_MISS_STALL_CYCLES          = PMU_Common::MAIN_TLB_MISS_STALL_CYCLES,
    STREX_PASSED                        = PMU_Common::STREX_PASSED,
    STREX_FAILED                        = PMU_Common::STREX_FAILED,
    DATA_EVICTION                       = PMU_Common::DATA_EVICTION,
    ISSUE_DOESNT_DISPATCH               = PMU_Common::ISSUE_DOESNT_DISPATCH,
    ISSUE_EMPTY                         = PMU_Common::ISSUE_EMPTY,
    ISSUE_CORE_RENAMING                 = PMU_Common::ISSUE_CORE_RENAMING,
    PREDICTABLE_FUNCTION_RETURNS        = PMU_Common::PREDICTABLE_FUNCTION_RETURNS,
    MAIN_EXECUTION_UNIT_RETURNS         = PMU_Common::MAIN_EXECUTION_UNIT_RETURNS,
    SECOND_EXECUTION_UNIT_RETURNS       = PMU_Common::SECOND_EXECUTION_UNIT_RETURNS,
    LOAD_STORE_INSTRUCTIONS             = PMU_Common::LOAD_STORE_INSTRUCTIONS,
    FLOATING_POINT_INSTRUCTIONS         = PMU_Common::FLOATING_POINT_INSTRUCTIONS,
    PROCESSOR_STALL_PLD                 = PMU_Common::PROCESSOR_STALL_PLD,
    PROCESSOR_STALL_WRITE_MEMORY        = PMU_Common::PROCESSOR_STALL_WRITE_MEMORY,
    PROCESSOR_STALL_ITLB_MISS           = PMU_Common::PROCESSOR_STALL_ITLB_MISS,
    PROCESSOR_STALL_DTLB_MISS           = PMU_Common::PROCESSOR_STALL_DTLB_MISS,
    PROCESSOR_STALL_IUTLB_MISS          = PMU_Common::PROCESSOR_STALL_IUTLB_MISS,
    PROCESSOR_STALL_DUTLB_MISS          = PMU_Common::PROCESSOR_STALL_DUTLB_MISS,
    PROCESSOR_STALL_DMB                 = PMU_Common::PROCESSOR_STALL_DMB,
    INTEGER_CLOCK_ENABLED               = PMU_Common::INTEGER_CLOCK_ENABLED,
    DATA_ENGINE_CLOCK_ENABLED           = PMU_Common::DATA_ENGINE_CLOCK_ENABLED,
    ISB_INSTRUCTIONS                    = PMU_Common::ISB_INSTRUCTIONS,
    DSB_INSTRUCTIONS                    = PMU_Common::DSB_INSTRUCTIONS,
    DMB_INSTRUCTIONS                    = PMU_Common::DMB_INSTRUCTIONS,
    EXTERNAL_INTERRUPTS                 = PMU_Common::EXTERNAL_INTERRUPTS,
    PLE_CACHE_LINE_REQUEST_COMPLETED    = PMU_Common::PLE_CACHE_LINE_REQUEST_COMPLETED,
    PLE_CACHE_LINE_REQUEST_SKIPPED      = PMU_Common::PLE_CACHE_LINE_REQUEST_SKIPPED,
    PLE_FIFO_FLUSH                      = PMU_Common::PLE_FIFO_FLUSH,
    PLE_REQUEST_COMPLETED               = PMU_Common::PLE_REQUEST_COMPLETED,
    PLE_FIFO_OVERFLOW                   = PMU_Common::PLE_FIFO_OVERFLOW,
    PLE_REQUEST_PROGRAMMED              = PMU_Common::PLE_REQUEST_PROGRAMMED,
    EVENTS                              = PMU_Common::EVENTS
};

template<>
class Clerk<PMU>: private PMU
{
public:
    using PMU::CHANNELS;
    typedef PMU::Count Data;
    typedef TSC::Hertz Hertz;

public:
    Clerk(const PMU_Clerk_Event & event, const Hertz frequency = 0, bool monitored = false) {
        for(_channel = 0; _in_use[CPU::id()][_channel]; _channel++);
        if(_channel != CHANNELS) {
            _in_use[CPU::id()][_channel] = true;
            PMU::config(_channel, reinterpret_cast<const Event &>(event));

            if(monitored)
                new (SYSTEM) Clerk_Monitor<Clerk>(this, frequency);
        }
    }
    ~Clerk() {
        if(_channel < CHANNELS) {
            PMU::stop(_channel);
            _in_use[CPU::id()][_channel] = false;
        }
    }

    Data read() { return (_channel < CHANNELS) ? PMU::read(_channel) : 0; }
    void start() { if(_channel < CHANNELS) PMU::start(_channel); }
    void stop() { if(_channel < CHANNELS) PMU::stop(_channel); }
    void reset() { if(_channel < CHANNELS) PMU::reset(_channel); }

private:
    Channel _channel;

    static bool _in_use[Traits<Build>::CPUS][CHANNELS];
};

#endif

__END_SYS

#endif
