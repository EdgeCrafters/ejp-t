#include "http.h"

char cookie[PATHSIZE];
static unsigned int writeidx;

static int parseInt(const char data[], const char property[])
{
    cJSON *root = cJSON_Parse(data);
    if(!root)
        return -1;
    
    cJSON *target = cJSON_GetObjectItem(root,property);
    if(!target)
        return -1;
    
    return target->valueint;
}

static size_t plainWrite(void *data, size_t size, size_t nmemb, void *clientp)
{
    if (clientp)
    {
        strncpy((char *)clientp + writeidx, (char *)data, strlen((char*)data));
        writeidx += strlen((char*)data);
    }
    return size * nmemb;
}

int loginHTTP(const char home[], const char id[], const char pw[])
{
    char url[URLSIZE], payload[URLSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/auth/login", home);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

        list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        sprintf(payload, "{\"username\": \"%s\",\"password\":\"%s\"}", id, pw);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK){
            fprintf(stderr,"Error on connection... (url : %s , error : %d)\n", url, res);
            return -1;
        }

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat != 201){
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
        return -1;
    }
    return 0;
}

int logoutHTTP(const char home[])
{
    char url[URLSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/auth/logout", home);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat != 201){
            fprintf(stderr, "Error on connection... %s\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }
    return 0;
}

int initRepoHTTP(const char home[], const char repoName[], char repoId[], size_t bufSize)
{
    char url[URLSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/repos", home);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char payload[BUFSIZE];
        sprintf(payload,"{\"repoName\":\"%s\"}",repoName);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat == 401) {
            curl_easy_cleanup(curl);
            userLogin(home);
            return initRepoHTTP(home,repoName,repoId,bufSize);
        } else if (stat != 201) {
            fprintf(stderr, "Error on connection... %s\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);

        sprintf(repoId,"%d",parseInt(response,"repoId"));
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int createProblemHTTP(const char home[], const char repoID[], char title[], char description[], char problemId[])
{
    char url[URLSIZE], payload[STRSIZE]; 
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/problem/%s", home, repoID);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        sprintf(payload, "{\"title\": \"%s\",\"text\":\"%s\"}", title, description);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return createProblemHTTP(home,repoID,title,description,problemId);
        }else if (stat != 201){
            fprintf(stderr, "Error on connection... %s\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);

        sprintf(problemId,"%d",parseInt(response,"id"));
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int updateProblemHTTP(const char home[], const char problemID[], char title[], char description[])
{
    char url[URLSIZE], payload[STRSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/problem/%s", home, problemID);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        if (title[0] && description[0])
            sprintf(payload, "{\"title\": \"%s\",\"text\":\"%s\"}", title, description);
        else if (title[0] && !description[0])
            sprintf(payload, "{\"title\": \"%s\"}", title);
        else if (!title[0] && description[0])
            sprintf(payload, "{\"text\":\"%s\"}", description);
        else
            sprintf(payload, "");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return updateProblemHTTP(home,problemID,title,description);
        }else if (stat != 200){
            fprintf(stderr, "Error on connection... (message : %s)\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int deleteRepoHTTP(const char home[], const char repoID[])
{
    char url[URLSIZE],payload[STRSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/repos/%s", home, repoID);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return deleteProblemHTTP(home,repoID);
        } else if (stat != 200){
            fprintf(stderr, "Error on connection... (message : %s)\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int deleteProblemHTTP(const char home[], const char problemID[])
{
    char url[URLSIZE],payload[STRSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/problem/%s", home, problemID);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return deleteProblemHTTP(home,problemID);
        } else if (stat != 200){
            fprintf(stderr, "Error on connection... (message : %s)\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int uploadHiddencasesHTTP(const char home[], const char repoID[], const char problemID[], const char input[], const char output[])
{

    char url[URLSIZE], payload[STRSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/testcase", home);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        sprintf(payload,
         "{\"repoId\": %s,\
         \"problemId\":%s,\
         \"input\":\"%s\",\
         \"output\":\"%s\",\
         \"isHidden\":\"HIDDEN\"\
         }",repoID,problemID,input,output);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return uploadHiddencasesHTTP(home,repoID,problemID,input,output);
        } else if (stat != 201){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;

}

int uploadFileHTTP(const char home[], const char problemID[], const char path[])
{
    char url[URLSIZE], payload[STRSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/repos/files/%s", home,problemID);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: multipart/form-data");
        list = curl_slist_append(list, "Accept-Encoding: gzip, deflate, br");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        curl_mime *form = NULL;
        curl_mimepart *field = NULL;

        form = curl_mime_init(curl);

        field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_filedata(field, path);
        curl_mime_type(field, "application/x-tar"); 

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return uploadFileHTTP(home,problemID,path);
        } else if (stat != 201){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int getReposHTTP(const char home[], char repoID[], cJSON **responseJson) {
	char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/repos/%s", home, repoID);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return getReposHTTP(home,repoID,responseJson);
        } 

        *responseJson = (cJSON*) cJSON_Parse(response);
		curl_easy_cleanup(curl);
	} else {
		return -1;
	}

	return 0;
}

int createUsersHTTP(const char home[], char usernames[][IDSIZE], char passwords[][PWSIZE], const int studentNum) 
{
	char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/user", home);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        cJSON *req = cJSON_CreateObject();
        cJSON *username = cJSON_CreateArray();
        cJSON *password = cJSON_CreateArray();
        for(int i = 0; i<studentNum; ++i){
            cJSON *_username = cJSON_CreateString(usernames[i]);
            cJSON_AddItemToArray(username,_username);
            cJSON *_password = cJSON_CreateString(passwords[i]);
            cJSON_AddItemToArray(password,_password);
        }
        cJSON_AddItemToObject(req,"username",username);
        cJSON_AddItemToObject(req,"password",password);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cJSON_Print(req));

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return createUsersHTTP(home,usernames,passwords,studentNum);
        } else if (stat != 201){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

		curl_easy_cleanup(curl);
	} else {
		return -1;
	}

	return 0;
}

int enrollUsersHTTP(const char home[], char usernames[][IDSIZE], char repoIDs[][IDSIZE], const int studentNum) 
{
	char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/user/enroll", home);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        cJSON *req = cJSON_CreateObject();
        cJSON *username = cJSON_CreateArray();
        cJSON *repoID = cJSON_CreateArray();
        for(int i = 0; i<studentNum; ++i){
            cJSON *_username = cJSON_CreateString(usernames[i]);
            cJSON_AddItemToArray(username,_username);
            cJSON *_repoID = cJSON_CreateString(repoIDs[i]);
            cJSON_AddItemToArray(repoID,_repoID);
        }
        cJSON_AddItemToObject(req,"username",username);
        cJSON_AddItemToObject(req,"repoId",repoID);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cJSON_Print(req));

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return createUsersHTTP(home,usernames,repoIDs,studentNum);
        } else if (stat != 201){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

		curl_easy_cleanup(curl);
	} else {
		return -1;
	}

	return 0;
}

int userProblemScoreHTTP(const char home[], const char repoID[], const char problemID[],const char userName[], cJSON **responseJson) 
{
	char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/submit/result/userName/%s/problemId/%s", home, userName, problemID);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return userProblemScoreHTTP(home,repoID,problemID,userName,responseJson);
        } else if (stat != 200){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

		curl_easy_cleanup(curl);

        *responseJson = cJSON_Parse(response);
	} else {
		return -1;
	}

	return 0;
}

int userRepoScoreHTTP(const char home[], const char repoID[],const char userName[], cJSON **responseJson) 
{
	char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/submit/result/all/username/%s/reopId/%s", home, userName, repoID);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return userRepoScoreHTTP(home,repoID,userName,responseJson);
        } else if (stat != 200){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

		curl_easy_cleanup(curl);

        *responseJson = cJSON_Parse(response);
	} else {
		return -1;
	}

	return 0;
}

int problemScoreHTTP(const char home[], const char repoID[],const char problemID[],cJSON **responseJson) 
{
	char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/submit/result/all/problemId/%s/reopId/%s", home, problemID, repoID);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return problemScoreHTTP(home,repoID,problemID,responseJson);
        } else if (stat != 200){
            fprintf(stderr, "Error on connection... %s at %s\n", response,url);
            return -1;
        }

		curl_easy_cleanup(curl);

        *responseJson = cJSON_Parse(response);
	} else {
		return -1;
	}

	return 0;
}

int deleteTestcasesHTTP(const char home[], const char repoID[], cJSON *testcases)
{
    char url[URLSIZE],payload[STRSIZE];
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    long stat;

    memset(url, 0, URLSIZE);
    sprintf(url, "%s/testcase/all/%s", home,repoID);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        sprintf(payload,
         "{\"id\":%s\
         }",cJSON_Print(testcases));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            return -1;

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if (stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return deleteTestcasesHTTP(home,testcases,repoID);
        } else if (stat != 200){
            fprintf(stderr, "Error on connection... (message : %s)\n", response);
            return -1;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error on curl...\n");
    }

    return 0;
}

int getAllReposHTTP(const char home[], cJSON **responseJson)
{
    char url[URLSIZE], response[BUFSIZE];
	CURL *curl;
	CURLcode res;
	struct curl_slist *list = NULL;
	long stat;

	memset(url, 0, URLSIZE);
	sprintf(url, "%s/repos", home);

	curl = curl_easy_init();

	if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_PORT, 4000L);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000L);

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
		list = curl_slist_append(list, "Accept: */*");
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        char response[BUFSIZE];
        writeidx = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, plainWrite);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			return -1;
		}
        
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &stat);
        if(stat == 401){
            curl_easy_cleanup(curl);
            userLogin(home);
            return getAllReposHTTP(home,responseJson);
        } 

        *responseJson = (cJSON*) cJSON_Parse(response);
		curl_easy_cleanup(curl);
	} else {
		return -1;
	}

	return 0;
}