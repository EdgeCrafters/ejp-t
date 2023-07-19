#include <curl/curl.h>
#include "sha256.h"
#include "common.h"

const int urlSize = 1024;

int login(char hoem[], char id[], char password[]);
int logout(char home[], char id[], char password[]);
