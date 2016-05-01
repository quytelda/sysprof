#include <sys/inotify.h>
#include <limits.h>
#include "tlpi_hdr.h"

int files_accessed = 0, permissions_changed = 0, files_modified = 0, files_opened = 0;
//General activity statistics

int ACCESS_ALERT = 0, PERM_CHANGE_ALERT = 0, MODIFY_ALERT = 0, READ_ALERT;
//Sensitive activity statistics

struct inotify_event {
	__s32		wd;		/* watch descriptor */
	__u32		mask;		/* watch mask */
	__u32		cookie;		/* cookie to synchronize two events */
	__u32		len;		/* length (including nulls) of name */
	char		name[];	/* stub for possible name */
};

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

static void
analyzeInotifyEvent(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS);        //File was accessed
		files_accessed++;
	
	if (i->mask & IN_ATTRIB);		//Permissions were changed
		permissions_changed++;
	
	if (i->mask & IN_MODIFY);		//File was modified
		files_modified++;
	
	if (i->mask & IN_OPEN);			//File was opened
		files_opened++;
	
}

static void		//Specific event handler for editing of the etc/shadow file (password hashes)
analyzeInotifyEvent_shadow(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS){					//Nobody but root should access this
		printk(KERN_INFO "etc/shadow was accessed\n");
		ACCESS_ALERT++;
		//Any way to tell the permissions or identify of the user who committed the action????????
			//I.E. if a daemon user accessed shadow
	}
	
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/shadow had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printk(KERN_INFO "etc/shadow was modified!\n");
		MODIFY_ALERT++;
	}
}

static void		//Specific event handler for editing of the etc/ssh/ssh_config file
analyzeInotifyEvent_ssh_config(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/ssh/ssh_config had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printk(KERN_INFO "etc/ssh/ssh_config was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_sudoers(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO "etc/sudoers had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	//Changing this is VERY delicate. Perhaps add 2 to modify alert
		printk(KERN_INFO "etc/sudoers was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_passwd(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS){
		printk(KERN_INFO "etc/passwd was accessed\n");
		ACCESS_ALERT++;
		//Any way to tell the permissions or identify of the user who committed the action????????
			//I.E. if a daemon user accessed shadow
	}
	
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/passwd had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printk(KERN_INFO "etc/passwd was modified!\n");
		MODIFY_ALERT++;
	}
}

//Call inotifyevent with "etc" and "etc/ssh" as directories to add to the watchlist

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int
inotify(int watch_list_size, char *files_and_directories[])
{
    int inotifyFd, wd, j;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (watch_list_size < 2 || strcmp(files_and_directories[1], "--help") == 0)
        usageErr("%s pathname...\n", argv[0]);

    inotifyFd = inotify_init();   /* Create an inotify instance and return a file 
									descriptor referring to the inotify instance */
    if (inotifyFd == -1)
        errExit("inotify_init");

    for (j = 1; j < watch_list_size; j++) {	//Enter all of the input pathnames into the watchlist
        wd = inotify_add_watch(inotifyFd, files_and_directories[j], IN_ALL_EVENTS);
		/*manipulates the "watch list" associated with an inotify instance. Each item ("watch") in the watch list
          specifies the pathname of a file or directory, along with some set of events that the kernel should monitor
		  for the file referred to by that pathname*/
		  
		/*The IN_ALL_EVENTS macro is defined as a bit mask of all of the above events.  This macro can be used as the
		mask argument when calling inotify_add_watch(2)*/
		
        if (wd == -1)
            errExit("inotify_add_watch");

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    for (;;) {                                  /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);			
		//read(fd, buf, count); 					Generates IN_ACCESS events for both dir and dir/myfile.
        
		if (numRead == 0)
            fatal("read() from inotify fd returned 0!");

        if (numRead == -1)
            errExit("read");

        printf("Read %ld bytes from inotify fd\n", (long) numRead);

        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead; ) {		//Print out the event details 1 event at a time
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);
			
			//So far, we're analyzing /etc/shadow, /etc/ssh/ssh_config, /etc/sudoers, and /etc/passwd

			
			if(strcmp(event->name, "shadow") == 0)	//Have a different function for each file/directory we're focusing on
				anaylyzeInotifyEvent_shadow(event);	
			
			else if (strcmp(event->name, "ssh_config") == 0)
				analyzeInotifyEvent_ssh_config(event);
			
			else if (strcmp(event->name, "sudoers") == 0)
				analyzeInotifyEvent_sudoers(event);

			else if (strcmp(event->name, "passwd") == 0)
				analyzeInotifyEvent_passwd(event);
			
			//The name field is present only when an event is returned for a file inside a watched directory
				
			anaylyzeInotifyEvent(event);		//General analysis
			
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}
