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
#include <omp.h>

#include "join.h"
#include "options.h"

int main(int argc, char *argv[])
{
	const char *path = parse_args(argc, argv);
	if (path == NULL) return 1;

	if (!opt_replicate && !opt_symmetric) {
		fprintf(stderr, "Invalid arguments: parallel algorithm (\'-r\' or \'-s\') is not specified\n");
		print_usage(argv);
		return 1;
	}

	if (opt_nthreads <= 0) {
		fprintf(stderr, "Invalid arguments: number of threads (\'-t\') not specified\n");
		print_usage(argv);
		return 1;
	}
	omp_set_num_threads(opt_nthreads);

	int students_count, tas_count;
	student_record *students;
	ta_record *tas;
	if (load_data(path, &students, &students_count, &tas, &tas_count) != 0) return 1;

	int result = 1;
	join_func_t *join_f = opt_nested ? join_nested : (opt_merge ? join_merge : join_hash);

	double t_start = omp_get_wtime();

	//TODO: parallel join using OpenMP
	(void)join_f;
	int count = 0;
	int i;
	int n = opt_nthreads;
	int student_size = students_count / n;
	int ta_size = tas_count / n;

	#pragma omp parallel reduction(+:count) shared(n, student_size, ta_size) private(i) num_threads(n) 
	{	
		i = omp_get_thread_num();
		if (opt_replicate) { // Fragment-and-replicate
			// partition the larger fragment
			if (students_count > tas_count) {
				int student_start = i * student_size;
				int new_students_count = student_size;
				if (i == n - 1 && (student_start + student_size) < students_count) {
					new_students_count = students_count - student_start;
				}
				count += join_f(&students[student_start], new_students_count, tas, tas_count);
			} else {
				int ta_start = i * ta_size;
				int new_ta_count = ta_size;
				if (i == n - 1 && (ta_start + ta_size) < tas_count) {
					new_ta_count = tas_count - ta_start;
				}
				count += join_f(students, students_count, &tas[ta_start], new_ta_count);
			}
		} else { // Symmetric partitioning
			int ta_start = i * ta_size;
			int new_ta_count = ta_size;
			if (i == n - 1 && (ta_start + ta_size) < tas_count) {
				new_ta_count = tas_count - ta_start;
			}
			// take advantege of sorted sid, map ta directly to sid
			int first_ta = tas[ta_start].sid;
			int last_ta = tas[ta_start + new_ta_count - 1].sid;
			count += join_f(&students[first_ta], last_ta - first_ta + 1, &tas[ta_start], new_ta_count);
		}
	}

	double t_end = omp_get_wtime();

	if (count < 0) goto end;
	printf("%d\n", count);
	printf("%f\n", (t_end - t_start) * 1000.0);
	result = 0;

end:
	free(students);
	free(tas);
	return result;
}
