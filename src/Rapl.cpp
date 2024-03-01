#include "Rapl.h"

rapl::RaplCounter::RaplCounter() {
  int fd;
  uint64_t res;
    
  fd = open_msr(sched_getcpu());
  res = read_msr(fd, kMsrIntelRaplPowerUnit);
  close(fd);
  
  rapl_power_units = pow(0.5,(double)(res&0xf));
  rapl_cpu_energy_units = pow(0.5,(double)((res>>8)&0x1f));
  rapl_time_units = pow(0.5,(double)((res>>16)&0xf));
  rapl_dram_energy_units = rapl_cpu_energy_units;

  /*printf("Rapl Power Units:\n");
  printf("\tPower units = %.6f W\n",rapl_power_units);
  printf("\tCPU Energy units = %.8f J\n",rapl_cpu_energy_units);
  printf("\tDRAM Energy units = %.8f J\n",rapl_dram_energy_units);
  printf("\tTime units = %.8f s\n",rapl_time_units);
  */
}

void rapl::RaplCounter::Start() {
  int fd;
  uint64_t res;

  // CPU package energy
  fd = open_msr(sched_getcpu());
  res = read_msr(fd, kMsrIntelPkgEnergyStatus);
  close(fd);  
  pkg_counter_offset = (double)res*rapl_cpu_energy_units;

  fd = open_msr(sched_getcpu());
  res = read_msr(fd, kMsrDramEnergyStatus);
  close(fd);  
  dram_counter_offset = (double)res*rapl_dram_energy_units;
}

void rapl::RaplCounter::Stop() {
  int fd;
  uint64_t res;
  double after1, after2;
  
  // CPU package energy
  fd = open_msr(sched_getcpu());
  res = read_msr(fd, kMsrIntelPkgEnergyStatus);
  close(fd);
  after1 = (double)res*rapl_cpu_energy_units;
  pkg_counter_offset = after1 - pkg_counter_offset;

  // DRAM package energy
  fd = open_msr(sched_getcpu());
  res = read_msr(fd, kMsrDramEnergyStatus);
  close(fd);
  after2 = (double)res*rapl_dram_energy_units;
  dram_counter_offset = after2 - dram_counter_offset;
}

void rapl::RaplCounter::Clear() {
  pkg_counter_offset = 0.0;
  dram_counter_offset = 0.0;
}

double rapl::RaplCounter::ReadPkg() {
  return pkg_counter_offset;
}

double rapl::RaplCounter::ReadDram() {
  return dram_counter_offset;
}
