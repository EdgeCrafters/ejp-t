#include "common.h" 

int listInfo(int argc, char*argv[]);

static int list(int argc, char*argv[])
{
    char error[ERRORISZE];
    char home[VALUESIZE], problemName[VALUESIZE], repoName[VALUESIZE];
    char *values[] = {home, repoName, problemName,NULL};
    char *cache[] = {homeCache, NULL, NULL};

    fprintf(stderr,"list : ");

    int opts = parseOpt(argc,argv,"h:r:p:",3,values,cache);
    switch(opts){
        case 3: case 2: case 1: case 0:
            listInfo(opts,values);
            break;

        default:
            sprintf(error,"Missing Opts");
            goto exception;  
    }

    return 0;

exception:
    fprintf(stderr,"%s",error);
    return -1;
}

static int score(int argc, char*argv[])
{
    char error[ERRORISZE];

    return 0;

exception:
    fprintf(stderr,"%s",error);
    return -1;
}

static int enroll(int argc, char*argv[])
{
    char error[ERRORISZE];

    return 0;

exception:
    fprintf(stderr,"%s",error);
    return -1;
}

int manage(int argc, char*argv[])
{
	char command[CMDSIZE];
	if(argc<3 || !strcpy(command,argv[2])){
		fprintf(stderr,"no command ...\n");
		workbookInfo();
		exit(-1);
	}

	if(!strncmp(command,"enroll",6)){
		if(enroll(argc,argv))
            goto exception;
	}else if(!strncmp(command,"score",6)){
		if(score(argc,argv))
            goto exception;
	}else if(!strncmp(command,"list",6)){
		if(list(argc,argv))
			goto exception;
	}else{
		fprintf(stderr,"unknown command...\n");
		manageInfo();
		goto exception;
	}
	return 0;

exception:
    fprintf(stderr,"Wrapper error...\n");
    exit(-1);
}