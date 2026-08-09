#include <stdio.h>
#include "include/user_t.h"
#include "include/file_op.h"
#include "include/vault.h"
#include "include/subject_ops.h"
#include "include/task_ops.h"
#include "include/budget_ops.h"
#include "include/work_ops.h"
#include "include/reminders.h"
#include "include/dashboard.h"
#include "include/command.h"

typedef enum
{
    SELECTION_MENU,
    LOGIN,
    CREATE_USER,
    RUNNING,
    BUDGET_MENU,
    ADMIN_MENU,
    LOGOUT,
    SHUTDOWN
} state_t;

static void print_menu_header(const char *title)
{
    printf("\n========================================================\n");
    printf(" %s\n", title);
    printf("========================================================\n");
}

static void print_menu_footer(void)
{
    printf("--------------------------------------------------------\n");
}

// Prints one labelled row of commands, wrapping every 3 entries so long
// rows stay readable instead of running off the edge of the terminal.
static void print_menu_section(const char *label, const char *const commands[], int count)
{
    printf(" %-9s: ", label);
    for (int i = 0; i < count; i++)
    {
        printf("%-19s", commands[i]);
        if ((i + 1) % 3 == 0 && i + 1 < count)
        {
            printf("\n            ");
        }
    }
    printf("\n");
}

static void print_selection_menu(void)
{
    print_menu_header("MAIN MENU");
    const char *cmds[] = {"login", "create_user", "shutdown"};
    print_menu_section("actions", cmds, 3);
    print_menu_footer();
}

static void print_student_menu(const char *username, unsigned int semester)
{
    char title[NAME_LEN + 32];
    snprintf(title, sizeof(title), "STUDENT MENU - %s (semester %u)", username, semester);
    print_menu_header(title);

    const char *profile_cmds[] = {"dashboard", "reminders", "set_semester"};
    print_menu_section("profile", profile_cmds, 3);

    const char *subject_cmds[] = {"add_subject", "list_subjects", "remove_subject", "average"};
    print_menu_section("subjects", subject_cmds, 4);

    const char *exam_cmds[] = {"add_exam", "list_exams", "complete_exam", "remove_exam"};
    print_menu_section("exams", exam_cmds, 4);

    const char *project_cmds[] = {"add_project", "list_projects", "complete_project", "remove_project"};
    print_menu_section("projects", project_cmds, 4);

    const char *work_cmds[] = {"add_shift", "list_shifts", "remove_shift", "work_summary"};
    print_menu_section("work", work_cmds, 4);

    const char *budget_cmds[] = {"budget"};
    print_menu_section("budget", budget_cmds, 1);

    const char *session_cmds[] = {"logout", "shutdown"};
    print_menu_section("session", session_cmds, 2);

    print_menu_footer();
}

static void print_budget_menu(void)
{
    print_menu_header("BUDGET MENU");

    const char *account_cmds[] = {"create_account", "list_accounts"};
    print_menu_section("accounts", account_cmds, 2);

    const char *tx_cmds[] = {"add_income", "add_expense", "list_transactions"};
    print_menu_section("cashflow", tx_cmds, 3);

    const char *debt_cmds[] = {"add_debt", "list_debts", "pay_installment", "settle_debt", "debt_history", "delete_debt"};
    print_menu_section("debts", debt_cmds, 6);

    const char *family_cmds[] = {"send_family", "list_family"};
    print_menu_section("family", family_cmds, 2);

    const char *view_cmds[] = {"dashboard", "back"};
    print_menu_section("view", view_cmds, 2);

    print_menu_footer();
}

static void print_admin_menu(void)
{
    print_menu_header("ADMIN MENU");

    const char *user_cmds[] = {"list_users", "delete_user", "reset_password", "reset_attempts", "view_student"};
    print_menu_section("users", user_cmds, 5);

    const char *session_cmds[] = {"logout", "shutdown"};
    print_menu_section("session", session_cmds, 2);

    print_menu_footer();
}

static void read_line(char *buf, const char *prompt)
{
    printf("%s", prompt);
    fgets(buf, CMD_LEN, stdin);
    cmd_clean(buf);
}

