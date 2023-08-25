#include "common.h" 

int deleteRepo(char home[], char repoName[]);
int deleteProblem(char home[], char repoName[], char problemName[]);
int deleteTestcases(char home[], char repoName[], char problemName[], int testcases[], const int num);
int makeProblem(char home[], char repoName[], char problemDir[]);
int makeTestcase(char home[], char repoName[], char problemName[], char testcase[]);
int initRepo(const char home[], const char repoName[]);

// delete an additional problem in repo
static int delete(int argc, char *argv[])
{
	char home[VALUESIZE]={0},problemName[VALUESIZE] = {0},repoName[VALUESIZE] = {0},testcaseName[VALUESIZE] = {0};
	char *values[] = {home,problemName,repoName,testcaseName};
	char *cache[] = {homeCache,NULL,repoCache,NULL};
	
	fprintf(stderr,"delete : ");
	int opts = parseOpt(argc,argv,"h:p:r:t:",4,values,cache);
	if(opts<2){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}else if(opts == 4){
		fprintf(stderr,"Enter testcases' id to delete (seperate with space, press enter) : ");
		int testcases[VALUESIZE] = {0}, i = 0;
		do
			scanf("%d",&testcases[i++]);
		while(getchar()!='\n' && i < VALUESIZE);
		deleteTestcases(home,repoName,problemName,testcases, i);
	}else if(problemName[0] && repoName[0])
		deleteProblem(home, repoName, problemName);
	else if(repoName[0])
		deleteRepo(home, repoName);
	else{
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

//append an additional problem in repo
static int append(int argc, char*argv[])
{
	char home[VALUESIZE]={0},location[VALUESIZE]={0},repoName[VALUESIZE]={0},problemName[VALUESIZE]={0};
	char *values[] = {home,location,repoName,problemName};
	char *cache[] = {homeCache, locationCache, repoCache, NULL};
	
	fprintf(stderr,"append : ");
	int opts = parseOpt(argc,argv,"h:l:r:p:",4,values,cache);
	if(opts<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}else if(problemName[0] && repoName[0])
		makeTestcase(home,repoName,problemName,location);
	else if(repoName[0])
		makeProblem(home, repoName, location);
	else{
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

// create new workbook in repo
// *	description
//		create new repo and workbook in it with login and logout
// *	required options
//		h for home address(http) l for location of resources r for name of repo
static int create(int argc, char*argv[])
{
	//parsing options
	fprintf(stderr,"create : ");
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,location,repoName};
	char *cache[] = {homeCache, locationCache, repoCache};
	if(parseOpt(argc,argv,"h:l:r:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	if(initRepo(home,repoName)<0)
		goto exception;

	DIR *workbookDir;
	if((workbookDir = opendir(location)) == NULL)
		goto exception;
	char problem[URLSIZE];
	struct dirent *dent;
	while((dent = readdir(workbookDir)))
		if(dent->d_type == DT_DIR && dent->d_name[0] != '.')
		{
			sprintf(problem,"%s/%s",location,dent->d_name);
			makeProblem(home,repoName,problem);
		}
	closedir(workbookDir);

	return 0;

exception:
	fprintf(stderr,"error ...\n");
	exit(EXIT_FAILURE);
}

//workbook module wrapper
// *	description
//		parsing arguments and call a corresponding function
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
	}else if(!strncmp(command,"append",6)){
		if(append(argc,argv)){
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
