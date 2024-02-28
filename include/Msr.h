//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef _MSR_H_
#define _MSR_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>

namespace msr {
const constexpr uint32_t kIa32ApicBase = 0x0000001b;
const constexpr uint32_t kX2apicIdr = 0x00000802;
const constexpr uint32_t kX2apicEoi = 0x0000080b;
const constexpr uint32_t kX2apicSvr = 0x0000080f;
const constexpr uint32_t kX2apicIcr = 0x00000830;
const constexpr uint32_t kX2apicLvtTimer = 0x832;
const constexpr uint32_t kX2apicInitCount = 0x838;
const constexpr uint32_t kX2apicCurrentCount = 0x839;
const constexpr uint32_t kX2apicDcr = 0x83e;
const constexpr uint32_t kKvmWallClockNew = 0x4b564d00;
const constexpr uint32_t kKvmSystemTimeNew = 0x4b564d01;
const constexpr uint32_t kKvmPvEoi = 0x4b564d04;
const constexpr uint32_t kIa32FsBase = 0xC0000100;

inline uint64_t Read(uint32_t index) {
  uint32_t low, high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(index));
  return low | (uint64_t(high) << 32);
}

inline void Write(uint32_t index, uint64_t data) {
  uint32_t low = data;
  uint32_t high = (data >> 32) & 0xFFFFFFFF;
  
  asm volatile("wrmsr" : : "c"(index), "a"(low), "d"(high));
}
}  // namespace msr

int open_msr(int core);
uint64_t read_msr(int fd, unsigned int which);

#endif  // _MSR_H_
