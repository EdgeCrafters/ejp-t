#include "common.h"
#include "parse.h"

int entry(int fd)
{
	printf("entry fd: %d\n", fd);
	return write(fd,basicEntry,sizeof(basicEntry)-1);
}

int closing(int fd)
{
	return write(fd,basicClosing, sizeof(basicClosing)-1);
}

int putContents(int sourceFd, char prefix[], size_t prefixSize, char postfix[], size_t postfixSize, char *inputPath)
{
	char buf[bufSize];
	int readNum, inputFd, inputNum, stored, stride;

	if((inputFd =	open(inputPath, O_RDONLY)) < 0)
		return -1;

	stored = 0;
	stride = 1;
	inputNum = write(sourceFd,prefix,prefixSize-1);
	while((readNum = read(inputFd,buf+stored,stride)) > 0){
		if(buf[stored+readNum-1] == '\n'){
			buf[stored+readNum-1] = '\0';
			strcat(buf,"\\n");
			readNum += 1;
		}

		stored += readNum;

		if(stored+stride >= bufSize){
			stored = 0;
			inputNum += write(sourceFd, buf, stored);
		}
	}
	inputNum += write(sourceFd, buf, stored);
	inputNum += write(sourceFd, postfix, postfixSize-1);

	return inputNum;
}

int gen(int argc, char*argv[]){

	char resultFilePath[bufSize], inputFilePath[bufSize], outputFilePath[bufSize];
//	int c, flagI = 0, flagO = 0;
//	while((c = getopt(argc,argv, "i:o:"))!=-1){
//		switch(c){
//			case 'i':
//				flagI = 1;
//				strcpy(inputFilePath,optarg);
//				break;
//			case 'o':
//				flagO = 1;
//				strcpy(outputFilePath,optarg);
//				break;
//			case '?':
//				if(optopt == 'd' || optopt == 'i' || optopt == 'o')
//					fprintf(stdout, "Missing FileName\n");
//				else 
//					fprintf(stdout,"Unkown Option: %c \n", optopt);
//				exit(-1);
//				break;
//		}
//	}
//
//	if(!(flagI&&flagO)){
//		fprintf(stdout, "Missing Opts");	
//		exit(-1);
//	}

	if(argc < 3)
		return -1;

	strcpy(resultFilePath,argv[0]);
	strcpy(inputFilePath,argv[1]);
	strcpy(outputFilePath,argv[2]);

	int totalWrite = 0;
	int sourceFd = open(resultFilePath,O_WRONLY|O_CREAT|O_TRUNC); // config

	totalWrite += entry(sourceFd);
	totalWrite += putContents(sourceFd, inputEntry, sizeof(inputEntry), inputClosing, sizeof(inputClosing), inputFilePath);
	totalWrite += putContents(sourceFd, outputEntry, sizeof(outputEntry), outputClosing, sizeof(outputClosing), outputFilePath);
	totalWrite += closing(sourceFd);

	close(sourceFd);

	return 0;
}
