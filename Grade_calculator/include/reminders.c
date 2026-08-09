#include "reminders.h"
#include "date_util.h"

typedef struct
{
    char category[12];
    char label[2 * NAME_LEN + 4];
    date_t date;
} reminder_item_t;

static int date_compare(const date_t *a, const date_t *b)
{
    if (a->year != b->year)
    {
        return a->year - b->year;
    }
    if (a->month != b->month)
    {
        return a->month - b->month;
    }
    return a->day - b->day;
}

void print_reminders(const student_data_t *data)
{
    reminder_item_t items[MAX_EXAMS + MAX_PROJECTS + MAX_DEBTS];
    unsigned int count = 0;

    for (unsigned int i = 0; i < data->exam_count; i++)
    {
        if (data->exams[i].done)
        {
            continue;
        }
        snprintf(items[count].category, sizeof(items[count].category), "EXAM");
        snprintf(items[count].label, sizeof(items[count].label), "%s (%s)", data->exams[i].title, data->exams[i].subject);
        items[count].date = data->exams[i].date;
        count++;
    }

    for (unsigned int i = 0; i < data->project_count; i++)
    {
        if (data->projects[i].done)
        {
            continue;
        }
        snprintf(items[count].category, sizeof(items[count].category), "PROJECT");
        snprintf(items[count].label, sizeof(items[count].label), "%s (%s)", data->projects[i].title, data->projects[i].subject);
        items[count].date = data->projects[i].date;
        count++;
    }

    for (unsigned int i = 0; i < data->budget.debt_count; i++)
    {
        const debt_t *d = &data->budget.debts[i];
        if (d->settled)
        {
            continue;
        }
        snprintf(items[count].category, sizeof(items[count].category), "DEBT");
        snprintf(items[count].label, sizeof(items[count].label), "%s %s: %.2f %s",
                 d->direction == DEBT_OWED_TO_ME ? "collect from" : "pay", d->counterparty, d->remaining_amount, d->currency);
        items[count].date = d->due_date;
        count++;
    }

    if (count == 0)
    {
        printf("nothing pending - you're all caught up. \n");
        return;
    }

    // simple insertion sort by date ascending - count is at most a few dozen
    for (unsigned int i = 1; i < count; i++)
    {
        reminder_item_t key = items[i];
        int j = (int)i - 1;
        while (j >= 0 && date_compare(&items[j].date, &key.date) > 0)
        {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = key;
    }

    date_t today = date_today();

    printf("+----------+----------+------------------------------------------------+---------------+\n");
    printf("| %-8s | %-8s | %-46s | %-13s |\n", "type", "date", "item", "status");
    printf("+----------+----------+------------------------------------------------+---------------+\n");
    for (unsigned int i = 0; i < count; i++)
    {
        char date_buf[9];
        date_format(&items[i].date, date_buf, sizeof(date_buf));
        long days = date_days_between(&today, &items[i].date);

        char remark[32];
        if (days < 0)
        {
            snprintf(remark, sizeof(remark), "overdue %ldd", -days);
        }
        else if (days == 0)
        {
            snprintf(remark, sizeof(remark), "today");
        }
        else
        {
            snprintf(remark, sizeof(remark), "in %ldd", days);
        }

        printf("| %-8s | %-8s | %-46s | %-13s |\n", items[i].category, date_buf, items[i].label, remark);
    }
    printf("+----------+----------+------------------------------------------------+---------------+\n");
}
