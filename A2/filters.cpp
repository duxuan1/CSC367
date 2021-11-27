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
#include <iostream>

typedef struct common_work_t {
  const filter *f;
  const int32_t *original_image;
  int32_t *output_image;
  int32_t width;
  int32_t height;
  int32_t max_threads;
  pthread_barrier_t barrier;
  int32_t method;
} common_work;

typedef struct work_t {
  common_work *common;
  int32_t id;
} work;

typedef struct chunk_t {
  int32_t row_start;
  int32_t row_end;
  int32_t col_start;
  int32_t col_end;
} chunk;

// int infinite
int32_t maxInt = 2147483647;
int32_t minInt = -2147483648;
//
int32_t *local_smallests;
int32_t *local_largests;
// 
int32_t chunks_count;
chunk *chunks_queue;
int32_t chunk_in_filter;
int32_t chunk_in_norm;
pthread_mutex_t mutex;

/************** FILTER CONSTANTS*****************/
/* laplacian */
int8_t lp3_m[] = {
    0,  1, 0, 
    1, -4, 1, 
    0,  1, 0,
};
filter lp3_f = {3, lp3_m};

int8_t lp5_m[] = {
    -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, 
    -1, -1, 24, -1, -1, 
    -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1,
};
filter lp5_f = {5, lp5_m};

/* Laplacian of gaussian */
int8_t log_m[] = {
    0, 1, 1,   2,   2,   2, 1, 1, 0, 
    1, 2, 4,   5,   5,   5, 4, 2, 1, 
    1, 4, 5,   3,   0,   3, 5, 4, 1, 
    2, 5, 3, -12, -24, -12, 3, 5, 2, 
    2, 5, 0, -24, -40, -24, 0, 5, 2, 
    2, 5, 3, -12, -24, -12, 3, 5, 2, 
    1, 4, 5,   3,   0,   3, 5, 4, 1, 
    1, 2, 4,   5,   5,   5, 4, 2, 1, 
    0, 1, 1,   2,   2,   2, 1, 1, 0,
};
filter log_f = {9, log_m};

/* Identity filter */
int8_t identity_m[] = {1};
filter identity_f = {1, identity_m};

filter *builtin_filters[NUM_FILTERS] = {&lp3_f, &lp5_f, &log_f, &identity_f};

/* Normalizes a pixel given the smallest and largest integer values
 * in the image */
void normalize_pixel(int32_t *target, int32_t pixel_idx, int32_t smallest,
                     int32_t largest) {
  if (smallest == largest) {
    return;
  }

  target[pixel_idx] =
      ((target[pixel_idx] - smallest) * 255) / (largest - smallest);
}

/* Normalizes an array by range
 * */
void normalize_array_by_range(int32_t *target, int32_t smallest, int32_t largest, 
                              int32_t width,int32_t i_start, int32_t i_end, 
                              int32_t j_start, int32_t j_end, bool is_col_major) {
  int32_t i, j;
  for (i = i_start; i < i_end; i++) {
    for (j = j_start; j < j_end; j++) {
      if (is_col_major) {
        normalize_pixel(target, j * width + i, smallest, largest);
      } else {
        normalize_pixel(target, i * width + j, smallest, largest);
      }
    }
  }
}

/*************** COMMON WORK ***********************/
/* Processes a single pixel and returns the value of processed pixel
 * TODO: you don't have to implement/use this function, but this is a hint
 * on how to reuse your code.
 * */
int32_t apply2d(const filter *f, const int32_t *original, int32_t width, 
                int32_t height, int row, int column) {
  int sum = 0; // value of processed pixel
  int dimension = f->dimension; 
  int total = dimension*dimension; // total pixels in filter
  int cr = dimension / 2, cc = dimension / 2; // center of filter

  for (int i = 0; i < total; i++) {
    // filter is a single array, translate i to row and col
    int fr = i / dimension, fc = i % dimension; 
    // only calculate valid neighbour
    int x = row+(fr-cr), y = column+(fc-cc);
    if (x >= 0 && x < height && y >= 0 && y < width) {
      sum += original[x * width + y] * f->matrix[i];
    }
  }
  return sum;
}

/*********SEQUENTIAL IMPLEMENTATIONS ***************/
/* TODO: your sequential implementation goes here.
 * IMPORTANT: you must test this thoroughly with lots of corner cases and
 * check against your own manual calculations on paper, to make sure that your
 * code produces the correct image. Correctness is CRUCIAL here, especially if
 * you re-use this code for filtering pieces of the image in your parallel
 * implementations!
 */
