//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
#include <stdio.h>
#include <stdlib.h>

#include "Cpuid.h"
#include "Msr.h"
#include "Perf.h"

namespace {
thread_local uint8_t pmcs;
};

thread_local uint64_t perf::perf_global_ctrl;

perf::PerfCounter::~PerfCounter() {
  int fd;
  if (evt_ == PerfEvent::nil_event) {
    return;
  }

  fd = open_msr(sched_getcpu());
  
  // stop counter
  perf_global_ctrl &= ~(1ull << pmc_num_);
  //msr::Write(kIa32PerfGlobalCtrlMsr, perf_global_ctrl);
  if (pwrite(fd, &perf_global_ctrl, sizeof(perf_global_ctrl), kIa32PerfGlobalCtrlMsr)
	 != sizeof(perf_global_ctrl) ) {
    printf("%d Error writing MSR %x\n", __LINE__, kIa32PerfGlobalCtrlMsr);
    exit(127);
  }
  
  // disable counter
  FixedCtrCtrl fixed_ctrl;
  PerfEvtSelMsr perfevtsel;
  perfevtsel.val = 0;

  //fixed_ctrl.val |= msr::Read(kIa32FixedCtrCtrlMsr);
  fixed_ctrl.val |= read_msr(fd, kIa32FixedCtrCtrlMsr);

  switch (evt_) {
  case PerfEvent::nil_event:
    close(fd);
    return;
  case PerfEvent::fixed_instructions:
    fixed_ctrl.ctr0_enable = kIa32FixedCtlRingLvlNone;
    //msr::Write(kIa32FixedCtrCtrlMsr, fixed_ctrl.val);
    if (pwrite(fd, &fixed_ctrl.val, sizeof(fixed_ctrl.val), kIa32FixedCtrCtrlMsr)
	!= sizeof(fixed_ctrl.val)) {
      //printf("%d Error writing MSR %x\n", __LINE__, kIa32FixedCtrCtrlMsr);
      //exit(127);
    }
    break;
  case PerfEvent::fixed_cycles:
    fixed_ctrl.ctr1_enable = kIa32FixedCtlRingLvlNone;
    //msr::Write(kIa32FixedCtrCtrlMsr, fixed_ctrl.val);
    if (pwrite(fd, &fixed_ctrl.val, sizeof(fixed_ctrl.val), kIa32FixedCtrCtrlMsr)
	!= sizeof(fixed_ctrl.val) ) {
      //printf("%d Error writing MSR %x\n", __LINE__, kIa32FixedCtrCtrlMsr);
      //exit(127);
    }
    break;
  case PerfEvent::fixed_reference_cycles:
    fixed_ctrl.ctr2_enable = kIa32FixedCtlRingLvlNone;
    //msr::Write(kIa32FixedCtrCtrlMsr, fixed_ctrl.val);
    if (pwrite(fd, &fixed_ctrl.val, sizeof(fixed_ctrl.val), kIa32FixedCtrCtrlMsr)
	!= sizeof(fixed_ctrl.val) ) {
      //printf("%d Error writing MSR %x\n", __LINE__, kIa32FixedCtrCtrlMsr);
      //exit(127);
    }
    break;
  default:
    perfevtsel.usermode = 0;
    perfevtsel.osmode = 0;
    perfevtsel.en = 0;
    //msr::Write(kIa32PerfEvtSelMsr(pmc_num_), perfevtsel.val);
    if (pwrite(fd, &perfevtsel.val, sizeof(perfevtsel.val), kIa32PerfEvtSelMsr(pmc_num_))
	!= sizeof(perfevtsel.val) ) {
      //printf("%d Error writing MSR %x\n", __LINE__, kIa32PerfEvtSelMsr(pmc_num_));
      //exit(127);
    }
    pmcs &= ~(1ull << pmc_num_);
    break;
  }

  close(fd);
}

