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
#include <stdio.h>
#include <stdlib.h>

// create 2 big images in a header file
void img1() {
  /* WARNING: if you generate an image of total size 64MB, GCC will need
   * 11 GB of RAM to compile the corresponding .c file
   */
  int width = 1 << 10;
  int height = 1 << 10;
  pgm_image image;
  create_random_pgm_image(&image, width, height);

  FILE *f = fopen("very_big_sample.cpp", "w");
  fprintf(f, "#include \"pgm.h\"\n");
  fprintf(f, "int32_t arr1[] = {\n");
  int i;
  for (i = 0; i < width * height; i++) {
    fprintf(f, "%d,", image.matrix[i]);
  }
  fprintf(f, "};\n");
  fprintf(f, "pgm_image very_big_sample= {%d, %d, 255, arr1};\n", width,
          height);

  fclose(f);

  f = fopen("very_big_sample.h", "w");
  fprintf(f, "#ifndef BIG_SAMPLE_H\n");
  fprintf(f, "#define BIG_SAMPLE_H\n");
  fprintf(f, "#include \"pgm.h\"\n");
  fprintf(f, "extern pgm_image very_big_sample;\n");
  fprintf(f, "#endif\n");
}

void img2() {
  int width = 1;
  int height = 1 << 20;
  pgm_image image;
  create_random_pgm_image(&image, width, height);

  FILE *f = fopen("very_tall_sample.cpp", "w");
  fprintf(f, "#include \"pgm.h\"\n");
  fprintf(f, "int32_t arr2[] = {\n");
  int i;
  for (i = 0; i < width * height; i++) {
    fprintf(f, "%d,", image.matrix[i]);
  }
  fprintf(f, "};\n");
  fprintf(f, "pgm_image very_tall_sample = {%d, %d, 255, arr2};\n", width,
          height);

  fclose(f);

  f = fopen("very_tall_sample.h", "w");
  fprintf(f, "#ifndef TALL_SAMPLE_H\n");
  fprintf(f, "#define TALL_SAMPLE_H\n");
  fprintf(f, "#include \"pgm.h\"\n");
  fprintf(f, "extern pgm_image very_tall_sample;\n");
  fprintf(f, "#endif\n");
}

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("creating headers and sources for big images...\n");
    img1();
    img2();
    return 0;
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);

  pgm_image image;

  create_random_pgm_image(&image, width, height);
  int32_t err = save_pgm_to_file(argv[3], &image);

  if (err != NO_ERR) {
    printf("ERR = %d\n", err);
    return 1;
  }

  return 0;
}
