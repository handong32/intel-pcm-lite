#ifndef WORK_ALU_H
#define WORK_ALU_H

#ifdef __cplusplus
extern "C" {
#endif

// This just *declares* the *existence* of the function.
// The compiler can use this information (in gen_permu.cpp)
// to create a call-to-placeholder. The linker will then
// turn that into a call-to-function when you link the two
// object files together.
int work(unsigned long long, unsigned long long, unsigned long long);
  
// End of the C++ compatibility construct.    
#ifdef __cplusplus
}
#endif

#endif
