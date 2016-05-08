#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "include/stats.h"

int main(){

	//size_t FILESIZE = sizeof(struct nf_data);
	int PAGE_SIZE = getpagesize();
	size_t FILESIZE = PAGE_SIZE * 32;
	char * FILEPATH = "/I/Dont/Know";
	sigset_t mask;
	int index = 0;

	sigfillset(&mask);
	sigdelset(&mask, SIGRTMIN+1);
	sigsuspend(&mask);

	while (1) {
		  (void) sigsuspend(&mask);			
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
			printf("pack in %d\npack out %d", current.pac_in, current.pac_out);
			index++;
	}

}
