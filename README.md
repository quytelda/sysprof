# sysprof ![Linux Build Status] (https://travis-ci.org/quytelda/sysprof.svg?branch=master)
Linux Statistical System Profiler

## Kernel Module
The kernel module records real-time data about incoming/outgoing network packets using Netfilter.  The records of the real time data are periodically sampled at regular interval and made available in a shared memory buffer accessible via a character device (/dev/sysprof).

##Inotify
Inotify is an executable that takes monitors filesystem activity. Every minute seconds, it will report the following statistics in 
2 lines:

Line 1: how many files have been accessed, how many files have had their attributes changed, how many files have been modified, and
how many files have been opened (all since the last 1 minute interval)

Line 2: the access alerts, the attribute modification alerts, the modification alerts, and the read alerts 
(all since the last 1 minute interval). Alerts indicate potentially suspicious activity

##Master_IO.bash
Master_IO.bash is a bash script that gathers disk I/O (in blocks read from and written to per device), CPU usage (percentage by user, nice, system, iowait, steal, and idle), and memory (megabytes total, used, free, shared, buffers, cached). Data is gathered every minute and sent to named pipes "memory_pipe", "disk_pipe", and "CPU_pipe"
