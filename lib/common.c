#include "common.h"

int parseOpt(int argc, char *argv[], const char targetOpt[], const int optNum,
             char *optArg[], char *caches[])
{
    int c, result = 0;
    int flags[MAXOPT] = {0};

    while ((c = getopt(argc, argv, targetOpt)) != -1)
        for (int i = 0; i < optNum; ++i)
            if (c != '?' && c == targetOpt[2 * i])
            {
                flags[i] = 1;
                strcpy(optArg[i], optarg);
                result += 1;
            }
            else if (c == '?' && optopt == targetOpt[2 * i])
            {
                flags[i] = 1;
                optArg[i][0] = '\0';
                result += 1;
            }

    for (int i = 0; i < optNum; ++i)
        if (flags[i] && caches[i])
        {
            int cache;
            if ((cache = open(caches[i], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXO | S_IRWXU)) < 0)
                goto cacheException;
            else if (write(cache, optArg[i], strlen(optArg[i])+1) < 0)
            {
                close(cache);
                goto cacheException;
            }
            close(cache);
        }
        else if (caches[i])
        {
            int cache;
            if ((cache = open(caches[i], O_RDONLY)) < 0 || read(cache, optArg[i], BUFSIZE) < 0)
                goto exception;
            ++result;
        }

    return result;

exception:
    fprintf(stderr, "Missing options require %d, receive %d\n", optNum, result);
    return -1;

cacheException:
    fprintf(stderr, "Wrong with cache\n");
    return -1;
}

void userLogin(const char home[])
{
    char userID[IDSIZE], userPW[PWSIZE];
    memset(userID, 0, IDSIZE);
    memset(userPW, 0, PWSIZE);

    printf("\nEnter %s ID :", home);
    scanf("%s", userID);

    printf("Enter %s PW :", home);
    scanf("%s", userPW);

    if (!loginHTTP(home, userID, userPW))
    {
        fprintf(stdout, "Login Success!\n");
    }
    else
    {
        fprintf(stdout, "Try again");
        userLogin(home);
    }
}

void userLogout(const char home[])
{
    logoutHTTP(home);
}

char *getExtension(char *target)
{
    int i;
    for (i = 0; target[i] != '.' && target != '\0'; ++i)
        ;
    return target + i;
}

int getExecutablePath(char path[])
{
    if (!path)
        goto exception;

    char buf[PATHSIZE];
    ssize_t len = readlink("/proc/self/exe", buf, PATHSIZE);
    if (len != -1)
    {
        buf[len] = '\0';
        strcpy(path, dirname(buf));
        return 0;
    }

exception:
    return -1;
}

int getInfoByPath(const char path[], struct info *info)
{
    int infoFile;
    char buf[BUFSIZE];
    if ((infoFile = open(path, O_RDONLY)) < 0 || read(infoFile, buf, BUFSIZE) < 0)
        goto exception;
    close(infoFile);

    cJSON *root, *title, *description, *remoteAddr, *id;
    root = cJSON_Parse(buf);
    if(!root)
        goto exception;

    title = cJSON_GetObjectItem(root, "title");
    description = cJSON_GetObjectItem(root, "description");
    remoteAddr = cJSON_GetObjectItem(root, "remoteAddr");
    id = cJSON_GetObjectItem(root, "id");

    info->title = title ? strdup(title->valuestring) : strdup("");
    info->description = description ? strdup(description->valuestring) : strdup("");
    info->localPath = strdup(path);
    info->remoteAddr = remoteAddr ? strdup(remoteAddr->valuestring) : strdup("");
    info->id = id ? strdup(id->valuestring) : strdup("");

    return 0;

exception:
    return -1;
}

