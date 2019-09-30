// EPOS PMU Mediator Common Package

#ifndef __pmu_h
#define __pmu_h

#include <system/config.h>

__BEGIN_SYS

class PMU_Common
{
public:
    typedef unsigned int Channel;
    typedef long long int Count;

    enum Event {
        CLOCK,
        DVS_CLOCK,
        INSTRUCTION,
        BRANCH,
        IMMEDIATE_BRANCH,
        UNALIGNED_LOAD_STORE,
        BRANCH_MISS,
        L1_HIT,
        L2_HIT,
        L3_HIT,
        LLC_HIT = L3_HIT,
        CACHE_HIT = LLC_HIT,
        L1_MISS,
        L2_MISS,
        L3_MISS,
        LLC_MISS = L3_MISS,
        CACHE_MISS = LLC_MISS,
        LLC_HITM,
        DCACHE_STALL,
        // Cortex-A specific events
        L1I_MISS,
        L1I_TLB_MISS,
        L1D_TLB_MISS,
        EXCEPTION_TAKEN,
        PREDICTABLE_BRANCH_EXECUTED,
        DATA_MEMORY_ACCESS,
        L1I_HIT,
        L1D_WRITEBACK,
        L2D_WRITEBACK,
        BUS_ACCESS,
        LOCAL_MEMORY_ERROR,
        INSTRUCTION_SPECULATIVELY_EXECUTED,
        BUS_CYCLE,
        CHAIN,
        JAZELLE_BACKWARDS_BRANCHES_EXECUTED,
        COHERENT_LINEFILL_MISS,
        COHERENT_LINEFILL_HIT,
        ICACHE_DEPENDENT_STALL_CYCLES,
        DCACHE_DEPENDENT_STALL_CYCLES,
        MAIN_TLB_MISS_STALL_CYCLES,
        STREX_PASSED,
        STREX_FAILED,
        DATA_EVICTION,
        ISSUE_DOESNT_DISPATCH,
        ISSUE_EMPTY,
        ISSUE_CORE_RENAMING,
        PREDICTABLE_FUNCTION_RETURNS,
        MAIN_EXECUTION_UNIT_RETURNS,
        SECOND_EXECUTION_UNIT_RETURNS,
        LOAD_STORE_INSTRUCTIONS,
        FLOATING_POINT_INSTRUCTIONS,
        PROCESSOR_STALL_PLD,
        PROCESSOR_STALL_WRITE_MEMORY,
        PROCESSOR_STALL_ITLB_MISS,
        PROCESSOR_STALL_DTLB_MISS,
        PROCESSOR_STALL_IUTLB_MISS,
        PROCESSOR_STALL_DUTLB_MISS,
        PROCESSOR_STALL_DMB,
        INTEGER_CLOCK_ENABLED,
        DATA_ENGINE_CLOCK_ENABLED,
        ISB_INSTRUCTIONS,
        DSB_INSTRUCTIONS,
        DMB_INSTRUCTIONS,
        EXTERNAL_INTERRUPTS,
        PLE_CACHE_LINE_REQUEST_COMPLETED,
        PLE_CACHE_LINE_REQUEST_SKIPPED,
        PLE_FIFO_FLUSH,
        PLE_REQUEST_COMPLETED,
        PLE_FIFO_OVERFLOW,
        PLE_REQUEST_PROGRAMMED,
        EVENTS
    };

    enum Flags {
        NONE,
        INT
    };

protected:
    PMU_Common() {}
};

__END_SYS

#endif

#if defined(__PMU_H) && !defined(__common_only__)
#include __PMU_H
#endif
