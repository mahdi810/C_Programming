
#include "pass.h"


bool hash_password(char *plain_text_password, char *hashed_password)
{
    if (crypto_pwhash_str(hashed_password, plain_text_password, strlen(plain_text_password), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}