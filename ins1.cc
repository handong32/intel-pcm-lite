#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cmath>

#include "Perf.h"
#include "Rapl.h"

#define UINT32_MAXT 0xffffffff
#define TIME_CONVERSION_khz 2899999*1000

/*
 * To compile use the following, use the command:" gcc -o my_pi pi.c -lm"
 * Assuming you are using gcc compiler and the file name is pi.c
 */
inline double calc1(int num1)
{
  return (16/(num1 * pow(5.0,num1 * 1.0)));
}
inline double calc2(int num1)
{
  return (4/(num1 * pow(239.0,num1 * 1.0)));
}

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
	
  uint32_t j = 0; /* The counter for the loop */
  double ans1 = 0.0;
  double ans2 = 0.0;
  double ans3 = 0; /* The final answer*/
  int flag = 1; /* 1 means addition and 0 means subtraction */
  for(j = 1; j <= (UINT32_MAXT/50) ; j+= 2){
    if(flag == 1) {
      ans1 += calc1(j);
      ans2 += calc2(j);
      flag = 0;
    } else {
      ans1 -= calc1(j);
      ans2 -= calc2(j);
      flag = 1;
    }
  }
  ans3 = ans1 - ans2;
    
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
  printf("%.5lf,%.3lf,%.3lf,%lu,%lu,%lu\n", ans3, nrg, tdiff, ins, ref_cyc, llcm);
  
  return 0; 
}
