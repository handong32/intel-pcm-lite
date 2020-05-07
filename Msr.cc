#include "Msr.h"

int open_msr(int core) {
  char msr_filename[BUFSIZ];
  int fd;
  
  sprintf(msr_filename, "/dev/cpu/%d/msr", core);
  fd = open(msr_filename, O_RDWR);
  if ( fd < 0 ) {
    if ( errno == ENXIO ) {
      fprintf(stderr, "rdmsr: No CPU %d\n", core);
      exit(2);
    } else if ( errno == EIO ) {
      fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
	      core);
      exit(3);
    } else {
      perror("rdmsr:open");
      fprintf(stderr,"Trying to open %s\n",msr_filename);
      exit(127);
    }
  }  
  return fd;
}

uint64_t read_msr(int fd, unsigned int which) {
  uint64_t data;
  
  if ( pread(fd, &data, sizeof data, which) != sizeof data ) {
    perror("rdmsr:pread");
    fprintf(stderr,"Error reading MSR %x\n",which);
    exit(127);
  }  
  return data;
}
