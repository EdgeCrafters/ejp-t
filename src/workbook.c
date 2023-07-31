#include "../includes/common.h" 
const char homeCache[] = "./cache/home";
const char locationCache[] = "./cache/location";

static int makeProblem(char home[], char repoID[], char workbook[], char result[])
{
	char error[STRSIZE];
	if(mkdir(result, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
		sprintf(error,"mkdir %d ", errno);
		goto exception;
	}

	DIR *workbookDir;
	if((workbookDir = opendir(workbook)) == NULL){
		sprintf(error,"opendir %d ", errno);
		goto exception;
	}

	struct dirent *dent;
	while((dent = readdir(workbookDir))){
		if(dent->d_type != DT_DIR)
			continue;

		char resultDir[URLSIZE], problemDir[URLSIZE], problem[URLSIZE];
		strcpy(problem,dent->d_name);
		sprintf(resultDir,"%s/%s",result, problem);
		sprintf(problemDir,"%s/%s",workbook, problem);
		char title[STRSIZE], description[STRSIZE], biases[BUFSIZE];
		if(encode(resultDir,problemDir, biases, title, description) < 0)
			goto exception;

//		if(uploadProblem(home,repoID,title,description)<0){
//			fprintf(stderr,"Fail to upload problem %s ...\n",title);
//			exit(EXIT_FAILURE);
//		}
	}
	
	closedir(workbookDir);

	return 0;

exception:
	fprintf(stderr, "%s error...\n",error);
	exit(EXIT_FAILURE);
}

//delete an additional testcase in repo
static int delete(int argc, char*argv[])
{
	char hvalue[VALUESIZE] , lvalue[VALUESIZE], nvalue[VALUESIZE];
	char *values[] = {hvalue,lvalue,nvalue};
	char *cache[] = {homeCache, locationCache, NULL};
	
	printf("delete : ");
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

//append an additional testcase in repo
static int append(int argc, char*argv[])
{
	char hvalue[VALUESIZE] , lvalue[VALUESIZE], nvalue[VALUESIZE];
	char *values[] = {hvalue,lvalue,nvalue};
	char *cache[] = {homeCache, locationCache, NULL};
	
	printf("append : ");
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

//update an exsiting testcase in repo
static int update(int argc, char*argv[])
{
	char hvalue[VALUESIZE] , lvalue[VALUESIZE], nvalue[VALUESIZE];
	char *values[] = {hvalue,lvalue,nvalue};
	char *cache[] = {homeCache, locationCache, NULL};
	
	printf("update : ");
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

//create new workbook in repo
// h for home address(http) l for location of resources n for name of repo
static int create(int argc, char*argv[])
{
	char home[VALUESIZE] , location[VALUESIZE], repoID[VALUESIZE];
	char *values[] = {home,location,repoID};
	char *cache[] = {homeCache, locationCache, NULL};

	printf("create : ");
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

//	userLogin(home);
//
//	char repoAddress[BUFSIZE];
//	if(initRepo(home,repoID,repoAddress,BUFSIZE)<0){
//		fprintf(stderr,"Fail to init repo ...\n");
//		exit(EXIT_FAILURE);
//	}else
//		fprintf(stdout,"Init repo (repo address : %s)\n",repoAddress);
	
	fprintf(stdout,"Creating workbook ... ");
	makeProblem(home,repoID,location,"./repomock");
	fprintf(stdout,"Complete !\n");

//	userLogout(home);

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
	}else if(!strncmp(command,"append",3)){
		if(append(argc,argv)){
			fprintf(stderr,"error...\n");
			exit(-1);
		}
	}else if(!strncmp(command,"update",6)){
		if(update(argc,argv)){
			fprintf(stderr,"error...\n");
			exit(-1);
		}
	}else if(!strncmp(command,"delete",6)){
		if(delete(argc,argv)){
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
