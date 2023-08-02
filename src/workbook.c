#include "../includes/common.h" 

const char homeCache[] = "./.ejs/cache/home.txt";
const char problemLocationCache[] = "./.ejs/cache/problemLocation.txt";
const char wbLocationCache[] = "./.ejs/cache/wbLocation.txt";
const char repos[] = "./.ejs/repos";

static int findRepo(char home[], char repoName[], struct repoInfo *info)
{
	char repoPath[URLSIZE];sprintf(repoPath,"%s/%s/%s",repos,home,repoName);
	DIR *repoDir;
	if((repoDir = opendir(repoPath))==NULL)
		goto exception;
	
	struct dirent *dent;
	while((dent = readdir(repoDir))){
		if(dent->d_type != DT_REG || strncmp(dent->d_name,"info",4))	
			continue;
		
		char infoPath[URLSIZE];sprintf(infoPath,"%s/%s",repoPath,dent->d_name);
		int infoFile; char buf[BUFSIZE];
		if((infoFile = open(infoPath,O_RDONLY))<0
				|| read(infoFile, buf, BUFSIZE)<0)
			goto exception;
		cJSON *root = cJSON_Parse(buf);
		cJSON *remoteAddrjson = cJSON_GetObjectItem(root,"remoteAddr");
		cJSON *idjson = cJSON_GetObjectItem(root,"id");
		if(!remoteAddrjson || !idjson)
			goto exception;

		info->name = strdup(repoName);
		info->localPath = strdup(repoPath);
		info->remoteAddr = strdup(remoteAddrjson->valuestring);
		info->id = atoi(idjson->valuestring);

		return 0;
	}

exception:
	return -1;
}

static int makeProblem(char home[], char repoName[], char problemDir[], char problem[], char result[])
{
	char error[STRSIZE];
	if(mkdir(result, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
		sprintf(error,"mkdir %d ", errno);
		goto exception;
	}

	char resultDir[URLSIZE];
	sprintf(resultDir,"%s/%s",result, problem);
	char title[STRSIZE], description[STRSIZE], biases[BUFSIZE];
	if(encode(resultDir,problemDir, biases, title, description) < 0)
		goto exception;

	struct repoInfo info;
	if(findRepo(home, repoName, &info) < 0){
		sprintf(error,"repoInfo");
		goto exception;
	}

//	if(uploadProblem(home,info.id,title,description)<0){
//		fprintf(stderr,"Fail to upload problem %s ...\n",title);
//		exit(EXIT_FAILURE);
//	}

	return 0;

exception:
	fprintf(stderr, "%s error...\n",error);
	exit(EXIT_FAILURE);
}

//delete an additional testcase in repo
static int delete(int argc, char*argv[])
{
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,location,repoName};
	char *cache[] = {homeCache, problemLocationCache, NULL};
	
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
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,location,repoName};
	char *cache[] = {homeCache, problemLocationCache, NULL};
	
	fprintf(stderr,"append : ");
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}
	
	char *problemName, *buf, *_location;
	_location = strdup(location);
	buf = strtok(_location,"/");
	while(buf  != NULL){
		problemName = strdup(buf);
		buf = strtok(NULL, "/");
	}
	fprintf(stderr,"location : %s\n",location);
	char repoAddr[URLSIZE]; sprintf(repoAddr,"%s/%s/%s",repos,home,repoName);
	makeProblem(home, repoName, location, problemName, repoAddr);

	free(_location);free(problemName);

	return 0;
}

//update an exsiting testcase in repo
static int update(int argc, char*argv[])
{
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,location,repoName};
	char *cache[] = {homeCache, problemLocationCache, NULL};
	
	printf("update : ");
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

// create new workbook in repo
// *	description
//		create new repo and workbook in it with login and logout
// *	required options
//		h for home address(http) l for location of resources n for name of repo
static int create(int argc, char*argv[])
{
	//parsing options
	printf("create : ");
	char home[VALUESIZE] , location[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,location,repoName};
	char *cache[] = {homeCache, wbLocationCache, NULL};
	if(parseOpt(argc,argv,"h:l:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}
	
	//arrange home and repo directory
	//>>> todo : apply git
	char homeAddr[URLSIZE]; sprintf(homeAddr,"%s/%s",repos,home);
	if(mkdir(homeAddr, S_IRWXU|S_IRWXO)<0 && errno != EEXIST)
		goto exception;
	char repoAddr[URLSIZE]; sprintf(repoAddr,"%s/%s",homeAddr,repoName);
	if(mkdir(repoAddr, S_IRWXU|S_IRWXO)<0 && errno != EEXIST)
		goto exception;

//	userLogin(home);


// ******** git clone & store repo info in cache ************//
//	char repoAddress[BUFSIZE];
//	if(initRepo(home,repoName,repoAddress,BUFSIZE)<0){
//		fprintf(stderr,"Fail to init repo ...\n");
//		exit(EXIT_FAILURE);
//	}else
//		fprintf(stdout,"Init repo (repo address : %s)\n",repoAddress);
	
  // open and read a directory to upload
	// search problems and upload them seperately 
	DIR *workbookDir;
	if((workbookDir = opendir(location)) == NULL)
		goto exception;
	char problem[URLSIZE];
	struct dirent *dent;
	while((dent = readdir(workbookDir)))
		if(dent->d_type == DT_DIR && dent->d_name[0] != '.')
		{
			sprintf(problem,"%s/%s",location,dent->d_name);
			makeProblem(home,repoName,problem,dent->d_name,repoAddr);
		}
	closedir(workbookDir);

//	userLogout(home);

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
