#include <git2.h>
#include "diff.h"

int init(char home[], char repoName[], char path[])
{
	git_libgit2_init();
	git_repository *repo = NULL;
	char remoteAddress[PATHSIZE]; sprintf(remoteAddress,"git@%s:%s",home,repoName);
	int error = git_clone(&repo, remoteAddress, path,NULL);
	if (error < 0) {
		const git_error *e = git_error_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}
	git_repository_free(repo);
	return 0;
}
