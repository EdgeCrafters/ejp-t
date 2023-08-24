#include "common.h"

char exe[PATHSIZE];
char homeCache[PATHSIZE];
char problemLocationCache[PATHSIZE];
char wbLocationCache[PATHSIZE];
char archives[PATHSIZE];
char repos[PATHSIZE];

int workbook(int argc, char *argv[]);
int manage(int argc, char *argv[]);
int setCaches(void);

int main(int argc, char *argv[])
{
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
        setCaches();
        workbook(argc, argv);
        fprintf(stdout,"Successfully done!\n");
    }
    else if (!strncmp(command, "manage",6))
    {
        fprintf(stderr,"manage : ");
        setCaches();
        manage(argc, argv);
        fprintf(stdout,"Successfully done!\n");
    }
    else
    {
        fprintf(stderr, "unknown command...\n");
        basicInfo();
        exit(-1);
    }
    return 0;
}

int setCaches(void)
{
	if(getExecutablePath(exe)<0){
		fprintf(stderr,"cannot configure current path ...\n");
		workbookInfo();
		exit(-1);
	}else{
		sprintf(homeCache,"%s/../.ejs/cache/home.txt",exe);
		sprintf(problemLocationCache,"%s/../.ejs/cache/problemLocation.txt",exe);
		sprintf(wbLocationCache,"%s/../.ejs/cache/wbLocation.txt",exe);
		sprintf(cookie,"%s/../.ejs/cache/cookie.txt",exe);
		sprintf(repos,"%s/../.ejs/repos",exe);
        sprintf(archives,"%s/../.ejs/archives",exe);
	}

    return 0;
}