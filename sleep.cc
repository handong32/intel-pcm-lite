#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <thread>         // std::thread

#include "Perf.h"
#include "Rapl.h"
#include "Cstate.h"

int main (int argc, char * argv[]) {  
  if (argc!=2) {printf("usage: main <iters> \n"); return -1;}
  uint64_t n = atoi(argv[1]);
  int fd;
  int cpu = sched_getcpu();
  
  printf("Sleep %.2f seconds, cpu: %d\n", (float)n/1000.0, cpu);
  
  std::unordered_map<std::string, perf::PerfCounter> counters;
  perf::PerfCounter ins_retired = perf::PerfCounter{perf::PerfEvent::fixed_instructions};
  perf::PerfCounter unhalted_cyc = perf::PerfCounter{perf::PerfEvent::fixed_cycles};
  perf::PerfCounter unhalted_ref_cyc_tsc = perf::PerfCounter{perf::PerfEvent::fixed_reference_cycles};
  
  fd = open_msr(cpu);
  
  uint64_t tsc_start = rdtsc();
  ins_retired.Start();
  unhalted_cyc.Start();
  unhalted_ref_cyc_tsc.Start();
  
  uint64_t c3_start = read_msr(fd, cstate::MSR_CORE_C3_RESIDENCY);
  uint64_t c6_start = read_msr(fd, cstate::MSR_CORE_C6_RESIDENCY);
  uint64_t c7_start = read_msr(fd, cstate::MSR_CORE_C7_RESIDENCY);  

  // sleep
  std::this_thread::sleep_for(std::chrono::milliseconds(n));
  
  uint64_t c3_end = read_msr(fd, cstate::MSR_CORE_C3_RESIDENCY);
  uint64_t c6_end = read_msr(fd, cstate::MSR_CORE_C6_RESIDENCY);
  uint64_t c7_end = read_msr(fd, cstate::MSR_CORE_C7_RESIDENCY);
  close(fd);
  
  ins_retired.Stop();
  unhalted_cyc.Stop();
  unhalted_ref_cyc_tsc.Stop();
  
  uint64_t ins = ins_retired.Read();
  uint64_t CPU_CLK_UNHALTED_CORE = unhalted_cyc.Read();
  uint64_t CPU_CLK_UNHALTED_REF = unhalted_ref_cyc_tsc.Read();
  
  uint64_t tsc_stop = rdtsc();
  uint64_t tsc_diff = tsc_stop - tsc_start;
    
  uint64_t CPU_CLK_C3 = c3_start > c3_end ? ((UINT64_MAX - c3_start) + c3_end) : (c3_end - c3_start);
  uint64_t CPU_CLK_C6 = c6_start > c6_end ? ((UINT64_MAX - c6_start) + c6_end) : (c6_end - c6_start);
  uint64_t CPU_CLK_C7 = c7_start > c7_end ? ((UINT64_MAX - c7_start) + c7_end) : (c7_end - c7_start);

  printf("INS \t %20lu\n", ins);
  printf("CPU_CLK_UNHALTED_CORE \t %20lu\n", CPU_CLK_UNHALTED_CORE);
  printf("CPU_CLK_UNHALTED_REF \t %20lu\n", CPU_CLK_UNHALTED_REF);
  printf("CPU_CLK_C3 \t %20lu\n", CPU_CLK_C3);
  printf("CPU_CLK_C6 \t %20lu\n", CPU_CLK_C6);
  printf("CPU_CLK_C7 \t %20lu\n", CPU_CLK_C7);
  printf("TSC_DIFF \t %20lu\n", tsc_diff);

  float c0_time = (float) CPU_CLK_UNHALTED_REF / tsc_diff;
  float c3_time = (float) CPU_CLK_C3 / tsc_diff;
  float c6_time = (float) CPU_CLK_C6 / tsc_diff;
  float c7_time = (float) CPU_CLK_C7 / tsc_diff;  

  printf("C0_time \t %20.4f\n", c0_time);
  printf("C3_time \t %20.4f\n", c3_time);
  printf("C6_time \t %20.4f\n", c6_time);
  printf("C7_time \t %20.4f\n", c7_time);
  
  return 0; 
}
