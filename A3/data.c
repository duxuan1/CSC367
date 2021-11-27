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

#include "data.h"


// Arrays are dynamically allocated, must be free'd when no longer needed
int load_data(const char *path, student_record **students, int *students_count, ta_record **tas, int *tas_count)
{
	assert(path != NULL);
	assert(students != NULL);
	assert(students_count != NULL);
	assert(tas != NULL);
	assert(tas_count != NULL);
	*students = NULL;
	*tas = NULL;
	*students_count = *tas_count = 0;

	FILE *f = fopen(path, "r");
	if (f == NULL) {
		perror(path);
		return -1;
	}

	if ((fread(students_count, sizeof(*students_count), 1, f) < 1) ||
	    (fread(tas_count, sizeof(*tas_count), 1, f) < 1))
	{
		fprintf(stderr, "Invalid input file %s\n", path);
		goto error;
	}

	if ((*students = (student_record*)malloc(*students_count * sizeof(**students))) == NULL) {
		perror("malloc");
		goto error;
	}
	if ((*tas = (ta_record*)malloc(*tas_count * sizeof(**tas))) == NULL) {
		perror("malloc");
		free(*students);
		goto error;
	}

	if ((fread(*students, sizeof(**students), *students_count, f) < *students_count) ||
	    (fread(*tas, sizeof(**tas), *tas_count, f) < *tas_count))
	{
		fprintf(stderr, "Invalid input file %s\n", path);
		goto error;
	}
	fclose(f);
	return 0;

error:
	fclose(f);
	if (*students != NULL) {
		free(*students);
		*students = NULL;
	}
	if (*tas != NULL) {
		free(*tas);
		*tas = NULL;
	}
	return -1;
}


int store_data(const char *path, const student_record *students, int students_count, const ta_record *tas, int tas_count)
{
	assert(path != NULL);
	assert(students != NULL);
	assert(tas != NULL);

	FILE *f = fopen(path, "w");
	if (f == NULL) {
		perror(path);
		return -1;
	}

	int result = -1;
	if ((fwrite(&students_count, sizeof(students_count), 1, f) < 1) ||
	    (fwrite(&tas_count, sizeof(tas_count), 1, f) < 1) ||
	    (fwrite(students, sizeof(*students), students_count, f) < students_count) ||
	    (fwrite(tas, sizeof(*tas), tas_count, f) < tas_count))
	{
		fprintf(stderr, "Failed to write to %s\n", path);
	} else {
		result = 0;
	}

	fclose(f);
	return result;
}
