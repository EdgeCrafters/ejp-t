#ifndef git__h
#define git__h
#include "git2.h"
#include "common.h"

int gitInit(const char home[], const char repoName[], const char path[]);
#endif