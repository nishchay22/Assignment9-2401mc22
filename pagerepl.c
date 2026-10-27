// ============================================================================
// Operating Systems Assignment 9 — Question 3: pagerepl.c
//
// Compares FIFO and LRU page replacement algorithms on a given reference string.
// Tasks:
//   a) Takes the number of frames and a reference string as input (or uses the
//      default assignment inputs if none are passed), implements FIFO and LRU,
//      and for every reference prints the frames and whether it was a HIT or FAULT.
//   b) Tests with reference string: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
//      and 3 frames (Expected: 15 page faults for FIFO, 12 page faults for LRU).
//   c) Runs both algorithms for 3, 4, and 5 frames on the same string and
//      prints a table of the number of page faults and the hit ratio for each.
// ============================================================================

#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_FRAMES 16
#define MAX_REFS   128

static int default_refs[20] = {
  7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
  0, 3, 2, 1, 2, 0, 1, 7, 0, 1
};

// Helper: search for page in frames[0..nframes-1]
static int
find_page(int *frames, int nframes, int page)
{
  int i;
  for(i = 0; i < nframes; i++){
    if(frames[i] == page)
      return i;
  }
  return -1;
}

// Helper: print current frame array state
static void
print_frames(int *frames, int nframes)
{
  int i;
  printf(1, "[");
  for(i = 0; i < nframes; i++){
    if(frames[i] == -1)
      printf(1, " -");
    else
      printf(1, " %d", frames[i]);
  }
  printf(1, " ]");
}

// Helper: print percentage and decimal hit ratio (since xv6 printf has no %f)
static void
print_hit_ratio(int hits, int total)
{
  int pct_whole = (hits * 100) / total;
  int pct_frac  = ((hits * 1000) / total) % 10;
  int dec_two   = (hits * 100) / total;

  if(dec_two < 10)
    printf(1, "%d/%d (0.0%d / %d.%d%%)", hits, total, dec_two, pct_whole, pct_frac);
  else
    printf(1, "%d/%d (0.%d / %d.%d%%)", hits, total, dec_two, pct_whole, pct_frac);
}

// FIFO Page Replacement simulation
static int
run_fifo(int *refs, int nrefs, int nframes, int verbose)
{
  int frames[MAX_FRAMES];
  int i, fifo_ptr = 0, faults = 0, hits = 0;

  for(i = 0; i < nframes; i++)
    frames[i] = -1;

  if(verbose){
    printf(1, "\n--- FIFO Page Replacement (%d Frames) ---\n", nframes);
  }

  for(i = 0; i < nrefs; i++){
    int page = refs[i];
    int hit = (find_page(frames, nframes, page) >= 0);

    if(hit){
      hits++;
    } else {
      frames[fifo_ptr] = page;
      fifo_ptr = (fifo_ptr + 1) % nframes;
      faults++;
    }

    if(verbose){
      printf(1, "Step %d | Ref: %d -> Frames: ", i + 1, page);
      print_frames(frames, nframes);
      printf(1, " | %s\n", hit ? "HIT" : "FAULT");
    }
  }

  if(verbose){
    printf(1, "FIFO Summary (%d frames): Page Faults = %d, Hits = %d, Hit Ratio = ",
           nframes, faults, hits);
    print_hit_ratio(hits, nrefs);
    printf(1, "\n");
  }

  return faults;
}