perf::PerfCounter::PerfCounter(perf::PerfEvent evt) : evt_{evt} {
  auto r = cpuid::Cpuid(kPerfCpuLeaf);
  auto pmc_version = r.eax & 0xFF;
  auto pmc_count = (r.eax >> 8) & 0xFF;
  auto pmc_events = (r.ebx) & 0xFF;
  auto evt_num = static_cast<uint8_t>(evt_);
  int fd;

  fd = open_msr(sched_getcpu());
  
  // check if PMC are supported on this architecture
  if (pmc_version == 0) {
    printf("Warning: performance monitoring counters are not supported.\n");
    evt_ = PerfEvent::nil_event;
  } else if (((pmc_events >> evt_num) & 0x1) == 1) {
    printf("Warning: event type (#) is not supported.\n");
    evt_ = PerfEvent::nil_event;
  }

  FixedCtrCtrl fixed_ctrl;
  PerfEvtSelMsr perfevtsel;
  perfevtsel.val = 0;
  fixed_ctrl.val = 0;

  switch (evt_) {
  case PerfEvent::tlb_store_misses:
    perfevtsel.event = kPerfEvtSelEvtTlbStoreMisses;
    perfevtsel.umask = kPerfEvtSelUmaskTlbStoreMisses;
    break;
  case PerfEvent::tlb_load_misses:
    perfevtsel.event = kPerfEvtSelEvtTlbLoadMisses;
    perfevtsel.umask = kPerfEvtSelUmaskTlbLoadMisses;
    break;
  case PerfEvent::cycles:
    perfevtsel.event = kPerfEvtSelEvtCycles;
    perfevtsel.umask = kPerfEvtSelUmaskCycles;
    break;
  case PerfEvent::instructions:
    perfevtsel.event = kPerfEvtSelEvtInstructions;
    perfevtsel.umask = kPerfEvtSelUmaskInstructions;
    break;
  case PerfEvent::reference_cycles:
    perfevtsel.event = kPerfEvtSelEvtCyclesRef;
    perfevtsel.umask = kPerfEvtSelUmaskCyclesRef;
    break;
  case PerfEvent::llc_references:
    perfevtsel.event = kPerfEvtSelEvtLLCRef;
    perfevtsel.umask = kPerfEvtSelUmaskLLCRef;
    break;
  case PerfEvent::llc_misses:
    perfevtsel.event = kPerfEvtSelEvtLLCMisses;
    perfevtsel.umask = kPerfEvtSelUmaskLLCMisses;
    break;
  case PerfEvent::branch_instructions:
    perfevtsel.event = kPerfEvtSelEvtBranchInstructions;
    perfevtsel.umask = kPerfEvtSelUmaskBranchInstructions;
    break;
  case PerfEvent::branch_misses:
    perfevtsel.event = kPerfEvtSelEvtBranchMisses;
    perfevtsel.umask = kPerfEvtSelUmaskBranchMisses;
    break;
  case PerfEvent::fixed_instructions:
    fixed_ctrl.ctr0_enable = kIa32FixedCtlRingLvlAll;    
    //counter_offset_ = msr::Read(kIa32FxdPmc(0));    
    counter_offset_ = read_msr(fd, kIa32FxdPmc(0));
    pmc_num_ = kFixedEvtOffset(0);
    break;
  case PerfEvent::fixed_cycles:
    fixed_ctrl.ctr1_enable = kIa32FixedCtlRingLvlAll;
    //counter_offset_ = msr::Read(kIa32FxdPmc(1));
    counter_offset_ = read_msr(fd, kIa32FxdPmc(1));
    pmc_num_ = kFixedEvtOffset(1);
    break;
  case PerfEvent::fixed_reference_cycles:
    fixed_ctrl.ctr2_enable = kIa32FixedCtlRingLvlAll;
    //counter_offset_ = msr::Read(kIa32FxdPmc(2));
    counter_offset_ = read_msr(fd, kIa32FxdPmc(2));
    pmc_num_ = kFixedEvtOffset(2);
    break;
  case PerfEvent::nil_event:
    // counter disabled
    break;
  default:
    printf("Error: unknown performance monitor counter \n");
    abort();
    break;
  }

  // Configure fixed event counter
  if (fixed_ctrl.val != 0) {
    //fixed_ctrl.val |= msr::Read(kIa32FixedCtrCtrlMsr);
    fixed_ctrl.val |= read_msr(fd, kIa32FixedCtrCtrlMsr);
    
    //msr::Write(kIa32FixedCtrCtrlMsr, fixed_ctrl.val);
    if ( pwrite(fd, &fixed_ctrl.val, sizeof(fixed_ctrl.val), kIa32FixedCtrCtrlMsr)
	 != sizeof(fixed_ctrl.val) ) {
      printf("Error writing MSR %x\n", kIa32FixedCtrCtrlMsr);
      exit(127);
    }
    
    printf("Perf fixed counter #%d initialized to evt=%u\n", pmc_num_,
            static_cast<uint8_t>(evt_));
    return;
  }

  // Configure general purpose counter
  if (perfevtsel.val != 0) {
    for (auto i = 0u; i < pmc_count; i++) {
      if (((pmcs >> i) & 0x1) == 0) {
        pmc_num_ = i;
        pmcs |= (0x1u << i);
        //printf("DEBUG#%d %x \n", pmc_num_, pmcs);
        perfevtsel.usermode = 1;
        perfevtsel.osmode = 1;
        perfevtsel.en = 1;
	
        //msr::Write(kIa32PerfEvtSelMsr(pmc_num_), perfevtsel.val);
	if ( pwrite(fd, &perfevtsel.val, sizeof(perfevtsel.val), kIa32PerfEvtSelMsr(pmc_num_)) != sizeof(perfevtsel.val) ) {
	  printf("Error writing MSR %x\n", kIa32PerfEvtSelMsr(pmc_num_));
	  exit(127);
	}

	//counter_offset_ = msr::Read(kIa32Pmc(pmc_num_));
	counter_offset_ = read_msr(fd, kIa32Pmc(pmc_num_));
	
        //printf("Perf counter #%d initialized to evt=%u\n", pmc_num_,
        //        static_cast<uint8_t>(evt_));
        return;
      }
    }
    printf("Warning: no available hardware counters.\n");
    evt_ = PerfEvent::nil_event;
  }
  close(fd);
}

