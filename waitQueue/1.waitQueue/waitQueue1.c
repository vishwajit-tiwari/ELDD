/**
 * @file waitQueue1.c
 * @author Vishwajit Tiwari (tvishwajit@cdac.in)
 * @brief   CharDriver to implement Wait Queue 
 * @version 0.1
 * @date 2022-07-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishwajit Tiwari");
MODULE_DESCRIPTION("A charDriver to implemnet Wait Queue");

dev_t myDev = 0;                    // Minor number
static struct cdev my_cdev;         // For charDevice registration
static struct class *dev_class;     // Return type of class_create (struct pointer)
#define MEM_SIZE 1024               // Kernel buffer size
uint8_t *kernel_buffer;             // for kamlloc

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

static int charDev_open(struct inode *pinode, struct file *pfile) 
{
    return 0;
}

static int charDev_release(struct inode *pinode, struct file *pfile)
{
    return 0;
}

static ssize_t charDev_write(struct file *pfile, const char *ubuff, size_t len, loff_t *offp) 
{
    return 0;
}

static ssize_t charDev_read(struct file *pfile, char __user *ubuff, size_t len, loff_t *offp) 
{
    return 0;
}


/******************************Module Entry Section********************************/
static int __init charDriver_init(void) 
{
    printk(KERN_ALERT "Inside %s module\n", __FUNCTION__);
    pr_info("Inserting Char Driver into Kernel\n");

    // Allocating Major Number Dynamically
    if((alloc_chrdev_region(&myDev,0,1,"charDriverWQ")) < 0) {
        pr_err("Can not allocate Major Number\n");
        return -1;
    }
    pr_info("Device Number: Major = %d Minor = %d\n", MAJOR(myDev), MINOR(myDev));

    // Creating cdev structure
    cdev_init(&my_cdev,&fops);

    // Adding charDevice to the system
    if((cdev_add(&my_cdev,myDev,1)) < 0) {
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

    pr_info("Device Device Driver Inserted.....Sucessfully\n");

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
    device_destroy(dev_class,myDev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(myDev,1);
    pr_info("Device Driver Removed.... Successfully\n");    
}
/********************************************************************************/

module_init(charDriver_init);
module_exit(charDriver_exit);
