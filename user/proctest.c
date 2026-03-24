#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static char *states[] = {
  [0] "UNUSED",
  [1] "USED",
  [2] "SLEEPING",
  [3] "RUNNABLE",
  [4] "RUNNING",
  [5] "ZOMBIE"
};

int
main(int argc, char *argv[])
{
  struct procinfo info;
  int pid;

  // Nếu có argument thì dùng pid từ argument
  // Nếu không có argument thì dùng pid của chính mình
  if(argc >= 2) {
    pid = atoi(argv[1]);
  } else {
    pid = getpid();
  }

  if(procinfo(pid, &info) == 0) {
    printf("Name:   %s\n",   info.name);
    printf("PID:    %d\n",   info.pid);
    printf("PPID:   %d\n",   info.ppid);
    printf("State:  %s\n",   states[info.state]);
    printf("Memory: %d bytes\n", (int)info.sz);
  } else {
    printf("procinfo failed\n");
  }

  exit(0);
}