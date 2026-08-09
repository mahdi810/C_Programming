#include "date_util.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

date_t date_today(void)
{
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    date_t d = {lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday};
    return d;
}

bool date_parse(const char *text, date_t *out)
{
    if (text == NULL || out == NULL)
    {
        return false;
    }

    int d, m, y;
    if (sscanf(text, "%d.%d.%d", &d, &m, &y) != 3)
    {
        return false;
    }
    if (y < 100)
    {
        y += 2000; // "DD.MM.YY" - assume the 21st century
    }
    if (m < 1 || m > 12 || d < 1 || d > 31 || y < 1900 || y > 3000)
    {
        return false;
    }

    out->year = y;
    out->month = m;
    out->day = d;
    return true;
}

void date_format(const date_t *date, char *buf, size_t buf_len)
{
    snprintf(buf, buf_len, "%02d.%02d.%02d", date->day, date->month, date->year % 100);
}

void datetime_now_format(char *buf, size_t buf_len)
{
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    strftime(buf, buf_len, "%d.%m.%y %H:%M:%S", lt);
}

static time_t date_to_time(const date_t *d)
{
    struct tm tmv;
    memset(&tmv, 0, sizeof(tmv));
    tmv.tm_year = d->year - 1900;
    tmv.tm_mon = d->month - 1;
    tmv.tm_mday = d->day;
    tmv.tm_hour = 12; // noon, sidesteps DST edge cases in the day-difference below
    return mktime(&tmv);
}

long date_days_between(const date_t *from, const date_t *to)
{
    double seconds = difftime(date_to_time(to), date_to_time(from));
    return (long)(seconds >= 0 ? (seconds / 86400.0 + 0.5) : (seconds / 86400.0 - 0.5));
}

bool time_parse(const char *text, time_of_day_t *out)
{
    if (text == NULL || out == NULL)
    {
        return false;
    }

    int h, m;
    if (sscanf(text, "%d:%d", &h, &m) != 2)
    {
        return false;
    }
    if (h < 0 || h > 23 || m < 0 || m > 59)
    {
        return false;
    }

    out->hour = h;
    out->minute = m;
    return true;
}

void time_format(const time_of_day_t *t, char *buf, size_t buf_len)
{
    snprintf(buf, buf_len, "%02d:%02d", t->hour, t->minute);
}

double time_diff_hours(const time_of_day_t *start, const time_of_day_t *end)
{
    int start_minutes = start->hour * 60 + start->minute;
    int end_minutes = end->hour * 60 + end->minute;
    int diff = end_minutes - start_minutes;
    if (diff < 0)
    {
        diff += 24 * 60; // overnight shift spilling into the next day
    }
    return diff / 60.0;
}
