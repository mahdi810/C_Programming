#include "budget_ops.h"
#include "date_util.h"
#include <ctype.h>

static void normalize_currency(char *out, size_t out_len, const char *in)
{
    size_t i = 0;
    for (; in[i] != '\0' && i < out_len - 1; i++)
    {
        out[i] = (char)toupper((unsigned char)in[i]);
    }
    out[i] = '\0';
}

static bool resolve_date(const char *date_text, date_t *out)
{
    if (date_text == NULL || date_text[0] == '\0')
    {
        *out = date_today();
        return true;
    }
    return date_parse(date_text, out);
}

// ---- accounts ----

bool budget_account_create(budget_t *budget, const char *name, const char *currency, double opening_balance)
{
    if (budget == NULL || name == NULL || currency == NULL || name[0] == '\0' || currency[0] == '\0')
    {
        printf("invalid account name or currency. \n");
        return false;
    }
    if (budget->account_count >= MAX_BUDGET_ACCOUNTS)
    {
        printf("you have reached the maximum number of budget accounts. \n");
        return false;
    }

    budget_account_t *acc = &budget->accounts[budget->account_count];
    memset(acc, 0, sizeof(*acc));
    strncpy(acc->name, name, BUDGET_DESC_LEN - 1);
    normalize_currency(acc->currency, sizeof(acc->currency), currency);
    acc->opening_balance = opening_balance;
    acc->transaction_count = 0;

    budget->account_count++;
    return true;
}

double budget_account_balance(const budget_account_t *account)
{
    double balance = account->opening_balance;
    for (unsigned int i = 0; i < account->transaction_count; i++)
    {
        if (account->transactions[i].type == TRANSACTION_INCOME)
        {
            balance += account->transactions[i].amount;
        }
        else
        {
            balance -= account->transactions[i].amount;
        }
    }
    return balance;
}

bool budget_account_add_transaction(budget_account_t *account, const char *description, double amount,
                                     transaction_type_t type, const char *date_text)
{
    if (account == NULL || description == NULL || amount <= 0.0)
    {
        printf("invalid transaction (amount must be positive). \n");
        return false;
    }
    if (account->transaction_count >= MAX_TRANSACTIONS)
    {
        printf("this account's transaction history is full. \n");
        return false;
    }

    date_t d;
    if (!resolve_date(date_text, &d))
    {
        printf("invalid date, please use DD.MM.YY (or leave blank for today). \n");
        return false;
    }

    transaction_t *t = &account->transactions[account->transaction_count];
    strncpy(t->description, description, BUDGET_DESC_LEN - 1);
    t->description[BUDGET_DESC_LEN - 1] = '\0';
    t->amount = amount;
    t->type = type;
    t->date = d;

    account->transaction_count++;
    return true;
}

void budget_accounts_print(const budget_t *budget)
{
    if (budget == NULL || budget->account_count == 0)
    {
        printf("no budget accounts yet. \n");
        return;
    }

    printf("+-----+--------------------------+----------+---------------+\n");
    printf("| %-3s | %-24s | %-8s | %-13s |\n", "#", "account", "currency", "balance");
    printf("+-----+--------------------------+----------+---------------+\n");
    for (unsigned int i = 0; i < budget->account_count; i++)
    {
        printf("| %-3u | %-24s | %-8s | %13.2f |\n", i + 1, budget->accounts[i].name,
               budget->accounts[i].currency, budget_account_balance(&budget->accounts[i]));
    }
    printf("+-----+--------------------------+----------+---------------+\n");
}

void budget_transactions_print(const budget_account_t *account)
{
    if (account == NULL || account->transaction_count == 0)
    {
        printf("no transactions yet for this account. \n");
        return;
    }

    printf("+-----+----------+---------+------------------------------+---------------+\n");
    printf("| %-3s | %-8s | %-7s | %-28s | %-13s |\n", "#", "date", "type", "description", "amount");
    printf("+-----+----------+---------+------------------------------+---------------+\n");
    for (unsigned int i = 0; i < account->transaction_count; i++)
    {
        char date_buf[9];
        date_format(&account->transactions[i].date, date_buf, sizeof(date_buf));
        printf("| %-3u | %-8s | %-7s | %-28s | %13.2f |\n", i + 1, date_buf,
               account->transactions[i].type == TRANSACTION_INCOME ? "income" : "expense",
               account->transactions[i].description, account->transactions[i].amount);
    }
    printf("+-----+----------+---------+------------------------------+---------------+\n");
}

