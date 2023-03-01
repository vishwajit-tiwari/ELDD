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
MODULE_DESCRIPTION("A charDriver to implemnet (WaitQueue Static Method)");

dev_t myDev = 0;                    // Minor number
static struct cdev my_cdev;         // For charDevice registration
static struct class *dev_class;     // Return type of class_create (struct pointer)
#define MEM_SIZE 1024               // Kernel buffer size
uint8_t *kernel_buffer;             // for kamlloc

// For semaphore
struct semaphore wr_semaphore;

// Static method for waitQueue
DECLARE_WAIT_QUEUE_HEAD(my_queue);
static struct task_struct *wait_thread;     // return type of kthread_create
uint32_t read_count = 0;
int wait_queue_flag = 0;

// Dynamic method for waitQueue
// wait_queue_head_t my_queue;
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
    wait_event_interruptible(my_queue, wait_queue_flag == 0);
    pr_info("Write function : sleeping......\n");
    pr_info("wait-queue : wating for interrupt\n");

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
    pr_info("Reading from : user application....");

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
    /**
     * @def             : int cdev_add (struct cdev * p, dev_t dev, unsigned count);
     * 
     * @param   
     * struct cdev * p  : the cdev structure for the device
     * dev_t dev        : the first device number for which this device is responsible
     * unsigned count   : the number of consecutive minor numbers corresponding to this device
     * 
     * @brief           : cdev_add adds the device represented by p to the system, making it live immediately. 
     *                    A negative error code is returned on failure.
    */
    if((cdev_add(&my_cdev,myDev,2)) < 0) {
        pr_err("Can not add device to the system\n");
        goto r_class;
    }

    // Creating class structre for device
    /**
     * @def     : struct class * __class_create(struct module * owner, const char * name, struct lock_class_key * key);
     * 
     * @param
     * owner    : pointer to the module that is to “own” this struct class
     * name     : pointer to a string for the name of this class.
     * key      : the lock_class_key for this class; used by mutex lock debugging
     * 
     * @brief   : This is used to create a struct class pointer that can then be used in calls to device_create.
     *          : Returns struct class pointer on success, or ERR_PTR on error.
     *          : Note, the pointer created here is to be destroyed when finished by making a call to class_destroy

    */
    if((dev_class = class_create(THIS_MODULE,"CharDriver_Class")) == NULL) {
        pr_err("Can not create class structure for device\n");
        goto r_class;
    }

    // Creating Device File
    /**
     * @def     : struct device * device_create(struct class * class, struct device * parent, dev_t devt,
     *                                   void * drvdata, const char * fmt, ...);
     * @param
     * class    : pointer to the struct class that this device should be registered to
     * parent   : pointer to the parent struct device of this new device, if any
     * devt     : the dev_t for the char device to be added
     * drvdata  : the data to be added to the device for callbacks
     * fmt      : string for the device's name
     * ...      : variable arguments
     * 
     * @brief   : This function can be used by char device classes. A struct device will be created in sysfs, registered to the specified class.
     *          : A “dev” file will be created, showing the dev_t for the device, if the dev_t is not 0,0. If a pointer to a parent struct device is passed in, 
     *            the newly created struct device will be a child of that device in sysfs. 
     *            The pointer to the struct device will be returned from the call. 
     *            Any further sysfs files that might be required can be created using this pointer.
     *          : Returns struct device pointer on success, or ERR_PTR on error.
     * 
     * @note    : the struct class passed to this function must have previously been created with a call to class_create.

    */
    if((device_create(dev_class,NULL,myDev,NULL,"charDev_deviceWQ")) == NULL) {
        pr_err("Can not create device file\n");
        goto r_device;
    }

    // Creating Physical Memory
    /**
     * @def         : void *kmalloc(size_t size, int flags);
     * @param    
     * size         : Size of the block to be allocated. 
     * flags        : This is much more interesting, because it controls the behavior of kmalloc in a number of ways.
     * 
     * GFP_ATOMIC   : Used to allocate memory from interrupt handlers and other code outside of a process context. Never sleeps.
     * GFP_KERNEL   : Normal allocation of kernel memory. May sleep.
     * 
     * @brief       : The function is fast (unless it blocks) and doesn't clear the memory it obtains; 
     *                the allocated region still holds its previous content.[1] 
     *                The allocated region is also contiguous in physical memory
    */
    if((kernel_buffer = kmalloc(MEM_SIZE,GFP_KERNEL)) == 0) {
        pr_err("Can not allocate memory in Kernel\n");
        goto r_device;
    }

    strcpy(kernel_buffer,"Hello from Kernel Buffer!\n");

    // Initializing semaphore
    /**
     * @def : int sem_init(sem_t *sem, int pshared, unsigned int value);
     * Link with -pthread.
     * 
     * @param
     * 
     * sem      : unnamed semaphore
     * pshared  : argument indicates whether this semaphore is to be shared between the threads of a process, or between processes.
     * value    : specifies the initial value for the semaphore
     * 
     * If pshared has the value 0, then the semaphore is shared between the threads of a process, and should be located at some 
     * address that is visible to all threads (e.g., a global variable, or a variable allocated dynamically on the heap).
     * 
     * If pshared is nonzero, then the semaphore is shared between processes, and should be located in a region of shared memory
     * (see shm_open(3), mmap(2), and shmget(2)).(Since a child created by fork(2) inherits its parent's memory mappings, it can
     * also access the semaphore.)  Any process that can access the shared memory region can operate on the semaphore using 
     * sem_post(3), sem_wait(3), and so on.
     *
     * @brief   : sem_init() initializes the unnamed semaphore at the address pointed to by sem. 
     *            
    */
    sema_init(&wr_semaphore,1);
    
    // Initializing wait-queue Dynamically
    // init_waitqueue_head(my_queue);

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

/**
 * @def     : void unregister_chrdev_region(dev_t from, unsigned count);
 * 
 * @param  
 * from     : the first in the range of numbers to unregister
 * count    : the number of device numbers to unregister
 * 
 * @brief   : This function will unregister a range of count device numbers, starting with from. 
 *            The caller should normally be the one who allocated those numbers in the first place...
*/

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
