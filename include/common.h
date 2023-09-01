#ifndef common__h
#define common__h
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>


#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#endif

#include "cJSON.h"

#define STRSIZE 1048576

#define BUFSIZE 4096

#define CMDSIZE 512
#define URLSIZE 512
#define PATHSIZE 1024

#define VALUESIZE 32
#define ERRORISZE 1024

#define IDSIZE 16
#define PWSIZE 16

#define MAXOPT 16
#define MAXTC 100
#define MAXSTUDENT 100

enum type {problem, repo, testcase};

struct tcInfo
{
    char *name;
    char *localPath;
    char bias;
};

struct info
{
    char *title;
    char *description;
    char *localPath;
    char *remoteAddr;
    char *id;
    enum type type;
};

struct problemTestcase
{
    char *input[MAXTC];
    char *output[MAXTC];
    int num;
};

extern char exe[PATHSIZE];
extern char homeCache[PATHSIZE];
extern char repoCache[PATHSIZE];
extern char problemCache[PATHSIZE];
extern char locationCache[PATHSIZE];
extern char cookie[PATHSIZE];
extern char repos[PATHSIZE];
extern char archives[PATHSIZE];

int parseOpt(int argc, char *argv[], const char targetOpt[], const int optNum, char *optArg[], char *caches[], char flags[]);
void userLogin(const char home[]);
void userLogout(const char home[]);
char *getExtension(char *target);
int getExecutablePath(char path[]);
int setInfo(char home[], char repoName[], char problemName[], struct info *info);
int getInfo(char home[], char repoName[], char problemName[], struct info *info);
int getInfoByPath(const char path[], struct info *info);

void sleep_ms(int milliseconds);
int remove_directory(const char *path);

#endif