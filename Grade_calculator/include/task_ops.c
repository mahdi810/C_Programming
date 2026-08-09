#include "task_ops.h"
#include "date_util.h"

bool task_list_add(task_t *list, unsigned int *count, unsigned int max_count,
                    const char *title, const char *subject, const char *date_text)
{
    if (list == NULL || count == NULL || title == NULL || subject == NULL || date_text == NULL)
    {
        return false;
    }
    if (*count >= max_count)
    {
        printf("the list is full. \n");
        return false;
    }

    date_t d;
    if (!date_parse(date_text, &d))
    {
        printf("invalid date, please use DD.MM.YY. \n");
        return false;
    }

    task_t *t = &list[*count];
    strncpy(t->title, title, NAME_LEN - 1);
    t->title[NAME_LEN - 1] = '\0';
    strncpy(t->subject, subject, NAME_LEN - 1);
    t->subject[NAME_LEN - 1] = '\0';
    t->date = d;
    t->done = false;
    (*count)++;
    return true;
}

bool task_list_remove(task_t *list, unsigned int *count, unsigned int index)
{
    if (list == NULL || count == NULL || index >= *count)
    {
        return false;
    }
    for (unsigned int i = index; i + 1 < *count; i++)
    {
        list[i] = list[i + 1];
    }
    (*count)--;
    return true;
}

bool task_list_mark_done(task_t *list, unsigned int count, unsigned int index)
{
    if (list == NULL || index >= count)
    {
        return false;
    }
    list[index].done = true;
    return true;
}

void task_list_print(const task_t *list, unsigned int count, const char *label)
{
    if (list == NULL || count == 0)
    {
        printf("no %s entries yet. \n", label);
        return;
    }

    date_t today = date_today();

    printf("+-----+------------------------------+------------------+------------+---------+---------------+\n");
    printf("| %-3s | %-28s | %-16s | %-10s | %-7s | %-13s |\n", "#", "title", "subject", "date", "status", "");
    printf("+-----+------------------------------+------------------+------------+---------+---------------+\n");
    for (unsigned int i = 0; i < count; i++)
    {
        char date_buf[11];
        date_format(&list[i].date, date_buf, sizeof(date_buf));
        long days = date_days_between(&today, &list[i].date);

        char remark[32];
        if (list[i].done)
        {
            snprintf(remark, sizeof(remark), "-");
        }
        else if (days < 0)
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

        printf("| %-3u | %-28s | %-16s | %-10s | %-7s | %-13s |\n", i + 1, list[i].title, list[i].subject,
               date_buf, list[i].done ? "done" : "pending", remark);
    }
    printf("+-----+------------------------------+------------------+------------+---------+---------------+\n");
}
