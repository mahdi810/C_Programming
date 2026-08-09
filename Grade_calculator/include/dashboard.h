#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "user_t.h"

// Prints the full student dashboard: profile, today's date/time, time
// enrolled, subjects table, upcoming exams, and projects.
void print_dashboard(const user_t *user, const student_data_t *data);

// Prints the budget dashboard: accounts, per-currency summary, debts
// and a debt summary by currency.
void print_budget_dashboard(const user_t *user, const budget_t *budget);

#endif
