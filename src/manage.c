#include "common.h" 

int listRepos(const char home[]);
int listProblems(const char home[],const char repoName[]);
int createUser(const char home[], const char username[], const char password[]);
int createUsers(const char home[], const char location[]);
int enrollUser(const char home[], const char username[], const char repoName[]);
int enrollUsers(const char home[], const char location[]);
int userProblemScore(const char home[],const char repoName[],const char problemName[],const char userName[]);
int userRepoScore(const char home[],const char repoName[],const char userName[]);
int problemScore(const char home[],const char repoName[],const char problemName[], const char location[]);
int repoScore(const char home[],const char repoName[], const char location[]);

static int list(int argc, char*argv[])
{
    char error[ERRORISZE];
    char home[VALUESIZE], repoName[VALUESIZE];
    char *values[] = {home, repoName};
    char *cache[] = {homeCache, NULL};
    char flags[2] = {0};

    fprintf(stderr,"list : ");

    int opts = parseOpt(argc,argv,"h:r:",2,values,cache,flags);

    switch(opts){
        case 1:
            listRepos(home);
            break;

        case 2:
            listProblems(home,repoName);
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

    char home[VALUESIZE]={0},userName[VALUESIZE]={0},repoName[VALUESIZE]={0},problemName[VALUESIZE]={0},location[PATHSIZE]={0};
    char *values[] = {home, userName, repoName,problemName,location};
    char *cache[] = {homeCache,NULL,repoCache,NULL,locationCache};
    char flags[5] = {0};

    fprintf(stderr,"score : ");
    int opts = parseOpt(argc,argv,"h:u:r:p:l:",5,values,cache,flags);
    if(!home[0] || !repoName[0]){
        sprintf(error,"missing opts");
        goto exception;
    }

    if(userName[0]&&problemName[0])
        userProblemScore(home,repoName,problemName,userName);
    else if(userName[0])
        userRepoScore(home,repoName,userName);
    else if(problemName[0]&&location[0])
        problemScore(home,repoName,problemName,location);
    else if(location[0])
        repoScore(home,repoName,location);
    else{
        sprintf(error,"missing opts");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s",error);
    return -1;
}

static int enroll(int argc, char*argv[])
{
    char error[ERRORISZE];

    char home[VALUESIZE]={0},username[VALUESIZE]={0},repoName[VALUESIZE]={0},location[PATHSIZE]={0};
    char *values[] = {home, username, repoName,location};
    char *cache[] = {homeCache, NULL, repoCache,NULL};
    char flags[4] = {0};

    fprintf(stderr,"enroll : ");
    int opts = parseOpt(argc,argv,"h:u:r:l:",4,values,cache,flags);
    if(!home[0]){
        sprintf(error,"missing home");
        goto exception;
    }

    if(location[0]){
        if(enrollUsers(home,location)<0){
            sprintf(error,"fail to enroll users");
            goto exception;
        }
    }else if(username[0] && repoName[0]){
        if(enrollUser(home,username,repoName)<0){
            sprintf(error,"faile to enroll a user");
            goto exception;
        }
    }else{
        sprintf(error,"missing opts");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s",error);
    return -1;
}

static int create(int argc, char*argv[])
{
    char error[ERRORISZE];

    char home[VALUESIZE]={0},username[VALUESIZE]={0},password[VALUESIZE]={0},location[PATHSIZE]={0};
    char *values[] = {home, username, password,location};
    char *cache[] = {homeCache, NULL, NULL,NULL};
    char flags[4] = {0};

    fprintf(stderr,"create : ");
    int opts = parseOpt(argc,argv,"h:u:p:l:",4,values,cache,flags);
    if(!home[0]){
        sprintf(error,"missing home");
        goto exception;
    }

    if(location[0]){
        if(createUsers(home,location)<0){
            sprintf(error,"fail to create users");
            goto exception;
        }
    }else if(username[0] && password[0]){
        if(createUser(home,username,password)<0){
            sprintf(error,"faile to create a user");
            goto exception;
        }
    }else{
        sprintf(error,"missing opts");
        goto exception;
    }

    return 0;
    
exception:
    fprintf(stderr,"%s ",error);
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
	}else if(!strncmp(command,"score",5)){
		if(score(argc,argv))
            goto exception;
    }else if(!strncmp(command,"create",6)){
        if(create(argc,argv))
            goto exception;
	}else if(!strncmp(command,"list",4)){
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