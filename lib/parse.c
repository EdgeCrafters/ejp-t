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

int encode(char resultPath[], char inputPath[],
		struct tcInfo biases[], char title[], char description[], struct problemTestcase *result)
{
	char error[STRSIZE];

	if(mkdir(resultPath, S_IRWXU|S_IRWXO)<0 && errno != EEXIST){
		sprintf(error,"mkdir %d ", errno);
		goto exception;
	}

	DIR *inputDir;
	if((inputDir = opendir(inputPath)) == NULL){
		sprintf(error,"opendir");
		fprintf(stderr,"inputDir : %s \n", inputPath);
		goto exception;
	}

	int cnt = 0; // count for testcase
	srand(time(NULL));
	struct dirent *dent;
	while((dent = readdir(inputDir))){
		if(dent->d_type != DT_REG)
			continue;

		static int isJson;
		char filename[BUFSIZE],*extension,inputFilePath[BUFSIZE],resultFilePath[BUFSIZE];
		strcpy(filename,dent->d_name);extension = getExtension(filename);
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

		char inputStr[STRSIZE];
		read(inputFile, inputStr, STRSIZE);
			
		fprintf(stderr,"converting %s to %s ",inputFilePath, resultFilePath);

		for(int i = 0; i<5; ++i){
			sleep_ms(10);
			fprintf(stderr,".");
		}

		if(!isJson){
			int resultFile;
			if((resultFile = open(resultFilePath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU)) <0 )
				goto exception;
			if(cnvtNormal(resultFile, inputStr, STRSIZE)<0)
				fprintf(stderr," fail!\n");
			else
				fprintf(stderr," done!\n");

			close(inputFile);
			close(resultFile);
			continue;
		}

		if(!strncmp(filename,"info",4)){
			int resultFile;
			if((resultFile = open(resultFilePath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU)) <0 )
				goto exception;
			if(cnvtInfo(resultFile, inputStr, STRSIZE, title, description)<0)
				fprintf(stderr,"f ail!\n");
			else
				fprintf(stderr," done!\n");

			close(inputFile);
			close(resultFile);
			continue;
		}
		
		static char bias; static int biasIdx = 0;
		biases[biasIdx].name = strdup(filename);
		biases[biasIdx].localPath = strdup(resultFilePath);
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
