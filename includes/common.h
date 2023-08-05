#pragma once
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

#include "cJSON.h"

#define STRSIZE 1048576

#define BUFSIZE 4096

#define CMDSIZE 512
#define URLSIZE 512
#define PATHSIZE 512

#define VALUESIZE 32

#define IDSIZE 16 
#define PWSIZE 16 
#define MAXOPT 16

struct tcInfo{
	char *name;
	char *localPath;
	char bias;
};

struct info{
	char *title;
	char *description;
	char *localPath;
	char *remoteAddr;
	char *id;
};

extern char exe[PATHSIZE];
extern char homeCache[PATHSIZE];
extern char problemLocationCache[PATHSIZE];
extern char wbLocationCache[PATHSIZE];
extern char repos[PATHSIZE];

int parseOpt(int argc,char *argv[],const char targetOpt[],const int optNum,char *optArg[],char *caches[]);
void userLogin(const char home[]);
void userLogout(const char home[]);
char* getExtension(char *target);
int getExecutablePath(char path[]);
int setInfo(char home[], char repoName[], char problemName[], struct info *info);
int getInfo(char home[], char repoName[], char problemName[], struct info *info);
