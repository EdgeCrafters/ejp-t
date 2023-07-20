#include "includes/sha256.h"
#include "includes/diff.h"
//#include <curl/curl.h>
#include "includes/http.h"

int main(int argc, char*argv[])
{
	char input[] = "2345";
	char output[] = "1234";
	char bias[] = "9";
	char *args[] = {"result.c","a.input","a.output","9"};
	gen(4,args);

//	char url_post[] = "https://google.com";
//	char url_get[] = "https://google.com";
//
//	CURL *curl;
//	CURLcode res;
//	curl = curl_easy_init();
//
//	struct curl_slist *list = NULL;
//
//	if(curl){
//		curl_easy_setopt(curl, CURLOPT_URL, url_get);
//		res = curl_easy_perform(curl);
//		curl_easy_cleanup(curl);
//	}

	login(argv[1],argv[2],argv[3]);

	return 0;
}
