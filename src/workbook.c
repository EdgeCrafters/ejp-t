#include "common.h" 

int deleteRepo(char home[], char repoName[]);
int deleteProblem(char home[], char repoName[], char problemName[]);
int updateProblem(char home[], char repoName[], char problemDir[], char problemName[]);
int makeProblem(char home[], char repoName[], char problemDir[]);
int initRepo(const char home[], const char repoName[]);

// delete an additional problem in repo
static int delete(int argc, char *argv[])
{
	char home[VALUESIZE] , problemName[VALUESIZE] = {'\0'}, repoName[VALUESIZE] = {'\0'};
	char *values[] = {home,problemName,repoName};
	char *cache[] = {homeCache,NULL,NULL};
	
	fprintf(stderr,"delete : ");
	if(parseOpt(argc,argv,"h:p:r:",3,values,cache)<2){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}else if(problemName[0] && repoName[0])
		deleteProblem(home, repoName, problemName);
	else if(repoName[0])
		deleteRepo(home, repoName);

	return 0;
}

//append an additional problem in repo
static int append(int argc, char*argv[])
{
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,location,repoName};
	char *cache[] = {homeCache, problemLocationCache, NULL};
	
	fprintf(stderr,"append : ");
	if(parseOpt(argc,argv,"h:l:r:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	makeProblem(home, repoName, location);

	return 0;
}

//update an exsiting testcase in repo
static int update(int argc, char*argv[])
{
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE], problemName[VALUESIZE];
	char *values[] = {home,location,repoName,problemName};
	char *cache[] = {homeCache, problemLocationCache, NULL, NULL};
	
	fprintf(stderr,"update : ");
	if(parseOpt(argc,argv,"h:l:r:p:",4,values,cache)<4){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	updateProblem(home,repoName,location,problemName);

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
	char *cache[] = {homeCache, wbLocationCache, NULL};
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
