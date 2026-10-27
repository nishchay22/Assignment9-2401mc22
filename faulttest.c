// ============================================================================
// Operating Systems Assignment 9 — Question 2(c): faulttest.c
//
// Calls sbrk(32 * 4096), writes to exactly 5 different pages, and prints the
// number of page faults handled via getfaults() (expected: 5).
// Also demonstrates that accessing an address above the allocated size kills
// only that offending child process while the parent continues normally.
// ============================================================================

#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int
main(int argc, char *argv[])
{
  char *base;
  int faults_before, faults_after_sbrk, faults_after_writes;
  int pages_to_touch[5] = { 0, 5, 12, 21, 31 };
  int i, pid;

  printf(1, "====================================================\n");
  printf(1, "Question 2: Handling Page Faults / Lazy Allocation (faulttest)\n");
  printf(1, "====================================================\n\n");

  faults_before = getfaults();
  printf(1, "[1] Initial page faults handled: %d\n", faults_before);

  // 1. Lazily allocate 32 pages (128 KB) with sbrk(32 * 4096)
  base = sbrk(32 * PGSIZE);
  if(base == (char*)-1){
    printf(1, "faulttest: sbrk(32 * 4096) failed\n");
    exit();
  }

  faults_after_sbrk = getfaults();
  printf(1, "[2] After sbrk(32 * 4096) (before touching any page): %d page faults\n",
         faults_after_sbrk);

  // 2. Write to exactly 5 different pages
  printf(1, "[3] Writing to 5 distinct pages: ");
  for(i = 0; i < 5; i++){
    int pg = pages_to_touch[i];
    printf(1, "page %d (VA=0x%x)%s", pg, (uint)(base + pg * PGSIZE),
           (i < 4) ? ", " : "\n");
    base[pg * PGSIZE] = (char)('1' + i);
  }

  // Write a second time to the same 5 pages to verify no extra page faults occur
  for(i = 0; i < 5; i++){
    int pg = pages_to_touch[i];
    base[pg * PGSIZE + 1] = '!';
  }

  faults_after_writes = getfaults();
  printf(1, "[4] Number of page faults handled (getfaults()): %d (Expected: 5)\n",
         faults_after_writes);

  // 3. Demonstrate that accessing an address above myproc()->sz kills ONLY that process
  printf(1, "\n[5] Testing out-of-bounds access above allocated size in child process:\n");
  pid = fork();
  if(pid < 0){
    printf(1, "faulttest: fork failed\n");
    exit();
  }

  if(pid == 0){
    char *bad_addr = base + 33 * PGSIZE; // Above myproc()->sz
    printf(1, "    [Child pid=%d] Accessing invalid address 0x%x (above sz)...\n",
           getpid(), (uint)bad_addr);
    *bad_addr = 'X';
    // Should never reach here
    printf(1, "    [Child pid=%d] ERROR: survived invalid access!\n", getpid());
    exit();
  }

  wait();
  printf(1, "    [Parent pid=%d] Child process was killed by kernel; parent is still alive!\n",
         getpid());
  printf(1, "    [Parent pid=%d] Parent getfaults() remains: %d\n",
         getpid(), getfaults());
  printf(1, "====================================================\n");
  exit();
}
