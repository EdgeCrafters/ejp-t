#include "parse.h"
#include "sha256.h"
#include "cJSON.h"

int convert(int resultFile, char *inputContent, int inputSize, int bias)
{
	cJSON *root = cJSON_Parse(inputContent);
	cJSON *result = cJSON_CreateObject();
	if(!root || !result){
		fprintf(stderr,"cJSON fail...\n");
		return -1;
	}
	cJSON *inputjson = cJSON_GetObjectItem(root,"input");
	cJSON *outputjson = cJSON_GetObjectItem(root,"output");
	if(!inputjson || !outputjson){
		fprintf(stderr, "missing target...\n");
		return -1;
	}

	const char *inputstr = inputjson->valuestring;
	const char *outputstr = outputjson->valuestring;
	char buffer[STRSIZE];
	
	for(int i = 0; inputstr[i] != '\0'; ++i){
 		char c = inputstr[i];
 
 		if(c > 32){
 			c += bias;
 		}else if(c != ' ' && c != '\n'){
 			fprintf(stderr, "invalid character ...\n");
 			return -1;
 		}
 		
		buffer[i] = c;
	}

	cJSON *_input = cJSON_CreateString(buffer);
	if(!_input){
		fprintf(stderr, "cJSON error...\n");
		return -1;
	}
	cJSON_AddItemToObject(result,"input",_input);

	cJSON *_output = cJSON_CreateString(SHA256(outputstr));
	cJSON_AddItemToObject(result,"output",_output);
	
	char *resultStr = cJSON_Print(result);
	int k = write(resultFile,resultStr,strlen(resultStr));

	return 0;
}

int encode(int argc, char*argv[]){

	char resultPath[BUFSIZE], inputPath[BUFSIZE];

	if(argc < 3)
		return -1;

	strcpy(resultPath,argv[0]);
	strcpy(inputPath,argv[1]);
	
	int bias = atoi(argv[2]);
	int resultFile = open(resultPath, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU); // config
	int inputFile = open(inputPath, O_RDONLY);

	char inputStr[STRSIZE];

	read(inputFile, inputStr, STRSIZE);

	convert(resultFile, inputStr, STRSIZE, bias);

	close(inputFile);
	close(resultFile);

	return 0;
}
