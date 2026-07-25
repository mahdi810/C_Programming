#include "file_op.h"

void open_file_and_update_user_db(FILE *fd1, user_dbt_t *user_db)
{
    fd1 = fopen(USER_FILE, "rb");
    if (fd1 == NULL)
    {
        printf("failed to load the file. \n");
        user_db->count = 0;
        printf("count has been set to 0. \n");
    }
    else
    {
        if ((fread(&user_db, sizeof(user_dbt_t), 1, fd1)) != 1)
        {
            printf("there is no file, a file will be created. \n");
        }
        else
        {
            printf("the user database updated successfully. \n");
        }
    }

    fclose(fd1);
}

void update_file_and_save(FILE *fd1, user_dbt_t *user_db)
{
    if (fwrite(&user_db, sizeof(user_db->users[0]), user_db->count, fd1))
    {
        printf("the data was not written to the file. \n");
        printf("please try again later. \n");
    }
    printf("the data has been save to the file. \n");
}