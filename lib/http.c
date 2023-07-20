#include "http.h"

int login(char home[], char id[], char pw[]){
	char url[URLSIZE], payload[URLSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/auth/login",home);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		list = curl_slist_append(list, "Accept: */*");
		list = curl_slist_append(list, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		
		sprintf(payload,"{\"studentId\": \"%s\",\"password\":\"%s\"}",id,pw);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,payload);
		
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE,"./tmp/cookie.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR,"./tmp/cookie.txt");

		curl_easy_perform(curl);

		curl_easy_cleanup(curl);
	}else{
		perror("Error on curl...\n");
	}
	return 0;
}

int logout(char home[], char id[], char password[]){
	return 0;
}

int initRepo(char home[]){
	return 0;
}
