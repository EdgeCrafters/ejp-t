#include "common.h"

int parseOpt(int argc, char *argv[], const char targetOpt[], const int optNum, char *optArg[])
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

	for(int i = 0, check = 1; i<optNum; check *= flags[i++])
		if(!check){
			fprintf(stderr,"Missing options require %d, receive %d\n", check, result);
			exit(EXIT_FAILURE);
		}

	return result;
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

