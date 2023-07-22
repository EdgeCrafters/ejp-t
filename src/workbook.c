#include "../includes/common.h" 

//append additional testcase in repo
static int add(int argc, char*argv[])
{
	int c, rflag = 0, lflag = 0;
	char *rvalue = NULL, *lvalue = NULL;

	printf("add : ");

	while((c = getopt(argc,argv,"r:l:")))
		switch(c)
		{
			case 't':
				//testcase option
				//additional testcase
				rflag = 1;
				rvalue = optarg;
				break;
			case 'l':
				//location option
				//location of repo
				lflag = 1;
				lvalue = optarg;
				break;
			case '?':
				if(optopt == 'r' || optopt == 'l')
					fprintf(stderr, "Option -%c requires \
							an argument.\n", optopt);
				else
					fprintf(stderr, "Unknown option \
							character '\\x%x'",optopt);
				exit(EXIT_FAILURE);
			default:
				abort();
		}


	return 0;
}

//create new workbook in repo
static int create(int argc, char*argv[])
{
	int c, rflag = 0, lflag = 0;
	char *rvalue = NULL, *lvalue = NULL;
	char userID[IDSIZE];
	char userPW[PWSIZE];

	printf("create : ");

	while((c = getopt(argc,argv,"r:l:"))!=-1)
		switch(c)
		{
			case 'r':
				//resource option
				//resource for testcase and workbook information
				rflag = 1;
				rvalue = optarg;
				break;
			case 'l':
				//location option
				//location of repo
				lflag = 1;
				lvalue = optarg;
				break;
			case '?':
				if(optopt == 'r' || optopt == 'l')
					fprintf(stderr, "Option -%c requires \
							an argument.\n", optopt);
				else
					fprintf(stderr, "Unknown option \
							character '\\x%x'",optopt);
				exit(EXIT_FAILURE);
			default:
				fprintf(stderr, "Unknown option \
							character '\\x%x'",optopt);
				exit(EXIT_FAILURE);
		}

	if(!rflag || !lflag){
		fprintf(stderr,"indispensable option missing ...");
		exit(EXIT_FAILURE);
	}

	while(1){
		memset(userID,0,IDSIZE);memset(userPW,0,PWSIZE);
 	  printf("\nEnter %s ID :", rvalue);
 	  scanf("%s", userID);
  	printf("Enter %s PW :", rvalue);
  	scanf("%s", userPW);
		if(!login(rvalue,userID,userPW)){
			fprintf(stdout,"Login Success!\n");
			break;
		}else
			fprintf(stdout,"Try again");
	}

	return 0;
}

int workbook(int argc, char*argv[])
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
	}else{
		fprintf(stderr,"unknown command...\n");
		workbookInfo();
		exit(-1);
	}
	return 0;
}
