#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cmath>

#include "Perf.h"
#include "Rapl.h"

#define UINT32_MAXT 0xffffffff
#define TIME_CONVERSION_khz 2899999*1000

int main () {
  std::unordered_map<std::string, perf::PerfCounter> counters;
  perf::PerfCounter ins_retired = perf::PerfCounter{perf::PerfEvent::fixed_instructions};
  perf::PerfCounter unhalted_ref_cyc_tsc = perf::PerfCounter{perf::PerfEvent::fixed_reference_cycles};
  perf::PerfCounter llc_miss = perf::PerfCounter{perf::PerfEvent::llc_misses};
  
  ins_retired.Start();
  unhalted_ref_cyc_tsc.Start();
  llc_miss.Start();
  rapl::RaplCounter rp = rapl::RaplCounter();
  rp.Start();
  
  uint64_t tsc_start = rdtsc();
	
  volatile uint64_t ans, j;
  ans = j = 0;
  for(j=0;j<(UINT32_MAXT/2);j++) {
    ans ++;
  }
  
  rp.Stop();        
  ins_retired.Stop();
  unhalted_ref_cyc_tsc.Stop();
  llc_miss.Stop();

  float nrg = rp.Read();
  uint64_t ins = ins_retired.Read();
  uint64_t ref_cyc = unhalted_ref_cyc_tsc.Read();
  uint64_t llcm = llc_miss.Read();
  
  uint64_t tsc_stop = rdtsc();
  uint64_t tsc_diff = tsc_stop - tsc_start;
  float tdiff = (tsc_diff/(float)TIME_CONVERSION_khz)/1000000.0;
  
  rp.Clear();
  printf("%lu,%.3lf,%.3lf,%lu,%lu,%lu\n", ans, nrg, tdiff, ins, ref_cyc, llcm);
  
  return 0; 
}
