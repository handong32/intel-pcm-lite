//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef _RAPL_H_
#define _RAPL_H_

#include <math.h>

#include "Msr.h"

namespace rapl {
  const constexpr uint32_t kMsrIntelRaplPowerUnit = 0x606;

  /* Package RAPL Domain */
  const constexpr uint32_t kMsrPkgRaplPowerLimit = 0x610;
  const constexpr uint32_t kMsrIntelPkgEnergyStatus = 0x611;
  const constexpr uint32_t kMsrPkgPerfStatus = 0x613;
  const constexpr uint32_t kMsrPkgPowerInfo = 0x614;

  /* PP0 RAPL Domain */
  const constexpr uint32_t kMsrPp0PowerLimit = 0x638;
  const constexpr uint32_t kMsrIntelPp0EnergyStatus = 0x639;
  const constexpr uint32_t kMsrPp0Policy = 0x63A;
  const constexpr uint32_t kMsrPp0PerfStatus = 0x63B;

  /* PP1 RAPL Domain, may reflect to uncore devices */
  const constexpr uint32_t kMsrPp1PowerLimit = 0x640;
  const constexpr uint32_t kMsrPp1EnergyStatus = 0x641;
  const constexpr uint32_t kMsrPp1Polcy = 0x642;

  /* DRAM RAPL Domain */
  const constexpr uint32_t kMsrDramPowerLimit = 0x618;
  const constexpr uint32_t kMsrDramEnergyStatus = 0x619;
  const constexpr uint32_t kMsrDramPerfStatus = 0x61B;
  const constexpr uint32_t kMsrDramPowerInfo = 0x61C;

  /* PSYS RAPL Domain */
  const constexpr uint32_t kMsrPlatformEnergyStatus = 0x64d;
  
  /* RAPL UNIT BITMASK */
  const constexpr uint32_t POWER_UNIT_OFFSET = 0;
  const constexpr uint32_t POWER_UNIT_MASK = 0x0F;

  const constexpr uint32_t ENERGY_UNIT_OFFSET = 0x08;
  const constexpr uint32_t ENERGY_UNIT_MASK = 0x1F00;

  const constexpr uint32_t TIME_UNIT_OFFSET = 0x10;
  const constexpr uint32_t TIME_UNIT_MASK = 0xF000;

  class RaplCounter { 
  public:
    RaplCounter();
    
    // move constructors
    RaplCounter(RaplCounter&& other) {
      rapl_power_units = other.rapl_power_units;
      rapl_cpu_energy_units = other.rapl_cpu_energy_units;
      rapl_time_units = other.rapl_time_units;
      rapl_dram_energy_units = other.rapl_dram_energy_units;

      other.rapl_power_units = 0.0;
      other.rapl_cpu_energy_units = 0.0;
      other.rapl_time_units = 0.0;
      other.rapl_dram_energy_units = 0.0;
    };
    RaplCounter& operator=(RaplCounter&& other) {
      rapl_power_units = other.rapl_power_units;
      rapl_cpu_energy_units = other.rapl_cpu_energy_units;
      rapl_time_units = other.rapl_time_units;
      rapl_dram_energy_units = other.rapl_dram_energy_units;

      other.rapl_power_units = 0.0;
      other.rapl_cpu_energy_units = 0.0;
      other.rapl_time_units = 0.0;
      other.rapl_dram_energy_units = 0.0;
      
      return *this;
    };

    ~RaplCounter() {
      return;
    }

    // delete implicitly created copy constructor
    RaplCounter(const RaplCounter& other) = delete;
    RaplCounter& operator=(const RaplCounter& other) = delete;
    
    void Clear();
    void Start();
    void Stop();
    
    /*

      

    void SetLimit(uint32_t v) {
      uint64_t result = ebbrt::msr::Read(kMsrPkgRaplPowerLimit);
      uint64_t m = 0x7FFF;
      uint32_t npower = (uint32_t)(v / 0.125);
      
      // resetting values
      result = result & (~m);
      result = result & (~(m << 32));
      
      // new power
      result = result | npower;
      result = result | ((uint64_t)npower << 32);

      // set clamp
      result |= 1LL << 15;
      result |= 1LL << 16;
      result |= 1LL << 47;
      result |= 1LL << 48;

      uint32_t low = result & 0xFFFFFFFF;
      uint32_t high = (result >> 32) & 0xFFFFFFFF;
      asm volatile("wrmsr" : : "c"(kMsrPkgRaplPowerLimit), "a"(low), "d"(high));
      
      result=ebbrt::msr::Read(kMsrPkgRaplPowerLimit);
      ebbrt::kprintf("%u Package power limits are %s\n", v, (result >> 63) ? "locked" : "unlocked");
      double pkg_power_limit_1 = rapl_power_units*(double)((result>>0)&0x7FFF);
      double pkg_time_window_1 = rapl_time_units*(double)((result>>17)&0x007F);
      ebbrt::kprintf("Package power limit #1: %.3fW for %.6fs (%s, %s)\n",
	     pkg_power_limit_1, pkg_time_window_1,
	     (result & (1LL<<15)) ? "enable power limit" : "disabled",
	     (result & (1LL<<16)) ? "clamped" : "not_clamped");
      double pkg_power_limit_2 = rapl_power_units*(double)((result>>32)&0x7FFF);
      double pkg_time_window_2 = rapl_time_units*(double)((result>>49)&0x007F);
      ebbrt::kprintf("Package power limit #2: %.3fW for %.6fs (%s, %s)\n", 
	     pkg_power_limit_2, pkg_time_window_2,
	     (result & (1LL<<47)) ? "enable power limit" : "disabled",
	     (result & (1LL<<48)) ? "clamped" : "not_clamped");
    }
    */
    double Read();
    
  private:
    double rapl_power_units{0.0};
    double rapl_cpu_energy_units{0.0};
    double rapl_time_units{0.0};
    double rapl_dram_energy_units{0.0};
    double counter_offset{0.0};
  };
  
}  // namespace rapl

#endif
