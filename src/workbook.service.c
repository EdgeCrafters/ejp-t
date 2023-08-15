#include "parse.h"
#include "http.h"
#include "common.h"

int initRepo(const char home[], const char repoName[])
{
    char repoId[BUFSIZE];
    if (initRepoHTTP(home, repoName, repoId, BUFSIZE) < 0)
    {
        fprintf(stderr, "Fail to init repo ...\n");
        exit(EXIT_FAILURE);
    }
    else
        fprintf(stdout, "Init repo (repo address : %d)\n", atoi(repoId));

    char path[PATHSIZE];sprintf(path,"%s/%s/%s",repos,home,repoName);
    gitInit(home,repoName,path);

    struct info repoInfo;
    repoInfo.title = strdup(repoName);
    repoInfo.id = atoi(repoId);
    // repoInfo.localPath = strdup(path);
    char address[PATHSIZE]; sprintf(address,"git@%s:%s",home,repoName);
    repoInfo.remoteAddr = strdup(address);

    if (setInfo(home, repoName, NULL, &repoInfo) < 0)
        goto exception;

    return 0;

exception:
    return -1;
}

int deleteProblem(char home[], char repoName[], char problemName[])
{
    char error[STRSIZE];

    struct info problemInfo;
    if (getInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        sprintf(error, "repoInfo");
        goto exception;
    }

    fprintf(stderr, "delete path : %s\n", problemInfo.localPath);
    char cmd[CMDSIZE];
    sprintf(cmd, "rm -rf %s", problemInfo.localPath);
    system(cmd);

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
    struct tcInfo biases[BUFSIZE];
    if (encode(resultDir, problemDir, biases, title, description) < 0)
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

int makeProblem(char home[], char repoName[], char problemDir[], char problemName[], char result[])
{
    char error[STRSIZE];

    if (mkdir(result, S_IRWXU | S_IRWXO) < 0 && errno != EEXIST)
    {
        sprintf(error, "mkdir %d ", errno);
        goto exception;
    }

    char resultDir[URLSIZE];
    sprintf(resultDir, "%s/%s", result, problemName);
    fprintf(stderr, "resultDir : %s\n", resultDir);

    char title[STRSIZE], description[STRSIZE];
    struct tcInfo biases[BUFSIZE];
    if (encode(resultDir, problemDir, biases, title, description) < 0)
        goto exception;
    // git upload testcases...

    struct info repoInfo;
    if (getInfo(home, repoName, NULL, &repoInfo) < 0)
    {
        sprintf(error, "Info");
        goto exception;
    }

    char buffer[BUFSIZE];
    if (createProblemHTTP(home, repoInfo.id, title, description, buffer) < 0)
    {
        exit(EXIT_FAILURE);
    }
    cJSON *response = cJSON_Parse(buffer);
    cJSON *id = cJSON_GetObjectItem(response, "id");
    char problemId[IDSIZE];
    sprintf(problemId, "%d", id->valueint);

    struct info problemInfo = {.title = strdup(title), .description = strdup(description), .remoteAddr = "", .id = strdup(problemId)}; // strdup(problemId)};
    if (setInfo(home, repoName, problemName, &problemInfo) < 0)
    {
        sprintf(error, "Info");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr, "%s error...\n", error);
    exit(EXIT_FAILURE);
}