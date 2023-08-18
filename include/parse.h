#ifndef parse__h
#define parse__h

#include <time.h>
#include "sha256.h"
#include "common.h"

int encode(char resultPath[], char inputPath[],
		struct tcInfo biases[], char title[], char description[], struct problemTestcase *result);
#endif