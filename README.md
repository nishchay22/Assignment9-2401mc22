# Operating Systems — Assignment 9 (xv6)

**Student Name**: Nishchay  
**Roll Number**: 2401mc22  

This repository contains the complete kernel modifications, user test programs, terminal output screenshots, and 1-page report for **Operating Systems Assignment 9** on **MIT xv6 (x86)**.

---

## Overview of Implemented Solutions

### Question 1 — Printing the Page Table of a Process (`vmprint` & `pttest.c`)
- **System Call `int vmprint(void)` (`SYS_vmprint = 22`)**:
  - Implemented in [`vm.c`](vm.c) using `walkpgdir()` and the structure of `freevm()` as a guide.
  - Walks the user portion of the calling process's page directory (`0` to `PDX(KERNBASE) - 1`, ignoring kernel addresses `>= KERNBASE`).
  - For every valid user page (`PTE_P`), prints its virtual address (`VA`), physical frame address (`PA`), and flags `P`, `W`, and `U`.
  - Prints the total number of valid user pages and the number of page-table pages used by the process at the end.
- **User Program [`pttest.c`](pttest.c)**:
  - Calls `vmprint()` at start-up (showing 3 pages: code/data at `0x0`, stack guard page at `0x1000`, stack at `0x2000`) and again after `sbrk(4 * 4096)`, demonstrating that **exactly 4 new user pages (`0x3000`, `0x4000`, `0x5000`, `0x6000`)** appear.

### Question 2 — Handling Page Faults / Lazy Allocation (`T_PGFLT`, `getfaults` & `faulttest.c`)
- **Lazy `sys_sbrk()` in [`sysproc.c`](sysproc.c)**:
  - When `n > 0`, increases `myproc()->sz` without allocating physical memory.
  - Updated `deallocuvm()` and `copyuvm()` in [`vm.c`](vm.c) to skip unallocated lazy pages instead of calling `panic()`.
- **Page Fault Handler (`T_PGFLT`) in [`trap.c`](trap.c)**:
  - Reads the faulting virtual address with `rcr2()`.
  - If below `myproc()->sz` (and not an already-present protection fault), allocates a zeroed page using `kalloc()` and `memset()`, maps it with `mappages()` using `PTE_W | PTE_U`, increments `myproc()->page_faults` (added in [`proc.h`](proc.h)), and resumes the process.
  - Otherwise prints the `pid` and faulting `address` and kills the offending process.
- **System Call `int getfaults(void)` (`SYS_getfaults = 23`) & User Program [`faulttest.c`](faulttest.c)**:
  - Calls `sbrk(32 * 4096)`, writes to **5 different pages**, verifies `getfaults()` returns **`5`**, and verifies in a forked child that accessing an address above `myproc()->sz` kills only that process.

### Question 3 — Page Replacement Algorithms: FIFO and LRU ([`pagerepl.c`](pagerepl.c))
- **User Program [`pagerepl.c`](pagerepl.c)**:
  - Takes the number of frames and a reference string as input (or defaults to `7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1`), implements **FIFO** and **LRU**, and prints the frame state and `HIT`/`FAULT` for every reference.
  - Verifies **15 page faults for FIFO** and **12 page faults for LRU** with 3 frames.
  - Runs both algorithms for **3, 4, and 5 frames** and prints a summary comparison table of page faults and hit ratios.

---

## Modified & Added Files

| File | Description |
| :--- | :--- |
| [`vm.c`](vm.c) | Exposed `walkpgdir()` & `mappages()`; implemented `vmprint()`; updated `copyuvm()` & `deallocuvm()` to skip unallocated lazy pages. |
| [`trap.c`](trap.c) | Added `T_PGFLT` (page fault) handler using `rcr2()`, `kalloc()`, `memset()`, and `mappages()`. |
| [`proc.h`](proc.h), [`proc.c`](proc.c), [`exec.c`](exec.c) | Added and initialized `uint page_faults` per process. |
| [`sysproc.c`](sysproc.c) | Modified `sys_sbrk()` for lazy allocation; added `sys_vmprint()` and `sys_getfaults()`. |
| [`syscall.h`](syscall.h), [`syscall.c`](syscall.c), [`defs.h`](defs.h), [`user.h`](user.h), [`usys.S`](usys.S) | Added system call numbers, prototypes, and user-space stubs for `vmprint()` and `getfaults()`. |
| [`pttest.c`](pttest.c) | User test program for Question 1. |
| [`faulttest.c`](faulttest.c) | User test program for Question 2. |
| [`pagerepl.c`](pagerepl.c) | User program for Question 3 (FIFO and LRU page replacement). |
| [`REPORT.md`](REPORT.md) | 1-page report explaining kernel changes and outputs for all 3 questions. |
| [`screenshots/`](screenshots/) | Terminal output screenshots (`q1_pttest.png`, `q2_faulttest.png`, `q3_pagerepl.png`). |

---

## How to Build and Run

```bash
# Clean and build xv6
make clean
make

# Launch xv6 in headless QEMU
make qemu-nox

# Inside the xv6 shell ($):
$ pttest
$ faulttest
$ pagerepl
$ pagerepl 3 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
```
