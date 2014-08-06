#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ftw.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int walk(const char *, const struct stat *, int, struct FTW *);
int mycp(const char *, const char *);

char *dest;
int dest_type = 0;
int source_type = 0;
int recurse = 0;
int true_r = 0;

int main(int argc, char *argv[])
{
	char replace[1024] = "";
	char shaved1[1024] = "";
	char shaved2[1024] = "";
	struct stat st, st1;
	char *source;
	char foo = 0;
	int opts = -1;
	int len1, len2;

	while(foo != -1) {
		foo = getopt(argc, argv, "R");
		++opts;

		if(foo == 'R') recurse = 1;
	}
	
	if(argc != opts + 3) {
		fprintf(stderr, "mycp only supports one source and one destination :\\\n");
		exit(1);
	}

	umask(0000);

	if(stat(argv[opts + 1], &st) == -1) {
		fprintf(stderr, "mycp: cannot stat \"%s\": No such file or directory\n", argv[opts + 1]);
		exit(2);
	};
	
	if(S_ISDIR(st.st_mode))
		source_type = 1;

	if(S_ISDIR(st.st_mode)) {
		if(recurse == 0) {
			fprintf(stderr, "mycp: omitting directory \"%s\"\n", argv[opts + 1]);
			exit(3);
		}

		true_r = 1;
	}

	source = argv[opts + 1];
	dest = argv[opts + 2];

	strcat(shaved1, source);
	strcat(shaved2, dest);
	len1 = strlen(source);
	len2 = strlen(dest);
	if(shaved1[len1 - 1] == '/') shaved1[len1 - 1] = '\0';
	if(shaved2[len2 - 1] == '/') shaved2[len2 - 1] = '\0';

	if(strcmp(shaved1, shaved2) == 0) {
		fprintf(stderr, "mycp: \"%s\" and \"%s\" are the same file\n", source, dest);
		exit(4);
	}

	if(stat(dest, &st1) != -1) {
		if(S_ISDIR(st1.st_mode))
			dest_type = 1;
	} else dest_type = 1;

	if(dest_type == 0 && source_type == 1) {
		fprintf(stderr, "mycp: cannot overwrite non-directory \"%s\" with directory \"%s\"\n", dest, source);
		exit(4);
	}

	nftw(source, walk, 64, FTW_PHYS);

	return 0;
}

int walk(const char *source, const struct stat *sb, int flag, struct FTW *ftw)
{
	struct stat st;
	char final_dest[512] = "";
	char *trim = strstr(source, "/");

	strcat(final_dest, dest);
	stat(final_dest, &st);

	if(S_ISDIR(st.st_mode) && true_r && trim) strcat(final_dest, trim);
	if(S_ISDIR(sb->st_mode)) {
		mkdir(final_dest, sb->st_mode);
	} else {
		mycp(final_dest, source);
	}

	return 0;
}

int mycp(const char *to, const char *from)
{
	struct stat st;
	char buf[512];
	ssize_t nread;
	int fd_to, fd_from;

	stat(from, &st);

	fd_from = open(from, O_RDONLY);
	if(fd_from < 0) { fprintf(stderr, "mycp: cannot open \"%s\" for reading: %s\n", from, strerror(errno)); return -1; }

	fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
	if(fd_to < 0) { fprintf(stderr, "mycp: cannot open \"%s\" for writing: %s\n", from, strerror(errno)); goto error; }

	while(nread = read(fd_from, buf, sizeof(buf)), nread > 0) {
		char *out_ptr = buf;
		ssize_t writ;

		do {
			writ = write(fd_to, out_ptr, nread);

			if(writ >= 0) {
				nread -= writ;
				out_ptr += writ;
			} else if(errno != EINTR) {
				fprintf(stderr, "mycp: %s\n", strerror(errno));
				goto error;
			}
		} while(nread > 0);
	}

	if(nread == 0) {
		if(close(fd_to) < 0) {
			fd_to = -1;
			fprintf(stderr, "mycp: %s\n", strerror(errno));
			goto error;
		}

		close(fd_from);

		return 0;
	}

	error:

	close(fd_from);
	if(fd_to >= 0) close(fd_to);

	return -1;
}
