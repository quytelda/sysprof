/*
 * shmem.c - Manage shared memory buffer with userspace.
 *
 * Copyright (C) 2016 Quytelda Kahja
 * This file is part of Sysprof.
 *
 * Sysprof is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sysprof is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Sysprof.	If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/workqueue.h>
#include <linux/vmalloc.h>

#include "shmem.h"
#include "register.h"

static void * shmem_buffer = NULL;
static void * shmem_cursor = NULL;

static struct shmem_operations shmem_ops;

static struct workqueue_struct * workqueue;
static struct delayed_work * sample_work;

static int chrdev_mmap(struct file * file, struct vm_area_struct * vma)
{
    long vm_start = vma->vm_start;
    long vm_size = vma->vm_end - vm_start;

    int res;
    long offset = 0;
    void * ph_addr = shmem_buffer;
    unsigned long pfn, vm_addr;
    for(int i = 0; (i < SHMEM_PAGES) && (offset < vm_size); i++)
    {
	offset = i * PAGE_SIZE;

	ph_addr = shmem_buffer + offset;
	pfn = vmalloc_to_pfn(ph_addr);

	vm_addr = vm_start + offset;
	if((res = remap_pfn_range(vma, vm_addr, pfn, PAGE_SIZE, PAGE_SHARED)) > 0)
	    return res;
    }
    
    return 0;
}

static void sample_data(struct work_struct * work)
{
    if(!shmem_ops.report)
    {
	printk(KERN_ERR "sysprof: data reporting callback undefined.");
	return;
    }

    void * data = NULL;
    ssize_t data_size = shmem_ops.report(&data);

    insert_data(data, (size_t) data_size);

    // requeue work to be executed again
    if(!queue_delayed_work(workqueue, sample_work, msecs_to_jiffies(SAMPLE_DELAY)))
	printk(KERN_ERR "sysprof: Unable to queue sampling task.");
}

static dev_t dev_num;
static struct cdev * dev;
static const struct file_operations dev_fops =
{
    .owner   = THIS_MODULE,
    .mmap    = chrdev_mmap,
};

/**
 * create_shmem_buffer() - create a shared memory buffer and character device
 * Allocates a shared memory buffer that will be accessible in userspace via a
 * memory mapped character device file.  Returns zero on success or a negative
 * error code.
 */
int create_shmem_buffer(struct shmem_operations ops)
{
    int err = 0;
    shmem_ops = ops;

    // allocate buffer space for shared memory
    shmem_buffer = vmalloc(SHMEM_SIZE);
    if(!shmem_buffer) return -ENOMEM;

    void * ptr;
    for(int i = 0; i < SHMEM_PAGES; i++)
    {
	ptr = shmem_buffer + (i * PAGE_SIZE);
	set_bit(PG_reserved, &vmalloc_to_page(ptr)->flags);
    }

    // set up shared memory character device
    err = alloc_chrdev_region(&dev_num, 0, 1, SHMEM_CDEV_NAME);
    if(err < 0) return err;

    dev = cdev_alloc();
    if(!dev) return -ENOMEM;

    cdev_init(dev, &dev_fops);
    err = cdev_add(dev, dev_num, 1);
    if(err < 0) return err;

    // point the cursor at the start of the memory region
    shmem_cursor = shmem_buffer;

    // set up delayed workqueue
    workqueue = create_workqueue(SHMEM_WQ_NAME);

    sample_work = (struct delayed_work *)
	kmalloc(sizeof(struct delayed_work), GFP_KERNEL);
    INIT_DELAYED_WORK(sample_work, sample_data);

    if(!queue_delayed_work(workqueue, sample_work, msecs_to_jiffies(SAMPLE_DELAY)))
	printk(KERN_ERR "sysprof: Unable to queue sampling task.");

    return 0;
}

/**
 * destroy_shmem_buffer() - destroy a shared memory buffer and character device
 * Deallocates and cleans up the shared memory buffer and it's associated
 * character device.
 */
void destroy_shmem_buffer(void)
{
    // destroy period workqueue
    // XXX: do we need to flush this queue?
    cancel_delayed_work(sample_work);
    destroy_workqueue(workqueue);
    kfree(sample_work);

    // clean up the shared memory character device
    cdev_del(dev);
    unregister_chrdev_region(dev_num, 1);

    // clean up shared userspace memory buffer
    void * ptr;
    for(int i = 0; i < SHMEM_PAGES; i++)
    {
	ptr = shmem_buffer + (i * PAGE_SIZE);
	clear_bit(PG_reserved, &vmalloc_to_page(ptr)->flags);
    }

    vfree(shmem_buffer);
    shmem_buffer = NULL;
    shmem_cursor = NULL;
}

/**
 * insert_data() - insert data into the shared buffer
 * @data a pointer to the region of memory where the data resides
 * @size the size of the region of memory where the data resides
 */
void insert_data(void * data, size_t size)
{
    // is there space for this?
    // if not, we reset to the beginning of the buffer
    if((shmem_buffer + SHMEM_SIZE) - shmem_cursor <= size)
	shmem_cursor = shmem_buffer;

    memcpy(shmem_cursor, data, size);

    // advance cursor
    shmem_cursor += size;

    // notify listeners that something was inserted
    signal_processes();
}
