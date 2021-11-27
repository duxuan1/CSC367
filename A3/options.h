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

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>


extern bool opt_nested;
extern bool opt_merge;
extern bool opt_hash;
extern bool opt_replicate;
extern bool opt_symmetric;
extern int opt_nthreads;

void print_usage(char *const argv[]);

// Returns the data file path, or NULL if the arguments are invalid; sets the opt_xxx flags according to the arguments
const char *parse_args(int argc, char *const argv[]);


#endif// _OPTIONS_H_