void apply_filter2d(const filter *f, const int32_t *original, int32_t *target, 
                int32_t width, int32_t height) {
  int smallest = maxInt, largest = minInt;
  int32_t i, j;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      // get the sum of the pairwise multiplications
      // and assign new pixel to target
      int sum = apply2d(f, original, width, height, i, j);
      target[i * width + j] = sum;
      // update smallest and largest
      if (sum < smallest) { smallest = sum; }
      if (sum > largest) { largest = sum; }
    }
  }
  normalize_array_by_range(target, smallest, largest, width, 0, height, 0, width, false);
}

/* apply_filter2d for parallel implementation by range
 */
void apply_filter2d_by_range(int32_t id, const filter *f, const int32_t *original, int32_t *target, 
                             int32_t width, int32_t height, int32_t i_start, int32_t i_end,
                             int32_t j_start, int32_t j_end, bool is_col_major) {
  int32_t i, j, sum;
  for (i = i_start; i < i_end; i++) {
    for (j = j_start; j < j_end; j++) {
      if (is_col_major) {
        sum = apply2d(f, original, width, height, j, i);
        target[j * width + i] = sum;
      } else {
        sum = apply2d(f, original, width, height, i, j);
        target[i * width + j] = sum;
      }
      if (sum < local_smallests[id]) { local_smallests[id] = sum; }
      if (sum > local_largests[id]) { local_largests[id] = sum; }
    }
  }
}

/****************** ROW/COLUMN SHARDING ************/
/* TODO: you don't have to implement this. It is just a suggestion for the
 * organization of the code.
 */

/* Recall that, once the filter is applied, all threads need to wait for
 * each other to finish before computing the smallest/largets elements
 * in the resulting matrix. To accomplish that, we declare a barrier variable:
 *      pthread_barrier_t barrier;
 * And then initialize it specifying the number of threads that need to call
 * wait() on it:
 *      pthread_barrier_init(&barrier, NULL, num_threads);
 * Once a thread has finished applying the filter, it waits for the other
 * threads by calling:
 *      pthread_barrier_wait(&barrier);
 * This function only returns after *num_threads* threads have called it.
 */
void *sharding_work(void *arg) {
  // Deserialize work and common work
  work* w = (work*)arg;
  common_work* cw = w->common;
  int32_t id = w->id, width = cw->width, height = cw->height, threads = cw->max_threads;

  // 1) Apply the filter on the image
  int32_t row_start, row_end, row_process, col_start, col_end, col_process;
  row_process = (height%threads != 0) ? ((height+threads-1)/threads) : (height/threads);
  row_start = id*row_process;
  row_end = (row_start+row_process < height) ? (row_start+row_process) : height;
  col_process = (width%threads != 0) ? ((width+threads-1)/threads) : (width/threads);
  col_start = id*col_process;
  col_end = (col_start+col_process < width) ? (col_start+col_process) : width;

  if (cw->method == parallel_method::SHARDED_COLUMNS_COLUMN_MAJOR) {
    apply_filter2d_by_range(id, cw->f, cw->original_image, cw->output_image, 
                            width, height, col_start, col_end, 0, height, true);
  } else if (cw->method == parallel_method::SHARDED_COLUMNS_ROW_MAJOR) {
    apply_filter2d_by_range(id, cw->f, cw->original_image, cw->output_image, 
                            width, height, 0, height, col_start, col_end, false);
  } else if (cw->method == parallel_method::SHARDED_ROWS) {
    apply_filter2d_by_range(id, cw->f, cw->original_image, cw->output_image, 
                            width, height, row_start, row_end, 0, width, false);
  }

  // 2) Wait for all threads to do the same
  pthread_barrier_wait(&cw->barrier);

  // 3) Calculate global smallest/largest elements on the resulting image
  int smallest = maxInt, largest = minInt;
  int32_t i;
  for (i = 0; i < threads; i++) {
    if (local_smallests[i] < smallest) { smallest = local_smallests[i]; }
    if (local_largests[i] > largest) { largest = local_largests[i]; }
  }

  // 4) Scale back the pixels of the image
  if (cw->method == parallel_method::SHARDED_COLUMNS_COLUMN_MAJOR) {
    normalize_array_by_range(cw->output_image, smallest, largest,
                             width, col_start, col_end, 0, height, true);
  } else if (cw->method == parallel_method::SHARDED_COLUMNS_ROW_MAJOR) {
    normalize_array_by_range(cw->output_image, smallest, largest, 
                             width, 0, height, col_start, col_end, false);
  } else if (cw->method == parallel_method::SHARDED_ROWS) {
    normalize_array_by_range(cw->output_image, smallest, largest, 
                             width, row_start, row_end, 0, width, false);
  }
  return NULL;
}

/***************** WORK QUEUE *******************/
/* TODO: you don't have to implement this. It is just a suggestion for the
 * organization of the code.
 */

/* Build all the tiles (work chunks) are statically placed 
 * in a queue at the start of the program
 */