void budget_currency_summary_print(const budget_t *budget)
{
    if (budget == NULL || budget->account_count == 0)
    {
        printf("no budget accounts yet. \n");
        return;
    }

    char seen[MAX_BUDGET_ACCOUNTS][CURRENCY_LEN];
    unsigned int seen_count = 0;

    printf("+----------+---------------+---------------+---------------+\n");
    printf("| %-8s | %-13s | %-13s | %-13s |\n", "currency", "balance", "income", "expenses");
    printf("+----------+---------------+---------------+---------------+\n");

    for (unsigned int i = 0; i < budget->account_count; i++)
    {
        const char *cur = budget->accounts[i].currency;

        bool already_seen = false;
        for (unsigned int s = 0; s < seen_count; s++)
        {
            if (strcmp(seen[s], cur) == 0)
            {
                already_seen = true;
                break;
            }
        }
        if (already_seen)
        {
            continue;
        }
        strncpy(seen[seen_count], cur, CURRENCY_LEN - 1);
        seen[seen_count][CURRENCY_LEN - 1] = '\0';
        seen_count++;

        double balance = 0.0, income = 0.0, expense = 0.0;
        for (unsigned int j = 0; j < budget->account_count; j++)
        {
            if (strcmp(budget->accounts[j].currency, cur) != 0)
            {
                continue;
            }
            balance += budget_account_balance(&budget->accounts[j]);
            for (unsigned int k = 0; k < budget->accounts[j].transaction_count; k++)
            {
                const transaction_t *t = &budget->accounts[j].transactions[k];
                if (t->type == TRANSACTION_INCOME)
                {
                    income += t->amount;
                }
                else
                {
                    expense += t->amount;
                }
            }
        }

        printf("| %-8s | %13.2f | %13.2f | %13.2f |\n", cur, balance, income, expense);
    }
    printf("+----------+---------------+---------------+---------------+\n");
}

// ---- debts ----

bool budget_debt_add(budget_t *budget, const char *counterparty, const char *note, double amount,
                      const char *currency, debt_direction_t direction, const char *date_text)
{
    if (budget == NULL || counterparty == NULL || currency == NULL || amount <= 0.0)
    {
        printf("invalid debt entry (amount must be positive). \n");
        return false;
    }
    if (budget->debt_count >= MAX_DEBTS)
    {
        printf("the debt list is full. \n");
        return false;
    }

    date_t d;
    if (!resolve_date(date_text, &d))
    {
        printf("invalid date, please use DD.MM.YY (or leave blank for today). \n");
        return false;
    }

    debt_t *debt = &budget->debts[budget->debt_count];
    memset(debt, 0, sizeof(*debt));
    strncpy(debt->counterparty, counterparty, BUDGET_DESC_LEN - 1);
    if (note != NULL)
    {
        strncpy(debt->note, note, BUDGET_DESC_LEN - 1);
    }
    debt->original_amount = amount;
    debt->remaining_amount = amount;
    normalize_currency(debt->currency, sizeof(debt->currency), currency);
    debt->direction = direction;
    debt->due_date = d;
    debt->settled = false;
    debt->payment_count = 0;

    budget->debt_count++;
    return true;
}

bool budget_debt_pay_installment(debt_t *debt, double amount, const char *date_text)
{
    if (debt == NULL || amount <= 0.0)
    {
        printf("invalid payment (amount must be positive). \n");
        return false;
    }
    if (debt->settled)
    {
        printf("this debt is already settled. \n");
        return false;
    }
    if (amount > debt->remaining_amount + 0.005)
    {
        printf("that payment (%.2f) is more than the remaining balance (%.2f). \n", amount, debt->remaining_amount);
        return false;
    }
    if (debt->payment_count >= MAX_DEBT_PAYMENTS)
    {
        printf("this debt's payment history is full. \n");
        return false;
    }

    date_t d;
    if (!resolve_date(date_text, &d))
    {
        printf("invalid date, please use DD.MM.YY (or leave blank for today). \n");
        return false;
    }

    debt_payment_t *p = &debt->payments[debt->payment_count++];
    p->amount = amount;
    p->date = d;

    debt->remaining_amount -= amount;
    if (debt->remaining_amount < 0.005)
    {
        debt->remaining_amount = 0.0;
        debt->settled = true;
    }

    return true;
}

