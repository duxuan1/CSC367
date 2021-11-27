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

#include "pgm.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_pgm_image(pgm_image *image) {
  image->width = 0;
  image->height = 0;
  image->max_gray = 0;
  image->matrix = NULL;
}

void destroy_pgm_image(pgm_image *image) { free(image->matrix); }

/* Helper function that advances the file stream past the
 * next comment, if it exists.
 */
void remove_comments(FILE *f) {
  char c = getc(f);
  if (c == '#') {
    while (c != '\n') {
      c = getc(f);
    }
  } else {
    ungetc(c, f);
  }
}

int32_t load_pgm_from_file(const char *filename, pgm_image *image) {
  FILE *file = fopen(filename, "rb");

  if (file == NULL) {
    return ERR_NO_FILE;
  }

  char magic_number[2];

  int32_t num = fscanf(file, "%c%c ", &magic_number[0], &magic_number[1]);
  remove_comments(file);
  num += fscanf(file, "%u ", &image->width);
  remove_comments(file);
  num += fscanf(file, "%u ", &image->height);
  remove_comments(file);
  num += fscanf(file, "%u", &image->max_gray);
  char c = getc(file);
  if (!isspace(c)) {
    return ERR_INVALID_HEADER;
  }

  if (num != 5 || magic_number[0] != 'P' || magic_number[1] != '5') {
    fclose(file);
    return ERR_INVALID_HEADER;
  }

  uint8_t *temp =
      (uint8_t *)malloc(image->height * image->width * sizeof(uint8_t));
  image->matrix =
      (int32_t *)malloc(image->height * image->width * sizeof(int32_t));
  if (image->matrix == NULL || temp == NULL) {
    fclose(file);
    return ERR_MALLOC;
  }

  int32_t count = fread(temp, image->width * image->height, 1, file);

  if (count != 1 || ferror(file) != 0) {
    fclose(file);
    return ERR_INVALID_RASTER;
  }

  int32_t i;
  for (i = 0; i < image->height * image->width; i++) {
    image->matrix[i] = temp[i];
  }

  free(temp);
  fclose(file);
  return NO_ERR;
}

int32_t save_pgm_to_file(const char *filename, const pgm_image *image) {
  FILE *file = fopen(filename, "wb");

  if (file == NULL) {
    return ERR_OPEN_SAVEFILE;
  }

  fprintf(file, "P5 %d %d %d\n", image->width, image->height, image->max_gray);

  int32_t i;
  for (i = 0; i < image->height * image->width; i++) {
    uint8_t temp = image->matrix[i];
    if (fwrite(&temp, sizeof(uint8_t), 1, file) != 1) {
      return ERR_WRITING_TO_FILE;
    }
  }

  fclose(file);
  return NO_ERR;
}

int32_t copy_pgm_image_size(const pgm_image *image, pgm_image *target) {
  int32_t *matrix =
      (int32_t *)malloc(image->width * image->height * sizeof(int32_t));
  if (matrix == NULL) {
    return ERR_MALLOC;
  }

  target->width = image->width;
  target->height = image->height;
  target->max_gray = image->max_gray;
  target->matrix = matrix;

  return NO_ERR;
}

int32_t create_random_pgm_image(pgm_image *image, int32_t width,
                                int32_t height) {
  image->width = width;
  image->height = height;
  image->max_gray = 255;
  int32_t *matrix =
      (int32_t *)malloc(image->width * image->height * sizeof(int32_t));

  if (matrix == NULL) {
    return ERR_MALLOC;
  }

  image->matrix = matrix;

  uint8_t pixel = 1;
  int32_t i, j;

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      matrix[i * width + j] = pixel++;
    }
  }

  return NO_ERR;
}
