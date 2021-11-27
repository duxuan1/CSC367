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

#ifndef __PGM__H
#define __PGM__H

#include <stdint.h>

#define NO_ERR 0
#define ERR_NO_FILE 1
#define ERR_INVALID_HEADER 2
#define ERR_INVALID_RASTER 3
#define ERR_OPEN_SAVEFILE 4
#define ERR_WRITING_TO_FILE 5
#define ERR_MALLOC 6

typedef struct pgm_image_t {
  int32_t width;
  int32_t height;
  int32_t max_gray;
  int32_t *matrix;
} pgm_image;

/* Initialization function, must be called before
 * the struct is used by load/save functions.
 */
void init_pgm_image(pgm_image *image);

/* Must be called to free the memory used by this struct,
 * even if it was used by a load/save function which returned
 * with errors.
 */
void destroy_pgm_image(pgm_image *image);

/* Creates an image of the same size as the original, allocating a buffer
 * of appropriate size.
 */
int32_t copy_pgm_image_size(const pgm_image *image, pgm_image *target);

/* Create an image with arbitrary contents, with the given dimensions
 */
int32_t create_random_pgm_image(pgm_image *image, int32_t width,
                                int32_t height);

int32_t load_pgm_from_file(const char *filename, pgm_image *image);
int32_t save_pgm_to_file(const char *filename, const pgm_image *image);
#endif
