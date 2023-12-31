#include "parse.h"
#include "http.h"
#include "common.h"

int uploadFile(const char home[], const char repoName[], const char problemName[], const char problemId[])
{
    char error[BUFSIZE];

    char archHome[PATHSIZE]; sprintf(archHome,"%s/%s",archives,home);
    char archRepo[URLSIZE]; sprintf(archRepo,"%s/%s",archHome,repoName);
	if(mkdir(archHome, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
        sprintf(error,"mkdir");
        return -1;
    }
    if(mkdir(archRepo, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
		sprintf(error,"mkdir");
        return -1;
    }
    char targetDir[PATHSIZE]; sprintf(targetDir,"%s/%s/%s/%s",repos,home,repoName,problemName);
    char resultTar[PATHSIZE]; sprintf(resultTar,"%s/%s.tar",archRepo,problemName);
    char cmd[CMDSIZE]; sprintf(cmd,"tar -czf %s %s 2> /dev/null",resultTar,targetDir);
    if(system(cmd) < 0){
        sprintf(error,"tar");
        goto exception;
    }

    if(uploadFileHTTP(home,problemId,resultTar)<0){
        sprintf(error,"upload file");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s error...\n",error);
    return -1;
}

int initRepo(const char home[], const char repoName[])
{	
	char homeAddr[URLSIZE]; sprintf(homeAddr,"%s/%s",repos,home);
	if(mkdir(homeAddr, S_IRWXU|S_IRWXO)<0 && errno != EEXIST)
		goto exception;
	char repoAddr[URLSIZE]; sprintf(repoAddr,"%s/%s",homeAddr,repoName);
	if(mkdir(repoAddr, S_IRWXU|S_IRWXO)<0 && errno != EEXIST)
		goto exception;

    char repoId[BUFSIZE];
    if (initRepoHTTP(home, repoName, repoId, BUFSIZE) < 0)
    {
        fprintf(stderr, "Fail to init repo ...\n");
        goto exception;
    }
        
    struct info repoInfo = {NULL};
    repoInfo.title = strdup(repoName);
    repoInfo.id = strdup(repoId);
    // repoInfo.localPath = strdup(path);
    char address[PATHSIZE]; sprintf(address,"git@%s:%s",home,repoName);
    repoInfo.remoteAddr = strdup(address);

    if (setInfo(home, repoName, NULL, &repoInfo) < 0)
        goto exception;

    return 0;

exception:
    return -1;
}

int deleteRepo(char home[], char repoName[])
{
    char error[STRSIZE];

    struct info repoInfo;
    if (getInfo(home, repoName, NULL, &repoInfo) < 0)
    {
        sprintf(error, "repoInfo");
        goto exception;
    }

    if (deleteRepoHTTP(home, repoInfo.id) < 0)
    {
        fprintf(stderr, "Fail to delete problem in local %s ...\n", repoInfo.title);
        exit(EXIT_FAILURE);
    }

    if(remove_directory(repoInfo.localPath) < 0)
        fprintf(stderr,"Fail to remove repo in local ... remove it manually (path : %s)",repoInfo.localPath);

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}

int deleteProblem(char home[], char repoName[], char problemName[])
{
    char error[STRSIZE];

    struct info repoInfo;
    if (getInfo(home, repoName, NULL, &repoInfo) < 0)
    {
        sprintf(error, "repoInfo");
        goto exception;
    }

    struct info problemInfo;
    if (!problemName[0] || getInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        cJSON *response = NULL;
        if(getReposHTTP(home,repoInfo.id, &response)<0 || !response){
            fprintf(stderr,"fail to get repo info...");
            return -1;
        }

        cJSON *problemArray = cJSON_GetObjectItem(response,"Problem");
        if(!problemArray){
            fprintf(stderr,"fail to get problem array...");
            return -1;
        }else if(cJSON_GetArraySize(problemArray) < 1){
            fprintf(stderr,"no such problem...");
        }
        fprintf(stderr, "problem List:\n");
        fprintf(stderr, "%-10s | %-30s | %s\n", "ID", "Title", "Description");

        cJSON *problem;
        int tcsize = cJSON_GetArraySize(problemArray), i = 0;
        for(problem = cJSON_GetArrayItem(problemArray, i); 
            problem; 
            problem = cJSON_GetArrayItem(problemArray, ++i)) {

            int id = cJSON_GetObjectItem(problem, "id")->valueint;
            char title[VALUESIZE] = {0}, description[VALUESIZE] = {0};
            strncpy(title, cJSON_GetObjectItem(problem, "title")->valuestring, VALUESIZE - 1);
            strncpy(description, cJSON_GetObjectItem(problem, "text")->valuestring, VALUESIZE - 1);

            fprintf(stderr, "%-10d | %-30s | %s\n", id, title, description);
        }

        fprintf(stderr,"Enter problems' id to delete (seperate with space, press enter) : ");
        do{
            int _id;scanf("%d",&_id);
            char id[IDSIZE]; sprintf(id,"%d",_id);
            deleteProblemHTTP(home,id);
        } while(getchar()!='\n');
    }else if (deleteProblemHTTP(home, problemInfo.id) < 0)
    {
        fprintf(stderr, "Fail to delete problem %s ...\n", problemInfo.title);
        exit(EXIT_FAILURE);
    }else if( remove_directory(problemInfo.localPath) < 0)
        fprintf(stderr,"Fail to remove problem in local ... remove it manually (path : %s)",problemInfo.localPath);

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}

int updateProblem(char home[], char repoName[], char problemDir[], char problemName[])
{
    char error[STRSIZE];

    struct info problemInfo;
    if (getInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        sprintf(error, "Info");
        goto exception;
    }

    char *resultDir = strdup(problemInfo.localPath);
    char title[STRSIZE] = {'\0'}, description[STRSIZE] = {'\0'};
    struct problemTestcase testcases = {.num = 0};
    struct tcInfo biases[BUFSIZE];
    if (encode(resultDir, problemDir, biases, title, description, &testcases) < 0)
        goto exception;

    if (title[0])
        problemInfo.title = strdup(title);
    if (description[0])
        problemInfo.description = strdup(description);

    if (setInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        sprintf(error, "Info");
        goto exception;
    }
    // git upload testcases...
    if (updateProblemHTTP(home, problemInfo.id, title, description) < 0)
    {
        exit(EXIT_FAILURE);
    }

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}

int makeProblem(char home[], char repoName[], char problemDir[])
{
    char error[STRSIZE];

    struct info rawInfo;
    char infoPath[PATHSIZE];
    sprintf(infoPath,"%s/info.json",problemDir);
    getInfoByPath(infoPath,&rawInfo);
    char problemName[VALUESIZE];
    sprintf(problemName,"%s",rawInfo.title);

    char resultDir[URLSIZE];
    sprintf(resultDir, "%s/%s/%s/%s", repos, home, repoName,problemName);
    if (mkdir(resultDir, S_IRWXU | S_IRWXO) < 0 && errno != EEXIST) {
        sprintf(error, "mkdir %d ", errno);
        goto exception;
    }

    char title[STRSIZE], description[STRSIZE];
    struct tcInfo biases[BUFSIZE];
    struct problemTestcase testcases = {.num = 0};
    if (encode(resultDir, problemDir, biases, title, description, &testcases) < 0)
        goto exception;

    struct info repoInfo;
    if (getInfo(home, repoName, NULL, &repoInfo) < 0){
        sprintf(error, "Info");
        goto exception;
    }

    char problemId[IDSIZE];
    if (createProblemHTTP(home, repoInfo.id, title, description, problemId) < 0){
        sprintf(error,"createProblemHTTP");
        goto exception;
    }

    struct info problemInfo = {.title = strdup(title), .description = strdup(description), .remoteAddr = "", .id = strdup(problemId)};
    if (setInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        sprintf(error, "Info");
        goto exception;
    }

    if(uploadFile(home,repoName,problemName,problemInfo.id)){
        sprintf(error,"Upload file");
        goto exception;
    }

    for(int i = 0 ; i < testcases.num; ++i)
        if (uploadHiddencasesHTTP(home,repoInfo.id,problemInfo.id,testcases.input[i],testcases.output[i]))
        {
            sprintf(error,"uploadHiddencasesHTTP");
            goto exception;
        }

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}

int makeTestcase(char home[], char repoName[], char problemName[], char testcase[])
{
    char error[ERRORISZE];

    struct problemTestcase testcases = {.num = 0};
    encode(NULL,testcase,NULL,NULL,NULL, &testcases);

    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);
    struct info problemInfo;
    getInfo(home,repoName,problemName,&problemInfo);
    if(uploadHiddencasesHTTP(home,repoInfo.id,problemInfo.id,testcases.input[0],testcases.output[0])){
        sprintf(error,"uploadHiddencasesHTTP");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}

int deleteTestcases(char home[], char repoName[], char problemName[])
{
    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);
    struct info problemInfo;
    getInfo(home,repoName,problemName,&problemInfo);

    cJSON *response = NULL;
    if(getReposHTTP(home,repoInfo.id, &response)<0 || !response){
        fprintf(stderr,"fail to get repo info...");
        return -1;
    }

    cJSON *problemArray = cJSON_GetObjectItem(response,"Problem");
    if(!problemArray){
        fprintf(stderr,"fail to get problem array...");
        return -1;
    }else if(cJSON_GetArraySize(problemArray) < 1){
        fprintf(stderr,"no such problem...");
    }

    cJSON *problemJson = cJSON_GetArrayItem(problemArray,0);
    if(!problemJson){
        fprintf(stderr,"fail to get problem info...");
        return -1;
    }
    
    cJSON *testcaseArray = cJSON_GetObjectItem(problemJson,"testCase");
    if(!testcaseArray){
        fprintf(stderr,"fail to get testcase array...");
        return -1;
    }

    
    fprintf(stderr, "Testcase List:\n");
    fprintf(stderr, "%-10s | %-30s | %s\n", "ID", "Input (First 30 chars)", "Is Hidden");

    cJSON *testcase;
    int tcsize = cJSON_GetArraySize(problemArray), i = 0;
    for(testcase = cJSON_GetArrayItem(testcaseArray, i); 
        testcase; 
        testcase = cJSON_GetArrayItem(testcaseArray, ++i)) {

        int id = cJSON_GetObjectItem(testcase, "id")->valueint;
        char input[VALUESIZE] = {0}, isHidden[VALUESIZE] = {0};
        strncpy(input, cJSON_GetObjectItem(testcase, "input")->valuestring, VALUESIZE - 1);
        strncpy(isHidden, cJSON_GetObjectItem(testcase, "isHidden")->valuestring, VALUESIZE - 1);

        fprintf(stderr, "%-10d | %-30s | %s\n", id, input, isHidden);
    }
    
    fprintf(stderr,"Enter testcases' id to delete (seperate with space, press enter) : ");
    int testcases[VALUESIZE] = {0};
    i = 0;
    do
        scanf("%d",&testcases[i++]);
    while(getchar()!='\n' && i < VALUESIZE);

    cJSON *targets = cJSON_CreateIntArray(testcases,i);
    if(!targets){
        fprintf(stderr,"unexcpected error...");
        return -1;
    }

    if(deleteTestcasesHTTP(home,repoInfo.id,targets)<0){
        fprintf(stderr,"fail to delte testcase...");
        return -1;
    }

    return 0;
}