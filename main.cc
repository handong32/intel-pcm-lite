#include <stdio.h>
#include <unordered_map>
#include <thread>         // std::thread
#include <mutex>
#include <vector>
#include <iostream>

#include "Perf.h"
#include "Rapl.h"

int main (int argc, char * argv[]) {  
  if (argc!=2) {printf("usage: main <iters> \n"); return -1;}
  uint64_t n = atoi(argv[1]);

  printf("Nil counter: %lu iterations\n", n);

  constexpr unsigned num_threads = 1;

  // A mutex ensures orderly access to std::cout from multiple threads.
  std::mutex iomutex;
  std::vector<std::thread> threads(num_threads);
  for (unsigned i = 0; i < num_threads; ++i) {
    threads[i] = std::thread([&iomutex, i, n] {

	std::unordered_map<std::string, perf::PerfCounter> counters;
	counters.emplace(std::make_pair(std::string("instructions"),
					perf::PerfCounter{perf::PerfEvent::fixed_instructions}));
	/*counters.emplace(std::make_pair(std::string("cycles"),
					perf::PerfCounter{perf::PerfEvent::fixed_cycles}));  
	counters.emplace(std::make_pair(std::string("tlb_load_miss"),
					perf::PerfCounter{perf::PerfEvent::tlb_load_misses}));
	counters.emplace(std::make_pair(std::string("tlb_store_miss"),
					perf::PerfCounter{perf::PerfEvent::tlb_store_misses}));
	counters.emplace(std::make_pair(std::string("llc_misses"),
					perf::PerfCounter{perf::PerfEvent::llc_misses}));
	counters.emplace(std::make_pair(std::string("llc_references"),
					perf::PerfCounter{perf::PerfEvent::llc_references}));
	*/
	for(auto& i : counters){
	  i.second.Start();
	}
	
	for( size_t j = 0; j<n; j++){
	  asm volatile("nop"); 
	}

	uint64_t nins;
	for(auto& i : counters) {
	  i.second.Stop();
	  nins = i.second.Read();
	  //printf("%20s\t\t%20lu\n", i.first.c_str(), i.second.Read());
	  i.second.Clear();
	}

	std::lock_guard<std::mutex> iolock(iomutex);
	std::cout << "nins =" << nins << ": on CPU " << sched_getcpu() << "\n";

	rapl::RaplCounter rp = rapl::RaplCounter();
	
	while(1) {
	  rp.Start();
	  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	  rp.Stop();
	  printf("%.6lf J\n", rp.Read());
	  rp.Clear();
	}
      });
  }
  
  for (auto& t : threads) {
    t.join();
  }
  
  return 0; 
}
