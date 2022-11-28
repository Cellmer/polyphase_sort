#pragma once
#include <stdio.h>

#include "disk_operations.h" 

// structure that helps with merging phase of sorting
typedef struct Runs
{
    int larger_tape_runs, smaller_tape_runs, dummy, larger_tape_index;
} Runs;

// sort file records
void sort(const char* filename);

// initial sorting phase
Runs distribute(FILE* file, const char* tape1_name, const char* tape2_name);

// main sorting recursive function
// arguments pos and record_index should be NULL in the first call, they are used inside later recursive calls to set position of a pointer to a tape
char* merge(const char* tape1_name, const char* tape2_name, fpos_t pos, int record_index, const char* tape3_name, Runs runs, int* phase);
