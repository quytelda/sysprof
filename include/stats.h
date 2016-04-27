/* 
 * stats.h - Linux Statistical System Profilerure
 * Definitions for shared data structures.
 *
 * Copyright (C) 2016 Roger Xiao, Quytelda Kahja
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

struct timespec {
    time_t  tv_sec;     /* seconds */
    long    tv_nsec;    /* nanoseconds */
};

struct FS {
    struct timespec timestamp;
    int UID;
    unsigned int R;
    unsigned int W;
    unsigned int X;
};

struct NET {
    struct timespec timestamp;
   
   //inputs
    unsigned int pac_in;
    unsigned int udp_in;
    unsigned int tcp_in;
    unsigned int icmp_in;
    unsigned int other_in;
    
    unsigned int * source_ips;
    unsigned int * source_ports;
    
    
    //outputs
    unsigned int pac_out;
    unsigned int udp_out;
    unsigned int tcp_out;
    unsigned int icmp_out;
    unsigned int other_out;
    
    unsigned int * dest_ips;
    unsigned int * dest_ports;
    
};

#typedef TIMESPEC_DEFAULT struct timespec {0, 0};
#typedef FS_DEFAULT  struct FS  {TIMESPEC_DEFAULT,0,0,0,0};
#typedef NET_DEFAULT struct NET {TIMESPEC_DEFUALT,0,0};
