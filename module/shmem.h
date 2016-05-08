/*
 * shmem.h - Manage shared memory buffer with userspace.
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

#define SHMEM_PAGES 32
#define SHMEM_SIZE (PAGE_SIZE * SHMEM_PAGES)
#define SHMEM_CDEV_NAME "sysprof"
#define SHMEM_WQ_NAME "sysprof"
#define SAMPLE_DELAY 10000 // 1 minute (ms)

/**
 * insert_struct() - adds a struct to the shared memory buffer
 * @s_ptr: a pointer to the data structure
 * @type: the data type of the struct (i.e. struct task_struct)
 *
 * Calls insert_data() with the appropriate arguments.
 */
#define insert_struct(s_ptr, type): insert_data((void *) s_ptr, sizeof(type))

struct shmem_operations
{
    ssize_t (*report) (void ** data);
};

int create_shmem_buffer(struct shmem_operations shmem_ops);
void destroy_shmem_buffer(void);

void insert_data(void * data, size_t bytes);
