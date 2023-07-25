#include <git2.h>
#include "diff.h"

int diff(char* input, char*output, int bias)
{
	git_libgit2_init();
	git_repository *repo = NULL;
	int error = git_clone(&repo, "https://github.com/SKKU-EJS/ejs-be.git", "/home/skkusoynet/.ejs/ejs-be",NULL);
	if (error < 0) {
		const git_error *e = git_error_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}
	git_repository_free(repo);
	return strcmp(input, output);
}
