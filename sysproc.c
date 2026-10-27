#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

// Assignment 9 - Question 2(a): Lazy Memory Allocation in sys_sbrk()
// When n > 0, only increase myproc()->sz and do not allocate physical memory
// immediately; pages will be allocated on demand when T_PGFLT occurs in trap.c.
// When n < 0, deallocate any mapped pages in the shrunk range via growproc(n).
int
sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *curproc = myproc();

  if(argint(0, &n) < 0)
    return -1;
  addr = curproc->sz;
  if(n > 0){
    if(curproc->sz + (uint)n >= KERNBASE || curproc->sz + (uint)n < curproc->sz)
      return -1;
    curproc->sz += n;
  } else if(n < 0){
    if((uint)(-n) > curproc->sz)
      return -1;
    if(growproc(n) < 0)
      return -1;
  }
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Assignment 9 - Question 1(a): System call to print the calling process's page table
int
sys_vmprint(void)
{
  return vmprint();
}

// Assignment 9 - Question 2(c): System call to return the number of page faults
// handled for the calling process
int
sys_getfaults(void)
{
  return myproc()->page_faults;
}
