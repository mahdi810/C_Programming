#include "user_t.h"
#include "subject_t.h"

void add_subject_to_db(subject_db_t *subject_db, subject_t *subject_x, unsigned int index)
{
    subject_db->subject_pool[index] = *subject_x;
    subject_db->no_of_subject++;
}

void add_subject_db_to_file(subject_db_t *subject_db, unsigned int index)
{
    FILE *fd = fopen("subject.dat", "wb");
    if (fd == NULL)
    {
        printf("the file could not be opened. \n");
        return;
    }

    fwrite(&index, sizeof(index), 1, fd);
    for (int i = 0; i < index; i++)
    {
        if (!fwrite(&subject_db->subject_pool[i], sizeof(subject_t), 1, fd))
        {
            printf("the %d th strcut could not be saved to the file. \n");
            fclose(fd);
        }
    }
    fclose(fd);
}

void load_subject_db_from_file(subject_db_t *subject_db, unsigned int *no_of_subject)
{
    FILE *fd = fopen("subject.dat", "rb");
    if (fd == NULL)
    {
        printf("couldn't open the file. \n");
        return;
    }

    *no_of_subject = 0;
    fread(no_of_subject, sizeof(unsigned int), 1, fd);

    for (int i = 0; i < *no_of_subject; i++)
    {
        if (!fread(&subject_db->subject_pool[i], sizeof(subject_t), 1, fd))
        {
            printf("could not read the %d th struct from the file. \n");
            fclose(fd);
            return;
        }
    }
    fclose(fd);
}

void print_final_results(subject_db_t *subject_db, unsigned int no_of_subject, double gpa, unsigned int total_ects)
{
    printf("--------------------------------FINAL RESULTS------------------------------- \n");
    printf("%-10s%-10s%-10s \n", "SUBJECT", "GRADE", "ECTS");
    for (int i = 0; i < no_of_subject; i++)
    {
        printf("%-10s%-10.2lf%-10u \n", subject_db->subject_pool[i].name, subject_db->subject_pool[i].grade, subject_db->subject_pool[i].ects);
    }
    printf("-------------------------------------------------------------------------- \n");
    printf("GPA : %.2lf \n", gpa);
    printf("TOTAL ECTS : %u \n", total_ects);
}

void calc_sem_data(subject_db_t *subject_db, unsigned int no_of_subject, double *gpa, unsigned int *total_ects)
{
    double total_grade = 0.0f;
    unsigned int ects_sum = 0;
    double temp;

    // calculation of the total grade and the total ects
    for (int i = 0; i < no_of_subject; i++)
    {
        temp += subject_db->subject_pool[i].grade * subject_db->subject_pool[i].ects;
        ects_sum += subject_db->subject_pool[i].ects;
    }
    total_grade = temp / ects_sum;

    *gpa = total_grade;
    *total_ects = ects_sum;
}
