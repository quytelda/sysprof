#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../include/stats.h"

int main(){

	//	Constants
	int MAX_BUF = 3072;
	char buf[MAX_BUF];
	
	//	Set up database connection
	/*sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open("gather.db", &db);
	if( rc ){
  	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  	exit(0);
  }*/
  
	//	Set up pipes
	int fd_cpu;
	int fd_disk;
	int fd_mem;
	fd_cpu = open("../IO/CPU_pipe", O_RDONLY );
	fd_disk = open("../IO/disk_pipe", O_RDONLY );
	fd_mem = open("../IO/memory_pipe", O_RDONLY );

	//	Loop and wait on pipes
	for(;;) 
	{
		while(read(fd_cpu, buf, MAX_BUF ) > 0 )
			printf("Received cpu: %s\n", buf);
		while(read(fd_disk, buf, MAX_BUF ) > 0 )
			printf("Received disk: %s\n", buf);
		while(read(fd_mem, buf, MAX_BUF ) > 0 )
			printf("Received memory: %s\n", buf);

		//	Use data
	}

	//	lean up and exit
	close(fd_cpu);
	close(fd_disk);
	close(fd_mem);
	//sqlite3_close(db);
	return 0;
}
