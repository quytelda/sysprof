#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/timeb.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
//#include "tlpi_hdr.h"

//using namespace std;

int files_accessed = 0, permissions_changed = 0, files_modified = 0, files_opened = 0;
//General activity statistics

int ACCESS_ALERT = 0, PERM_CHANGE_ALERT = 0, MODIFY_ALERT = 0, READ_ALERT = 0;
//Sensitive activity statistics

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

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
	if (i->mask & IN_ACCESS)	       //File was accessed
		files_accessed++;
	
	if (i->mask & IN_ATTRIB)		//Permissions were changed
		permissions_changed++;
	
	if (i->mask & IN_MODIFY)		//File was modified
		files_modified++;
	
	if (i->mask & IN_OPEN)			//File was opened
		files_opened++;
	
}

static void		//Specific event handler for editing of the etc/shadow file (password hashes)
analyzeInotifyEvent_shadow(struct inotify_event *i)
{
	/*if (i->mask & IN_ACCESS){					//Nobody but root should access this
		printf("etc/shadow was accessed\n");
		ACCESS_ALERT++;
		//Any way to tell the permissions or identify of the user who committed the action????????
			//I.E. if a daemon user accessed shadow
	}*/
	
	if (i->mask & IN_ATTRIB){
		printf("etc/shadow had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printf("etc/shadow was modified!\n");
		MODIFY_ALERT++;
	}
}

static void		//Specific event handler for editing of the etc/ssh/ssh_config file
analyzeInotifyEvent_ssh_config(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){
		printf("etc/ssh/ssh_config had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printf("etc/ssh/ssh_config was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_sudoers(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printf("etc/sudoers had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	//Changing this is VERY delicate. Perhaps add 2 to modify alert
		printf("etc/sudoers was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_passwd(struct inotify_event *i)
{
	/*if (i->mask & IN_ACCESS){
		printf("etc/passwd was accessed\n");
		ACCESS_ALERT++;
		//Any way to tell the permissions or identify of the user who committed the action????????
			//I.E. if a daemon user accessed shadow
	}*/
	
	if (i->mask & IN_ATTRIB){
		printf("etc/passwd had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printf("etc/passwd was modified!\n");
		MODIFY_ALERT++;
	}
	
	
}

static void
analyzeInotifyEvent_group(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printf("etc/group had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printf("etc/group was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_hosts(struct inotify_event *i)
{

	/*if (i->mask & IN_ACCESS){
		printf("etc/hosts was accessed\n");
		ACCESS_ALERT++;
	}*/

	if (i->mask & IN_ATTRIB){
		printf("etc/host had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printf("etc/host was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_host_allow(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printf("etc/host.allow had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printf("etc/host.allow was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_host_deny(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printf("etc/host_deny had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printf("etc/host.deny was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_securetty(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printf("etc/securetty had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printf("etc/securetty was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_sources_list(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printf("etc/sources_list had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printf("etc/sources_list was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_crontab(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printf("etc/crontab had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printf("etc/crontab was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_bashrc(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printf(".bashrc had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printf(".bashrc was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_bash_aliases(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printf(".bash_aliases had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printf(".bash_asiases was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_id_rsa(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS){
		printf("ssh private keys were accessed\n");
		ACCESS_ALERT++;
	}
	
	if (i->mask & IN_ATTRIB){		
		printf("ssh private keys had their permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printf("ssh private keys were modified!\n");
		MODIFY_ALERT++;
	}
}

//Somehow call this every time file activity is recorded????????????????????? Or every time a certain time interval passes??????
void analyze_activity(int inotifyFd, char buf[BUF_LEN]){

        ssize_t numRead = read(inotifyFd, buf, BUF_LEN);	//Finds out how many file activity events have been detected		
        struct inotify_event *event;

	if (numRead == 0){
            printf("read() from inotify fd returned 0!\n");
	    return;
	}

        if (numRead == -1){
            printf("read");
	    return;
	}

        //printf("Read %ld bytes from inotify fd\n", (long) numRead);

        /* Process all of the events in buffer returned by read() */

	char * p = buf;

        while(p < buf + numRead) {		//Print out the event details 1 event at a time
            event = (struct inotify_event *) p;
            //displayInotifyEvent(event);

	    //printf("Event for %s\n", event->name);

	    if(strcmp(event->name, "shadow") == 0)	//Password hashes
	    	analyzeInotifyEvent_shadow(event);	
			
	    else if (strcmp(event->name, "ssh_config") == 0)	
	       analyzeInotifyEvent_ssh_config(event);
			
	    else if (strcmp(event->name, "sudoers") == 0)	//Who can sudo
	       analyzeInotifyEvent_sudoers(event);

      	    //else if (strcmp(event->name, "passwd") == 0)	//Information regarding registered system users
      		//analyzeInotifyEvent_passwd(event);
      		
      	    else if (strcmp(event->name, "group") == 0)		//Information regarding security group definitions
      		analyzeInotifyEvent_group(event);
      	
      	    else if (strcmp(event->name, "securetty") == 0)	//List of terminals where root can login
      		analyzeInotifyEvent_securetty(event);	

            //else if (strcmp(event->name, "hosts") == 0)	//Contains a list of hosts used for name resolution
	      //  analyzeInotifyEvent_hosts(event);
      		
      	    else if (strcmp(event->name, "hosts.allow") == 0)	//Contains a list of hosts allowed to access services
	        analyzeInotifyEvent_host_allow(event);	
	
	    else if (strcmp(event->name, "hosts.deny") == 0)   //Contains a list of hosts forbidden to access services
	      	analyzeInotifyEvent_host_deny(event);
	    
	    else if (strcmp(event->name, "sources.list") == 0)   //List of sources accessed during updates
	      	analyzeInotifyEvent_sources_list(event);
	      	
	    else if (strcmp(event->name, "crontab") == 0)   //Contains the list of cron jobs
	      	analyzeInotifyEvent_crontab(event);
	      	
	    else if (strcmp(event->name, ".bashrc") == 0)   //Shell script that Bash runs whenever it is started interactively
	      	analyzeInotifyEvent_bashrc(event);
	      	
	    else if (strcmp(event->name, ".bash_aliases") == 0)   //Stores command aliases
	      	analyzeInotifyEvent_bash_aliases(event);
	      	
	    else if ((strcmp(event->name, "id_rsa") == 0) || (strcmp(event->name, "authorized_key") == 0)
	    		|| (strcmp(event->name, "authorized_key2") == 0))   //ssh private keys
	      	analyzeInotifyEvent_id_rsa(event);
			
	    //The name field is present only when an event is returned for a file inside a watched directory
				
	    analyzeInotifyEvent(event);		//General analysis
			
            p += sizeof(struct inotify_event) + event->len;
        }
}

//Call inotifyevent with "etc" and "etc/ssh" as directories to add to the watchlist
int
main(int watch_list_size, char *files_and_directories[])		//Sets up the watchlist
{
    /*int i;

    printf("watch_list_size is %x\n", watch_list_size);

    for(i = 0; i < watch_list_size; i++){
	printf("%s\n", files_and_directories[i]);
    }*/

    int inotifyFd, wd, j;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    //struct inotify_event *event;

    if (watch_list_size < 2 || strcmp(files_and_directories[1], "--help") == 0){
        printf("pathname...\n");
	return -1;
    }

    inotifyFd = inotify_init();   /* Create an inotify instance and return a file 
									descriptor referring to the inotify instance */
    if (inotifyFd == -1){
        printf("inotify_init\n");
	return -1;
    }

    for (j = 1; j < watch_list_size; j++) {	//Enter all of the input pathnames into the watchlist
        wd = inotify_add_watch(inotifyFd, files_and_directories[j], IN_ALL_EVENTS);
	/*manipulates the "watch list" associated with an inotify instance. Each item ("watch") in the watch list specifies the 
	pathname of a file or directory, along with a set of events that the kernel should monitor for the file/directory*/
		  
	/*The IN_ALL_EVENTS macro is defined as a bit mask of all of the above events*/
		
        if (wd == -1){
            printf("inotify_add_watch\n");
	    return -1;
	}

        printf("Watching %s using wd %d\n", files_and_directories[j], wd);
    }

    struct timeb current, timestamp;

    ftime(&timestamp);

    while(1){

        //sleep(15);

        analyze_activity(inotifyFd, buf);

        ftime(&current);

        if(current.time == timestamp.time + 60){

                timestamp.time = current.time;

                printf("files accessed: %i, attributes changed: %i, files modified %i, files opened %i\n", files_accessed, permissions_changed, files_modified, files_opened);
                printf("access alerts: %i, attribute alerts: %i, modify alerts %i, read alerts %i\n", ACCESS_ALERT, PERM_CHANGE_ALERT, MODIFY_ALERT, READ_ALERT);

                files_accessed = 0;
                permissions_changed = 0;
                files_modified = 0;
                files_opened = 0;

                ACCESS_ALERT = 0;
                PERM_CHANGE_ALERT = 0;
                MODIFY_ALERT = 0;
                READ_ALERT = 0;
        }
    }


    return 0;
	
}
