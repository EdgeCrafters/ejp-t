#include "common.h"

int parseOpt(int argc, char *argv[], const char targetOpt[], const int optNum,\
		char *optArg[], char *caches[])
{
	int c, result = 0;
	int flags[MAXOPT] = {0};
	
	while((c = getopt(argc,argv,targetOpt))!=-1)
		for(int i = 0; i < optNum; ++i)
			if(c != '?' && c == targetOpt[2*i]){
				flags[i] = 1;
				strcpy(optArg[i],optarg);
				result += 1;
			}else if(c == '?' && optopt == targetOpt[i]){
				fprintf(stderr, "Option -%c requires \
						an argument.\n", optopt);
				exit(EXIT_FAILURE);
			}

	for(int i = 0; i<optNum; ++i)
		if(flags[i]){
			continue;
		}else if(caches[i]){
			int cache;char buf[BUFSIZE];
			if((cache = open(caches[i], O_RDONLY)) < 0 
					|| read(cache, buf, BUFSIZE) < 0)
				goto exception;

			strcpy(optArg[i],buf);
		}else
			goto exception;

	return result;
	
exception:
	fprintf(stderr,"Missing options require %d, receive %d\n", optNum, result);
	exit(EXIT_FAILURE);
}

void userLogin(const char home[])
{
	char userID[IDSIZE], userPW[PWSIZE];
	memset(userID,0,IDSIZE);memset(userPW,0,PWSIZE);
	
	printf("\nEnter %s ID :", home);
	scanf("%s", userID);

 	printf("Enter %s PW :", home);
 	scanf("%s", userPW);

	if(!login(home, userID,userPW)){
		fprintf(stdout,"Login Success!\n");
	}else{
		fprintf(stdout,"Try again");
		userLogin(home);
	}
}

void userLogout(const char home[])
{
	logout(home);
}

char* getExtension(char *target)
{
	int i;
	for(i = 0; target[i] != '.' && target != '\0'; ++i)
		;
	return target+i;
}
