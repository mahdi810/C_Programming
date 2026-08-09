#include "dashboard.h"
#include "subject_ops.h"
#include "task_ops.h"
#include "budget_ops.h"
#include "work_ops.h"
#include "reminders.h"
#include "date_util.h"

void print_dashboard(const user_t *user, const student_data_t *data)
{
    char now_buf[32];
    datetime_now_format(now_buf, sizeof(now_buf));

    char enrolled_buf[9];
    date_format(&data->profile.enrollment_date, enrolled_buf, sizeof(enrolled_buf));

    date_t today = date_today();
    long days_enrolled = date_days_between(&data->profile.enrollment_date, &today);

    printf("========================================================\n");
    printf(" STUDENT DASHBOARD\n");
    printf("========================================================\n");
    printf(" name           : %s\n", data->profile.full_name);
    printf(" username       : %s\n", user->username);
    printf(" roll number    : %s\n", data->profile.roll_number);
    printf(" program        : %s\n", data->profile.program);
    printf(" semester       : %u\n", data->profile.semester);
    printf(" enrolled since : %s (%ld days ago)\n", enrolled_buf, days_enrolled);
    printf(" today          : %s\n", now_buf);
    printf("--------------------------------------------------------\n");
    printf(" REMINDERS\n");
    print_reminders(data);
    printf("--------------------------------------------------------\n");
    printf(" SUBJECTS\n");
    subject_list_print(&data->subjects);
    printf("--------------------------------------------------------\n");
    printf(" EXAMS\n");
    task_list_print(data->exams, data->exam_count, "exam");
    printf("--------------------------------------------------------\n");
    printf(" PROJECTS\n");
    task_list_print(data->projects, data->project_count, "project");
    printf("--------------------------------------------------------\n");
    printf(" WORK SUMMARY\n");
    work_summary_print(&data->work);
    printf("========================================================\n");
}

void print_budget_dashboard(const user_t *user, const budget_t *budget)
{
    printf("========================================================\n");
    printf(" BUDGET DASHBOARD - %s\n", user->username);
    printf("========================================================\n");
    printf(" ACCOUNTS\n");
    budget_accounts_print(budget);
    printf("--------------------------------------------------------\n");
    printf(" SUMMARY BY CURRENCY\n");
    budget_currency_summary_print(budget);
    printf("--------------------------------------------------------\n");
    printf(" DEBTS\n");
    budget_debts_print(budget);
    printf("\n DEBT SUMMARY (unsettled)\n");
    budget_debt_summary_print(budget);
    printf("--------------------------------------------------------\n");
    printf(" FAMILY TRANSFERS\n");
    budget_family_transfers_print(budget);
    printf("\n FAMILY TRANSFER SUMMARY\n");
    budget_family_summary_print(budget);
    printf("========================================================\n");
}
