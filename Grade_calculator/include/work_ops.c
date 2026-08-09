#include "work_ops.h"
#include "date_util.h"

bool work_log_add(work_log_t *log, const char *note, const char *date_text, const char *start_text, const char *end_text)
{
    if (log == NULL || start_text == NULL || end_text == NULL)
    {
        return false;
    }
    if (log->count >= MAX_WORK_SHIFTS)
    {
        printf("the work log is full. \n");
        return false;
    }

    date_t d;
    if (date_text == NULL || date_text[0] == '\0')
    {
        d = date_today();
    }
    else if (!date_parse(date_text, &d))
    {
        printf("invalid date, please use DD.MM.YY (or leave blank for today). \n");
        return false;
    }

    time_of_day_t start, end;
    if (!time_parse(start_text, &start) || !time_parse(end_text, &end))
    {
        printf("invalid time, please use HH:MM (24-hour). \n");
        return false;
    }

    work_shift_t *shift = &log->shifts[log->count];
    shift->date = d;
    shift->start = start;
    shift->end = end;
    if (note != NULL)
    {
        strncpy(shift->note, note, BUDGET_DESC_LEN - 1);
        shift->note[BUDGET_DESC_LEN - 1] = '\0';
    }
    else
    {
        shift->note[0] = '\0';
    }

    log->count++;
    return true;
}

bool work_log_remove(work_log_t *log, unsigned int index)
{
    if (log == NULL || index >= log->count)
    {
        return false;
    }
    for (unsigned int i = index; i + 1 < log->count; i++)
    {
        log->shifts[i] = log->shifts[i + 1];
    }
    log->count--;
    return true;
}

double work_shift_hours(const work_shift_t *shift)
{
    return time_diff_hours(&shift->start, &shift->end);
}

void work_log_print(const work_log_t *log)
{
    if (log == NULL || log->count == 0)
    {
        printf("no work shifts logged yet. \n");
        return;
    }

    printf("+-----+----------+---------+---------+---------+------------------------+\n");
    printf("| %-3s | %-8s | %-7s | %-7s | %-7s | %-22s |\n", "#", "date", "start", "end", "hours", "note");
    printf("+-----+----------+---------+---------+---------+------------------------+\n");
    for (unsigned int i = 0; i < log->count; i++)
    {
        const work_shift_t *s = &log->shifts[i];
        char date_buf[9], start_buf[6], end_buf[6];
        date_format(&s->date, date_buf, sizeof(date_buf));
        time_format(&s->start, start_buf, sizeof(start_buf));
        time_format(&s->end, end_buf, sizeof(end_buf));

        printf("| %-3u | %-8s | %-7s | %-7s | %7.2f | %-22s |\n", i + 1, date_buf, start_buf, end_buf,
               work_shift_hours(s), s->note);
    }
    printf("+-----+----------+---------+---------+---------+------------------------+\n");
}

void work_summary_print(const work_log_t *log)
{
    if (log == NULL || log->count == 0)
    {
        printf(" no work shifts logged yet. \n");
        return;
    }

    date_t today = date_today();
    double total_hours = 0.0, last_7_days = 0.0, this_month = 0.0;

    for (unsigned int i = 0; i < log->count; i++)
    {
        double hours = work_shift_hours(&log->shifts[i]);
        total_hours += hours;

        long days_ago = date_days_between(&log->shifts[i].date, &today);
        if (days_ago >= 0 && days_ago < 7)
        {
            last_7_days += hours;
        }

        if (log->shifts[i].date.year == today.year && log->shifts[i].date.month == today.month)
        {
            this_month += hours;
        }
    }

    printf(" %-20s: %.2f \n", "total hours logged", total_hours);
    printf(" %-20s: %.2f \n", "last 7 days", last_7_days);
    printf(" %-20s: %.2f \n", "this month", this_month);
}
