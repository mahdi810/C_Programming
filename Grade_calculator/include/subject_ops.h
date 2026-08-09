#ifndef SUBJECT_OPS_H
#define SUBJECT_OPS_H

#include "user_t.h"

// Returns true if grade is one of the German university grading-scale
// steps: 1.0, 1.3, 1.7, 2.0, 2.3, 2.7, 3.0, 3.3, 3.7, 4.0 (passing,
// 1.0 best) or 5.0 (fail).
bool is_valid_grade(double grade);

// Appends a subject. Fails if the list is full or the grade is not a
// valid German grading-scale value.
bool subject_list_add(subject_list_t *list, const char *name, double grade, unsigned int ects);

// Removes the subject at the given 0-based index.
bool subject_list_remove(subject_list_t *list, unsigned int index);

// Computes the ECTS-weighted average grade across all entered subjects.
// Returns false if there are no subjects or the total ECTS is zero.
bool subject_list_weighted_average(const subject_list_t *list, double *out_average);

void subject_list_print(const subject_list_t *list);

#endif
