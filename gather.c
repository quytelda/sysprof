#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "include/stats.h"

int main(){

	//Constants
	int PAGE_SIZE = getpagesize();
	size_t FILESIZE = PAGE_SIZE * 32;
	char * FILEPATH = "/dev/sysprof";
	sigset_t mask;
	int index = 0;

	//Setup to recieve signal from kernel module
	sigfillset(&mask);
	sigdelset(&mask, SIGRTMIN+1);
	sigsuspend(&mask);

	while (1) {
			//Wait on signal
		  //(void) sigsuspend(&mask);		

			//Extract data	
			int fd = open(FILEPATH, O_RDONLY);
			if (fd == -1) 
			{
				perror("Error opening file for reading");
				exit(EXIT_FAILURE);
			}
			void *map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
			struct nf_data *all = (struct nf_data*)map;
			if(map + index * sizeof(struct nf_data) > map + FILESIZE)
				index = 0;
			struct nf_data current = all[index];
			index++;

			//Use data
			printf("pack in %d\npack out %d", current.pac_in, current.pac_out);
	}

	return 0;
}
