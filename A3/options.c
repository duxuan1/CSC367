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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"


bool opt_nested = false;
bool opt_merge = false;
bool opt_hash = false;
bool opt_replicate = false;
bool opt_symmetric = false;
int opt_nthreads = 0;

void print_usage(char *const argv[])
{
	printf("usage: %s { -n | -m | -h } [{ -r | -s }] [ -t <number of threads> ] <data file path>\n"
	       "\t-n: use Nested loop join\n"
	       "\t-m: use sort-Merge join\n"
	       "\t-h: use Hash join\n"
	       "\t-r: use fragment-and-Replicate"
	       "\t-s: use Symmetric partitioning"
	       "The \'-r\'/\'-s\' argument is mandatory for the OpenMP and MPI versions, ignored by the sequential version"
	       "The \'-t\' argument is mandatory for the OpenMP version, ignored by other versions\n", argv[0]);
}

// Returns the data file path, or NULL if the arguments are invalid; sets the opt_xxx flags according to the arguments
const char *parse_args(int argc, char *const argv[])
{
	assert(argc > 0);
	assert(argv != NULL);

	opt_nested = opt_merge = opt_hash = opt_replicate = opt_symmetric = false;

	char option;
	while ((option = getopt(argc, argv, "nmhrst:")) != -1) {
		switch (option) {
			case 'n': opt_nested = true; break;
			case 'm': opt_merge = true; break;
			case 'h': opt_hash = true; break;
			case 'r': opt_replicate = true; break;
			case 's': opt_symmetric = true; break;
			case 't': opt_nthreads = atoi(optarg); break;
			default:
				print_usage(argv);
				return NULL;
		}
	}

	int join_kinds = (opt_nested ? 1 : 0) + (opt_merge ? 1 : 0) + (opt_hash ? 1 : 0);
	int part_kinds = (opt_replicate ? 1 : 0) + (opt_symmetric ? 1 : 0);
	if ((optind >= argc) || (join_kinds != 1) || (part_kinds > 1)) {
		fprintf(stderr, "Invalid arguments\n");
		print_usage(argv);
		return NULL;
	}

	return argv[optind];
}
