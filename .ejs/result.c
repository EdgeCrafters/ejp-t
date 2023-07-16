#include <stdio.h>
#include "common.h"
int main(int argc, char*argv[])
{
char input[] = "1-1 input\n1-2 input\n";char output[] = "1-1 output\n1-2 output\n";
printf("%d",diff(input,output));
return 0;
}