void perf::PerfCounter::Clear() {
  int fd = open_msr(sched_getcpu());
  
  switch (evt_) {
  case PerfEvent::nil_event:
    counter_offset_ = 0;
    break;
  case PerfEvent::fixed_instructions:
    //counter_offset_ = msr::Read(kIa32FxdPmc(0));
    counter_offset_ = read_msr(fd, (kIa32FxdPmc(0)));
    break;
  case PerfEvent::fixed_cycles:
    //counter_offset_ = msr::Read(kIa32FxdPmc(1));
    counter_offset_ = read_msr(fd, (kIa32FxdPmc(1)));
    break;
  case PerfEvent::fixed_reference_cycles:
    //counter_offset_ = msr::Read(kIa32FxdPmc(2));
    counter_offset_ = read_msr(fd, (kIa32FxdPmc(2)));
    break;
  default:
    //counter_offset_ = msr::Read(kIa32Pmc(pmc_num_));
    counter_offset_ = read_msr(fd, (kIa32Pmc(pmc_num_)));
    break;
  }

  close(fd);
  return;
}

uint64_t perf::PerfCounter::Read() {
  int fd = open_msr(sched_getcpu());
  uint64_t ret;
  
  switch (evt_) {
  case PerfEvent::nil_event:
    return 0;
  case PerfEvent::fixed_instructions:
    //return msr::Read((kIa32FxdPmc(0))) - counter_offset_;
    ret = read_msr(fd, (kIa32FxdPmc(0))) - counter_offset_;
    close(fd);
    return ret;   
  case PerfEvent::fixed_cycles:
    //return msr::Read((kIa32FxdPmc(1))) - counter_offset_;
    ret = read_msr(fd, (kIa32FxdPmc(1))) - counter_offset_;
    close(fd);
    return ret;        
  case PerfEvent::fixed_reference_cycles:
    //return msr::Read((kIa32FxdPmc(2))) - counter_offset_;
    ret = read_msr(fd, (kIa32FxdPmc(2))) - counter_offset_;
    close(fd);
    return ret;        
  default:
    //return msr::Read(kIa32Pmc(pmc_num_)) - counter_offset_;
    ret = read_msr(fd, (kIa32Pmc(pmc_num_))) - counter_offset_;
    close(fd);
    return ret;        
  }
}

void perf::PerfCounter::Start() {
  if (evt_ != PerfEvent::nil_event) {
    int fd = open_msr(sched_getcpu());
    
    perf_global_ctrl |= (1ull << pmc_num_);
    
    if ( pwrite(fd, &perf_global_ctrl, sizeof(perf_global_ctrl), kIa32PerfGlobalCtrlMsr) != sizeof(perf_global_ctrl) ) {
      printf("Error writing MSR %x\n", kIa32PerfGlobalCtrlMsr);
      exit(127);
    }
    close(fd);
    
    //msr::Write(kIa32PerfGlobalCtrlMsr, perf_global_ctrl);
  }
  return;
}

void perf::PerfCounter::Stop() {
  
  if (evt_ != PerfEvent::nil_event) {
    int fd = open_msr(sched_getcpu());
    
    perf_global_ctrl &= ~(1ull << pmc_num_);
    
    if(pwrite(fd, &perf_global_ctrl, sizeof(perf_global_ctrl),
	      kIa32PerfGlobalCtrlMsr) != sizeof(perf_global_ctrl) ) {
      printf("Error writing MSR %x\n", kIa32PerfGlobalCtrlMsr);
      exit(127);
    }
    
    close(fd);
    //msr::Write(kIa32PerfGlobalCtrlMsr, perf_global_ctrl);
  }
  return;
}
