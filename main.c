#include <stdio.h>
#include "includes/sha256.h"
#include "includes/parse.h"
#include "includes/diff.h"

int main(void)
{
	char* result = SHA256("yeah");
	printf("%s\n",result);
	return 0;
}
