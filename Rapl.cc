#include "Rapl.h"

rapl::RaplCounter::RaplCounter() {
  int fd;
  uint64_t res;
    
  fd = open_msr(sched_getcpu());
  //res = ebbrt::msr::Read(kMsrIntelRaplPowerUnit);
  res = read_msr(fd, kMsrIntelRaplPowerUnit);
  close(fd);
  
  rapl_power_units = pow(0.5,(double)(res&0xf));
  rapl_cpu_energy_units = pow(0.5,(double)((res>>8)&0x1f));
  rapl_time_units = pow(0.5,(double)((res>>16)&0xf));
  rapl_dram_energy_units = rapl_cpu_energy_units;

  printf("Rapl Power Units:\n");
  printf("\tPower units = %.6f W\n",rapl_power_units);
  printf("\tCPU Energy units = %.8f J\n",rapl_cpu_energy_units);
  printf("\tDRAM Energy units = %.8f J\n",rapl_dram_energy_units);
  printf("\tTime units = %.8f s\n",rapl_time_units);
}

void rapl::RaplCounter::Start() {
  int fd;
  uint64_t res;
    
  fd = open_msr(sched_getcpu());
  //uint64_t res = ebbrt::msr::Read(kMsrIntelPkgEnergyStatus);
  res = read_msr(fd, kMsrIntelPkgEnergyStatus);
  close(fd);
  
  counter_offset = (double)res*rapl_cpu_energy_units;
}

void rapl::RaplCounter::Stop() {
  int fd;
  uint64_t res;

  fd = open_msr(sched_getcpu());
  //res = ebbrt::msr::Read(kMsrIntelPkgEnergyStatus);
  res = read_msr(fd, kMsrIntelPkgEnergyStatus);
  close(fd);

  double after = (double)res*rapl_cpu_energy_units;
  counter_offset = after - counter_offset;
}

void rapl::RaplCounter::Clear() {
  counter_offset = 0.0;
}

double rapl::RaplCounter::Read() {
  return counter_offset;
}
