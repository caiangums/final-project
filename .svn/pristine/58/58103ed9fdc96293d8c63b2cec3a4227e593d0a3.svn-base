// EPOS ARMv8 PMU Mediator Implementation

#include <architecture/armv8/pmu.h>

__BEGIN_SYS

// Class attributes
const CPU::Reg32 ARMv8_A_PMU::_events[EVENTS] = {
    // Architecture                             // API
    CYCLE,                                      // CLOCK
    0,                                          // DVS_CLOCK
    ISSUE_CORE_RENAMING,                        // INSTRUCTION
    BRANCHES_ARCHITECTURALLY_EXECUTED,          // BRANCH
    MISPREDICTED_BRANCH,                        // BRANCH_MISS
    L1D_ACCESS,                                 // L1_HIT
    0,                                          // L2_HIT
    0,                                          // L3_HIT
    L1D_REFILL,                                 // L1_MISS
    0,                                          // L2_MISS
    0,                                          // L3_MISS
};

__END_SYS
