#pragma once
#ifndef PASS
#define PASS

#include <sodium.h>
#include <stdbool.h>
#include <string.h>

bool hash_password(char *plain_text_password, char *hashed_password);

#endif