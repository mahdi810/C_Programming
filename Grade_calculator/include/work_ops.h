#ifndef WORK_OPS_H
#define WORK_OPS_H

#include "user_t.h"

// Adds a work shift. date_text is "DD.MM.YY" (blank = today), start_text
// and end_text are "HH:MM" (24-hour). An end time earlier than the start
// time is treated as an overnight shift.
bool work_log_add(work_log_t *log, const char *note, const char *date_text, const char *start_text, const char *end_text);

// Removes the shift at the given 0-based index.
bool work_log_remove(work_log_t *log, unsigned int index);

// Hours worked in a single shift.
double work_shift_hours(const work_shift_t *shift);

void work_log_print(const work_log_t *log);

// Prints total hours worked all-time, in the last 7 days, and this
// calendar month.
void work_summary_print(const work_log_t *log);

#endif
