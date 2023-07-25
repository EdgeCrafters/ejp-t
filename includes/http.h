#pragma once
#include <curl/curl.h>
#include "sha256.h"
#include "common.h"

#define URLSIZE 512
#define BUFSIZE 512

struct cookie{
	char data[BUFSIZE];
	int isStore;
};

int login(const char hoem[], const char id[], const char password[]);
int logout(const char home[]);
int intiRepo(const char home[], const char repoID[]);
