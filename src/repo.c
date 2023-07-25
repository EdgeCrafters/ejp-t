#include "../includes/common.h"
#include <stdio.h>

static int result(int argc, char*argv[])
{
	printf("result : ");
	return 0;
}

static int info(int argc, char*argv[])
{
	printf("info : ");
	return 0;
}

static int upload(int argc, char*argv[])
{
	printf("upload : ");
	return 0;
}

static int init(int argc, char*argv[])
{
	printf("init : ");
	return 0;
}

int repo(int argc, char*argv[])
{
	char command[CMDSIZE];
	if(argc<3 || !strcpy(command,argv[2])){
		fprintf(stderr,"no command ...\n");
		repoInfo();
		exit(-1);
	}

	if(!strncmp(command,"init",4)){
		if(init(argc-2,argv+2)<0){
			fprintf(stderr,"error...");
			exit(-1);
		}
	}else if(!strncmp(command,"upload",6)){
		if(upload(argc-2,argv+2)<0){
			fprintf(stderr,"error...");
			exit(-1);
		}
	}else if(!strncmp(command,"info",4)){
		if(info(argc-2,argv+2)<0){
			fprintf(stderr,"unknown error...");
			exit(-1);
		}
	}else if(!strncmp(command,"result",6)){
		if(result(argc-2,argv+2)<0){
			fprintf(stderr,"error...");
			exit(-1);
		}
	}else{
		fprintf(stderr,"unkown command...\n");
		repoInfo();
		exit(-1);
	}
	return 0;
}

