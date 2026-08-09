#include "file_op.h"
#include <sys/stat.h>

// The database is kept read-only outside of an active save, so nothing
// but this application can modify it: writes briefly clear the
// read-only attribute, then restore it once the file is closed.
static void set_read_only(bool read_only)
{
    chmod(USER_FILE, read_only ? S_IREAD : (S_IREAD | S_IWRITE));
}

void open_file_and_update_user_db(user_dbt_t *user_db)
{
    FILE *fd1 = fopen(USER_FILE, "rb");
    if (fd1 == NULL)
    {
        printf("no existing user file found, starting with an empty database. \n");
        user_db->count = 0;
        return;
    }

    if (fread(user_db, sizeof(user_dbt_t), 1, fd1) != 1)
    {
        printf("failed to read the user database, starting with an empty database. \n");
        user_db->count = 0;
    }
    else
    {
        printf("the user database updated successfully. \n");
    }

    fclose(fd1);
}

void update_file_and_save(user_dbt_t *user_db)
{
    set_read_only(false);

    FILE *fd1 = fopen(USER_FILE, "wb");
    if (fd1 == NULL)
    {
        printf("failed to open the user file for writing. \n");
        return;
    }

    if (fwrite(user_db, sizeof(user_dbt_t), 1, fd1) != 1)
    {
        printf("the data was not written to the file. \n");
        printf("please try again later. \n");
    }
    else
    {
        printf("the data has been saved to the file. \n");
    }

    fclose(fd1);
    set_read_only(true);
}
