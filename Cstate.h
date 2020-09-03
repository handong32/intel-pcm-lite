#ifndef _CSTATE_H_
#define _CSTATE_H_

#include <math.h>
#include "Msr.h"

namespace cstate {

  const constexpr uint32_t MSR_CORE_C3_RESIDENCY = 0x3FC;
  const constexpr uint32_t MSR_CORE_C6_RESIDENCY = 0x3FD;
  const constexpr uint32_t MSR_CORE_C7_RESIDENCY = 0x3FE;
  
} // namespace cstate

#endif
