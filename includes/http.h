#pragma once
#include <curl/curl.h>
#include "sha256.h"
#include "common.h"

#define URLSIZE 512
int login(char hoem[], char id[], char password[]);
int logout(char home[], char id[], char password[]);