int getInfo(char home[], char repoName[], char problemName[], struct info *info)
{
    char path[URLSIZE];
    if (!problemName)
        sprintf(path, "%s/%s/%s", repos, home, repoName);
    else
        sprintf(path, "%s/%s/%s/%s", repos, home, repoName, problemName);

    char infoPath[URLSIZE];
    sprintf(infoPath, "%s/info.json", path);
    int infoFile;
    char buf[BUFSIZE];
    if ((infoFile = open(infoPath, O_RDONLY)) < 0 || read(infoFile, buf, BUFSIZE) < 0)
        goto exception;
    close(infoFile);

    cJSON *root, *title, *description, *remoteAddr, *id, *type;
    root = cJSON_Parse(buf);
    title = cJSON_GetObjectItem(root, "title");
    description = cJSON_GetObjectItem(root, "description");
    remoteAddr = cJSON_GetObjectItem(root, "remoteAddr");
    id = cJSON_GetObjectItem(root, "id");
    type = cJSON_GetObjectItem(root,"type");


    info->title = title ? strdup(title->valuestring) : NULL;
    info->description = description ? strdup(description->valuestring) : NULL;
    info->localPath = strdup(path);
    info->remoteAddr = remoteAddr ? strdup(remoteAddr->valuestring) : NULL;
    info->id = id ? strdup(id->valuestring) : NULL;
    info->type = type->valueint;

    return 0;

exception:
    return -1;
}

static int overwrite(cJSON *dest, cJSON *src, const char property[], const char text[], int type)
{
    if(type == problem || type == repo){
        cJSON_AddNumberToObject(dest,property,type);
        return 0;
    }
    
    if(text){
        cJSON_AddStringToObject(dest, property, text);
        return 0;
    }

    cJSON *value = NULL;
    if(src && (value =  cJSON_GetObjectItem(src,property)))
        cJSON_AddStringToObject(dest, property, value->valuestring);
    else
        cJSON_AddStringToObject(dest, property, "");

    return 0;
}

int setInfo(char home[], char repoName[], char problemName[], struct info *info)
{
    char path[PATHSIZE];
    if (!problemName){
        sprintf(path, "%s/%s/%s", repos, home, repoName);
        info->type = repo;
    }else{
        sprintf(path, "%s/%s/%s/%s", repos, home, repoName, problemName);
        info->type = problem;
    }
    char infoPath[PATHSIZE];
    sprintf(infoPath, "%s/info.json", path);
    int infoFile;
    char buf[BUFSIZE];
    cJSON *root = NULL;
    if ((infoFile = open(infoPath, O_RDONLY)) > 0 && read(infoFile, buf, BUFSIZE) > 0)
        root = cJSON_Parse(buf);
    else
        close(infoFile);

    cJSON *result = cJSON_CreateObject();

    overwrite(result,root,"title",info->title,-1);
    overwrite(result,root,"description",info->description,-1);
    overwrite(result,root,"id",info->id,-1);
    overwrite(result,root,"type",NULL,info->type);

    char *resultstr = cJSON_Print(result);
    if ((infoFile = open(infoPath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) < 0 
        || write(infoFile, resultstr, strlen(resultstr)) == 0)
        goto exception;

    return 0;

exception:
    return -1;
}

// from stackOverflow
void sleep_ms(int milliseconds){ // cross-platform sleep function
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
      sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

// from GPT-4.0
int remove_directory(const char *path) 
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if(!d)
        goto exit;

    struct dirent *p;
    r = 0;

    while (!r && (p = readdir(d))) {
        int r2 = -1;
        char *buf;
        size_t len;

        // Skip the names "." and ".." as we don't want to recurse on them.
        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            continue;

        len = path_len + strlen(p->d_name) + 2; 
        buf = malloc(len);

        if(!buf){
            r = r2;
            continue;
        }

        struct stat statbuf;
        snprintf(buf, len, "%s/%s", path, p->d_name);
        if(stat(buf,&statbuf)){
            free(buf);
            r = r2;
            continue;
        }
        
        if (S_ISDIR(statbuf.st_mode)) 
            r2 = remove_directory(buf);
        else
            r2 = remove(buf);

        free(buf);
        r = r2;
    }

    closedir(d);

exit:
    if (!r) {
        r = rmdir(path);
    }

    return r;
}