bool budget_debt_settle_full(debt_t *debt, const char *date_text)
{
    if (debt == NULL)
    {
        return false;
    }
    return budget_debt_pay_installment(debt, debt->remaining_amount, date_text);
}

bool budget_debt_delete(budget_t *budget, unsigned int index)
{
    if (budget == NULL || index >= budget->debt_count)
    {
        return false;
    }
    for (unsigned int i = index; i + 1 < budget->debt_count; i++)
    {
        budget->debts[i] = budget->debts[i + 1];
    }
    budget->debt_count--;
    return true;
}

void budget_debts_print(const budget_t *budget)
{
    if (budget == NULL || budget->debt_count == 0)
    {
        printf("no debts recorded. \n");
        return;
    }

    date_t today = date_today();

    printf("+-----+----------------------+---------+---------+---------+----------+------------+------------+-----------------+\n");
    printf("| %-3s | %-20s | %-7s | %-7s | %-7s | %-8s | %-10s | %-10s | %-15s |\n",
           "#", "counterparty", "original", "paid", "left", "currency", "direction", "due date", "status");
    printf("+-----+----------------------+---------+---------+---------+----------+------------+------------+-----------------+\n");
    for (unsigned int i = 0; i < budget->debt_count; i++)
    {
        const debt_t *d = &budget->debts[i];
        char date_buf[9];
        date_format(&d->due_date, date_buf, sizeof(date_buf));
        double paid = d->original_amount - d->remaining_amount;

        char status[16];
        if (d->settled)
        {
            snprintf(status, sizeof(status), "settled");
        }
        else if (date_days_between(&today, &d->due_date) < 0)
        {
            snprintf(status, sizeof(status), "overdue");
        }
        else if (paid > 0.005)
        {
            snprintf(status, sizeof(status), "partially paid");
        }
        else
        {
            snprintf(status, sizeof(status), "open");
        }

        printf("| %-3u | %-20s | %7.2f | %7.2f | %7.2f | %-8s | %-10s | %-10s | %-15s |\n", i + 1, d->counterparty,
               d->original_amount, paid, d->remaining_amount, d->currency,
               d->direction == DEBT_I_OWE ? "i owe" : "owed to me", date_buf, status);
    }
    printf("+-----+----------------------+---------+---------+---------+----------+------------+------------+-----------------+\n");
}

void budget_debt_payments_print(const debt_t *debt)
{
    if (debt == NULL || debt->payment_count == 0)
    {
        printf("no payments recorded for this debt yet. \n");
        return;
    }

    printf(" %s - original %.2f %s, remaining %.2f %s \n", debt->counterparty, debt->original_amount,
           debt->currency, debt->remaining_amount, debt->currency);
    printf("+-----+----------+---------------+\n");
    printf("| %-3s | %-8s | %-13s |\n", "#", "date", "amount");
    printf("+-----+----------+---------------+\n");
    for (unsigned int i = 0; i < debt->payment_count; i++)
    {
        char date_buf[9];
        date_format(&debt->payments[i].date, date_buf, sizeof(date_buf));
        printf("| %-3u | %-8s | %13.2f |\n", i + 1, date_buf, debt->payments[i].amount);
    }
    printf("+-----+----------+---------------+\n");
}

void budget_debt_summary_print(const budget_t *budget)
{
    if (budget == NULL || budget->debt_count == 0)
    {
        printf(" no debts recorded. \n");
        return;
    }

    char seen[MAX_DEBTS][CURRENCY_LEN];
    unsigned int seen_count = 0;
    bool printed_any = false;

    for (unsigned int i = 0; i < budget->debt_count; i++)
    {
        const char *cur = budget->debts[i].currency;

        bool already_seen = false;
        for (unsigned int s = 0; s < seen_count; s++)
        {
            if (strcmp(seen[s], cur) == 0)
            {
                already_seen = true;
                break;
            }
        }
        if (already_seen)
        {
            continue;
        }
        strncpy(seen[seen_count], cur, CURRENCY_LEN - 1);
        seen[seen_count][CURRENCY_LEN - 1] = '\0';
        seen_count++;

        double owed_to_me = 0.0, i_owe = 0.0;
        for (unsigned int j = 0; j < budget->debt_count; j++)
        {
            if (strcmp(budget->debts[j].currency, cur) != 0 || budget->debts[j].settled)
            {
                continue;
            }
            if (budget->debts[j].direction == DEBT_OWED_TO_ME)
            {
                owed_to_me += budget->debts[j].remaining_amount;
            }
            else
            {
                i_owe += budget->debts[j].remaining_amount;
            }
        }

        if (owed_to_me > 0.0 || i_owe > 0.0)
        {
            printf(" %-8s owed to me: %10.2f   i owe: %10.2f \n", cur, owed_to_me, i_owe);
            printed_any = true;
        }
    }

    if (!printed_any)
    {
        printf(" all debts settled. \n");
    }
}

