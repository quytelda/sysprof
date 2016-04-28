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

int create_shmem_buffer(void);
void destroy_shmem_buffer(void);
