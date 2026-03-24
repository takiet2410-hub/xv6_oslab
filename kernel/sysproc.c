#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()->trace_mask = mask;
  return 0;
}

uint64
sys_procinfo(void)
{
  extern struct proc proc[];
  int pid;
  uint64 addr;
  struct procinfo info;
  struct proc *p;

  // Lấy 2 arguments: pid (int) và pointer tới struct procinfo
  argint(0, &pid);
  argaddr(1, &addr);

  // Duyệt proc[] y hệt kill() trong proc.c
  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->pid == pid && p->state != UNUSED) {
      // Copy thông tin ra local variable TRONG KHI giữ lock
      info.pid   = p->pid;
      info.state = p->state;
      info.sz    = p->sz;
      memmove(info.name, p->name, sizeof(p->name));

      // Lấy ppid: cần wait_lock để đọc p->parent an toàn
      // Nhưng KHÔNG acquire wait_lock khi đang giữ p->lock
      // (deadlock risk: lock ordering phải là wait_lock -> p->lock)
      // Cách an toàn: đọc parent pointer tạm, release p->lock trước
      struct proc *parent = p->parent;
      release(&p->lock);  // ← PHẢI release TRƯỚC khi copyout

      // Sau khi release, lấy ppid
      if(parent != 0) {
        acquire(&parent->lock);
        info.ppid = parent->pid;
        release(&parent->lock);
      } else {
        info.ppid = 0;
      }

      // copyout về user space — KHÔNG được giữ spinlock khi gọi hàm này
      if(copyout(myproc()->pagetable, addr,
                 (char*)&info, sizeof(info)) < 0)
        return -1;

      return 0;
    }
    release(&p->lock);
  }

  return -1;  // pid không tìm thấy
}