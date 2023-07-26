#pragma once
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define STRSIZE 1048576

#define BUFSIZE 512
#define CMDSIZE 512
#define URLSIZE 512

#define VALUESIZE 32

#define IDSIZE 16 
#define PWSIZE 16 
#define MAXOPT 16

int parseOpt(int argc, char *argv[], const char targetOpt[], const int optNum, char *optArg[]);
void userLogin(const char home[]);
void userLogout(const char home[]);
char* getExtension(char *target);
