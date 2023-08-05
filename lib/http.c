#include "http.h"

struct cookie session = {.isStore = 0};
unsigned int writeidx;

static size_t plainWrite(void *data, size_t size, size_t nmemb, void *clientp)
{
	if(clientp){
		strncpy((char*)clientp + writeidx, (char*)data, nmemb);
		writeidx += nmemb;
	}
	return size*nmemb;
}

size_t storeCookie(char *buffer, size_t size, size_t nitems, void *userdata)
{
	if(!strncmp(buffer,"Set-Cookie",10)){
		int i;
		for(i = 0; buffer[i]!=';'; ++i)
			;
		buffer[i] = '\0';
		sprintf(session.data,"%s",buffer+12);
		session.isStore = 1;
	}
	return nitems * size;
}

int loginHTTP(const char home[], const char id[], const char pw[])
{
	char url[URLSIZE], payload[URLSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/auth/login",home);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);  
		curl_easy_setopt(curl, CURLOPT_PORT, 4000L);
		
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

		list = curl_slist_append(list,"Accept: */*");
		list = curl_slist_append(list,"Content-Type: application/json");
		curl_easy_setopt(curl,CURLOPT_HTTPHEADER,list);

		sprintf(payload,"{\"username\": \"%s\",\"password\":\"%s\"}",id,pw);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,payload);
		
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, storeCookie);
		
		writeidx = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			return -1;

		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
		if(stat != 201)
			return -1;

		curl_easy_cleanup(curl);
	}else{
		fprintf(stderr,"Error on curl...\n");
		return -1;
	}
	return 0;
}

int logoutHTTP(const char home[])
{
	char url[URLSIZE],cookie[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/auth/logout",home);

	memset(cookie,0,BUFSIZE);
	sprintf(cookie,"Cookie: %s",session.data);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_PORT, 4000L);
		
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

		list = curl_slist_append(list, cookie);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,"");

		writeidx = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			return -1;
		
		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
		if(stat != 201)
			return -1;

		curl_easy_cleanup(curl);
	}else{
		fprintf(stderr,"Error on curl...\n");
	}
	return 0;
}

int initRepoHTTP(const char home[], const char repoName[], char buffer[], size_t bufSize)
{
	char url[URLSIZE],cookie[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/repos/%s",home,repoName);

	memset(cookie,0,BUFSIZE);
	sprintf(cookie,"Cookie: %s",session.data);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);		
		curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

		list = curl_slist_append(list,cookie);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,"");
		
		writeidx = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			return -1;

		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
		if(stat != 201)
			return -1;

		curl_easy_cleanup(curl);

	}else{
		fprintf(stderr,"Error on curl...\n");
	}

	return 0;
}

int createProblemHTTP(const char home[],const char repoID[],char title[],char description[],char buffer[])
{
	char url[URLSIZE],cookie[BUFSIZE],payload[STRSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/problem/%s",home,repoID);

	memset(cookie,0,BUFSIZE);
	sprintf(cookie,"Cookie: %s",session.data);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

		list = curl_slist_append(list,cookie);
		list = curl_slist_append(list, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		sprintf(payload,"{\"title\": \"%s\",\"text\":\"%s\"}",title,description);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,payload);
		
		writeidx = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			return -1;

		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
		if(stat != 201)
			return -1;

		curl_easy_cleanup(curl);

	}else{
		fprintf(stderr,"Error on curl...\n");
	}

	return 0;
}

int updateProblemHTTP(const char home[],const char problemID[],char title[],char description[])
{
	char url[URLSIZE],cookie[BUFSIZE],payload[STRSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/problem/%s",home,problemID);

	memset(cookie,0,BUFSIZE);
	sprintf(cookie,"Cookie: %s",session.data);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

		list = curl_slist_append(list,cookie);
		list = curl_slist_append(list, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		if(title[0] && description[0])
			sprintf(payload,"{\"title\": \"%s\",\"text\":\"%s\"}",title,description);
		else if(title[0] && !description[0])
			sprintf(payload,"{\"title\": \"%s\"}",title);
		else if(!title[0] && description[0])
			sprintf(payload,"{\"text\":\"%s\"}",description);
		else
			sprintf(payload,"");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,payload);
		
		writeidx = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			return -1;

		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
		if(stat != 200)
			return -1;

		curl_easy_cleanup(curl);

	}else{
		fprintf(stderr,"Error on curl...\n");
	}

	return 0;
}

int deleteProblemHTTP(const char home[],const char problemID[])
{
	char url[URLSIZE],cookie[BUFSIZE],payload[STRSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url,0,URLSIZE);
	sprintf(url,"%s/problem/%s",home,problemID);

	memset(cookie,0,BUFSIZE);
	sprintf(cookie,"Cookie: %s",session.data);

	curl = curl_easy_init();

	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

		list = curl_slist_append(list,cookie);
		list = curl_slist_append(list, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);


		writeidx = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			return -1;

		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
		if(stat != 200)
			return -1;

		curl_easy_cleanup(curl);

	}else{
		fprintf(stderr,"Error on curl...\n");
	}

	return 0;
}
