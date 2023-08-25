#ifndef http__h
#define http__h
#include "curl/curl.h"
#include "common.h"

struct cookie
{
    char data[BUFSIZE];
    int isStore;
};

int loginHTTP(const char hoem[], const char id[], const char password[]);
int logoutHTTP(const char home[]);
int initRepoHTTP(const char home[], const char repoID[], char buffer[], size_t bufSize);
int createProblemHTTP(const char home[], const char repoID[], char title[], char description[], char buffer[]);
int deleteProblemHTTP(const char home[], const char problemID[]);
int deleteRepoHTTP(const char home[], const char repoID[]);
int updateProblemHTTP(const char home[], const char problemID[], char title[], char description[]);
int uploadHiddencasesHTTP(const char home[], const char repoID[], const char problemID[], const char input[], const char output[]);
int uploadFileHTTP(const char home[], const char problemID[], const char path[]);
int getReposHTTP(const char home[], char repoID[]);
int createUsersHTTP(const char home[],  char usernames[][IDSIZE],  char passwords[][PWSIZE], const int studentNum);
int enrollUsersHTTP(const char home[], char usernames[][IDSIZE], char repoIDs[][IDSIZE], const int studentNum);
#endif