/**
 * CS 3013 Project 2
 * Yuan Wang and YuanDa Song ywang19 & ysong5
 * processinfo.h
 * struct task information as the instructions required
 */


#ifndef PROCESSINFO
#define PROCESSINFO

struct processinfo {
    long state;            // current state of process
    pid_t pid;             // process ID of this process
    pid_t parent_pid;      // process ID of parent
    pid_t youngestChild;   // process ID of youngest child
    pid_t youngerSibling;  // pid of next younger sibling
    pid_t olderSibling;    // pid of next older sibling
    uid_t uid;             // user ID of process owner
    long long start_time;  // process start time in nanoseconds since user mode
    long long user_time;   // CPU time in user mode (microseconds)
    long long sys_time;    // CPU time in system mode (microseconds)
    long long cutime;      // user time of children (microseconds)
    long long cstime;      // system time of children (microseconds)
};


#endif
