#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>

/*  
 *  Prototypes - this would normally go in a .h file
 */
 
int init_module(void);
void cleanup_module(void);
void getnstimeofday(struct timespec *ts);

struct timespec {
    time_t  tv_sec;     /* seconds */
    long    tv_nsec;    /* nanoseconds */
};

struct FS{
	int UID;
	int R;
	int W;
	int X;
}

struct NET{
	int pac_in;
	int pac_out;
	struct timespec timestamp;
}

struct proc_dir_entry *Our_Proc_File;

struct workqueue_struct *create_workqueue(const char *name);

struct FS fs{0,0,0,0};
struct NET net{0,0,/*how do you initialize a struct here??????*/};

int init_module(void)
{
	printk(KERN_INFO "Hello world 1.\n");

	/* create the /proc file */
	Our_Proc_File = create_proc_entry(PROC_ENTRY_FILENAME, 0644, NULL);
	
	if (Our_Proc_File == NULL) {
		remove_proc_entry(procfs_name, &proc_root);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
		       procfs_name);
		return -ENOMEM;
	}
	
	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

int do_awesome_stuff(){
	/*Collect data*/
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}



/*Queue stuff:

To submit a task to a workqueue, you need to fill in a work_struct structure:

DECLARE_WORK(name, void (*function)(void *), void *data);

where "name" is the name of the structure to be declared, "function" is the function that is to be called from the workqueue,
and data is a value to pass to that function. 
