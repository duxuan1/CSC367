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

#include <limits.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "join.h"
#include "options.h"

/* Build MPI type for student_record
 */
MPI_Datatype create_student_type() {
	MPI_Datatype student_type;
    int lengths[3] = { 1, 20, 1 }; // equivlent to student_record

    MPI_Aint displacements[3];
    struct _student_record dummy_student;
    MPI_Aint base_address;
    MPI_Get_address(&dummy_student, &base_address);
    MPI_Get_address(&dummy_student.sid, &displacements[0]);
    MPI_Get_address(&dummy_student.name[0], &displacements[1]);
    MPI_Get_address(&dummy_student.gpa, &displacements[2]);
    displacements[0] = MPI_Aint_diff(displacements[0], base_address);
    displacements[1] = MPI_Aint_diff(displacements[1], base_address);
    displacements[2] = MPI_Aint_diff(displacements[2], base_address);
 
    MPI_Datatype types[3] = { MPI_INT, MPI_CHAR, MPI_DOUBLE };
    MPI_Type_create_struct(3, lengths, displacements, types, &student_type);
    MPI_Type_commit(&student_type);
	return student_type;
}

/* Build MPI type for ta_record
 */
MPI_Datatype create_ta_type() {
	MPI_Datatype ta_type;
    int lengths[3] = { 1, 1, 8 }; // equivlent to ta_record

    MPI_Aint displacements[3];
    struct _ta_record dummy_ta;
    MPI_Aint base_address;
    MPI_Get_address(&dummy_ta, &base_address);
    MPI_Get_address(&dummy_ta.cid, &displacements[0]);
    MPI_Get_address(&dummy_ta.sid, &displacements[1]);
    MPI_Get_address(&dummy_ta.course[0], &displacements[2]);
    displacements[0] = MPI_Aint_diff(displacements[0], base_address);
    displacements[1] = MPI_Aint_diff(displacements[1], base_address);
    displacements[2] = MPI_Aint_diff(displacements[2], base_address);
 
    MPI_Datatype types[3] = { MPI_INT, MPI_INT, MPI_CHAR };
    MPI_Type_create_struct(3, lengths, displacements, types, &ta_type);
    MPI_Type_commit(&ta_type);
	return ta_type;
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int result = 1;
	student_record *students = NULL;
	ta_record *tas = NULL;

	const char *path = parse_args(argc, argv);
	if (path == NULL) goto end;

	if (!opt_replicate && !opt_symmetric) {
		fprintf(stderr, "Invalid arguments: parallel algorithm (\'-r\' or \'-s\') is not specified\n");
		print_usage(argv);
		goto end;
	}

	int parts, id;
	MPI_Comm_size(MPI_COMM_WORLD, &parts);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	// Load this process's partition of data
	char part_path[PATH_MAX] = "";
	snprintf(part_path, sizeof(part_path), "%s_%d", path, id);
	int students_count, tas_count;
	if (load_data(part_path, &students, &students_count, &tas, &tas_count) != 0) goto end;
	join_func_t *join_f = opt_nested ? join_nested : (opt_merge ? join_merge : join_hash);
	int count = 0;
	
	MPI_Barrier(MPI_COMM_WORLD);
	double t_start = MPI_Wtime();

	//TODO: parallel join using MPI
	(void)join_f;
	int local_count = 0;
	student_record *all_students = NULL;
	// ta_record *all_tas = NULL;

	if (opt_replicate) { // Fragment-and-replicate
		// sum students_count and tas_count from all processes
		int all_students_count, all_tas_count;
		MPI_Allreduce(&students_count, &all_students_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		MPI_Allreduce(&tas_count, &all_tas_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);

		// Create the datatype
		MPI_Datatype student_type = create_student_type();
		// message passing to add all partitions from processes together
		all_students = (student_record*)malloc(all_students_count * sizeof(student_record));
		MPI_Allgather(students, students_count, student_type, 
						all_students, students_count, student_type, 
						MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		local_count += join_f(all_students, all_students_count, tas, tas_count);
	} else { // Symmetric partitioning
		local_count += join_f(students, students_count, tas, tas_count);
	}
	// add local count to global count
	MPI_Allreduce(&local_count, &count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	double t_end = MPI_Wtime();

	if (count < 0) goto end;
	if (id == 0) {
		printf("%d\n", count);
		printf("%f\n", (t_end - t_start) * 1000.0);
	}
	result = 0;

end:
	if (students != NULL) free(students);
	if (tas != NULL) free(tas);
	MPI_Finalize();
	return result;
}
