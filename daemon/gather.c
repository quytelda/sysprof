#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "../include/stats.h"

int main(){

	//	Constants
	int PAGE_SIZE = getpagesize();
	size_t FILESIZE = PAGE_SIZE * 32;
	char * FILEPATH = "/dev/sysprof";
	sigset_t mask;
	int index = 0;

	//	Setup to recieve signal from kernel module
	sigfillset(&mask);
	sigdelset(&mask, SIGCONT);
	sigsuspend(&mask);

	//	Set up database connection
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open("gather.db", &db);
	if( rc ){
  	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  	exit(0);
  }
  

	//	Get a file descriptor to the char device.
	int fd = open(FILEPATH, O_RDONLY|O_SYNC);
	if (fd < 0)
	{
		perror("Error opening file for reading.");
		exit(EXIT_FAILURE);
	}

	//	Memory map the shared memory area
	void * map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(!map)
	{
		fprintf(stderr, "Failed to mmap device file.\n");
		return -1;
	}
	struct nf_data * all = (struct nf_data *)map;

	//	Loop and wait for signal
	for(;;) 
	{
		(void) sigsuspend(&mask);

		//	Extract data
		if(map + (index + 1) * sizeof(struct nf_data) > map + FILESIZE)
			index = 0;
		struct nf_data current = all[index];
		index++;

		//	Use data
		printf("pack in %u\npack out %u\n", current.pac_in, current.pac_out);
	}

	//	Clean up and exit
	sqlite3_close(db);
	munmap(map, FILESIZE);
	close(fd);
	return 0;
}
