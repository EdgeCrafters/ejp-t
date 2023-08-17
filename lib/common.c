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
            else if (c == '?' && optopt == targetOpt[i])
            {
                fprintf(stderr, "Option -%c requires \
						an argument.\n",
                        optopt);
                exit(EXIT_FAILURE);
            }

    for (int i = 0; i < optNum; ++i)
        if (flags[i] && caches[i])
        {
            int cache;
            if ((cache = open(caches[i], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXO | S_IRWXU)) < 0)
                goto cacheException;
            else if (write(cache, optArg[i], strlen(optArg)) < 0)
            {
                close(cache);
                goto cacheException;
            }
            close(cache);
        }
        else if (caches[i])
        {
            int cache;
            char buf[BUFSIZE];
            if ((cache = open(caches[i], O_RDONLY)) < 0 || read(cache, buf, BUFSIZE) < 0)
                goto exception;

            strcpy(optArg[i], buf);
        }
        else if (flags[i])
            continue;
        else
            goto exception;

    return result;

exception:
    fprintf(stderr, "Missing options require %d, receive %d\n", optNum, result);
    exit(EXIT_FAILURE);

cacheException:
    fprintf(stderr, "Wrong with cache\n");
    exit(EXIT_FAILURE);
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
    ssize_t len = readlink("/proc/self/exe", buf, PATH_MAX);
    if (len != -1)
    {
        buf[len] = '\0';
        strcpy(path, dirname(buf));
        return 0;
    }

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

    cJSON *root, *title, *description, *remoteAddr, *id;
    root = cJSON_Parse(buf);
    title = cJSON_GetObjectItem(root, "title");
    description = cJSON_GetObjectItem(root, "description");
    remoteAddr = cJSON_GetObjectItem(root, "remoteAddr");
    id = cJSON_GetObjectItem(root, "id");

    info->title = title ? strdup(title->valuestring) : NULL;
    info->description = description ? strdup(description->valuestring) : NULL;
    info->localPath = strdup(path);
    info->remoteAddr = remoteAddr ? strdup(remoteAddr->valuestring) : NULL;
    info->id = id ? strdup(id->valuestring) : NULL;

    return 0;

exception:
    return -1;
}

static int overwrite(cJSON *dest, cJSON *src, const char property[], const char text[])
{
    if(text){
        cJSON_AddItemToObject(dest, property, text);
        return 0;
    }

    cJSON *value = NULL;
    if(src && (value =  cJSON_GetObjectItem(src,property)))
        cJSON_AddItemToObject(dest, property, value->valuestring);
    else 
        cJSON_AddItemToObject(dest, property, "");

    return 0;
}

int setInfo(char home[], char repoName[], char problemName[], struct info *info)
{
    char path[PATHSIZE];
    if (!problemName)
        sprintf(path, "%s/%s/%s", repos, home, repoName);
    else
        sprintf(path, "%s/%s/%s/%s", repos, home, repoName, problemName);

    char infoPath[PATHSIZE];
    sprintf(infoPath, "%s/info.json", path);
    int infoFile;
    char buf[BUFSIZE];
    cJSON *root = NULL;
    if ((infoFile = open(infoPath, O_RDONLY)) < 0 || read(infoFile, buf, BUFSIZE) < 0)
        close(infoFile);
    else
        root = cJSON_Parse(buf);

    cJSON *result = cJSON_CreateObject();

    overwrite(result,root,"title",info->title);
    overwrite(result,root,"description",info->description);
    overwrite(result,root,"remoteAddr",info->remoteAddr);
    overwrite(result,root,"id",info->id);

    char *resultstr = cJSON_Print(result);
    if ((infoFile = open(infoPath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) < 0 
        || write(infoFile, resultstr, strlen(resultstr)) == 0)
        goto exception;

    return 0;

exception:
    return -1;
}
