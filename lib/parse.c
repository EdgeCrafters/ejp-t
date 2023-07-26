#include "parse.h"
#include "sha256.h"
#include "cJSON.h"

int convert(int resultFile, char *inputContent, int inputSize, int bias)
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
	int k = write(resultFile,resultstr,strlen(resultstr));

	return 0;
	
exception:
	fprintf(stderr, "error...\n");
	return -1;
}

int encode(char resultPath[], char inputPath[], int bias)
{
	DIR *inputDir, *resultDir;
	if((inputDir = opendir(inputPath)) == NULL)
		goto exception;

	struct dirent *dent;
	while((dent = readdir(inputDir))){
		if(dent->d_type != DT_REG)
			continue;

		char filename[BUFSIZE],*extension;
		strcpy(filename,dent->d_name);extension = getExtension(filename);
		if(strncmp(extension,".json",5))
			continue;

		char inputFilePath[BUFSIZE], resultFilePath[BUFSIZE];
		sprintf(inputFilePath,"%s/%s",inputPath,filename);
		sprintf(resultFilePath,"%s/",resultPath);
		strncat(resultFilePath,filename,extension-filename);
		strcat(resultFilePath,".result.json");

		int resultFile = open(resultFilePath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU);
		int inputFile = open(inputFilePath, O_RDONLY);

		char inputStr[STRSIZE];
		read(inputFile, inputStr, STRSIZE);
		
		fprintf(stdout,"converting %s to %s...\n",inputFilePath, resultFilePath);
		convert(resultFile, inputStr, STRSIZE, bias);

		close(inputFile);
		close(resultFile);
	}

	closedir(inputDir);
	closedir(resultDir);
	return 0;

exception:
	fprintf(stderr, "error...\n");
	return -1;
}
