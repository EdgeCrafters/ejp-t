#include "common.h" 
#include "http.h"

int printInfo(const char path[])
{
    struct info info;
    getInfoByPath(path,&info);

    fprintf(stderr,"title : %s\n",info.title);
    fprintf(stderr,"description : %s\n\n",info.description);

    return 0;

exception:
    return -1;
}

int listInfo(int argc, char*argv[])
{
    char error[ERRORISZE];
    char path[PATHSIZE]; sprintf(path,"%s",repos);
    for(int i = 0; i < argc; ++i)
        sprintf(path,"%s/%s",path,argv[i]);

    DIR *dir;
    if((dir = opendir(path)) == NULL){
        fprintf(stderr,"opendir ");
        return -1;
    }

    fprintf(stderr,"\n=========================================\n");
    for(struct dirent *dent = readdir(dir); dent; dent = readdir(dir)){
        char target[PATHSIZE];
        sprintf(target,"%s/%s",path,dent->d_name);
        if(dent->d_type == DT_DIR && strncmp(dent->d_name,"..",2) &&
            strcat(target,"/info.json") && !printInfo(target))
            ;
    }
    fprintf(stderr,"=========================================\n");

    return 0;
}

int createUser(const char home[], const char username[], const char password[])
{
    char error[ERRORISZE];

    char usernames[1][IDSIZE], passwords[1][PWSIZE];
    strcpy(usernames[0],username); strcpy(passwords[0],password);
    if(createUsersHTTP(home, usernames, passwords,1)<0){
        sprintf(error,"faile to create a user...");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s error...",error);
    return -1;
}

int createUsers(const char home[], const char location[])
{
    char error[ERRORISZE];

    FILE *fp = fopen(location, "r");
    if (fp == NULL) {
        sprintf(error,"failed to open file");
        goto exception;
    }

    char buf[BUFSIZE], usernames[MAXSTUDENT][IDSIZE], passwords[MAXSTUDENT][PWSIZE];
    int recordCount = 0;

    fgets(buf, sizeof(buf), fp);
    while (fgets(buf, sizeof(buf), fp)) {
        size_t length = strlen(buf);
        if (length > 0 && buf[length - 1] == '\n')
            buf[length - 1] = '\0';

        char *token;
        if((token = strtok(buf,",")))
            strncpy(usernames[recordCount],token,IDSIZE);
        
        if((token = strtok(NULL,","))){
            size_t tokenLength = strlen(token);
            if(token[tokenLength-1] == '\r' || token[tokenLength-1] == '\n')
                token[tokenLength-1] = '\0';
            strncpy(passwords[recordCount++],token,PWSIZE);
        }
    }

    fclose(fp);

    if(createUsersHTTP(home,usernames,passwords,recordCount)<0){
        sprintf(error,"fail to create users...");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s error...", error);
    return -1;
}

int enrollUser(const char home[], const char username[], const char repoName[])
{
    char error[ERRORISZE];

    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);

    char usernames[1][IDSIZE], repoIDs[1][IDSIZE];
    strcpy(usernames[0],username); strcpy(repoIDs[0],repoInfo.id);
    if(enrollUsersHTTP(home, usernames, repoIDs,1)<0){
        sprintf(error,"faile to enroll a user...");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s error...",error);
    return -1;
}

int enrollUsers(const char home[], const char location[])
{
    char error[ERRORISZE];

    FILE *fp = fopen(location, "r");
    if (fp == NULL) {
        sprintf(error,"failed to open file");
        goto exception;
    }

    char buf[BUFSIZE], usernames[MAXSTUDENT][IDSIZE], repoIDs[MAXSTUDENT][IDSIZE];
    int recordCount = 0;

    fgets(buf, sizeof(buf), fp);
    while (fgets(buf, sizeof(buf), fp)) {
        size_t length = strlen(buf);
        if (length > 0 && buf[length - 1] == '\n')
            buf[length - 1] = '\0';

        char *token;
        if((token = strtok(buf,",")))
            strncpy(usernames[recordCount],token,IDSIZE);
        
        if((token = strtok(NULL,","))){
            size_t tokenLength = strlen(token);
            if(token[tokenLength-1] == '\r' || token[tokenLength-1] == '\n')
                token[tokenLength-1] = '\0';

            struct info repoInfo;
            getInfo(home,token,NULL,&repoInfo);
            strncpy(repoIDs[recordCount++],repoInfo.id,IDSIZE);
        }
    }

    fclose(fp);

    if(enrollUsersHTTP(home,usernames,repoIDs,recordCount)<0){
        sprintf(error,"fail to enroll users...");
        goto exception;
    }

    return 0;

exception:
    fprintf(stderr,"%s error...", error);
    return -1;
}

int  userProblemScore(const char home[],const char repoName[],const char problemName[],const char userName[])
{
    char error[ERRORISZE];

    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);
    struct info problemInfo;
    getInfo(home,repoName,problemName,&problemInfo);

    cJSON *response = NULL;
    if(userProblemScoreHTTP(home,repoInfo.id,problemInfo.id,userName,&response)<0){
        sprintf(error,"fail to get user problem score");
        goto exception;
    }else if(!response){
        sprintf(error,"fail to parse user problem score");
        goto exception;
    }

    cJSON *pass = cJSON_GetObjectItem(response,"pass");
    cJSON *total = cJSON_GetObjectItem(response,"total");
    if(!pass||!total){
        sprintf(error,"unexpected");
        goto exception;
    }

    fprintf(stderr, "\n%-20s | %-20s | %10s | %10s|\n", "Username", "Problem", "Passed", "Total");
    fprintf(stderr, "%-20s | %-20s | %10d | %10d|\n", userName, problemName, pass->valueint, total->valueint);

    return 0;

exception:
    fprintf(stderr,"%s error...", error);
    return -1;
}

int  userRepoScore(const char home[],const char repoName[],const char userName[])
{
    char error[ERRORISZE];

    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);

    cJSON *response = NULL;
    if(userRepoScoreHTTP(home,repoInfo.id,userName,&response)<0){
        sprintf(error,"fail to get user repo score");
        goto exception;
    }else if(!response){
        sprintf(error,"fail to parse user repo score");
        goto exception;
    }    
    
    int i = 0;
    fprintf(stderr,"\nRepo : %s\n",repoInfo.title);
    fprintf(stderr, "%-20s | %-20s | %10s | %10s|\n", "Username", "Problem", "Passed", "Total");
    for(cJSON *score = cJSON_GetArrayItem(response,i);
        score;
        score = cJSON_GetArrayItem(response,++i)){
        cJSON *pass = cJSON_GetObjectItem(score,"pass");
        cJSON *total = cJSON_GetObjectItem(score,"total");
        cJSON *problemName = cJSON_GetObjectItem(score,"problemName");
        if(!pass||!total){
            sprintf(error,"unexpected");
            goto exception;
        }

        fprintf(stderr, "%-20s | %-20s | %10d | %10d|\n", userName, problemName->valuestring, pass->valueint, total->valueint);
    }


    return 0;

exception:
    fprintf(stderr,"%s error...", error);
    return -1;
}

