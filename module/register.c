/*
 * register.c - Registration list for processes collecting data
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
#include <linux/pid.h>
#include <linux/sched.h>

/* TODO: Should be able to register multiple processes.
 * This should actually be a linked list of task_structs. We're taking this
 * shortcut now so the daemon team can run their tests in the meantime.
 */
static struct task_struct * process = NULL;

/**
 * register_pid() - Register a process so it will receive signals.
 * @pid The pid of the process
 */
void register_pid(pid_t pid)
{
    process = pid_task(find_get_pid(pid), PIDTYPE_PID);
}

/**
 * unregister_pid() - Unregister a process so it will no longer receive signals.
 * @pid The pid of the process.
 * The opposite of register_pid()
 */
void unregister_pid(pid_t pid)
{
    process = NULL;
}

/**
 * signal_process() - send a signal to a user process
 * Sends SIGCONT to the process whose task_struct we have, if it exists.
 */
void signal_processes(void)
{
    if(!process) return;
    
    // signal the process to continue (SIGCONT)
    send_sig(SIGCONT, process, 1);
}
