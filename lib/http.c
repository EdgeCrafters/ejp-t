#include "http.h"

static char cookie[urlSize];

int login(char home[], char id[], char password[]){
	char url[urlSize], shaPW[shaSize];
	CURL *curl;
	CURLcode res;

	memset(url,0,urlSize);
	sprintf(url,"%s/users/login",home);

	memset(shaPW,0,shaSize);
	sprintf(shaPW,"%s",SHA256(password));

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
	}else{
		perror("Error on curl...\n");
	}
//	memset(cookie,0,512);
//	strcpy(cookie,home);
	return 0;
}

int logout(char home[], char id[], char password[]){
	printf("%s\n",cookie);
	return 0;
}

int initRepo(char home[]){
	return 0;
}
