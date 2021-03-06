/*
 * sysprof.c - Linux Statistical System Profiler
 * This is the core of the sysprof kernel module.
 *
 * Copyright (C) 2016 Quytelda Kahja, Roger Xiao
 * This file is part of Sysprof.
 *
 * Sysprof is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sysprof is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Sysprof.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#include "shmem.h"
#include "register.h"
#include "data/netfilter.h"

MODULE_AUTHOR("Quytelda Kahja");
MODULE_AUTHOR("Roger Xiao");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Statistical System Profiler");

#define PROC_ENTRY_FILENAME "sysprof"
#define PROC_BUFSIZE 10 // large enough for 64bit max_pid

/**
 * This function is called when a program attempts to read the entry in /proc.
 * Whatever data we wish to send to the stream should be copied to @buf in userspace.
 */
static ssize_t sysprof_read(struct file * file, char __user * buf,
			    size_t length, loff_t * offset)
{
    return 0;
}

/**
 * This function is called when a program attempts to write the entry in /proc.
 * Whatever data was written can be copied from @buf in userspace.
 *
 * We expect to read in the PID of a running process from userspace.
 * That PID will be parsed out and registered to receive signals.
 */
static ssize_t sysprof_write(struct file * file, const char * buf,
			     size_t length, loff_t * offset)
{
    // receive input buffer from userspace
    char * buffer = (char *) kmalloc(PROC_BUFSIZE, GFP_KERNEL);
    if(!buffer) return -ENOMEM;
    memset(buffer, 0, PROC_BUFSIZE);

    unsigned long copied = copy_from_user(buffer, buf, length);
    if(copied > 0)
    {
	printk(KERN_ERR "sysprof: Unable to copy procfs input buffer from userspace.");
	return -ENOMEM;
    }

    // seperate command character and PID
    char * ptr = strim(buffer);
    char * cchr = strsep(&ptr, " ");
    if(!cchr || !ptr || strlen(cchr) <= 0 || strlen(ptr) <= 0)
    {
	printk(KERN_ERR "sysprof: Malformed input received via procfs: \"%s\"\n", buffer);
	return 0;
    }

    // try to parse out a pid
    unsigned int pid;
    int err = kstrtouint(ptr, 10, &pid);
    if(err > 0)
    {
	printk(KERN_ERR "sysprof: Unable to parse valid PID from input: \"%s\"", ptr);
	return err;
    }

    // (un)register the PID
    switch(cchr[0])
    {
    case 'R':
	printk("Registering PID: %u\n", pid);
	register_pid((pid_t) pid);
	break;
    case 'U':
	printk("Unregistering PID: %u\n", pid);
	unregister_pid((pid_t) pid);
	break;
    default:
	printk(KERN_ERR "sysprof: Unknown command received.");
    }

    kfree(buffer);
    return length;
}

static struct proc_dir_entry * proc_entry;
static const struct file_operations proc_fops =
{
    .owner = THIS_MODULE,
    .read  = sysprof_read,
    .write = sysprof_write,
};

int __init sysprof_init(void)
{
    int err = 0;

    printk(KERN_INFO "sysprof: Loading sysprof module...\n");    

    /* setup proc filesystem entry */
    proc_entry = proc_create(PROC_ENTRY_FILENAME, 0666, NULL, &proc_fops);
    if (!proc_entry)
    {
	remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
	printk(KERN_ERR "sysprof: Could not initialize /proc/%s.\n", PROC_ENTRY_FILENAME);
	return -ENOMEM;
    }

    /* set up network monitoring */
    init_netfilter();

    /* set up shareed memory buffer */
    struct shmem_operations shmem_ops =
    {
	.report = netfilter_report,
    };
    err = create_shmem_buffer(shmem_ops);
    if(err < 0)
    {
	printk(KERN_ERR "sysprof: Unable to create shared memory buffer!");
	return -ENOMEM;
    }


    printk(KERN_INFO "sysprof: Loaded module.");
    return 0;
}

void __exit sysprof_exit(void)
{
    printk(KERN_INFO "sysprof: Unloading sysprof module...\n");

    /* clean up network monitoring */
    exit_netfilter();

    /* clean up proc filesystem entry */
    remove_proc_entry(PROC_ENTRY_FILENAME, NULL);

    destroy_shmem_buffer();
}

module_init(sysprof_init);
module_exit(sysprof_exit);
