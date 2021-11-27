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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "join.h"

int size;

struct node {
    int key;
    float value; 
    struct node* next;      
};
typedef struct node node_t;

typedef struct {
    int size;
    node_t** list; 
} hash_table_t;

// hash code 1 : 10
int hash32shift(int key) {
	return key%(size/10);
}

void linkedlist_destroy(node_t** head) {
	if (!*head) return;
	node_t* cur = *head;
	node_t* next;
	while (cur != NULL) {
		next = cur->next; 
		free(cur);
		cur = next;
	}
	*head = NULL;
}

// returns -1 if not found
float linkedlist_find(node_t**head, int key) {
	if (!*head) return -1;
	for (node_t* cur = *head, *next; cur ; cur = next) {
		if (cur->key == key) {
			return cur->value;
		}
		next = cur->next;
	}
	return -1;
}

void linkedlist_prepend(node_t**head, int key, float value) {
	node_t* node = (node_t*) malloc(sizeof(node_t));
	node->key = key;
	node->value = value;
	node->next = NULL;
	if (!*head) {
		*head = node;
	} else {
		node_t* next = *head;
		*head = node;
		(*head)->next = next;
	}
}

// Create a hash table with 'size' buckets;
// the storage is allocated dynamically using malloc(); 
// returns NULL on error
hash_table_t *hash_create(int size) {
	assert(size > 0);
	hash_table_t* table = (hash_table_t*) malloc(sizeof(hash_table_t));
	table->size = size;
	table->list = (node_t**) malloc(size * sizeof(node_t*));
	for (int i = 0; i < size; ++i) {
			table->list[i] = NULL;
	}
	return table;
}

// Release all memory used by the hash table, 
// its buckets and entries
void hash_destroy(hash_table_t *table) {
	assert(table != NULL);
	for (int i = 0; i < table->size; ++i) {
		linkedlist_destroy(&(table->list[i]));
	}
	free(table->list);
}

// Returns -1 if key is not found
float hash_get(hash_table_t *table, int key) {
	assert(table != NULL);
	int hash = hash32shift(key) % table->size;
	if (table->list[hash] == NULL) {
		return -1;
	}
	return linkedlist_find(&(table->list[hash]), key);
}

// Returns 0 on success, -1 on failure
int hash_put(hash_table_t *table, int key, float value) {
	assert(table != NULL);
	int hash = hash32shift(key) % table->size;
	linkedlist_prepend(&(table->list[hash]), key, value);
	return 0;
}

int join_nested(const student_record *students, int students_count, const ta_record *tas, int tas_count)
{
	assert(students != NULL);
	assert(tas != NULL);

	int count = 0;
	for(int s = 0; s < students_count; s++) {
		for(int t = 0; t < tas_count; t++) {
			if (students[s].sid == tas[t].sid && students[s].gpa > 3.0) {
				count++;
			}	
		}
	}		

	return count;
}

// Assumes that the records in both tables are already sorted by sid
int join_merge(const student_record *students, int students_count, const ta_record *tas, int tas_count) {
	assert(students != NULL);
	assert(tas != NULL);

	int s = 0, t = 0;
	int count = 0;
	while (s < students_count && t < tas_count) {
		if (students[s].sid > tas[t].sid) {
			t++;
		} else if (students[s].sid < tas[t].sid) {
			s++;
		} else {
			if (students[s].gpa > 3.0) {
				count++;
			}
			int ns = s + 1;
			while (ns < students_count && students[ns].sid == tas[t].sid) {
				if (students[ns].gpa > 3.0) {
					count++;
				}
				ns++;
			}
			int nt = t + 1;
			while (nt < tas_count && students[s].sid == tas[nt].sid) {
				if (students[s].gpa > 3.0) {
					count++;
				}
				nt++;
			}
			s = ns;
			t = nt;
		}
	}
	return count;
}

int join_hash(const student_record *students, int students_count, const ta_record *tas, int tas_count) {
	assert(students != NULL);
	assert(tas != NULL);

	size = students_count;
	int count = 0;
	hash_table_t *student_record = hash_create(students_count/10);
	for(int s = 0; s < students_count; s++) {
		hash_put(student_record, students[s].sid, students[s].gpa);
	}	

	for(int t = 0; t < tas_count; t++) {
		float gpa = hash_get(student_record, tas[t].sid);
		if (gpa > 3.0) {
			count++;
		}
	}
	hash_destroy(student_record);
	return count;
}
