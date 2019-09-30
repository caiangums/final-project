// EPOS ARMv7 PMU Mediator Implementation

#include <architecture/armv7/pmu.h>

#ifdef __mach_cortex_a__

__BEGIN_SYS

// Class attributes
const CPU::Reg32 ARMv7_A_PMU::_events[EVENTS] = {
    // Architecture                             // API
    CYCLE,                                      // CLOCK
    0,                                          // DVS_CLOCK
    INSTRUCTIONS_ARCHITECTURALLY_EXECUTED,      // INSTRUCTION
    BRANCHES_ARCHITECTURALLY_EXECUTED,          // BRANCH
    IMMEDIATE_BRANCH,                           // IMMEDIATE_BRANCH
    UNALIGNED_LOAD_STORE,                       // UNALIGNED_LOAD_STORE
    MISPREDICTED_BRANCH,                        // BRANCH_MISS
    L1D_ACCESS,                                 // L1_HIT
    L2D_ACCESS,                                 // L2_HIT
    0,                                          // L3_HIT
    L1D_REFILL,                                 // L1_MISS
    L2D_REFILL,                                 // L2_MISS
    0,                                          // L3_MISS
    0,                                          // LLC_HITIM
    DCACHE_DEPENDENT_STALL_CYCLES,              // DCACHE_STALL
    L1I_REFILL,                                 // L1I_MISS,
    L1I_TLB_REFILL,                             // L1I_TLB_MISS,
    L1D_TLB_REFILL,                             // L1D_TLB_MISS,
    EXCEPTION_TAKEN,                            // EXCEPTION_TAKEN,
    PREDICTABLE_BRANCH_EXECUTED,                // PREDICTABLE_BRANCH_EXECUTED,
    DATA_MEMORY_ACCESS,                         // DATA_MEMORY_ACCESS,
    L1I_ACCESS,                                 // L1I_HIT,
    L1D_WRITEBACK,                              // L1D_WRITEBACK,
    L2D_WRITEBACK,                              // L2D_WRITEBACK,
    BUS_ACCESS,                                 // BUS_ACCESS,
    LOCAL_MEMORY_ERROR,                         // LOCAL_MEMORY_ERROR,
    INSTRUCTION_SPECULATIVELY_EXECUTED,         // INSTRUCTION_SPECULATIVELY_EXECUTED,
    BUS_CYCLE,                                  // BUS_CYCLE,
    CHAIN,                                      // CHAIN

    // Cortex-A specific events
    JAZELLE_BACKWARDS_BRANCHES_EXECUTED,        // JAZELLE_BACKWARDS_BRANCHES_EXECUTED
    COHERENT_LINEFILL_MISS,                     // COHERENT_LINEFILL_MISS,
    COHERENT_LINEFILL_HIT,                      // COHERENT_LINEFILL_HIT,
    ICACHE_DEPENDENT_STALL_CYCLES,              // ICACHE_DEPENDENT_STALL_CYCLES,
    DCACHE_DEPENDENT_STALL_CYCLES,              // DCACHE_DEPENDENT_STALL_CYCLES,
    MAIN_TLB_MISS_STALL_CYCLES,                 // MAIN_TLB_MISS_STALL_CYCLES,
    STREX_PASSED,                               // STREX_PASSED,
    STREX_FAILED,                               // STREX_FAILED,
    DATA_EVICTION,                              // DATA_EVICTION,
    ISSUE_DOESNT_DISPATCH,                      // ISSUE_DOESNT_DISPATCH,
    ISSUE_EMPTY,                                // ISSUE_EMPTY,
    ISSUE_CORE_RENAMING,                        // ISSUE_CORE_RENAMING,
    PREDICTABLE_FUNCTION_RETURNS,               // PREDICTABLE_FUNCTION_RETURNS,
    MAIN_EXECUTION_UNIT_RETURNS,                // MAIN_EXECUTION_UNIT_RETURNS,
    SECOND_EXECUTION_UNIT_RETURNS,              // SECOND_EXECUTION_UNIT_RETURNS,
    LOAD_STORE_INSTRUCTIONS,                    // LOAD_STORE_INSTRUCTIONS,
    FLOATING_POINT_INSTRUCTIONS,                // FLOATING_POINT_INSTRUCTIONS,
    PROCESSOR_STALL_PLD,                        // PROCESSOR_STALL_PLD,
    PROCESSOR_STALL_WRITE_MEMORY,               // PROCESSOR_STALL_WRITE_MEMORY,
    PROCESSOR_STALL_ITLB_MISS,                  // PROCESSOR_STALL_ITLB_MISS,
    PROCESSOR_STALL_DTLB_MISS,                  // PROCESSOR_STALL_DTLB_MISS,
    PROCESSOR_STALL_IUTLB_MISS,                 // PROCESSOR_STALL_IUTLB_MISS,
    PROCESSOR_STALL_DUTLB_MISS,                 // PROCESSOR_STALL_DUTLB_MISS,
    PROCESSOR_STALL_DMB,                        // PROCESSOR_STALL_DMB,
    INTEGER_CLOCK_ENABLED,                      // INTEGER_CLOCK_ENABLED,
    DATA_ENGINE_CLOCK_ENABLED,                  // DATA_ENGINE_CLOCK_ENABLED,
    ISB_INSTRUCTIONS,                           // ISB_INSTRUCTIONS,
    DSB_INSTRUCTIONS,                           // DSB_INSTRUCTIONS,
    DMB_INSTRUCTIONS,                           // DMB_INSTRUCTIONS,
    EXTERNAL_INTERRUPTS,                        // EXTERNAL_INTERRUPTS,
    PLE_CACHE_LINE_REQUEST_COMPLETED,           // PLE_CACHE_LINE_REQUEST_COMPLETED,
    PLE_CACHE_LINE_REQUEST_SKIPPED,             // PLE_CACHE_LINE_REQUEST_SKIPPED,
    PLE_FIFO_FLUSH,                             // PLE_FIFO_FLUSH,
    PLE_REQUEST_COMPLETED,                      // PLE_REQUEST_COMPLETED,
    PLE_FIFO_OVERFLOW,                          // PLE_FIFO_OVERFLOW,
    PLE_REQUEST_PROGRAMMED,                     // PLE_REQUEST_PROGRAMMED,
};

__END_SYS

#endif
