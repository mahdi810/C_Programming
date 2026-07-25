#ifndef FILE_OP 
#define FILE_OP 

#include "user_t.h"

void open_file_and_update_user_db(FILE *fd, user_dbt_t* user_db); 
void update_file_and_save(FILE *fd, user_dbt_t* user_db); 

#endif