#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

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

int reg_file_cp(const char *, const char *);
int my_cp(const char *, const struct stat *, int, struct FTW *);

int found_r = 0;
int true_r = 0;
char *dest_full = 0;

int main(int argc, char *argv[])
{
	if(argc < 3) return 1;

	struct stat st;
	char foo = 0;
	int opts = -1;

	while(foo != -1) {
		foo = getopt(argc, argv, "R");
		++opts;

		if(foo == 'R') found_r = 1;
	}

	if(argc != opts + 3) {
		fprintf(stderr, "mycp only supports one source and one destination\n");
		return 1;
	}

	stat(argv[opts + 1], &st);
	if(S_ISDIR(st.st_mode)) {
		if(found_r == 0) {
			printf("mycp: omitting directory '%s'\n", argv[opts + 1]);
			return 0;
		}

		true_r = 1;
	}

	dest_full = argv[opts + 2];
	nftw(argv[opts + 1], my_cp, 64, FTW_PHYS);

	return 0;
}

int my_cp(const char *src_full, const struct stat *sb, int flag, struct FTW *ftw)
{/*
	struct stat st_src;
	struct stat st_dest;
	char src_full_l[512] = "";
	char dest_full_l[512] = "";
	char src_root[512] = "";
	char dest_root[512] = "";
	char dest_final[512] = "";
	char FINALDESTINATION[512] = "";
	char *src_rel;
	char *dest_rel;
	char *ptr, *ptr2, *ptr3, *ptr4;
	int len1, len2;
	int i, e;

	strcat(src_full_l, src_full);
	strcat(dest_full_l, dest_full);

	// get paths of files relative to current directory
	len1 = strlen(src_full_l);
	len2 = strlen(dest_full_l);
	if(src_full_l[len1 - 1] == '/') src_full_l[len1 - 1] = '\0';
	if(dest_full_l[len2 - 1] == '/') dest_full_l[len2 - 1] = '\0';

	// get even more relative paths (name) of files
	src_rel = strrchr(src_full_l, '/');
	dest_rel = strrchr(dest_full_l, '/');
	if(src_rel) src_rel += 1; else src_rel = src_full_l;
	if(dest_rel) dest_rel += 1; else dest_rel = dest_full_l;

	// get relative root paths of files
	ptr = strstr(src_full_l, src_rel);
	for(i = 0, ptr2 = src_full_l; ptr2 != ptr; ++ptr2, ++i) src_root[i] = src_full_l[i]; src_root[i] = '\0';
	ptr = strstr(dest_full_l, dest_rel);
	for(i = 0, ptr2 = dest_full_l; ptr2 != ptr; ++ptr2, ++i) dest_root[i] = dest_full_l[i]; dest_root[i] = '\0';

	// get info on source file
	e = stat(src_full, &st_src);
	if(e) { printf("error returned (%i): %s\n", errno, strerror(errno)); exit(1); }
	e = stat(dest_full, &st_dest);
	if(e) { printf("dest: (%i): %s\n", errno, strerror(errno)); }

	ptr3 = strstr(src_full, "/");
	ptr4 = strstr(src_root, "/");

	if(S_ISDIR(st_src.st_mode)) {
		strcat(dest_final, dest_rel);
		if(ptr3) strcat(dest_final, ptr3);
	} else {
		if(S_ISDIR(st_dest.st_mode)) {
			strcat(dest_final, dest_root);
			if(dest_root[0] != '\0') strcat(dest_final, "/");
			strcat(dest_final, dest_rel);
			if(ptr3) strcat(dest_final, "/");
			strcat(dest_final, src_rel);
		} else {
			strcat(dest_final, dest_root);
			//if(ptr3) strcat(dest_final, "/");
			////if(S_ISDIR(st_src.st_mode)) strcat(dest_final, dest_rel);
			if(ptr3) strcat(dest_final, ptr3);
			if(ptr3) strcat(dest_final, "/");
			strcat(dest_final, dest_rel);
		}
	}

	printf("src_full:       %s\n", src_full_l);
	printf("src_root:       %s\n", src_root);
	printf("src_rel:        %s\n", src_rel);
	printf("dest_full:      %s\n", dest_full_l);
	printf("dest_root:      %s\n", dest_root);
	printf("dest_rel:       %s\n", dest_rel);
	printf("dest_final:     %s\n", dest_final);
	printf("\n");

	if(S_ISREG(st_src.st_mode)) {
		//if(true_r) reg_file_cp(dest_final, src_full_l);
		if(true_r) strcat(FINALDESTINATION, dest_rel);
		if(true_r) strcat(FINALDESTINATION, ptr4);
		if(true_r) strcat(FINALDESTINATION, src_rel); else strcat(FINALDESTINATION, dest_rel);
		printf("%s\n\n", FINALDESTINATION);
		reg_file_cp(FINALDESTINATION, src_full_l);
	} else if(S_ISDIR(st_src.st_mode))
		mkdir(dest_final, st_src.st_mode);
		//mkdir(new, S_IRWXU);
*//*
	// if source is directory
	if(S_ISDIR(st_src.st_mode)) {
		// scrap relative root 
		rel_ptr = strstr(src_full, "/");
		// the fuck am i doing
		//if(rel_ptr) strcat(src_full, rel_ptr);
		//printf("2: %s\n", new);
	}

	//printf("%s\n%s\n", path, new);
	//printf("in my_cp: %s - %o\n", path, st.st_mode);

	// 
	if(stat(dest_full, &st_dest) && S_ISDIR(st_dest.st_mode)) strcat(dest_full, rel_ptr);
	if(S_ISREG(st_src.st_mode))
		reg_file_cp(dest_full, src_full);
	else if(S_ISDIR(st_src.st_mode))
		mkdir(dest_full, st_src.st_mode);
		//mkdir(new, S_IRWXU);
*/

	struct stat st;
	char new[512] = "";
	char *sub;

	strcat(new, dest_full);
	stat(src_full, &st);

	sub = strstr(src_full, "/");
	if(sub) strcat(new, sub);

	//printf("in my_cp: %s - %o\n", path, st.st_mode);

	if(S_ISREG(st.st_mode))
		reg_file_cp(new, src_full);
	else if(S_ISDIR(st.st_mode))
		mkdir(new, st.st_mode);
		//mkdir(new, S_IRWXU);

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
