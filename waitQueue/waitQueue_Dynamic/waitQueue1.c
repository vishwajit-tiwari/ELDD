/**
 * @file waitQueue1.c
 * @author Vishwajit Tiwari (tvishwajit@cdac.in)
 * @brief   CharDriver : Wait Queue (Dynamic) 
 * @version 0.1
 * @date 2022-07-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/***
 * There are 3 important steps in Waitqueue.
 * Initializing Waitqueue
 * Queuing (Put the Task to sleep until the event comes)
 * Waking Up Queued Task
 **/

#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kdev_t.h>            // for MKDEV() , MAJOR(), MINOR()
#include<linux/types.h>             // for different data types
#include<linux/cdev.h>              // for cdev structure
#include<linux/device.h>            // to create device file
#include<linux/fs.h>                // for inode structure & File operations structure
#include<linux/uacce.h>             // for copy_to_user and copy_from_user
#include<linux/slab.h>              // for kmalloc function()
#include<linux/wait.h>              // for wait Queue
#include<linux/kthread.h>           // for Kernel Thread
#include<linux/semaphore.h>         // for semaphore


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishwajit Tiwari");
MODULE_DESCRIPTION("A charDriver to implemnet (WaitQueue Dynamic Method)");

dev_t myDev = 0;                    // Minor number
static struct cdev my_cdev;         // For charDevice registration
static struct class *dev_class;     // Return type of class_create (struct pointer)
#define MEM_SIZE 1024               // Kernel buffer size
uint8_t *kernel_buffer;             // for kamlloc

// For semaphore
struct semaphore wr_semaphore;

// Static method for waitQueue
// DECLARE_WAIT_QUEUE_HEAD(my_queue);
static struct task_struct *wait_thread;     // return type of kthread_create
uint32_t read_count = 0;
int wait_queue_flag = 0;

// Dynamic method for waitQueue
wait_queue_head_t my_queue;
// init_waitqueue_head(&my_queue);

/***
 * There are several macros are available for different uses. We will see each one by one.
 * wait_event
 * wait_event_timeout
 * wait_event_cmd
 * wait_event_interruptible
 * wait_event_interruptible_timeout
 * wait_event_killable
 **/

/***
 * sleep until a condition gets true.
 * wait_event(wq, condition);
 * wait_event_timeout(wq, condition, timeout);
 * wait_event_cmd(wq, condition, cmd1, cmd2);
 * wait_event_interruptible(wq, condition); 
 * wait_event_interruptible_timeout(wq, condition, timeout);
 * wait_event_killable(wq, condition);
 **/

/***
 * Waking Up Queued Task
 * wake_up
 * wake_up_all
 * wake_up_interruptible
 * wake_up_sync and wake_up_interruptible_sync
 **/

// Function Prototypes
static int __init charDriver_init(void);
static void __exit charDriver_exit(void);
static int charDev_open(struct inode *pinode, struct file *pfile);
static int charDev_release(struct inode *pinode, struct file *pfile);
static ssize_t charDev_write(struct file *pfile, const char *ubuff, size_t len, loff_t *offp);
static ssize_t charDev_read(struct file *pfile, char __user *ubuff, size_t len, loff_t *offp);

// File Operation Structure
struct file_operations fops = {
    .owner      = THIS_MODULE,
    .open       = charDev_open,
    .write      = charDev_write,
    .read       = charDev_read,
    .release    = charDev_release,
};

/***************************Thread Function****************************/
static int myWait_function(void *unused) 
{
    pr_alert("Inside : %s() function\n", __FUNCTION__);

    while (1)
    {
        pr_info("Wating for event....\n");
        wait_event_interruptible(my_queue,wait_queue_flag != 0);
        if(wait_queue_flag == 2) {
            pr_info("Event came from : exit() function\n");
            return 0;
        }
        if(wait_queue_flag == 3) {
            pr_info("Event came from : write() function\n");
            return 0;
        }
        pr_info("Event came from : read() function : read-count = %d & wait-queue-flag = %d\n", ++read_count, wait_queue_flag);
        wait_queue_flag = 0;
    }
    do_exit(0);
    return 0;
}
/**********************************************************************/


/****************************Open function*****************************/
static int charDev_open(struct inode *pinode, struct file *pfile) 
{
    pr_info("Inside : %s() function\n", __FUNCTION__);
    pr_info("Opening Device File\n");
    return 0;
}
/**********************************************************************/

/************************Release Function******************************/
static int charDev_release(struct inode *pinode, struct file *pfile)
{
    pr_info("Inside : %s() function\n", __FUNCTION__);
    pr_info("Closing Device File\n");
    return 0;
}
/**********************************************************************/

