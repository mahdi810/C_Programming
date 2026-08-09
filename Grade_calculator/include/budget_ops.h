#ifndef BUDGET_OPS_H
#define BUDGET_OPS_H

#include "user_t.h"

// ---- accounts ----

// Adds a new budget account (its own name, currency and starting balance).
bool budget_account_create(budget_t *budget, const char *name, const char *currency, double opening_balance);

// opening_balance + all income - all expenses recorded on the account.
double budget_account_balance(const budget_account_t *account);

// Records an income or expense on the account. amount must be positive.
// date_text is "DD.MM.YY", or blank/NULL for today.
bool budget_account_add_transaction(budget_account_t *account, const char *description, double amount,
                                     transaction_type_t type, const char *date_text);

void budget_accounts_print(const budget_t *budget);
void budget_transactions_print(const budget_account_t *account);

// Groups accounts by currency and prints combined balance/income/expenses.
void budget_currency_summary_print(const budget_t *budget);

// ---- debts ----

// Records a new debt (something owed, in either direction), fully unpaid.
// amount must be positive. date_text is "DD.MM.YY", or blank/NULL for today.
bool budget_debt_add(budget_t *budget, const char *counterparty, const char *note, double amount,
                      const char *currency, debt_direction_t direction, const char *date_text);

// Records a partial (or full) payment against a debt, reducing its
// remaining balance. Automatically marks the debt settled once the
// remaining balance reaches zero. amount must be positive and cannot
// exceed the remaining balance.
bool budget_debt_pay_installment(debt_t *debt, double amount, const char *date_text);

// Pays off whatever remains on the debt in one payment.
bool budget_debt_settle_full(debt_t *debt, const char *date_text);

// Removes a debt entry entirely (e.g. it was entered by mistake).
bool budget_debt_delete(budget_t *budget, unsigned int index);

void budget_debts_print(const budget_t *budget);
void budget_debt_payments_print(const debt_t *debt);

// Groups unsettled debts by currency and prints totals owed each way.
void budget_debt_summary_print(const budget_t *budget);

// ---- family transfers ----

// Records money sent to family. If account is non-NULL, the same amount
// is also posted as an expense on that account so its balance stays
// accurate. amount must be positive. date_text is "DD.MM.YY", or
// blank/NULL for today.
bool budget_family_transfer_add(budget_t *budget, budget_account_t *account, const char *recipient,
                                 const char *note, double amount, const char *currency, const char *date_text);

void budget_family_transfers_print(const budget_t *budget);

// Groups family transfers by currency and prints the total sent in each.
void budget_family_summary_print(const budget_t *budget);

#endif
