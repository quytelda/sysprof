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
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include "shmem.h"

static void * shmem_buffer = NULL;

static int chrdev_mmap(struct file * file, struct vm_area_struct * vma)
{
    return 0;
}

static dev_t dev_num;
static struct cdev * dev;
static const struct file_operations dev_fops =
{
    .owner   = THIS_MODULE,
    .mmap    = chrdev_mmap,
};

int create_shmem_buffer(void)
{
    int err = 0;

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

    return 0;
}

void destroy_shmem_buffer(void)
{
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
}
