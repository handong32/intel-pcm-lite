#include <stdio.h>
#include <unordered_map>
#include <thread>         // std::thread
#include <mutex>
#include <vector>
#include <iostream>
#include <cmath>

#include "Perf.h"
#include "Rapl.h"

#define UINT32_MAXT  (0xffffffff)

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

	/*
	std::unordered_map<std::string, perf::PerfCounter> counters;
	counters.emplace(std::make_pair(std::string("instructions"),
					perf::PerfCounter{perf::PerfEvent::fixed_instructions}));
	counters.emplace(std::make_pair(std::string("cycles"),
					perf::PerfCounter{perf::PerfEvent::fixed_cycles}));  
	counters.emplace(std::make_pair(std::string("tlb_load_miss"),
					perf::PerfCounter{perf::PerfEvent::tlb_load_misses}));
	counters.emplace(std::make_pair(std::string("tlb_store_miss"),
					perf::PerfCounter{perf::PerfEvent::tlb_store_misses}));
	counters.emplace(std::make_pair(std::string("llc_misses"),
					perf::PerfCounter{perf::PerfEvent::llc_misses}));
	counters.emplace(std::make_pair(std::string("llc_references"),
					perf::PerfCounter{perf::PerfEvent::llc_references}));
	
	
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
	*/

        std::cout << "Running on CPU " << sched_getcpu() << "\n";
	rapl::RaplCounter rp = rapl::RaplCounter();
	rp.Start();	

	//int limit = 1000000; /* How many times the looping will be done */
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
	printf("The value of pi = %1.10lf\n", ans3);
	
	rp.Stop();
	printf("Energy used: %.6lf J\n", rp.Read());
	rp.Clear();
	  
	/*while(1) {
	  rp.Start();
	  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	  rp.Stop();
	  printf("%.6lf J\n", rp.Read());
	  rp.Clear();
	  }*/
      });
  }
  
  for (auto& t : threads) {
    t.join();
  }
  
  return 0; 
}
