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


	// Counters
	int index = 0;
	int newsample_counter = 0;


	//	Setup mask to recieve signal from kernel module
	sigfillset(&mask);
	sigdelset(&mask, SIGCONT);
	sigsuspend(&mask);


	//	Store pid for kernel to use
	FILE *ifp;
	char *mode = "r";
	ifp = fopen("/proc/sysprof", mode);
	int my_pid = getpid();
	fprintf(ifp, "R %d", my_pid);


	//	Open database connection
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open("gather.db", &db);
	if(rc){
  	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  	exit(EXIT_FAILURE);
  }


	//	Create table if it doesn't exist
	char * sql_create = malloc(256);
	sql_create = "CREATE TABLE IF NOT EXISTS NET_DATA("  \
         "ID INT PRIMARY KEY     NOT NULL," \
         "PAC_IN				INT," \
         "UDP_IN				INT," \
         "TCP_IN        INT," \
         "ICMP_IN				INT," \
         "OTHER_IN			INT," \
         "PAC_OUT				INT," \
         "UDP_OUT				INT," \
         "TCP_OUT				INT," \
         "ICMP_OUT			INT," \
         "OTHER_OUT			INT);";
  rc = sqlite3_exec(db, sql_create, NULL, 0, &zErrMsg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		exit(EXIT_FAILURE);
	}
  

	//	Get a file descriptor to the char device.
	int fd = open(FILEPATH, O_RDONLY|O_SYNC);
	if(fd < 0){
		fprintf(stderr, "Error opening file for reading.");
		exit(EXIT_FAILURE);
	}


	//	Memory map the shared memory area
	void * map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(!map){
		fprintf(stderr, "Failed to mmap device file.\n");
		return -1;
	}
	struct nf_data * shmem = (struct nf_data *)map;


	//	Loop and wait for signal
	for(;;){
		(void) sigsuspend(&mask);
		newsample_counter = newsample_counter + 1;
		if(newsample_counter >= 1440){
			//create new thread for: bootstrap()
			newsample_counter = newsample_counter % 1440;
		}


		//	Extract data
		if(map + (index + 1) * sizeof(struct nf_data) > map + FILESIZE)
			index = 0;
		struct nf_data current = shmem[index];
		index++;


		//	Insert newly collected data into database
		char * sql_add = malloc(512);
		snprintf(sql_add, 512, 
			"INSERT INTO NET_DATA (PAC_IN,UDP_IN,TCP_IN,ICMP_IN,OTHER_IN,PAC_OUT,UDP_OUT,TCP_OUT,ICMP_OUT,OTHER_OUT)"\
			" VALUES (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u);", current.pac_in, current.udp_in, current.tcp_in, 
			current.icmp_in, current.other_in, current.pac_out, current.udp_out, current.tcp_out, current.icmp_out, current.other_out);
		rc = sqlite3_exec(db, sql_add, NULL, 0, &zErrMsg);
		if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}


		// Get out parameters from sqlite
		// Check if current data is within parameters
			// Get cutoff from sqlite golden table
			// Compare sample to samplecutoff
			// Create signal if neccessary
		printf("pack in %u\npack out %u\n", current.pac_in, current.pac_out);
	}


	//	Clean up and exit
	sqlite3_close(db);
	munmap(map, FILESIZE);
	close(fd);
	return 0;
}
