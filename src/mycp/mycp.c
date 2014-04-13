/********************************************************
 * SO THIS IS ALREADY A HUGE AND PROBABLY-UNUSABLE MESS	*
 * AND THERE'S PROBABLY SUCH AN EASIER WAY TO DO IT		*
 * I'M LIKELY TO THROW IT ALL AWAY RIGHT THE FUCK NOW	*
 * AND JUST USE SCANDIR OR SOME SHIT					*
 * DID I MENTION PROGRAMMING IS TEH GAY					*
 ********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUF_SIZE 512

/* hideous wrapper functions */
int get_type(mode_t);
int open_reg_file(char *, int, mode_t);
int read_from_file(int, char *);
int write_to_file(int, char *, int);

int main(int argc, char *argv[])
{
	/* validate command line arguments */
	if(argc < 2) {
		fprintf(stderr, "MyCP usage: mycp [-r] target destination\n");
		exit(1);
	}
	
	/* file permissions */
	const mode_t nf_mode = S_IRUSR | S_IWUSR;
	const mode_t nd_mode = S_IRUSR | S_IWUSR | S_IXUSR;
	/* open modes */
	const int tf_flag = O_RDONLY;
	const int nf_flag = O_RDWR | O_CREAT | O_TRUNC;
	/* oops they're not capitalized god damnit */

	/* pointers to target and destination strings */
	/* oh my god it's already so not scalable whatever i'll change it later */
	char *tar_str = argv[1];
	char *dest_str = argv[2];
	/* file descriptor containers for target and destination */
	/* i should really be referring to 'target' as 'source'. oops. */
	int tar_fd, dest_fd;
	/* buffer, counter for reading from target */
	char wr_buff[BUF_SIZE];
	int tf_bytes_read, df_bytes_writ;

	/* stat the file so i know what the fuck i'm looking at */
	struct stat tar_stat;
	int tar_type;
	int status = stat(tar_str, &tar_stat);
	if(status == -1) {
		printf("%d: %s\n", errno, strerror(errno));
		exit(3);
	}

	/* do stuff depending on target type */
	tar_type = get_type(tar_stat.st_mode);
	if(tar_type == 1) {
		/* oh good it's a regular ass file */
		tar_fd = open_reg_file(tar_str, tf_flag, 0);
		dest_fd = open_reg_file(dest_str, nf_flag, nf_mode);
		tf_bytes_read = read_from_file(tar_fd, wr_buff);
		df_bytes_writ = write(dest_fd, wr_buff, tf_bytes_read);
	} else if(tar_type == 2) {
		/* target is directory */
		printf("NYI\n");
	} else if(tar_type == 3) {
		/* target is link */
		printf("NYI\n");
	} else {
		fprintf(stderr, "what is this i dont even\n");
		exit(9001);
	}

	return 0;
}

/*****************************************
 * determine type of file i'm looking at *
 *---------------------------------------*
 * 1 -> regular							 *
 * 2 -> directory						 *
 * 3 -> link							 *	
 *****************************************/
int get_type(mode_t mode)
{
	if(S_ISREG(mode)) return 1;
	if(S_ISDIR(mode)) return 2;
	if(S_ISLNK(mode)) return 3;

	return 0;
}

/* create a file descriptor */
int open_reg_file(char *file, int flag, mode_t mode)
{
	int fd = open(file, flag, mode);
	
	if(fd == -1) {
		fprintf(stderr, "Was unable to open file '%s'\n", file);
		fprintf(stderr, "Returned error %d: %s\n", errno, strerror(errno));
		
		exit(2);
	}

	return fd;
}

/* read from target regular file */
int read_from_file(int fd, char *buff)
{
	int bytes_read = read(fd, buff, BUF_SIZE);
	
	if(bytes_read == -1) {
		fprintf(stderr, "Was unable to read from descriptor %d\n", fd);
		fprintf(stderr, "Returned error %d: %s\n", errno, strerror(errno));
		
		exit(4);
	}

	return bytes_read;
}

/* write to destination regular file */
int write_to_file(int fd, char *buff, int bytes)
{
	int bytes_writ = write(fd, buff, bytes);

	if(bytes_writ == -1){
		fprintf(stderr, "Was unable to write to descriptor %d\n", fd);
		fprintf(stderr, "Returned error %d: %s\n", errno, strerror(errno));
		
		exit(4);
	}

	return bytes_writ;
}
