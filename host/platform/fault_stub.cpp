// Host fault backend. The RP2 hard-fault capture machinery (watchdog
// scratch registers, stack guards) has no equivalent here — the OS
// delivers crashes via signals. Keep the API, report "never faulted".

#include "platform/fault.hpp"

namespace platform {

bool take_prior_fault(FaultInfo*) {
    return false;
}

void clear_fault_streak() {}

void paint_stack() {}

uint32_t stack_peak_used() {
    return 0;
}

uint32_t stack_total() {
    return 0;
}

}  // namespace platform
