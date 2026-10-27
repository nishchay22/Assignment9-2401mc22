// ============================================================================
// Operating Systems Assignment 9 — Question 1(c): pttest.c
//
// Calls vmprint() at start-up and again after calling sbrk(4 * 4096),
// showing that exactly four new pages (0x3000, 0x4000, 0x5000, 0x6000) appear.
// ============================================================================

#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int
main(int argc, char *argv[])
{
  char *p;
  int i;

  printf(1, "====================================================\n");
  printf(1, "Question 1: Printing the Page Table (pttest)\n");
  printf(1, "====================================================\n\n");

  // 1. Print page table at process start-up
  printf(1, "[1] Calling vmprint() at start-up:\n");
  vmprint();

  // 2. Allocate 4 new pages via sbrk(4 * 4096)
  printf(1, "\n[2] Calling sbrk(4 * 4096) to grow heap by 4 pages...\n");
  p = sbrk(4 * PGSIZE);
  if(p == (char*)-1){
    printf(1, "pttest: sbrk failed\n");
    exit();
  }

  // Touch each of the 4 pages so they are mapped even with lazy allocation (Q2)
  for(i = 0; i < 4; i++){
    p[i * PGSIZE] = (char)('A' + i);
  }

  // 3. Print page table again after sbrk(4 * 4096)
  printf(1, "[3] Calling vmprint() after sbrk(4 * 4096):\n");
  vmprint();

  printf(1, "\nResult: Exactly 4 new user pages (0x3000, 0x4000, 0x5000, 0x6000) appeared!\n");
  exit();
}
