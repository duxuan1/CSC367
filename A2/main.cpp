/* ------------
 * This code is provided solely for the personal and private use of
 * students taking the CSC367H5 course at the University of Toronto.
 * Copying for purposes other than this use is expressly prohibited.
 * All forms of distribution of this code, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Bogdan Simion, Felipe de Azevedo Piovezan
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2019 Bogdan Simion
 * -------------
 */

#include "filters.h"
#include "pgm.h"
#include "very_big_sample.h"
#include "very_tall_sample.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define HARDCODED_BIG_IMAGE 1
#define HARDCODED_TALL_IMAGE 2

#define SEQUENTIAL_METHOD 1
#define SHARDED_ROWS_METHOD 2
#define SHARDED_COLUMNS_COLUMN_MAJOR_METHOD 3
#define SHARDED_COLUMNS_ROW_MAJOR_METHOD 4
#define WORK_QUEUE_METHOD 5

void print_error_arguments() {
  printf("Incorrect usage. Please refer to the handout.\n");
}

filter *get_filter(int filter) { return builtin_filters[filter - 1]; }

int main(int argc, char **argv) {
  int32_t filter = 0;
  int32_t method = 0;
  int32_t chunk_size = 0;
  int32_t print_time = 0;
  int32_t nthreads = 0;
  char *source_file = NULL;
  int32_t hardcoded_source = 0;
  char *target_file = NULL;

  int32_t option;
  while ((option = getopt(argc, argv, "i:b:o:n:t:f:m:c:")) != -1) {
    switch (option) {
    case 'i':
      source_file = optarg;
      break;
    case 'b':
      hardcoded_source = atoi(optarg);
      if (hardcoded_source != HARDCODED_BIG_IMAGE &&
          hardcoded_source != HARDCODED_TALL_IMAGE) {
        print_error_arguments();
        return 1;
      }
      break;
    case 'o':
      target_file = optarg;
      break;
    case 'n':
      nthreads = atoi(optarg);
      break;
    case 't':
      print_time = atoi(optarg);
      break;
    case 'f':
      if (!(filter = atoi(optarg))) {
        print_error_arguments();
        return 1;
      }
      break;
    case 'm':
      if (!(method = atoi(optarg))) {
        print_error_arguments();
        return 1;
      }
      break;
    case 'c':
      chunk_size = atoi(optarg);
      break;
    case '?':
      print_error_arguments();
      return 1;
    }
  }

  if (method == 0 || filter == 0) {
    print_error_arguments();
    return 1;
  }

  /* argument values check */
  if (method != SEQUENTIAL_METHOD) {
    if (nthreads == 0) {
      print_error_arguments();
      return 1;
    }
  }

  if (method == WORK_QUEUE_METHOD) {
    if (chunk_size == 0) {
      print_error_arguments();
      return 1;
    }
  }

  if (source_file == NULL && hardcoded_source == 0) {
    print_error_arguments();
    return 1;
  }

  pgm_image source, target;

  // code for the hardcoded images
  if (hardcoded_source == HARDCODED_BIG_IMAGE) {
    source = very_big_sample;
  } else if (hardcoded_source == HARDCODED_TALL_IMAGE) {
    source = very_tall_sample;
  } else {
    init_pgm_image(&source);
    int err = load_pgm_from_file(source_file, &source);

    if (err != NO_ERR) {
      printf("error loading file (%d)\n", err);
      return 1;
    }
  }

  copy_pgm_image_size(&source, &target);

  struct timespec start, stop;
  clock_gettime(CLOCK_MONOTONIC, &start);

  switch (method) {
  case SEQUENTIAL_METHOD:
    apply_filter2d(get_filter(filter), source.matrix, target.matrix,
                   source.width, source.height);
    break;
  case SHARDED_ROWS_METHOD:
    apply_filter2d_threaded(get_filter(filter), source.matrix, target.matrix,
                            source.width, source.height, nthreads, SHARDED_ROWS,
                            0);
    break;
  case SHARDED_COLUMNS_COLUMN_MAJOR_METHOD:
    apply_filter2d_threaded(get_filter(filter), source.matrix, target.matrix,
                            source.width, source.height, nthreads,
                            SHARDED_COLUMNS_COLUMN_MAJOR, 0);
    break;
  case SHARDED_COLUMNS_ROW_MAJOR_METHOD:
    apply_filter2d_threaded(get_filter(filter), source.matrix, target.matrix,
                            source.width, source.height, nthreads,
                            SHARDED_COLUMNS_ROW_MAJOR, 0);
    break;
  case WORK_QUEUE_METHOD:
    apply_filter2d_threaded(get_filter(filter), source.matrix, target.matrix,
                            source.width, source.height, nthreads, WORK_QUEUE,
                            chunk_size);
    break;
  default:
    print_error_arguments();
    break;
  }

  clock_gettime(CLOCK_MONOTONIC, &stop);

  if (print_time) {
    printf("time=%.2lf\n",
           (stop.tv_sec - start.tv_sec) +
               (double)(stop.tv_nsec - start.tv_nsec) / 1000000000);
  }

  if (target_file != NULL) {
    save_pgm_to_file(target_file, &target);
  }

  return 0;
}
