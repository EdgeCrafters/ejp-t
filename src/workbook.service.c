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

    if( remove_directory(repoInfo.localPath) < 0)
        fprintf(stderr,"Fail to remove repo in local ... remove it manually (path : %s)",repoInfo.localPath);

    if (deleteRepoHTTP(home, repoInfo.id) < 0)
    {
        fprintf(stderr, "Fail to delete problem %s ...\n", repoInfo.title);
        exit(EXIT_FAILURE);
    }

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}

int deleteProblem(char home[], char repoName[], char problemName[])
{
    char error[STRSIZE];

    struct info problemInfo;
    if (getInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        sprintf(error, "problemInfo");
        goto exception;
    }

    if( remove_directory(problemInfo.localPath) < 0)
        fprintf(stderr,"Fail to remove problem in local ... remove it manually (path : %s)",problemInfo.localPath);

    if (deleteProblemHTTP(home, problemInfo.id) < 0)
    {
        fprintf(stderr, "Fail to delete problem %s ...\n", problemInfo.title);
        exit(EXIT_FAILURE);
    }

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