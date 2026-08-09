#ifndef DATE_UTIL_H
#define DATE_UTIL_H

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    int year;
    int month;
    int day;
} date_t;

typedef struct
{
    int hour;
    int minute;
} time_of_day_t;

// Today's date (local time).
date_t date_today(void);

// Parses "DD.MM.YY" (or "DD.MM.YYYY") into a date_t. Returns false on
// malformed input or an impossible calendar date.
bool date_parse(const char *text, date_t *out);

// Formats a date_t as "DD.MM.YY" into buf (must be at least 9 bytes).
void date_format(const date_t *date, char *buf, size_t buf_len);

// Formats the current local date and time as "DD.MM.YY HH:MM:SS".
void datetime_now_format(char *buf, size_t buf_len);

// Whole days from `from` to `to` (negative if `to` is before `from`).
long date_days_between(const date_t *from, const date_t *to);

// Parses "HH:MM" (24-hour) into a time_of_day_t.
bool time_parse(const char *text, time_of_day_t *out);

// Formats a time_of_day_t as "HH:MM" into buf (must be at least 6 bytes).
void time_format(const time_of_day_t *t, char *buf, size_t buf_len);

// Hours between start and end, both on the same date. If end is earlier
// than start (an overnight shift), it's treated as spilling into the
// next day.
double time_diff_hours(const time_of_day_t *start, const time_of_day_t *end);

#endif