void build_chunks_queue(int32_t width, int32_t height, int32_t work_chunk) {
  int32_t total = width * height, used = 0;
  int32_t row_start = 0, row_end = 0, col_start = 0, col_end = 0;

  while (used < total) {
    int32_t temp_row =  row_start + work_chunk;
    int32_t temp_col = col_start + work_chunk;
    row_end = (temp_row >= height) ? height : temp_row;
    col_end = (temp_col >= width) ? width : temp_col;

    chunks_queue[chunks_count].row_start = row_start;
    chunks_queue[chunks_count].row_end = row_end;
    chunks_queue[chunks_count].col_start = col_start;
    chunks_queue[chunks_count].col_end = col_end;

    used += (row_end - row_start) * (col_end - col_start);
    col_start = col_end;
    if (col_start >= width) {
      col_start = 0;
      row_start += work_chunk;
    }
    chunks_count += 1;
  }
}

/* parallel implementation for work queue
 */
void *queue_work(void *arg) { 
  work* w = (work*)arg;
  common_work* cw = w->common;
  int32_t id = w->id, width = cw->width, height = cw->height, threads = cw->max_threads;

  // 1) Apply the filter on the image
  while (chunk_in_filter < chunks_count) {
    chunk serve;
    
    pthread_mutex_lock(&mutex);
    if (chunk_in_filter >= chunks_count) {
      pthread_mutex_unlock(&mutex);
      break;
    }
    // filter next waiting chunk in queue
    serve = chunks_queue[chunk_in_filter];
    chunk_in_filter += 1;
    pthread_mutex_unlock(&mutex);

    apply_filter2d_by_range(id, cw->f, cw->original_image, cw->output_image, cw->width, cw->height, 
                            serve.row_start, serve.row_end, serve.col_start, serve.col_end, false);
  }
  
  // 2) Wait for all threads to do the same
  pthread_barrier_wait(&cw->barrier);

  // 3) Calculate global smallest/largest elements on the resulting image
  int smallest = maxInt, largest = minInt;
  int32_t i;
  for (i = 0; i < threads; i++) {
    if (local_smallests[i] < smallest) { smallest = local_smallests[i]; }
    if (local_largests[i] > largest) { largest = local_largests[i]; }
  }

  // 4) Scale back the pixels of the image
  while (chunk_in_norm < chunks_count) {
    chunk serve;

    pthread_mutex_lock(&mutex);
    if (chunk_in_norm >= chunks_count) {
      pthread_mutex_unlock(&mutex);
      break;
    }
    // norm next waiting chunk in queue
    serve = chunks_queue[chunk_in_norm];
    chunk_in_norm += 1;
    pthread_mutex_unlock(&mutex);

    normalize_array_by_range(cw->output_image, smallest, largest, width, 
                             serve.row_start, serve.row_end, serve.col_start, serve.col_end, false);
  }
  return NULL; 
}

/***************** MULTITHREADED ENTRY POINT ******/
/* TODO: this is where you should implement the multithreaded version
 * of the code. Use this function to identify which method is being used
 * and then call some other function that implements it.
 */
void apply_filter2d_threaded(const filter *f, const int32_t *original,
                             int32_t *target, int32_t width, int32_t height,
                             int32_t num_threads, parallel_method method,
                             int32_t work_chunk) {
  // init lock and threads instances
  pthread_barrier_t barrier;
  pthread_mutex_init(&mutex, NULL);
  pthread_barrier_init(&barrier, NULL, num_threads);
  pthread_t tpool[num_threads];
  common_work cw = {f, original, target, width, height, num_threads, barrier, method};
  work works[num_threads];

  // build arrays to store local smallest and largest for each thread
  local_smallests = (int32_t*)malloc(num_threads * sizeof(int32_t));
  local_largests = (int32_t*)malloc(num_threads * sizeof(int32_t));

  // build all the tiles (work chunks) are statically placed 
  // in a queue at the start of the program
  if (method == parallel_method::WORK_QUEUE) {
    chunks_count = 0, chunk_in_filter = 0, chunk_in_norm = 0;
    chunks_queue = (chunk*)malloc(width * height * sizeof(chunk));
    build_chunks_queue(width, height, work_chunk);
  }

  // create threads
  int err;
	for (int i = 0; i < num_threads; i++) {
    works[i].id = i, works[i].common = &cw;
    local_smallests[i] = maxInt, local_largests[i] = minInt;
    if (method != parallel_method::WORK_QUEUE) {
      err = pthread_create(&tpool[i], NULL, sharding_work, (void*)&works[i]);
    } else {
      err = pthread_create(&tpool[i], NULL, queue_work, (void*)&works[i]);
    }
	}

  // join threads
	for (int i = 0; i < num_threads; i++) {
    err = pthread_join(tpool[i], NULL);
	}

  // clean up
  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&mutex);
  free(local_smallests);
  free(local_largests);
  if (method == parallel_method::WORK_QUEUE) {
    free(chunks_queue);
  }
}
