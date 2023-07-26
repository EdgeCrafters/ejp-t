#include "../includes/common.h" 

//append additional testcase in repo
static int add(int argc, char*argv[])
{
	int c, hflag = 0, lflag = 0;
	char *hvalue = NULL, *lvalue = NULL;

	printf("add : ");

	return 0;
}

//update exsiting testcase in repo
static int update(int argc, char*argv[])
{
	int c, hflag = 0, lflag = 0;
	char *hvalue = NULL, *lvalue = NULL;

	printf("add : ");

	return 0;
}

//create new workbook in repo
// h for home address(http) l for location of resources n for name of repo
static int create(int argc, char*argv[])
{
	printf("create : ");

	char hvalue[VALUESIZE] , lvalue[VALUESIZE], nvalue[VALUESIZE];
	char *values[] = {hvalue,lvalue,nvalue};
	char targetOpt[] = "h:l:n:";

	if(parseOpt(argc,argv,targetOpt,3,values)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

//	userLogin(hvalue);
//
//	char repoAddress[BUFSIZE];
//	if(initRepo(hvalue,nvalue,repoAddress,BUFSIZE)<0){
//		fprintf(stderr,"Fail to init repo ...\n");
//		exit(EXIT_FAILURE);
//	}
//	fprintf(stdout,"repo => %s\n",repoAddress);
//
//	userLogout(hvalue);
	
	encode("./testcase/a.result.json","./testcase/a.json", 10);

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