// ---- family transfers ----

bool budget_family_transfer_add(budget_t *budget, budget_account_t *account, const char *recipient,
                                 const char *note, double amount, const char *currency, const char *date_text)
{
    if (budget == NULL || recipient == NULL || currency == NULL || amount <= 0.0)
    {
        printf("invalid transfer (amount must be positive). \n");
        return false;
    }
    if (budget->family_transfers.count >= MAX_FAMILY_TRANSFERS)
    {
        printf("the family transfer log is full. \n");
        return false;
    }

    date_t d;
    if (!resolve_date(date_text, &d))
    {
        printf("invalid date, please use DD.MM.YY (or leave blank for today). \n");
        return false;
    }

    if (account != NULL)
    {
        char description[BUDGET_DESC_LEN];
        snprintf(description, sizeof(description), "family transfer: %s", recipient);
        if (!budget_account_add_transaction(account, description, amount, TRANSACTION_EXPENSE, date_text))
        {
            return false;
        }
    }

    family_transfer_t *t = &budget->family_transfers.transfers[budget->family_transfers.count];
    strncpy(t->recipient, recipient, BUDGET_DESC_LEN - 1);
    t->recipient[BUDGET_DESC_LEN - 1] = '\0';
    if (note != NULL)
    {
        strncpy(t->note, note, BUDGET_DESC_LEN - 1);
        t->note[BUDGET_DESC_LEN - 1] = '\0';
    }
    else
    {
        t->note[0] = '\0';
    }
    t->amount = amount;
    normalize_currency(t->currency, sizeof(t->currency), currency);
    t->date = d;

    budget->family_transfers.count++;
    return true;
}

void budget_family_transfers_print(const budget_t *budget)
{
    if (budget == NULL || budget->family_transfers.count == 0)
    {
        printf("no family transfers recorded yet. \n");
        return;
    }

    printf("+-----+----------------------+---------------+----------+----------+------------------------+\n");
    printf("| %-3s | %-20s | %-13s | %-8s | %-8s | %-22s |\n", "#", "recipient", "amount", "currency", "date", "note");
    printf("+-----+----------------------+---------------+----------+----------+------------------------+\n");
    for (unsigned int i = 0; i < budget->family_transfers.count; i++)
    {
        const family_transfer_t *t = &budget->family_transfers.transfers[i];
        char date_buf[9];
        date_format(&t->date, date_buf, sizeof(date_buf));
        printf("| %-3u | %-20s | %13.2f | %-8s | %-8s | %-22s |\n", i + 1, t->recipient, t->amount, t->currency, date_buf, t->note);
    }
    printf("+-----+----------------------+---------------+----------+----------+------------------------+\n");
}

void budget_family_summary_print(const budget_t *budget)
{
    if (budget == NULL || budget->family_transfers.count == 0)
    {
        printf(" no family transfers recorded yet. \n");
        return;
    }

    char seen[MAX_FAMILY_TRANSFERS][CURRENCY_LEN];
    unsigned int seen_count = 0;

    for (unsigned int i = 0; i < budget->family_transfers.count; i++)
    {
        const char *cur = budget->family_transfers.transfers[i].currency;

        bool already_seen = false;
        for (unsigned int s = 0; s < seen_count; s++)
        {
            if (strcmp(seen[s], cur) == 0)
            {
                already_seen = true;
                break;
            }
        }
        if (already_seen)
        {
            continue;
        }
        strncpy(seen[seen_count], cur, CURRENCY_LEN - 1);
        seen[seen_count][CURRENCY_LEN - 1] = '\0';
        seen_count++;

        double total = 0.0;
        for (unsigned int j = 0; j < budget->family_transfers.count; j++)
        {
            if (strcmp(budget->family_transfers.transfers[j].currency, cur) == 0)
            {
                total += budget->family_transfers.transfers[j].amount;
            }
        }

        printf(" total sent in %-4s: %10.2f \n", cur, total);
    }
}
