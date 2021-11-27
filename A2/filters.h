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

#ifndef __FILTERS__H
#define __FILTERS__H

#include <stdint.h>

/**************FILTER STRUCT DEFINITIONS*****************/
/* Filters are square matrices with odd dimension.
 */
typedef struct filter_t {
  int32_t dimension;
  int8_t *matrix;
} filter;

/* Filter constants */
#define NUM_FILTERS 4
#define LAPLACIAN_FILTER_3 0 /* 3x3 filter */
#define LAPLACIAN_FILTER_5 1 /* 5x5 filter */
#define LAP_OF_GAUS_FILTER 2 /* 9x9 filter */
#define IDENTITY_FILTER 3    /* 1x1 filter */

extern filter *builtin_filters[NUM_FILTERS];

/**************FILTER METHODS********************/
/* sequential methods */

/* Applies a filter to an image using a single thread.
 * arguments: f - the filter to be used.
 *            original - the original image matrix.
 *            target - where the transformed image should be saved.
 *            width, height - width and height of the original image.
 * precondition: target should be as big as original.
 * precondition: original should be at least width * height long.
 */
void apply_filter2d(const filter *f, const int32_t *original, int32_t *target,
                    int32_t width, int32_t height);

/* parallel methods*/
typedef enum {
  SHARDED_ROWS,
  SHARDED_COLUMNS_COLUMN_MAJOR,
  SHARDED_COLUMNS_ROW_MAJOR,
  WORK_QUEUE
} parallel_method;

/* Applies a filter to an image using multiple threads.
 * arguments: f - the filter to be used.
 *            original - the original image matrix.
 *            target - where the transformed image should be saved.
 *            width, height - width and height of the original image.
 *            num_threads - the number of threads to be used.
 *            method - the method to use.
 *            work_chunk - the size of the submatrices used in the WORK_QUEUE
 *                         method.
 * precondition: target should be as big as original.
 * precondition: original should be at least width * height long.
 * precondition: num_threads > 0.
 * precondition: work_chunk > 0 if method == WORK_QUEUE.
 */
void apply_filter2d_threaded(const filter *f, const int32_t *original,
                             int32_t *target, int32_t width, int32_t height,
                             int32_t num_threads, parallel_method method,
                             int32_t work_chunk);
#endif
