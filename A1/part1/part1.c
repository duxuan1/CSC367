// ------------
// This code is provided solely for the personal and private use of
// students taking the CSC367H5 course at the University of Toronto.
// Copying for purposes other than this use is expressly prohibited.
// All forms of distribution of this code, whether as given or with
// any changes, are expressly prohibited.
//
// Authors: Bogdan Simion, Alexey Khrabrov
//
// All of the files in this directory and all subdirectories are:
// Copyright (c) 2019 Bogdan Simion
// -------------

#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include "time_util.h"

#define B  (1LL)
#define KB (1024LL)
#define MB (1024*1024LL)
#define GB (1024*1024*1024LL)

#define MINBYTES  2*KB   /* Working set size ranges from 2 KB */
#define MAXBYTES  32*MB  /* ... up to 32 MB */
#define MINSTRIDE 2*B    /* Working stride size ranges from 2 B */
#define MAXSTRIDE 128*B  /* ... up to 128 B */

char src[1*GB];

/*
 * measure the time taken to memset src with input size
 * return a time spec struct
 */
struct timespec measure_time_memset(int size) {
	memset(src, 1, size); // warm up cache/memory

	struct timespec start, end, diff;
	clock_gettime(CLOCK_MONOTONIC, &start);
	memset(src, 1, size);
	clock_gettime(CLOCK_MONOTONIC, &end);
	diff = difftimespec(end, start);

	return diff;
}


/*
 * measure the memory bandwidth
 * write bandwidth(GB/S) to memory_bandwidth.txt
 */
void measure_memory_bandwidth() {
	FILE *f = fopen("memory_bandwidth.txt", "w");
	unsigned int size = (unsigned int)1*GB;

	// run 10 times to avoid 
	for (int i=0; i < 10; i++) {
		// measure bandwidth
		double time = timespec_to_sec(measure_time_memset(size));
		double bandwidth = size/GB/time;
		fprintf(f, "memory bandwidth is %lf GB/S\n", bandwidth);
	}

	fclose(f);
}

/*
 * access data by random index, measure cache size
 */
double measure_time_random_access(int *rand_index, int size, int test_times) {
	memset(src, 1, size); // warm up cache/memory

	double sum = 0.0;	
	for (int i = 0; i < test_times; i++) {
		int idx = rand()%size;
		rand_index[i] = idx;
	}

	struct timespec start, end, diff;
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < test_times; i++) {
		sum += src[rand_index[i]];
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	diff = difftimespec(end, start);
	double time = timespec_to_msec(diff);

	double bandwidth = sum/time;
	return bandwidth;
}

/*
 * measure the L1, L2, L3 cache_hierarchy
 * write to cache_hierarchy.txt
 */
void measure_cache_hierarchy() {  
	FILE *f = fopen("cache_hierarchy.txt", "w");
	int test_times = 64*MB;
	int *rand_index = malloc(test_times * sizeof(int));
	srand(time(NULL));
    for (int size = MINBYTES; size <= MAXBYTES; size <<= 1) {
		fprintf(f, "%lldk\t", size/KB);
		double bandwidth = measure_time_random_access(rand_index, size, test_times);
		fprintf(f, "%.1f\n", bandwidth);
    }
	free(rand_index);
	fclose(f);
}

/*
 * helper of measure_cache_line
 */
double run_cache_line(int stride, int size) {
	memset(src, 1, size); // warm up cache/memory

	struct timespec start, end, diff;
	clock_gettime(CLOCK_MONOTONIC, &start);
    int sum = 0;
	for(int j=0; j<stride; j++) {
		for(int i=0; i<size; i+=stride) {
			sum += src[i];
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	diff = difftimespec(end, start);
	double time_sec = timespec_to_msec(diff);

	return sum/time_sec;
}

/*
 * measure the size of cache line
 * write to cache_line.txt
 */
void measure_cache_line() {  
	FILE *f = fopen("cache_line.txt", "w");
	unsigned int size = (unsigned int)1*MB;
	
	for (int stride = MINSTRIDE; stride <= MAXSTRIDE; stride <<= 1) {
		fprintf(f, "%db\t", stride);
		fprintf(f, "%.1f\n", run_cache_line(stride, size));
	}
	fclose(f);
}

int main(int argc, char *argv[]) {
	// Pin the thread to a single CPU to minimize the effects of scheduling
	// Don't use CPU #0 if possible, as it tends to be busier with servicing interrupts
	srandom(time(NULL));
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET((random() ?: 1) % get_nprocs(), &set);
	if (sched_setaffinity(getpid(), sizeof(set), &set) != 0) {
		perror("sched_setaffinity");
		return 1;
	}

	// -------------
	// part a) measure memory bandwidth
	// -------------
	measure_memory_bandwidth();

	// -------------
	// part b) determine the number of levels in the CPU cache hierarchy, 
	// and to measure cache sizes and cache (write) latencies for each level, 
	// as well as the write latency of main memory
	// -------------
	measure_cache_hierarchy();
	
	// -------------
	// BONUS: design an experiment to measure the cache line size.
	// -------------
	measure_cache_line();

	return 0;
}
