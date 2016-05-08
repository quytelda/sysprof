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
    //sigfillset(&mask);
    //sigdelset(&mask, SIGRTMIN+1);
    //sigsuspend(&mask);

    // Get a file descriptor to the char device.
    int fd = open(FILEPATH, O_RDONLY|O_SYNC);
    if (fd < 0)
    {
	perror("Error opening file for reading.");
	exit(EXIT_FAILURE);
    }

    // memory map the contents
    void * map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
    if(!map)
    {
	fprintf(stderr, "Failed to mmap device file.\n");
	return -1;
    }

    struct nf_data * all = (struct nf_data *) map;
    //for(;;) {
	//Wait on signal
	//(void) sigsuspend(&mask);

	// extract data
	if(map + (index + 1) * sizeof(struct nf_data) > map + FILESIZE)
	    index = 0;
	struct nf_data current = all[index];
	index++;

	//Use data
	printf("pack in %u\npack out %u\n", current.pac_in, current.pac_out);
//    }

    return 0;
}