int  problemScore(const char home[],const char repoName[],const char problemName[], const char location[])
{
    char error[ERRORISZE];

    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);
    struct info problemInfo;
    getInfo(home,repoName,problemName,&problemInfo);

    cJSON *response = NULL;
    if(problemScoreHTTP(home,repoInfo.id,problemInfo.id,&response)<0){
        sprintf(error,"fail to get problem score");
        goto exception;
    }else if(!response){
        sprintf(error,"fail to parse problem score");
        goto exception;
    }

    cJSON *data = cJSON_GetObjectItem(response,"data");
    if(!data){
        sprintf(error,"fail to parse data");
        goto exception;
    }

    char resultPath[PATHSIZE]; sprintf(resultPath,"%s/%s-%s_score.csv",location,repoName,problemName);
    int result = open(resultPath,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if(result<0){
        sprintf(error,"result : %s fail to create result csv file",data->valuestring);
        goto exception;
    }else if(write(result,data->valuestring,strlen(data->valuestring))<0){
        sprintf(error,"result : %s fail to write result csv file",data->valuestring);
        goto exception;
    }

    fprintf(stderr,"result file at %s ", resultPath);
    close(result);

    return 0;

exception:
    fprintf(stderr,"%s error...", error);
    return -1;
}

int  repoScore(const char home[],const char repoName[], const char location[])
{
    char error[ERRORISZE];

    struct info repoInfo;
    getInfo(home,repoName,NULL,&repoInfo);

    DIR *repoDir = opendir(repoInfo.localPath);
    if(!repoDir){
        sprintf(error,"fail to get repoInfo");
        goto exception;
    }

    fprintf(stderr,"Repo %s scores : \n",repoName);
    struct dirent *dent;
	while((dent = readdir(repoDir))){
		if(dent->d_type != DT_DIR || dent->d_name[0] == '.')
			continue;

        struct info problemInfo;
        getInfo(home,repoName,dent->d_name,&problemInfo);
        problemScore(home,repoName,problemInfo.title,location);
        fprintf(stderr,"\n");
    }

    return 0;

exception:
    fprintf(stderr,"%s error...", error);
    return -1;
}
