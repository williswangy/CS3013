/**
 * CS 3013 Project 2
 * Yuan Wang and YuanDa Song ywang19 & ysong5
 * getprocessinfo.h
 * get the cs3013_syscall2 information
 */

#include "processinfo.h"

// These values MUST match the unistd_32.h modifications:

// #define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
// #define __NR_cs3013_syscall3 379

long getprocessinfo (struct processinfo* info) {
    return (long) syscall(__NR_cs3013_syscall2, info);
}
