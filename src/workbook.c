#include "../includes/common.h" 

const char homeCache[] = "./.ejs/cache/home.txt";
const char problemLocationCache[] = "./.ejs/cache/problemLocation.txt";
const char wbLocationCache[] = "./.ejs/cache/wbLocation.txt";
const char repos[] = "./.ejs/repos";

static int getInfo(char home[], char repoName[], char problemName[], struct info *info)
{
	char path[URLSIZE];
	if(!problemName)
		sprintf(path,"%s/%s/%s",repos,home,repoName);
	else
		sprintf(path,"%s/%s/%s/%s",repos,home,repoName,problemName);

	char infoPath[URLSIZE];sprintf(infoPath,"%s/info.json",path);
	int infoFile; char buf[BUFSIZE];
	if((infoFile = open(infoPath,O_RDONLY))<0
			|| read(infoFile, buf, BUFSIZE)<0)
		goto exception;
	close(infoFile);

	cJSON *root, *title, *description, *remoteAddr, *id;
	root = cJSON_Parse(buf);
	title = cJSON_GetObjectItem(root,"title");
	description = cJSON_GetObjectItem(root,"description");
	remoteAddr = cJSON_GetObjectItem(root,"remoteAddr");
	id = cJSON_GetObjectItem(root,"id");

	info->title = title ? strdup(title->valuestring) : NULL;
	info->description = description ? strdup(description->valuestring) : NULL;
	info->localPath = strdup(path);
	info->remoteAddr = remoteAddr ? strdup(remoteAddr->valuestring):NULL;
	info->id = id ? id->valueint : -1;

	return 0;

exception:
	return -1;
}

static int setInfo(char home[], char repoName[], char problemName[], struct info *info)
{
	char path[URLSIZE];
	if(!problemName)
		sprintf(path,"%s/%s/%s",repos,home,repoName);
	else
		sprintf(path,"%s/%s/%s/%s",repos,home,repoName,problemName);

	char infoPath[URLSIZE];sprintf(infoPath,"%s/info.json",path);
	int infoFile; char buf[BUFSIZE];
	if((infoFile = open(infoPath,O_RDONLY))<0
			|| read(infoFile, buf, BUFSIZE)<0)
		goto exception;
	close(infoFile);

	cJSON *root, *title, *description, *remoteAddr, *id;
	root = cJSON_Parse(buf);

	cJSON *result = cJSON_CreateObject(), *bufjson;

	if(!(title = cJSON_GetObjectItem(root,"title")) && info->title)
		bufjson = cJSON_CreateString(info->title);
	else
		bufjson = cJSON_CreateString(title->valuestring);
	cJSON_AddItemToObject(result,"title",bufjson);

	if(!(description = cJSON_GetObjectItem(root,"description")) && info->description)
		bufjson = cJSON_CreateString(info->description);
	else
		bufjson = cJSON_CreateString(description->valuestring);
	cJSON_AddItemToObject(result,"description",bufjson);

	if(!(remoteAddr = cJSON_GetObjectItem(root,"remoteAddr")) && info->remoteAddr)
		bufjson = cJSON_CreateString(info->remoteAddr);
	else
		bufjson = cJSON_CreateString(remoteAddr->valuestring);
	cJSON_AddItemToObject(result,"remoteAddr",bufjson);
	
	if(!(id = cJSON_GetObjectItem(root,"id")) && info->id >= 0)
		bufjson = cJSON_CreateNumber(info->id);
	else
		bufjson = cJSON_CreateNumber(id->valueint);
	cJSON_AddItemToObject(result,"id",bufjson);

	char *resultstr = cJSON_Print(result);
	if((infoFile = open(infoPath,O_WRONLY|O_TRUNC))<0 
			|| write(infoFile, resultstr,strlen(resultstr)) == 0)
		goto exception;

	return 0;

exception:
	return -1;
}

static int removeProblem(char home[], char repoName[], char problemName[])
{
	char error[STRSIZE];
	
	struct info problemInfo;
	if(getInfo(home, repoName, problemName, &problemInfo) < 0){
		sprintf(error,"repoInfo");
		goto exception;
	}
	
	char cmd[CMDSIZE];sprintf(cmd,"rm -rf %s",problemInfo.localPath);
	system(cmd);

//	if(deleteProblem(home,problemInfo.id)<0){
//		fprintf(stderr,"Fail to delete problem %s ...\n",problemInfo.title);
//		exit(EXIT_FAILURE);
//	}

	return 0;

exception:
	fprintf(stderr, "%s error...\n",error);
	exit(EXIT_FAILURE);
}

static int makeProblem(char home[], char repoName[], char problemDir[],
		char problemName[], char result[])
{
	char error[STRSIZE];
	if(mkdir(result, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
		sprintf(error,"mkdir %d ", errno);
		goto exception;
	}

	char resultDir[URLSIZE];
	sprintf(resultDir,"%s/%s",result, problemName);
	char title[STRSIZE], description[STRSIZE], biases[BUFSIZE];
	if(encode(resultDir,problemDir, biases, title, description) < 0)
		goto exception;

	struct info repoInfo;
	if(getInfo(home, repoName, NULL, &repoInfo) < 0){
		sprintf(error,"Info");
		goto exception;
	}

//	char buffer[BUFSIZE]; int problemId;
//	if(uploadProblem(home,repoInfo.id,title,description,buffer)<0){
//		fprintf(stderr,"Fail to upload problem %s ...\n",title);
//		exit(EXIT_FAILURE);
//	}
//	cJSON *response = cJSON_Parse(buffer);
//	problemId = cJSON_GetObjectItem(response,"id")->valueint;

	struct info problemInfo = {.title = strdup(title), .description = strdup(description),
		.remoteAddr = "", .id = 130};
	if(setInfo(home, repoName, problemName, &problemInfo) < 0){
		sprintf(error,"Info");
		goto exception;
	}

	return 0;

exception:
	fprintf(stderr, "%s error...\n",error);
	exit(EXIT_FAILURE);
}
	

//delete an additional testcase in repo
static int delete(int argc, char*argv[])
{
	char home[VALUESIZE] , problemName[VALUESIZE], repoName[VALUESIZE];
	char *values[] = {home,problemName,repoName};
	char *cache[] = {homeCache,NULL,NULL};
	
	fprintf(stderr,"delete : ");
	if(parseOpt(argc,argv,"h:p:n:",3,values,cache)<3){
		fprintf(stderr,"Missing opts...\n");
		exit(EXIT_FAILURE);
	}

	removeProblem(home, repoName, problemName);

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
