#include "parse.h"
#include "sha256.h"
#include "cJSON.h"

int entry(int fd)
{
	printf("entry fd: %d\n", fd);
	return write(fd,basicEntry,sizeof(basicEntry)-1);
}

int closing(int fd)
{
	return write(fd,basicClosing, sizeof(basicClosing)-1);
}

int putContents(int sourceFd, char prefix[], size_t prefixSize,\
		char postfix[], size_t postfixSize, char *inputPath, int bias)
{
	char buf[bufSize];
	char shaBuf[bufSize];
	int readNum, inputFd, inputNum, stored, stride;

	if((inputFd =	open(inputPath, O_RDONLY)) < 0)
		return -1;

	stored = 0;
	stride = 1;
	inputNum = write(sourceFd,prefix,prefixSize-1);
	while(bias >=0 && (readNum = read(inputFd,buf+stored,stride)) > 0){
		for(int i = 0;buf[stored+i]>32 && bias >=0 && i<readNum; ++i){
			buf[stored+i] += bias;
		}

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

	memset(buf,0,bufSize);
	while(bias<0 && (readNum = read(inputFd, buf, bufSize-1))){
		strcat(shaBuf,SHA256(buf));
		strcpy(buf, SHA256(shaBuf));
		strncpy(shaBuf, buf, 64);
		shaBuf[64] = '\0';
		memset(buf,0,bufSize);
	}

	if(bias < 0)
		inputNum += write(sourceFd, shaBuf, 64);
	
	inputNum += write(sourceFd, postfix, postfixSize-1);

	return inputNum;
}

int gen(int argc, char*argv[]){

	char resultFilePath[bufSize], inputFilePath[bufSize], outputFilePath[bufSize];

	if(argc < 4)
		return -1;

	strcpy(resultFilePath,argv[0]);
	strcpy(inputFilePath,argv[1]);
	strcpy(outputFilePath,argv[2]);
	
	int bias = atoi(argv[3]);
	int totalWrite = 0;
	int sourceFd = open(resultFilePath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU); // config

	printf("args => %s %s %s %d\nsourceFd => %d\n",resultFilePath, inputFilePath, outputFilePath, bias, sourceFd);

	totalWrite += entry(sourceFd);
	totalWrite += putContents(sourceFd, inputEntry, sizeof(inputEntry),\
			inputClosing, sizeof(inputClosing), inputFilePath, bias);
	totalWrite += putContents(sourceFd, outputEntry, sizeof(outputEntry),\
			outputClosing, sizeof(outputClosing), outputFilePath, -1);
	totalWrite += closing(sourceFd);

	close(sourceFd);

	char compileCmd[bufSize];
	sprintf(compileCmd,"gcc -o result %s",resultFilePath);

	cJSON* dummy;

	return 0;
}
