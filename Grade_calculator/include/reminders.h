#ifndef REMINDERS_H
#define REMINDERS_H

#include "user_t.h"

// Prints a single, date-sorted list of everything with a deadline that
// still needs attention: pending exams, pending projects, and unsettled
// debts (by due date). Overdue items are called out.
void print_reminders(const student_data_t *data);

#endif
