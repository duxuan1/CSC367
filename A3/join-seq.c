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

#include <stdio.h>
#include <stdlib.h>

#include "join.h"
#include "options.h"
#include "time_util.h"


int main(int argc, char *argv[])
{
	const char *path = parse_args(argc, argv);
	if (path == NULL) return 1;

	int students_count, tas_count;
	student_record *students;
	ta_record *tas;
	if (load_data(path, &students, &students_count, &tas, &tas_count) != 0) return 1;

	int result = 1;
	join_func_t *join_f = opt_nested ? join_nested : (opt_merge ? join_merge : join_hash);

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int count = join_f(students, students_count, tas, tas_count);
	clock_gettime(CLOCK_MONOTONIC, &end);

	if (count < 0) goto end;
	printf("%d\n", count);
	printf("%f\n", timespec_to_msec(difftimespec(end, start)));
	result = 0;

end:
	free(students);
	free(tas);
	return result;
}
