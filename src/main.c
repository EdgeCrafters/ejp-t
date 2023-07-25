#include "../includes/common.h"

int repo(int argc, char*argv[]);
int workbook(int argc, char*argv[]);

int main(int argc, char*argv[])
{
//	char command[CMDSIZE];
//	if(argc < 2 || !strcpy(command,argv[1])){
//		fprintf(stderr,"no command ...\n");
//		basicInfo();
//		exit(-1);
//	}
//
//	if(!strncmp(command,"workbook",8)){
//		printf("workbook : ");
//		return workbook(argc,argv);
//	}else if(!strncmp(command,"repo",4)){
//		printf("repo : ");
//		return repo(argc,argv);
//	}else{
//		fprintf(stderr,"unknown command...\n");
//		basicInfo();
//		exit(-1);
//	}



//	char input[] = "2345";
//	char output[] = "1234";
//	char bias[] = "9";
//	char *args[] = {"result.c","../testcase/a.input","../testcase/a.output","9"};
//	gen(4,args);
//
	login(argv[1],argv[2],argv[3]);
	logout(argv[1]);
//
//	diff("diff","diff",0);

	return 0;
}