// LRU (Least Recently Used) Page Replacement simulation
static int
run_lru(int *refs, int nrefs, int nframes, int verbose)
{
  int frames[MAX_FRAMES];
  int last_used[MAX_FRAMES];
  int i, j, faults = 0, hits = 0;

  for(i = 0; i < nframes; i++){
    frames[i] = -1;
    last_used[i] = -1;
  }

  if(verbose){
    printf(1, "\n--- LRU Page Replacement (%d Frames) ---\n", nframes);
  }

  for(i = 0; i < nrefs; i++){
    int page = refs[i];
    int idx = find_page(frames, nframes, page);

    if(idx >= 0){
      hits++;
      last_used[idx] = i;
    } else {
      // Find empty frame or frame with minimum last_used timestamp
      int victim = 0;
      for(j = 0; j < nframes; j++){
        if(frames[j] == -1){
          victim = j;
          break;
        }
        if(last_used[j] < last_used[victim])
          victim = j;
      }
      frames[victim] = page;
      last_used[victim] = i;
      faults++;
    }

    if(verbose){
      printf(1, "Step %d | Ref: %d -> Frames: ", i + 1, page);
      print_frames(frames, nframes);
      printf(1, " | %s\n", (idx >= 0) ? "HIT" : "FAULT");
    }
  }

  if(verbose){
    printf(1, "LRU Summary  (%d frames): Page Faults = %d, Hits = %d, Hit Ratio = ",
           nframes, faults, hits);
    print_hit_ratio(hits, nrefs);
    printf(1, "\n");
  }

  return faults;
}

static void
print_comparison_table(int *refs, int nrefs)
{
  int f;

  printf(1, "\n========================================================================\n");
  printf(1, "Task (c): Comparison Table for 3, 4, and 5 Frames (Total Refs = %d)\n", nrefs);
  printf(1, "========================================================================\n");
  printf(1, "Frames | FIFO Faults | FIFO Hit Ratio        | LRU Faults | LRU Hit Ratio\n");
  printf(1, "-------+-------------+-----------------------+------------+----------------------\n");

  for(f = 3; f <= 5; f++){
    int fifo_faults = run_fifo(refs, nrefs, f, 0);
    int fifo_hits   = nrefs - fifo_faults;
    int lru_faults  = run_lru(refs, nrefs, f, 0);
    int lru_hits    = nrefs - lru_faults;

    printf(1, "  %d    |     %d      | ", f, fifo_faults);
    print_hit_ratio(fifo_hits, nrefs);
    printf(1, "  |     %d     | ", lru_faults);
    print_hit_ratio(lru_hits, nrefs);
    printf(1, "\n");
  }
  printf(1, "========================================================================\n");
}

int
main(int argc, char *argv[])
{
  int refs[MAX_REFS];
  int nrefs = 0;
  int nframes = 3;
  int i;

  printf(1, "========================================================================\n");
  printf(1, "Question 3: Page Replacement Algorithms — FIFO and LRU (pagerepl)\n");
  printf(1, "========================================================================\n");

  // Task (a): Support command-line input: pagerepl <frames> <ref1> <ref2> ...
  if(argc >= 3){
    nframes = atoi(argv[1]);
    if(nframes <= 0 || nframes > MAX_FRAMES){
      printf(1, "pagerepl: invalid frame count %d (1..%d)\n", nframes, MAX_FRAMES);
      exit();
    }
    nrefs = argc - 2;
    if(nrefs > MAX_REFS)
      nrefs = MAX_REFS;
    for(i = 0; i < nrefs; i++)
      refs[i] = atoi(argv[i + 2]);

    printf(1, "Input Frames: %d\nReference String (%d refs): ", nframes, nrefs);
    for(i = 0; i < nrefs; i++)
      printf(1, "%d ", refs[i]);
    printf(1, "\n");

    run_fifo(refs, nrefs, nframes, 1);
    run_lru(refs, nrefs, nframes, 1);
    print_comparison_table(refs, nrefs);
    exit();
  }

  // Default execution: Tasks (b) and (c) on the assignment reference string
  nrefs = 20;
  for(i = 0; i < nrefs; i++)
    refs[i] = default_refs[i];

  printf(1, "Task (b): Testing with 3 frames and reference string:\n  ");
  for(i = 0; i < nrefs; i++)
    printf(1, "%d ", refs[i]);
  printf(1, "\n");

  run_fifo(refs, nrefs, 3, 1);
  run_lru(refs, nrefs, 3, 1);

  print_comparison_table(refs, nrefs);
  exit();
}
