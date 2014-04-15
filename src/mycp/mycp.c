// to do:
//  support multiple targets
//   pass argc and argv to function instead of the laughable thing i'm doing now
//  un-plagiarize cp especially considering i already wrote it myself
//  give non-shitty names to things
//  become a gangster and sell drugs

#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ftw.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

int reg_file_cp(const char *, const char *);
int my_cp(const char *, const struct stat *, int, struct FTW *);

char *dest;

int main(int argc, char *argv[])
{
	if(argc != 3) return 1;

	dest = argv[2];
	nftw(argv[1], my_cp, 64, FTW_PHYS);

	return 0;
}

int my_cp(const char *path, const struct stat *sb, int flag, struct FTW *ftw)
{
	struct stat st;
	char new[512] = "";
	char *sub;

	strcat(new, dest);
	stat(path, &st);

	sub = strstr(path, "/");
	if(sub) strcat(new, sub);

	//printf("in my_cp: %s - %o\n", path, st.st_mode);

	if(S_ISREG(st.st_mode))
		reg_file_cp(new, path);
	else if(S_ISDIR(st.st_mode))
		//mkdir(new, S_IRWXU);
		mkdir(new, st.st_mode);

	return 0;
}

int reg_file_cp(const char *to, const char *from)
{
	int fd_to, fd_from;
	char buf[4096];
	ssize_t nread;
	int saved_errno;
	struct stat st;

	stat(from, &st);

	fd_from = open(from, O_RDONLY);
	if(fd_from < 0) return -1;

	//printf("in reg_file_cp: %s - %o\n", to, st.st_mode);
	
	//fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, st.st_mode);
	if(fd_to < 0) goto out_error;

	while (nread = read(fd_from, buf, sizeof buf), nread > 0)
	{
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			nwritten = write(fd_to, out_ptr, nread);

			if (nwritten >= 0)
			{
				nread -= nwritten;
				out_ptr += nwritten;
			}
			else if (errno != EINTR)
			{
				goto out_error;
			}
		} while (nread > 0);
	}

	if (nread == 0)
	{
		if (close(fd_to) < 0)
		{
			fd_to = -1;
			goto out_error;
		}
		close(fd_from);

		return 0;
	}

	out_error:
	saved_errno = errno;

	close(fd_from);
	if (fd_to >= 0) close(fd_to);

	errno = saved_errno;
	return -1;
}
