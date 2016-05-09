#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../include/stats.h"

int main(){

	//Constants
	int MAX_BUF = 1024;
	char buf[MAX_BUF];
	
	//Set up database connection
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open("gather.db", &db);
	if( rc ){
  	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  	exit(0);
  }
  
	//Set up pipes
	int fd_cpu;
	int fd_disk;
	int fd_mem;
	fd_cpu = open("CPU_pipe", O_RDONLY );
	fd_disk = open("disk_pipe", O_RDONLY );
	fd_mem = open("memory_pipe", O_RDONLY );

	//Loop and wait on pipes
	for(;;) 
	{
		if(read(fd_cpu, buf, MAX_BUF ) > 0 )
			printf("Received: %s\n", buf);
		if(read(fd_disk, buf, MAX_BUF ) > 0 )
			printf("Received: %s\n", buf);
		if(read(fd_mem, buf, MAX_BUF ) > 0 )
			printf("Received: %s\n", buf);

		//Use data
	}

	//Clean up and exit
	close(fd_cpu);
	close(fd_disk);
	close(fd_mem);
	sqlite3_close(db);
	return 0;
}
