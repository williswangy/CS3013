/**
 * CS 3013 Project 2
 * Yuan Wang and YuanDa Song
 * ywang19 & ysong5
 * cs3013_syscall2.c
 * Part 2：
 * we intercepted the syscall cs3013_syscall2. When the interception done, we are able to print the
 * information as the instructions required(parent and child processes by using the cs3013-syscall2).
 * Process:
 * As the instruction required,
 * 1. use processinfo kinfo struct
 * 2. define temporary task_struct var from the instruction : child, sibling, youngestChild, youngerSibling,
 * olderSibling and initialize them with NULL
 * 3. According to the definiation in processimfo, we need to initialize values with -1 withn
 * kinfo.state,pid,parent_pid,youngestChild,youngerSibling,olderSibling,uid as requied
 * 4. initialize time related values with 0 => start_time,user_time,sys_time,cutime,cstime.
 * 5. In order to compare the time difference, we need to first assign the values in kinfo based on the
 * current task: current -> state, pid from the kinfo.state and kinfo pid. current_uid().val for the uid
 * parent_pid needs to assign through current->parent->pid
 * 6. Then we will use the strcuct list_for_each_entry(pos, head, member) to iterate over the list of given
 * types.
 * 7. For each childprocess from the current, we need to add up the user and system time
 * 8. For the siblingprocess, we need to get the youngerSibling and olderSibling
 * 9. As the instruction mentioned, we need use the fucntion timespec_compare() by comparing the starttime and
 * endtime among the task_struct variables.
 * When we finished the comparing, we need to set the youngestChild, youngerSibling and olderSibling based 
 * the result list from the above part
 * 10. Assign the values to the process time
 * 11. As the instruction required, copy struct to userspace safely.
 */
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/time.h>
#include <asm/current.h>
#include <asm/cputime.h>
#include <asm/uaccess.h>
#include <asm/errno.h>

#include "processinfo.h"

unsigned long **sys_call_table;

asmlinkage long (*ref_sys_cs3013_syscall2)(void);

asmlinkage long new_sys_cs3013_syscall2(struct processinfo *info) {
    //kinfo struct
    struct processinfo kinfo;

    //temporary variables, initialize them to be NULL
    struct task_struct* child          = NULL;
    struct task_struct* sibling        = NULL;
    struct task_struct* youngestChild  = NULL;
    struct task_struct* youngerSibling = NULL;
    struct task_struct* olderSibling   = NULL;
    
    // Initialize the fields in processinfo with -1
    kinfo.state          = -1;
    kinfo.pid            = -1;
    kinfo.parent_pid     = -1;
    kinfo.youngestChild  = -1;
    kinfo.youngerSibling = -1;
    kinfo.olderSibling   = -1;
    kinfo.uid            = -1;
    //Initialize the timerelated fields in processinfo with 0
    kinfo.start_time     = 0;
    kinfo.user_time      = 0;
    kinfo.sys_time       = 0;
    kinfo.cutime         = 0;
    kinfo.cstime         = 0;

    // Assign values from current task_struct
    kinfo.state = current->state;
    kinfo.pid = current->pid;
    kinfo.uid = current_uid().val;
    kinfo.parent_pid = current->parent->pid;

    // list_for_each_entry section for the childprocess from current
    list_for_each_entry(child, &(current->children), sibling)
    {   // add up the user and systime
        kinfo.cutime += cputime_to_usecs(child->utime);
	    kinfo.cstime += cputime_to_usecs(child->stime);
        
        // If youngest child is null, or the current child has a time more recent than the current running youngest
        if (youngestChild == NULL || timespec_compare(&child->start_time, &youngestChild->start_time) > 0)
            youngestChild = child;
    }

    // list_for_each_entry section for the sibling process to get the younger and older siblings.
    list_for_each_entry(sibling, &(current->sibling), sibling)
    {
        // current sibling start time > current process start time, running younger is null or the current sibling > running younger
        if ((timespec_compare(&sibling->start_time, &current->start_time) > 0) && (youngerSibling == NULL || timespec_compare(&sibling->start_time, &youngerSibling->start_time) < 0))
            youngerSibling = sibling;

        // current sibling stratime <  current process start time, running older is null or the current sibling < older
        if ((timespec_compare(&sibling->start_time, &current->start_time) < 0) && (olderSibling == NULL || timespec_compare(&sibling->start_time, &olderSibling->start_time) > 0))
            olderSibling = sibling;
    }
    
    // From the reult list above, set the pid to processinfo
    if (youngestChild != NULL)
	kinfo.youngestChild = youngestChild->pid;
    else
	kinfo.youngestChild = -1;

    if (youngerSibling != NULL)
	kinfo.youngerSibling = youngerSibling->pid;
    else
	kinfo.youngerSibling = -1;

    if (olderSibling != NULL)
	kinfo.olderSibling = olderSibling->pid;
    else
	kinfo.olderSibling = -1;


    // Assign values to the process time fields
    kinfo.start_time = timespec_to_ns(&current->start_time);
    kinfo.user_time = cputime_to_usecs(current->utime);
    kinfo.sys_time = cputime_to_usecs(current->stime);

    // Copy struct to user space safely
    if (copy_to_user(info, &kinfo, sizeof kinfo))
        return EFAULT;

    return 0;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  
  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
   */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the 
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  
  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
