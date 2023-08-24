#include "common.h" 

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