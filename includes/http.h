#pragma once
#include <curl/curl.h>
#include "sha256.h"
#include "common.h"

struct cookie{
	char data[BUFSIZE];
	int isStore;
};

int login(const char hoem[], const char id[], const char password[]);
int logout(const char home[]);
int intiRepo(const char home[], const char repoID[], char buffer[], size_t bufSize);
