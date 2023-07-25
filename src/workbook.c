#include "../includes/common.h" 

static int parseOpt
(int argc, char *argv[], const char targetOpt[], const int optNum, char *optArg[])
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

	int check = 1;
	for(int i = 0; i<optNum; check *= flags[i++])
		;
	
	if(!check || result != optNum){
		fprintf(stderr,"Missing options check %d result %d\n", check, result);
		exit(EXIT_FAILURE);
	}

	return result;
}

static void userLogin
(const char home[])
{
	char userID[IDSIZE];
	char userPW[PWSIZE];

	while(1){
		memset(userID,0,IDSIZE);memset(userPW,0,PWSIZE);
 	  printf("\nEnter %s ID :", home);
 	  scanf("%s", userID);
  	printf("Enter %s PW :", home);
  	scanf("%s", userPW);
		if(!login(home, userID,userPW)){
			fprintf(stdout,"Login Success!\n");
			break;
		}else
			fprintf(stdout,"Try again");
	}
}

static void userLogout
(const char home[])
{
	logout(home);
}

//append additional testcase in repo
static int add
(int argc, char*argv[])
{
	int c, hflag = 0, lflag = 0;
	char *hvalue = NULL, *lvalue = NULL;

	printf("add : ");

	return 0;
}

//update exsiting testcase in repo
static int update
(int argc, char*argv[])
{
	int c, hflag = 0, lflag = 0;
	char *hvalue = NULL, *lvalue = NULL;

	printf("add : ");

	return 0;
}

//create new workbook in repo
static int create
(int argc, char*argv[])
{
	printf("create : ");

	// h for home address(http)
	// l for location of resources
	// n for name of repo
	char hvalue[VALUESIZE] , lvalue[VALUESIZE], nvalue[VALUESIZE];
	char *values[] = {hvalue,lvalue,nvalue};
	char targetOpt[] = "h:l:n:";

	if(parseOpt(argc,argv,targetOpt,3,values));

	userLogin(hvalue);

	char repoAddress[BUFSIZE];
	if(initRepo(hvalue,nvalue,repoAddress,BUFSIZE)<0){
		fprintf(stderr,"Fail to init repo ...\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout,"repo => %s\n",repoAddress);

	userLogout(hvalue);

	return 0;
}

int workbook
(int argc, char*argv[])
{
	char command[CMDSIZE];
	if(argc<3 || !strcpy(command,argv[2])){
		fprintf(stderr,"no command ...\n");
		workbookInfo();
		exit(-1);
	}

	if(!strncmp(command,"create",6)){
		if(create(argc,argv)){
			fprintf(stderr,"error...\n");
			exit(-1);
		}
	}else if(!strncmp(command,"add",3)){
		if(add(argc,argv)){
			fprintf(stderr,"error...\n");
			exit(-1);
		}
	}else if(!strncmp(command,"update",6)){
		if(update(argc,argv)){
			fprintf(stderr,"error...\n");
			exit(-1);
		}
	}else{
		fprintf(stderr,"unknown command...\n");
		workbookInfo();
		exit(-1);
	}
	return 0;
}