static bool prompt_add_task(char *cmd_buf, task_t *list, unsigned int *count, unsigned int max_count)
{
    char title[NAME_LEN];
    char subject[NAME_LEN];
    char date_text[16];

    read_line(cmd_buf, "title: \n");
    strncpy(title, cmd_buf, NAME_LEN - 1);
    title[NAME_LEN - 1] = '\0';

    read_line(cmd_buf, "subject: \n");
    strncpy(subject, cmd_buf, NAME_LEN - 1);
    subject[NAME_LEN - 1] = '\0';

    read_line(cmd_buf, "date (DD.MM.YY): \n");
    strncpy(date_text, cmd_buf, sizeof(date_text) - 1);
    date_text[sizeof(date_text) - 1] = '\0';

    return task_list_add(list, count, max_count, title, subject, date_text);
}

static bool prompt_index(char *cmd_buf, const char *prompt, unsigned int *out_index)
{
    read_line(cmd_buf, prompt);
    unsigned int idx;
    if (sscanf(cmd_buf, "%u", &idx) != 1 || idx < 1)
    {
        return false;
    }
    *out_index = idx - 1;
    return true;
}

int main()
{
    user_dbt_t user_db1;     // user database
    user_db_init(&user_db1); // initializing the user database.
    user_t user;
    user_t *current_user = NULL;
    char current_password[CMD_LEN] = {0};
    student_data_t current_data;
    memset(&current_data, 0, sizeof(current_data));

    char cmd[NAME_LEN];
    state_t state = SELECTION_MENU;
    printf("-----------------Grade Calculator with Extensive Password Protection-----------------\n");

    if (sodium_init() < 0)
    {
        printf("faile fot initialize the sodium. \n");
        return 1;
    }
    printf("sodium successfully initialized. \n");

    while (state != SHUTDOWN)
    {
        switch (state)
        {
        case SELECTION_MENU:
            print_selection_menu();
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            if (!strcmp(cmd, "shutdown"))
            {
                state = SHUTDOWN;
            }
            else if (!strcmp(cmd, "login"))
            {
                state = LOGIN;
            }
            else if (!strcmp(cmd, "create_user"))
            {
                state = CREATE_USER;
            }
            else
            {
                printf("invalid command \n");
            }
            break;

        case LOGIN:
            printf("this is the login section. \n");
            // first load the file and check if the file exist
            open_file_and_update_user_db(&user_db1);

            printf("please enter the username. \n");
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            if (sscanf(cmd, "%s", user.username) != 1)
            {
                printf("invalid username. \n");
                break;
            }

            printf("please enter the password. \n");
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            if (sscanf(cmd, "%s", user.password) != 1)
            {
                printf("invalid password. \n");
                break;
            }

            // check the username/password against the database (verifies the hashed password).
            current_user = user_login(&user_db1, user.username, user.password);
            if (current_user == NULL)
            {
                state = SELECTION_MENU;
            }
            else if (current_user->is_admin)
            {
                // the admin is not a student - no profile/vault to unlock, straight to administration.
                strncpy(current_password, user.password, CMD_LEN - 1);
                current_password[CMD_LEN - 1] = '\0';
                state = ADMIN_MENU;
            }
            else if (!user_vault_open_as_owner(current_user, user.password, &current_data))
            {
                printf("failed to unlock your student profile. \n");
                current_user = NULL;
                state = SELECTION_MENU;
            }
            else
            {
                strncpy(current_password, user.password, CMD_LEN - 1);
                current_password[CMD_LEN - 1] = '\0';
                state = RUNNING;
            }

            sodium_memzero(user.password, sizeof(user.password));
            break;

        case CREATE_USER:
            printf("this is the create user section. \n");
            // first load the file and check if the file exist
            open_file_and_update_user_db(&user_db1);

            printf("please enter the username. \n");
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            strncpy(user.username, cmd, NAME_LEN - 1);
            user.username[NAME_LEN - 1] = '\0';

            if (user_db1.count == 0)
            {
                printf("this is the first account - it will be the admin account (not a student). \n");
                printf("enter the master password. \n");
            }
            else
            {
                printf("enter the password. \n");
            }
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);
            strncpy(user.password, cmd, CMD_LEN - 1);
            user.password[CMD_LEN - 1] = '\0';

            if (user_create(&user_db1, user.username, user.password) == 0)
            {
                user_t *new_user = &user_db1.users[user_db1.count - 1];
                if (!new_user->is_admin)
                {
                    student_data_t data;
                    memset(&data, 0, sizeof(data));

                    read_line(cmd, "full name: \n");
                    strncpy(data.profile.full_name, cmd, NAME_LEN - 1);

                    read_line(cmd, "roll number: \n");
                    strncpy(data.profile.roll_number, cmd, ROLL_NUMBER_LEN - 1);

                    read_line(cmd, "program / major: \n");
                    strncpy(data.profile.program, cmd, NAME_LEN - 1);

                    read_line(cmd, "current semester (number): \n");
                    unsigned int sem = 1;
                    sscanf(cmd, "%u", &sem);
                    data.profile.semester = sem;

                    data.profile.enrollment_date = date_today();

                    if (!user_vault_save(new_user, user.password, &user_db1.admin_box, &data))
                    {
                        printf("failed to save the student profile. \n");
                    }
                    sodium_memzero(&data, sizeof(data));
                }

                update_file_and_save(&user_db1);
                printf("user created successfully. \n");
            }

            sodium_memzero(user.password, sizeof(user.password));
            state = SELECTION_MENU;
            break;

        case RUNNING:
            print_student_menu(current_user->username, current_data.profile.semester);
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);

            if (!strcmp(cmd, "dashboard"))
            {
                print_dashboard(current_user, &current_data);
            }
            else if (!strcmp(cmd, "reminders"))
            {
                print_reminders(&current_data);
            }
            else if (!strcmp(cmd, "set_semester"))
            {
                read_line(cmd, "new semester number: \n");
                unsigned int sem;
                if (sscanf(cmd, "%u", &sem) == 1)
                {
                    current_data.profile.semester = sem;
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("semester updated. \n");
                    }
                }
                else
                {
                    printf("invalid input. \n");
                }
            }
            else if (!strcmp(cmd, "add_subject"))
            {
                char name[NAME_LEN];
                double grade;
                unsigned int ects;

                read_line(cmd, "subject name: \n");
                strncpy(name, cmd, NAME_LEN - 1);
                name[NAME_LEN - 1] = '\0';

                read_line(cmd, "grade (1.0,1.3,1.7,2.0,2.3,2.7,3.0,3.3,3.7,4.0,5.0): \n");
                if (sscanf(cmd, "%lf", &grade) != 1)
                {
                    printf("invalid grade input. \n");
                    break;
                }

                read_line(cmd, "ECTS credits: \n");
                if (sscanf(cmd, "%u", &ects) != 1)
                {
                    printf("invalid ECTS input. \n");
                    break;
                }

                if (subject_list_add(&current_data.subjects, name, grade, ects))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("subject added and saved. \n");
                    }
                    else
                    {
                        printf("failed to save the subject to the encrypted vault. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_subjects"))
            {
                subject_list_print(&current_data.subjects);
            }
            else if (!strcmp(cmd, "remove_subject"))
            {
                subject_list_print(&current_data.subjects);
                unsigned int idx;
                if (prompt_index(cmd, "enter the number of the subject to remove: \n", &idx) &&
                    subject_list_remove(&current_data.subjects, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("subject removed and saved. \n");
                    }
                }
                else
                {
                    printf("invalid subject number. \n");
                }
            }
            else if (!strcmp(cmd, "average"))
            {
                double avg;
                if (subject_list_weighted_average(&current_data.subjects, &avg))
                {
                    printf("your ECTS-weighted average grade is %.2f \n", avg);
                }
                else
                {
                    printf("no subjects entered yet. \n");
                }
            }
            else if (!strcmp(cmd, "add_exam"))
            {
                if (prompt_add_task(cmd, current_data.exams, &current_data.exam_count, MAX_EXAMS))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("exam added and saved. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_exams"))
            {
                task_list_print(current_data.exams, current_data.exam_count, "exam");
            }
            else if (!strcmp(cmd, "complete_exam"))
            {
                task_list_print(current_data.exams, current_data.exam_count, "exam");
                unsigned int idx;
                if (prompt_index(cmd, "exam number to mark done: \n", &idx) &&
                    task_list_mark_done(current_data.exams, current_data.exam_count, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("exam marked done. \n");
                    }
                }
                else
                {
                    printf("invalid exam number. \n");
                }
            }
            else if (!strcmp(cmd, "remove_exam"))
            {
                task_list_print(current_data.exams, current_data.exam_count, "exam");
                unsigned int idx;
                if (prompt_index(cmd, "exam number to remove: \n", &idx) &&
                    task_list_remove(current_data.exams, &current_data.exam_count, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("exam removed and saved. \n");
                    }
                }
                else
                {
                    printf("invalid exam number. \n");
                }
            }
            else if (!strcmp(cmd, "add_project"))
            {
                if (prompt_add_task(cmd, current_data.projects, &current_data.project_count, MAX_PROJECTS))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("project added and saved. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_projects"))
            {
                task_list_print(current_data.projects, current_data.project_count, "project");
            }
            else if (!strcmp(cmd, "complete_project"))
            {
                task_list_print(current_data.projects, current_data.project_count, "project");
                unsigned int idx;
                if (prompt_index(cmd, "project number to mark done: \n", &idx) &&
                    task_list_mark_done(current_data.projects, current_data.project_count, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("project marked done. \n");
                    }
                }
                else
                {
                    printf("invalid project number. \n");
                }
            }
            else if (!strcmp(cmd, "remove_project"))
            {
                task_list_print(current_data.projects, current_data.project_count, "project");
                unsigned int idx;
                if (prompt_index(cmd, "project number to remove: \n", &idx) &&
                    task_list_remove(current_data.projects, &current_data.project_count, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("project removed and saved. \n");
                    }
                }
                else
                {
                    printf("invalid project number. \n");
                }
            }
            else if (!strcmp(cmd, "add_shift"))
            {
                char note[BUDGET_DESC_LEN];
                char date_text[16], start_text[8], end_text[8];

                read_line(cmd, "date (DD.MM.YY, blank = today): \n");
                strncpy(date_text, cmd, sizeof(date_text) - 1);
                date_text[sizeof(date_text) - 1] = '\0';

                read_line(cmd, "start time (HH:MM): \n");
                strncpy(start_text, cmd, sizeof(start_text) - 1);
                start_text[sizeof(start_text) - 1] = '\0';

                read_line(cmd, "end time (HH:MM): \n");
                strncpy(end_text, cmd, sizeof(end_text) - 1);
                end_text[sizeof(end_text) - 1] = '\0';

                read_line(cmd, "note (optional, e.g. employer/task): \n");
                strncpy(note, cmd, BUDGET_DESC_LEN - 1);
                note[BUDGET_DESC_LEN - 1] = '\0';

                if (work_log_add(&current_data.work, note, date_text, start_text, end_text))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("work shift added and saved. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_shifts"))
            {
                work_log_print(&current_data.work);
            }
            else if (!strcmp(cmd, "remove_shift"))
            {
                work_log_print(&current_data.work);
                unsigned int idx;
                if (prompt_index(cmd, "shift number to remove: \n", &idx) && work_log_remove(&current_data.work, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("shift removed and saved. \n");
                    }
                }
                else
                {
                    printf("invalid shift number. \n");
                }
            }
            else if (!strcmp(cmd, "work_summary"))
            {
                work_summary_print(&current_data.work);
            }
            else if (!strcmp(cmd, "budget"))
            {
                state = BUDGET_MENU;
            }
            else if (!strcmp(cmd, "logout"))
            {
                state = LOGOUT;
            }
            else if (!strcmp(cmd, "shutdown"))
            {
                state = SHUTDOWN;
            }
            else
            {
                printf("invalid command. \n");
            }
            break;

        case BUDGET_MENU:
            print_budget_menu();
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);

            if (!strcmp(cmd, "create_account"))
            {
                char name[BUDGET_DESC_LEN];
                char currency[CURRENCY_LEN];
                double opening;

                read_line(cmd, "account name: \n");
                strncpy(name, cmd, BUDGET_DESC_LEN - 1);
                name[BUDGET_DESC_LEN - 1] = '\0';

                read_line(cmd, "currency (e.g. EUR, USD): \n");
                strncpy(currency, cmd, CURRENCY_LEN - 1);
                currency[CURRENCY_LEN - 1] = '\0';

                read_line(cmd, "starting balance (0 if none): \n");
                opening = 0.0;
                sscanf(cmd, "%lf", &opening);

                if (budget_account_create(&current_data.budget, name, currency, opening))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("budget account created. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_accounts"))
            {
                budget_accounts_print(&current_data.budget);
            }
            else if (!strcmp(cmd, "add_income") || !strcmp(cmd, "add_expense"))
            {
                transaction_type_t type = !strcmp(cmd, "add_income") ? TRANSACTION_INCOME : TRANSACTION_EXPENSE;

                budget_accounts_print(&current_data.budget);
                unsigned int idx;
                if (!prompt_index(cmd, "account number: \n", &idx) || idx >= current_data.budget.account_count)
                {
                    printf("invalid account number. \n");
                    break;
                }

                char description[BUDGET_DESC_LEN];
                char date_text[16];
                double amount;

                read_line(cmd, "description: \n");
                strncpy(description, cmd, BUDGET_DESC_LEN - 1);
                description[BUDGET_DESC_LEN - 1] = '\0';

                read_line(cmd, "amount: \n");
                if (sscanf(cmd, "%lf", &amount) != 1)
                {
                    printf("invalid amount. \n");
                    break;
                }

                read_line(cmd, "date (DD.MM.YY, blank = today): \n");
                strncpy(date_text, cmd, sizeof(date_text) - 1);
                date_text[sizeof(date_text) - 1] = '\0';

                if (budget_account_add_transaction(&current_data.budget.accounts[idx], description, amount, type, date_text))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("transaction recorded. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_transactions"))
            {
                budget_accounts_print(&current_data.budget);
                unsigned int idx;
                if (prompt_index(cmd, "account number: \n", &idx) && idx < current_data.budget.account_count)
                {
                    budget_transactions_print(&current_data.budget.accounts[idx]);
                }
                else
                {
                    printf("invalid account number. \n");
                }
            }
            else if (!strcmp(cmd, "add_debt"))
            {
                char counterparty[BUDGET_DESC_LEN];
                char note[BUDGET_DESC_LEN];
                char currency[CURRENCY_LEN];
                char date_text[16];
                double amount;

                read_line(cmd, "counterparty (who): \n");
                strncpy(counterparty, cmd, BUDGET_DESC_LEN - 1);
                counterparty[BUDGET_DESC_LEN - 1] = '\0';

                read_line(cmd, "amount: \n");
                if (sscanf(cmd, "%lf", &amount) != 1)
                {
                    printf("invalid amount. \n");
                    break;
                }

                read_line(cmd, "currency (e.g. EUR, USD): \n");
                strncpy(currency, cmd, CURRENCY_LEN - 1);
                currency[CURRENCY_LEN - 1] = '\0';

                read_line(cmd, "do you owe them, or do they owe you? (owe/owed): \n");
                debt_direction_t direction = !strcmp(cmd, "owed") ? DEBT_OWED_TO_ME : DEBT_I_OWE;

                read_line(cmd, "due date (DD.MM.YY, blank = today): \n");
                strncpy(date_text, cmd, sizeof(date_text) - 1);
                date_text[sizeof(date_text) - 1] = '\0';

                read_line(cmd, "note (optional): \n");
                strncpy(note, cmd, BUDGET_DESC_LEN - 1);
                note[BUDGET_DESC_LEN - 1] = '\0';

                if (budget_debt_add(&current_data.budget, counterparty, note, amount, currency, direction, date_text))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("debt recorded. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_debts"))
            {
                budget_debts_print(&current_data.budget);
            }
            else if (!strcmp(cmd, "pay_installment"))
            {
                budget_debts_print(&current_data.budget);
                unsigned int idx;
                if (!prompt_index(cmd, "debt number: \n", &idx) || idx >= current_data.budget.debt_count)
                {
                    printf("invalid debt number. \n");
                    break;
                }

                double amount;
                read_line(cmd, "payment amount: \n");
                if (sscanf(cmd, "%lf", &amount) != 1)
                {
                    printf("invalid amount. \n");
                    break;
                }

                char date_text[16];
                read_line(cmd, "payment date (DD.MM.YY, blank = today): \n");
                strncpy(date_text, cmd, sizeof(date_text) - 1);
                date_text[sizeof(date_text) - 1] = '\0';

                if (budget_debt_pay_installment(&current_data.budget.debts[idx], amount, date_text))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("payment recorded. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "settle_debt"))
            {
                budget_debts_print(&current_data.budget);
                unsigned int idx;
                if (!prompt_index(cmd, "debt number to settle in full: \n", &idx) || idx >= current_data.budget.debt_count)
                {
                    printf("invalid debt number. \n");
                    break;
                }

                if (budget_debt_settle_full(&current_data.budget.debts[idx], NULL))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("debt settled. \n");
                    }
                }
                else
                {
                    printf("could not settle this debt. \n");
                }
            }
            else if (!strcmp(cmd, "debt_history"))
            {
                budget_debts_print(&current_data.budget);
                unsigned int idx;
                if (prompt_index(cmd, "debt number: \n", &idx) && idx < current_data.budget.debt_count)
                {
                    budget_debt_payments_print(&current_data.budget.debts[idx]);
                }
                else
                {
                    printf("invalid debt number. \n");
                }
            }
            else if (!strcmp(cmd, "delete_debt"))
            {
                budget_debts_print(&current_data.budget);
                unsigned int idx;
                if (prompt_index(cmd, "debt number to delete: \n", &idx) && budget_debt_delete(&current_data.budget, idx))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("debt deleted. \n");
                    }
                }
                else
                {
                    printf("invalid debt number. \n");
                }
            }
            else if (!strcmp(cmd, "send_family"))
            {
                char recipient[BUDGET_DESC_LEN];
                char note[BUDGET_DESC_LEN];
                char currency[CURRENCY_LEN];
                char date_text[16];
                double amount;

                read_line(cmd, "recipient: \n");
                strncpy(recipient, cmd, BUDGET_DESC_LEN - 1);
                recipient[BUDGET_DESC_LEN - 1] = '\0';

                read_line(cmd, "amount: \n");
                if (sscanf(cmd, "%lf", &amount) != 1)
                {
                    printf("invalid amount. \n");
                    break;
                }

                read_line(cmd, "currency (e.g. EUR, USD): \n");
                strncpy(currency, cmd, CURRENCY_LEN - 1);
                currency[CURRENCY_LEN - 1] = '\0';

                read_line(cmd, "note (optional): \n");
                strncpy(note, cmd, BUDGET_DESC_LEN - 1);
                note[BUDGET_DESC_LEN - 1] = '\0';

                read_line(cmd, "date (DD.MM.YY, blank = today): \n");
                strncpy(date_text, cmd, sizeof(date_text) - 1);
                date_text[sizeof(date_text) - 1] = '\0';

                budget_account_t *account = NULL;
                budget_accounts_print(&current_data.budget);
                read_line(cmd, "deduct from which account? (number, or blank to skip): \n");
                if (cmd[0] != '\0')
                {
                    unsigned int acc_idx;
                    if (sscanf(cmd, "%u", &acc_idx) == 1 && acc_idx >= 1 && acc_idx <= current_data.budget.account_count)
                    {
                        account = &current_data.budget.accounts[acc_idx - 1];
                    }
                }

                if (budget_family_transfer_add(&current_data.budget, account, recipient, note, amount, currency, date_text))
                {
                    if (user_vault_save(current_user, current_password, &user_db1.admin_box, &current_data))
                    {
                        update_file_and_save(&user_db1);
                        printf("family transfer recorded. \n");
                    }
                }
            }
            else if (!strcmp(cmd, "list_family"))
            {
                budget_family_transfers_print(&current_data.budget);
            }
            else if (!strcmp(cmd, "dashboard"))
            {
                print_budget_dashboard(current_user, &current_data.budget);
            }
            else if (!strcmp(cmd, "back"))
            {
                state = RUNNING;
            }
            else
            {
                printf("invalid command. \n");
            }
            break;

        case ADMIN_MENU:
            print_admin_menu();
            fgets(cmd, CMD_LEN, stdin);
            cmd_clean(cmd);

            if (!strcmp(cmd, "list_users"))
            {
                print_user_summary(&user_db1);
            }
            else if (!strcmp(cmd, "delete_user"))
            {
                read_line(cmd, "username to delete: \n");
                if (!strcmp(cmd, current_user->username))
                {
                    printf("you cannot delete your own account while logged in. \n");
                }
                else if (user_delete_by_username(&user_db1, cmd))
                {
                    update_file_and_save(&user_db1);
                    printf("user deleted. \n");
                }
                else
                {
                    printf("user not found. \n");
                }
            }
            else if (!strcmp(cmd, "reset_password"))
            {
                char target_name[NAME_LEN];
                read_line(cmd, "username: \n");
                strncpy(target_name, cmd, NAME_LEN - 1);
                target_name[NAME_LEN - 1] = '\0';

                user_t *target = find_user_in_db(&user_db1, target_name);
                if (target == NULL)
                {
                    printf("user not found. \n");
                    break;
                }

                read_line(cmd, "new password: \n");

                if (admin_reset_user_password(target, &user_db1.admin_box, current_password, cmd))
                {
                    target->attempts_remaining = MAX_ATTEMPT;
                    update_file_and_save(&user_db1);
                    printf("password reset. \n");
                }
                else
                {
                    printf("failed to reset the password. \n");
                }
                sodium_memzero(cmd, sizeof(cmd));
            }
            else if (!strcmp(cmd, "reset_attempts"))
            {
                read_line(cmd, "username: \n");
                user_t *target = find_user_in_db(&user_db1, cmd);
                if (target != NULL)
                {
                    target->attempts_remaining = MAX_ATTEMPT;
                    update_file_and_save(&user_db1);
                    printf("attempts reset. \n");
                }
                else
                {
                    printf("user not found. \n");
                }
            }
            else if (!strcmp(cmd, "view_student"))
            {
                read_line(cmd, "username: \n");
                user_t *target = find_user_in_db(&user_db1, cmd);
                if (target == NULL)
                {
                    printf("user not found. \n");
                    break;
                }
                if (target->is_admin)
                {
                    printf("this account is the admin - it has no student profile. \n");
                    break;
                }

                student_data_t target_data;
                if (user_vault_open_as_admin(target, &user_db1.admin_box, current_password, &target_data))
                {
                    print_dashboard(target, &target_data);
                    print_budget_dashboard(target, &target_data.budget);
                    sodium_memzero(&target_data, sizeof(target_data));
                }
                else
                {
                    printf("failed to unlock this user's vault. \n");
                }
            }
            else if (!strcmp(cmd, "logout"))
            {
                state = LOGOUT;
            }
            else if (!strcmp(cmd, "shutdown"))
            {
                state = SHUTDOWN;
            }
            else
            {
                printf("invalid command. \n");
            }
            break;

        case LOGOUT:
            sodium_memzero(&current_data, sizeof(current_data));
            sodium_memzero(current_password, sizeof(current_password));
            user_logout(&current_user);
            printf("you have been logged out. \n");
            state = SELECTION_MENU;
            break;

        default:
            break;
        }
    }

    sodium_memzero(&current_data, sizeof(current_data));
    sodium_memzero(current_password, sizeof(current_password));
    return 0;
}
