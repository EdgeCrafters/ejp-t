#pragma once
#include <curl/curl.h>
#include "sha256.h"
#include "common.h"

struct cookie{
	char data[BUFSIZE];
	int isStore;
};

int loginHTTP(const char hoem[],const char id[],const char password[]);
int logoutHTTP(const char home[]);
int initRepoHTTP(const char home[],const char repoID[],char buffer[],size_t bufSize);
int createProblemHTTP(const char home[],const char repoID[],char title[],char description[],char buffer[]);
int deleteProblemHTTP(const char home[],const char problemID[]);
int updateProblemHTTP(const char home[],const char problemID[],char title[],char description[]);
