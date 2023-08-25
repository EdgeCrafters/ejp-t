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




