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

int login(char hoem[], char id[], char password[]);
int logout(char home[]);
