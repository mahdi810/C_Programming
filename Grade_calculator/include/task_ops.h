#ifndef TASK_OPS_H
#define TASK_OPS_H

#include "user_t.h"

// Adds an entry (used for both exams and projects - list/count/max_count
// point at whichever array is being managed). date_text must be "YYYY-MM-DD".
bool task_list_add(task_t *list, unsigned int *count, unsigned int max_count,
                    const char *title, const char *subject, const char *date_text);

// Removes the entry at the given 0-based index.
bool task_list_remove(task_t *list, unsigned int *count, unsigned int index);

// Marks the entry at the given 0-based index as done.
bool task_list_mark_done(task_t *list, unsigned int count, unsigned int index);

// Prints a table with a due/overdue countdown. label is used in the
// "no <label> entries yet" message (e.g. "exam", "project").
void task_list_print(const task_t *list, unsigned int count, const char *label);

#endif
