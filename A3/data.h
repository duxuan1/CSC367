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

#ifndef _DATA_H_
#define _DATA_H_

#include <stdbool.h>


typedef struct _student_record {
	int sid;// Student ID - primary key
	char name[20];
	double gpa;
} student_record;

typedef struct _ta_record {
	int cid;// TA contract ID - primary key
	int sid;// Student ID - foreign key
	char course[8];
} ta_record;


// Arrays are dynamically allocated, must be free'd when no longer needed
int load_data(const char *path, student_record **students, int *students_count, ta_record **tas, int *tas_count);

int store_data(const char *path, const student_record *students, int students_count, const ta_record *tas, int tas_count);


#endif// _DATA_H_
