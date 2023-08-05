#include "parse.h"
#include "sha256.h"
#include "cJSON.h"

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

int cnvtTC(int resultFile, char *inputContent, int inputSize, char bias)
{
	cJSON *root = cJSON_Parse(inputContent);
	cJSON *result = cJSON_CreateObject();
	if(!root || !result)
		goto exception;

	cJSON *inputjson = cJSON_GetObjectItem(root,"input");
	cJSON *outputjson = cJSON_GetObjectItem(root,"output");
	if(!inputjson || !outputjson)
		goto exception;

	char *inputstr, *outputstr, *resultstr, buffer[STRSIZE];
	inputstr = inputjson->valuestring, outputstr = outputjson->valuestring, resultstr = NULL;
	
	for(int i = 0; inputstr[i] != '\0'; ++i){
 		char c = inputstr[i];
 
 		if(c > 32){
 			c += bias;
 		}else if(c != ' ' && c != '\n')
			goto exception;
 		
		buffer[i] = c;
	}

	cJSON *_input = cJSON_CreateString(buffer);
	if(!_input)
		goto exception;
	cJSON_AddItemToObject(result,"input",_input);

	cJSON *_output = cJSON_CreateString(SHA256(outputstr));
	if(!_output)
		goto exception;
	cJSON_AddItemToObject(result,"output",_output);
	
	resultstr = cJSON_Print(result);
	if(write(resultFile,resultstr,strlen(resultstr)) == 0)
		goto exception;

	return 0;
	
exception:
	fprintf(stderr, "convert TestCase error...\n");
	return -1;
}

int encode(char resultPath[], char inputPath[],
		struct tcInfo biases[], char title[], char description[])
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

	srand(time(NULL));
	struct dirent *dent;
	while((dent = readdir(inputDir))){
		if(dent->d_type != DT_REG)
			continue;

		char filename[BUFSIZE],*extension;
		strcpy(filename,dent->d_name);extension = getExtension(filename);
		if(strncmp(extension,".json",5))
			continue;
		*extension = '\0';

		char inputFilePath[BUFSIZE], resultFilePath[BUFSIZE];
		sprintf(inputFilePath,"%s/%s.json",inputPath,filename);
		sprintf(resultFilePath,"%s/%s.json",resultPath,filename);

		int resultFile, inputFile;
		if((resultFile = open(resultFilePath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU)) <0 
				|| (inputFile = open(inputFilePath, O_RDONLY)) < 0){
			sprintf(error,"open file");
			goto exception;
		}

		char inputStr[STRSIZE];
		read(inputFile, inputStr, STRSIZE);
			
		fprintf(stdout,"converting %s to %s...\n",inputFilePath, resultFilePath);

		if(!strncmp(filename,"info",4)){
			cnvtInfo(resultFile, inputStr, STRSIZE, title, description);

			close(inputFile);
			close(resultFile);
			continue;
		}
		
		static char bias; static int biasIdx = 0;
		biases[biasIdx].name = strdup(filename);
		biases[biasIdx].localPath = strdup(resultFilePath);
		biases[biasIdx++].bias = (bias = rand()%256);
		cnvtTC(resultFile, inputStr, STRSIZE, bias);

		close(inputFile);
		close(resultFile);
	}

	closedir(inputDir);
	return 0;

exception:
	fprintf(stderr, "%s error...\n", error);
	return -1;
}