/***********************************Write Function******************************************/
static ssize_t charDev_write(struct file *pfile, const char *ubuff, size_t len, loff_t *offp) 
{
    pr_info("Inside : %s() function\n", __FUNCTION__);
    pr_info("Writing to : Kernel Buffer\n");

    //Acquiring semaphore lock
    down(&wr_semaphore);
    pr_info("Acquired semaphore lock\n");
    pr_info("semaphore : locked\n");

    // wait-queue flag
    wait_queue_flag = 3;

    // Access to user space data
    if(copy_from_user(kernel_buffer,ubuff,len)) {
        pr_err("Data write: Error!!!\n");
    }

    pr_info("Data write: Done!\n");

    // Putting write function to sleep using wait-queue after write operation
    pr_info("Write function : sleeping......\n");
    pr_info("wait-queue : wating for interrupt\n");
    wait_event_interruptible(my_queue, wait_queue_flag == 0);
    

    // Releasing shemaphore lock
    up(&wr_semaphore);
    pr_info("Releasing semaphore lock\n");
    pr_info("semaphore : unlocked\n");

    return len;
}
/*******************************************************************************************/

/**************************************Read Function****************************************/
static ssize_t charDev_read(struct file *pfile, char __user *ubuff, size_t len, loff_t *offp) 
{
    pr_info("Inside : %s() function\n", __FUNCTION__);
    pr_info("Reading from : Kernel Buffer....");

    // Wait queue flag
    wait_queue_flag = 0;

    // Waking-up task from queue
    wake_up_interruptible(&my_queue);
    pr_info("wake-up task from queue\n");
    pr_info("wait-queue : interrupted\n"); 

    if(copy_to_user(ubuff,kernel_buffer,MEM_SIZE)) {
        pr_err("Error reading from user application!!!\n");
    }
    pr_info("Data read: Done!\n");

    return MEM_SIZE;
}
/*******************************************************************************************/


/******************************Module Entry Section********************************/
static int __init charDriver_init(void) 
{
    printk(KERN_ALERT "Inside : %s() module\n", __FUNCTION__);
    pr_info("Inserting Char Driver into Kernel\n");

    // Allocating Major Number Dynamically
    if((alloc_chrdev_region(&myDev,0,2,"charDriverWQ")) < 0) {
        pr_err("Can not allocate Major Number\n");
        return -1;
    }
    pr_info("Device Number: Major = %d Minor = %d\n", MAJOR(myDev), MINOR(myDev));

    // Creating cdev structure
    cdev_init(&my_cdev,&fops);

    // Adding charDevice to the system
    if((cdev_add(&my_cdev,myDev,2)) < 0) {
        pr_err("Can not add device to the system\n");
        goto r_class;
    }

    // Creating class structre for device
    if((dev_class = class_create(THIS_MODULE,"CharDriver_Class")) == NULL) {
        pr_err("Can not create class structure for device\n");
        goto r_class;
    }

    // Creating Device File
    if((device_create(dev_class,NULL,myDev,NULL,"charDev_deviceWQ")) == NULL) {
        pr_err("Can not create device file\n");
        goto r_device;
    }

    // Creating Physical Memory
    if((kernel_buffer = kmalloc(MEM_SIZE,GFP_KERNEL)) == 0) {
        pr_err("Can not allocate memory in Kernel\n");
        goto r_device;
    }

    strcpy(kernel_buffer,"Hello from Kernel Buffer!\n");

    // Initializing semaphore
    sema_init(&wr_semaphore,1);

    // Initializing wait-queue Dynamically
    init_waitqueue_head(&my_queue);

    // Create the Kernel Thread
    wait_thread = kthread_create(myWait_function,NULL,"my_WaitThread");
    if(wait_thread) {
        pr_info("Thread Created Successfully\n");
        wake_up_process(wait_thread);
    }
    else {
        pr_err("Thread creation failed!\n");
    }

    pr_info("Device Driver Inserted : Sucessfully\n");

    return 0;

// To free-up the resources
r_device:
    class_destroy(dev_class);

r_class:
    unregister_chrdev_region(myDev,1);
    return -1;

}
/********************************************************************************/


/*****************************Module Cleanup Section*****************************/
static void __exit charDriver_exit(void) 
{
    printk(KERN_ALERT "Inside %s module\n", __FUNCTION__);
    pr_info("Removing Device Driver from Kernel\n");
    // for wait queue
    wait_queue_flag = 2;
    wake_up_interruptible(&my_queue);

    //Deallocate the memory space of Kernel Buffer
    kfree(kernel_buffer);

    // for device file
    device_destroy(dev_class,myDev);
    // Structure class for device
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(myDev,1);
    pr_info("Device Driver Removed.... Successfully\n");    
}
/********************************************************************************/

module_init(charDriver_init);
module_exit(charDriver_exit);
