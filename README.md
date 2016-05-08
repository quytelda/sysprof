# sysprof
Linux Statistical System Profiler

## Kernel Module
The kernel module records real-time data about incoming/outgoing network packets using Netfilter.  The records of the real time data are periodically sampled at regular interval and made available in a shared memory buffer accessible via a character device (/dev/sysprof).