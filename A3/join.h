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

#ifndef _JOIN_H_
#define _JOIN_H_

#include "data.h"


typedef int join_func_t(const student_record *students, int students_count, const ta_record *tas, int tas_count);

int join_nested(const student_record *students, int students_count, const ta_record *tas, int tas_count);

// Assumes that records in both tables are already sorted by sid
int join_merge(const student_record *students, int students_count, const ta_record *tas, int tas_count);

int join_hash(const student_record *students, int students_count, const ta_record *tas, int tas_count);


#endif// _JOIN_H_
