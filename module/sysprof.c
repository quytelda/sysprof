/* 
 * sysprof.c - Linux Statistical System Profiler
 * This is the core of the sysprof kernel module.
 *
 * Copyright (C) 2016 Quytelda Kahja, Roger Xiao
 */

#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Quytelda Kahja");
MODULE_AUTHOR("Roger Xiao");
MODULE_DESCRIPTION("Linux Statistical System Profiler");

int __init sysprof_init(void)
{
    return 0;
}

void __exit sysprof_exit(void)
{
}

module_init(sysprof_init);
module_exit(sysprof_exit);
