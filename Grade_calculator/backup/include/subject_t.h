#pragma once
#include "user_t.h"

#define MAX_SUBJECT 50

typedef struct
{
    char name[cmd_len];
    double grade;
    unsigned int ects;
} subject_t;

typedef struct
{
    unsigned int no_of_subject;
    subject_t subject_pool[MAX_SUBJECT];
} subject_db_t;

void add_subject_to_db(subject_db_t *subject_db, subject_t *subject_x, unsigned int index);
void add_subject_db_to_file(subject_db_t *subject_db, unsigned int index);
void load_subject_db_from_file(subject_db_t *subject_db, unsigned int *no_of_subject);
void print_final_results(subject_db_t *subject_db, unsigned int no_of_subject, double gpa, unsigned int total_ects);
void calc_sem_data(subject_db_t *subject_db, unsigned int no_of_subject, double *gpa, unsigned int *total_ects);
