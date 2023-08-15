#include "git.h"

int workbook(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    gitInit(NULL,NULL,0);
    char command[CMDSIZE];
    if (argc < 2 || !strcpy(command, argv[1]))
    {
        fprintf(stderr, "no command ...\n");
        basicInfo();
        exit(-1);
    }

    if (!strncmp(command, "workbook", 8))
    {
        fprintf(stderr, "workbook : ");
        return workbook(argc, argv);
    }
    else
    {
        fprintf(stderr, "unknown command...\n");
        basicInfo();
        exit(-1);
    }
    return 0;
}