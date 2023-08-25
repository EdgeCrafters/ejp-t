#include "parse.h"

int cnvtNormal(int resultFile, char *inputContent, int inputSize)
{
	if(write(resultFile,inputContent,strlen(inputContent)) < 0)
		return -1;
	return 0;
}

int cnvtInfo(int resultFile, char *inputContent, int inputSize, char title[], char description[])
{
	cJSON *root = cJSON_Parse(inputContent);
	cJSON *result = cJSON_CreateObject();
	if(!root || !result)
		goto exception;

	cJSON *titlejson= cJSON_GetObjectItem(root,"title");
	cJSON *descriptionjson = cJSON_GetObjectItem(root,"description");
	if(!titlejson || !descriptionjson)
		goto exception;

	char *titlestr, *descriptionstr;
	titlestr = titlejson->valuestring, descriptionstr = descriptionjson->valuestring;
	strcpy(title,titlestr);
	strcpy(description, descriptionstr);

	if(write(resultFile,inputContent,strlen(inputContent)) < 0)
		goto exception;

	return 0;
	
exception:
	fprintf(stderr, "convert Info error...\n");
	return -1;
}

int cnvtTC(char *inputContent, int inputSize, struct problemTestcase *testcases)
{
	if(!testcases)
		goto exception;
	else if (testcases->num > MAXTC - 1)
		return 0;
	

	cJSON *root = cJSON_Parse(inputContent);
	cJSON *result = cJSON_CreateObject();
	if(!root || !result)
		goto exception;

	cJSON *inputjson = cJSON_GetObjectItem(root,"input");
	cJSON *outputjson = cJSON_GetObjectItem(root,"output");
	if(!inputjson || !outputjson)
		goto exception;

	char *inputstr, *outputstr;
	inputstr = inputjson->valuestring, outputstr = outputjson->valuestring;

	testcases->input[testcases->num] = strdup(inputstr);
	testcases->output[testcases->num] = strdup(SHA256(outputstr));
	++(testcases->num);

	return 0;
	
exception:
	fprintf(stderr, "convert TestCase error...\n");
	return -1;
}

int cnvtWrapper(const int inputFile, const char resultFilePath[], char title[], char description[])
{
	char inputStr[STRSIZE];
	read(inputFile, inputStr, STRSIZE);

	int resultFile;
	if((resultFile = open(resultFilePath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU)) <0 )
		return -1;
	if(!title && !description && cnvtNormal(resultFile, inputStr, STRSIZE)<0)
		fprintf(stderr," fail!\n");
	else if(title && description && cnvtInfo(resultFile, inputStr, STRSIZE, title, description)<0)
		fprintf(stderr," fail!\n");
	else
		fprintf(stderr," done!\n");

	close(inputFile);
	close(resultFile);

	return 0;
}

int encode(char resultPath[], char inputPath[],
		struct tcInfo biases[], char title[], char description[], struct problemTestcase *result)
{
	char error[STRSIZE];

	if(resultPath && mkdir(resultPath, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
		sprintf(error,"mkdir %s (errno : %d)", resultPath, errno);
		goto exception;
	}

	DIR *inputDir = opendir(inputPath);
	if(errno == ENOTDIR && inputDir == NULL){
		if(strncmp(strrchr(inputPath,'.'),".json",5)){
			sprintf(error,"wrong inputfile");
			goto exception;
		}

		int inputFile;
		if((inputFile = open(inputPath, O_RDONLY)) < 0){
			sprintf(error,"open file");
			goto exception;
		}

		fprintf(stderr,"converting ");
		for(int i = 0; i<5; ++i){
			sleep_ms(10);
			fprintf(stderr,".");
		}

		char inputStr[STRSIZE];read(inputFile, inputStr, STRSIZE);
		if(cnvtTC(inputStr, STRSIZE, result)<0)
			fprintf(stderr," fail!\n");
		else
			fprintf(stderr," done!\n");
		return 0;
	}else if(inputDir == NULL){
		sprintf(error,"opendir");
		fprintf(stderr,"inputDir : %s \n", inputPath);
		goto exception;
	}

	struct dirent *dent;
	while((dent = readdir(inputDir))){
		if(dent->d_type != DT_REG)
			continue;

		static int isJson;
		char filename[BUFSIZE],inputFilePath[BUFSIZE],resultFilePath[BUFSIZE],*extension;
		strcpy(filename,dent->d_name);extension = strrchr(filename,'.');
		if(strncmp(extension,".json",5)){
			isJson = 0;
			sprintf(inputFilePath,"%s/%s",inputPath,filename);
			sprintf(resultFilePath,"%s/%s",resultPath,filename);
		}else{
			isJson = 1;
			*extension = '\0';
			sprintf(inputFilePath,"%s/%s.json",inputPath,filename);
			sprintf(resultFilePath,"%s/%s.json",resultPath,filename);
		}

		int inputFile;
		if((inputFile = open(inputFilePath, O_RDONLY)) < 0){
			sprintf(error,"open file");
			goto exception;
		}
			
		fprintf(stderr,"converting ");

		for(int i = 0; i<5; ++i){
			sleep_ms(10);
			fprintf(stderr,".");
		}

		if(!isJson && cnvtWrapper(inputFile,resultFilePath,NULL,NULL)<0){
			sprintf(error,"open file");
			goto exception;
		}else if(!isJson)
			continue;
		int isInfo;
		if((isInfo = !strncmp(filename,"info",4)) && cnvtWrapper(inputFile,resultFilePath,title,description)<0){
			sprintf(error,"open file");
			goto exception;
		}else if(isInfo)
			continue;
		
		char inputStr[STRSIZE];
		read(inputFile, inputStr, STRSIZE);
		if(cnvtTC(inputStr, STRSIZE, result)<0)
			fprintf(stderr," fail!\n");
		else
			fprintf(stderr," done!\n");

		close(inputFile);
	}

	closedir(inputDir);
	return 0;

exception:
	fprintf(stderr, "%s error...\n", error);
	return -1;
}
