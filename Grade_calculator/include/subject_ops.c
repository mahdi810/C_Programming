#include "subject_ops.h"
#include <math.h>

static const double VALID_GRADES[] = {1.0, 1.3, 1.7, 2.0, 2.3, 2.7, 3.0, 3.3, 3.7, 4.0, 5.0};
#define VALID_GRADE_COUNT (sizeof(VALID_GRADES) / sizeof(VALID_GRADES[0]))

bool is_valid_grade(double grade)
{
    for (size_t i = 0; i < VALID_GRADE_COUNT; i++)
    {
        if (fabs(grade - VALID_GRADES[i]) < 0.001)
        {
            return true;
        }
    }
    return false;
}

bool subject_list_add(subject_list_t *list, const char *name, double grade, unsigned int ects)
{
    if (list == NULL || name == NULL)
    {
        return false;
    }
    if (list->count >= MAX_SUBJECTS)
    {
        printf("the subject list is full. \n");
        return false;
    }
    if (!is_valid_grade(grade))
    {
        printf("invalid grade, allowed values are 1.0,1.3,1.7,2.0,2.3,2.7,3.0,3.3,3.7,4.0,5.0. \n");
        return false;
    }

    subject_t *s = &list->subjects[list->count];
    strncpy(s->name, name, NAME_LEN - 1);
    s->name[NAME_LEN - 1] = '\0';
    s->grade = grade;
    s->ects = ects;
    list->count++;
    return true;
}

bool subject_list_remove(subject_list_t *list, unsigned int index)
{
    if (list == NULL || index >= list->count)
    {
        return false;
    }
    for (unsigned int i = index; i + 1 < list->count; i++)
    {
        list->subjects[i] = list->subjects[i + 1];
    }
    list->count--;
    return true;
}

bool subject_list_weighted_average(const subject_list_t *list, double *out_average)
{
    if (list == NULL || out_average == NULL || list->count == 0)
    {
        return false;
    }

    double weighted_sum = 0.0;
    unsigned int total_ects = 0;
    for (unsigned int i = 0; i < list->count; i++)
    {
        weighted_sum += list->subjects[i].grade * list->subjects[i].ects;
        total_ects += list->subjects[i].ects;
    }

    if (total_ects == 0)
    {
        return false;
    }

    *out_average = weighted_sum / total_ects;
    return true;
}

void subject_list_print(const subject_list_t *list)
{
    if (list == NULL || list->count == 0)
    {
        printf("no subjects entered yet. \n");
        return;
    }

    printf("+-----+--------------------------------+---------+---------+\n");
    printf("| %-3s | %-30s | %-7s | %-7s |\n", "#", "subject", "grade", "ECTS");
    printf("+-----+--------------------------------+---------+---------+\n");
    for (unsigned int i = 0; i < list->count; i++)
    {
        printf("| %-3u | %-30s | %-7.1f | %-7u |\n", i + 1, list->subjects[i].name, list->subjects[i].grade, list->subjects[i].ects);
    }
    printf("+-----+--------------------------------+---------+---------+\n");

    double avg;
    if (subject_list_weighted_average(list, &avg))
    {
        printf("ECTS-weighted average grade: %.2f \n", avg);
    }
}
