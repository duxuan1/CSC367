# ------------
# This code is provided solely for the personal and private use of
# students taking the CSC367H5 course at the University of Toronto.
# Copying for purposes other than this use is expressly prohibited.
# All forms of distribution of this code, whether as given or with
# any changes, are expressly prohibited.
#
# Authors: Bogdan Simion, Alexey Khrabrov
#
# All of the files in this directory and all subdirectories are:
# Copyright (c) 2019 Bogdan Simion
# -------------

CC = gcc
MPICC = mpicc
CFLAGS += -std=gnu11 -Wall -Werror -fopenmp -g3 -O3 -DNDEBUG
LDFLAGS += -lm -fopenmp

all: join-seq join-omp join-mpi

data.o: data.h
join.o: join.h data.h
options.o: options.h
join-seq: time_util.h

join-seq: join-seq.o join.o data.o options.o
	$(CC) $^ -o $@ $(LDFLAGS)

join-omp: join-omp.o join.o data.o options.o
	$(CC) $^ -o $@ $(LDFLAGS)

join-mpi.o: join-mpi.c
	$(MPICC) $(CFLAGS) -c $< -o $@

join-mpi: join-mpi.o join.o data.o options.o
	$(MPICC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o join-seq join-mpi join-omp
