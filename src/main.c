#include "common.h"

char exe[PATHSIZE];
char homeCache[PATHSIZE];
char repoCache[PATHSIZE];
char problemCache[PATHSIZE];
char locationCache[PATHSIZE];
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
		sprintf(homeCache,"%s/../.ejp/cache/home.txt",exe);		
        sprintf(repoCache,"%s/../.ejp/cache/repo.txt",exe);
		sprintf(problemCache,"%s/../.ejp/cache/problem.txt",exe);
		sprintf(locationCache,"%s/../.ejp/cache/location.txt",exe);
		sprintf(cookie,"%s/../.ejp/cache/cookie.txt",exe);
		sprintf(repos,"%s/../.ejp/repos",exe);
        sprintf(archives,"%s/../.ejp/archives",exe);
	}

    return 0;
}