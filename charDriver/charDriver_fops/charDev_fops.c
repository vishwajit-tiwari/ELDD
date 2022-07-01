/**
 * @file charDev_fops.c
 * @author Vishwajit Tiwari (tvishwajit@cdac.in)
 * @brief  A simple charDriver Program for open, read, write & release syscall.
 * @version 0.1
 * @date 2022-06-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include<linux/init.h>              // for __init & __exit
#include<linux/kernel.h>            // for printk pr_info
#include<linux/module.h>            // for module_init() & module_exit() MODULE_LICENSE() and so on...
#include<linux/kdev_t.h>            // for MKDEV() , MAJOR(), MINOR()
#include<linux/types.h>             // for different data types
#include<linux/fs.h>                // for inode structure & File operations structure
#include<linux/cdev.h>              // for cdev structure
#include<linux/device.h>
#include<linux/uaccess.h>           // for copy_to_user and copy_from_user
#include<linux/slab.h>              // for kmalloc() function

#define NAME RW_CharDriver
#define MEM_SIZE 1024
uint8_t *kernel_buffer;

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("A Module Prog for cdev struct & file operation");


dev_t mydev = 0;                        // Creating Device Number
static struct class *dev_class;         // Return type of class_create (struct pointer)
static struct cdev my_cdev;             // For charDevice registration

// Function prototypes
static int      __init charDev_fops_init(void);
static void     __exit charDev_fops_exit(void);
static int      charDev_open(struct inode *pinode, struct file *pfile);
static int      charDev_release(struct inode *pinode, struct file *pfile);
static ssize_t  charDev_read(struct file *filp, char __user *Ubuf, size_t len,loff_t * offp);
static ssize_t  charDev_write(struct file *filp, const char *Ubuf, size_t len, loff_t * offp);


// Structure for File Operations
struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .read       = charDev_read,
    .write      = charDev_write,
    .open       = charDev_open,
    .release    = charDev_release,

};

/**
 * @brief This function will be called when we open the Device file
 * 
 */
static int charDev_open(struct inode *pinode, struct file *pfile) 
{
    pr_info("Driver Open function called....!!!\n");
    printk(KERN_INFO "Device file opened....!!!\n");
    return 0;
}

/**
 * @brief This function will be called when we close the Device file
 * 
 */
static int charDev_release(struct inode *pinode, struct file *pfile) 
{
    pr_info("Driver Release function called......!!!\n");
    printk(KERN_INFO "Device File Closed...!!!\n");
    
    return 0;
}

/**
 * @brief This function will be called when we Write to the Device file
 * 
 */
static ssize_t charDev_write(struct file *filp, const char *Ubuf, size_t len, loff_t *offp) 
{
    pr_info("Driver Write function called.........!!!\n");
    printk(KERN_ALERT "This is the kernel write call...Inside %s call\n", __FUNCTION__);

    // Access to user space data
    if( copy_from_user(/*Kbuff*/kernel_buffer, Ubuf, len) ) {
        pr_err("Data Write : Err!\n");
    }
    printk("Data Write : Done!\n");;
    
    return len; 
}

/**
 * @brief This function will be called when we Read through Device File
 * 
 */
static ssize_t charDev_read(struct file *filp, char __user *Ubuf, size_t len, loff_t *offp) 
{
    pr_info("Driver Read function called...........!!!\n");
    printk(KERN_ALERT "This is the kernel read call...Inside %s call\n", __FUNCTION__);

    // Access to user space data
    if( copy_to_user(Ubuf, kernel_buffer, MEM_SIZE) ) {
        pr_err("Data Read : Err!\n");
    }
    pr_info("Data Read: Done!\n");

    return MEM_SIZE;
}


/***********************************Start of Init Module**********************************************/
static int __init charDev_fops_init(void) 
{
    printk(KERN_ALERT "Inside %s Module\n", __FUNCTION__);
    printk(KERN_ALERT "Inserting Device Driver into Kernel\n");
    
    /*Allocating Major number Dynamically*/
    if(alloc_chrdev_region(&mydev,0,1,"charDev_fops") < 0) {
        pr_err("Cannot allocate Major number\n");
        return -1;
    }
    pr_info("Device Number: Major = %d Minor = %d\n", MAJOR(mydev), MINOR(mydev));

    /*Creating cdev structure*/
    cdev_init(&my_cdev, &fops);

    /*Adding charDevice to the System*/
    if((cdev_add(&my_cdev,mydev,1)) < 0) {
        pr_err("Cannot add device to the syste\n");
        goto r_class;
    }

    /*Creating struct Class for Device*/
    if((dev_class = class_create(THIS_MODULE,"charDev_class")) == NULL) {
        printk(KERN_ERR "Cannot create the struct class for device\n");
        goto r_class;
    }

    /**
     * @brief Creating Device File
     * @file struct device *device_create (struct *class, struct device *parent, 
     *                               dev_t dev, void * drvdata, const char *fmt, ...);
     */
    if((device_create(dev_class,NULL,mydev,NULL,"charDev_device")) == NULL) {
        pr_err("Cannot create the Device\n");
        goto r_device;
    }

    /*Creating Physical memory*/
    if((kernel_buffer = kmalloc(MEM_SIZE, GFP_KERNEL)) == 0){
        pr_info("Cannot allocate memory in kernel\n");
        goto r_device;
    }
        
    strcpy(kernel_buffer, "Hello_From_Kernel");

    pr_alert("Device Driver Inserted... Successfully\n");

    return 0;

// To free-up the resources
r_device:
    class_destroy(dev_class);

r_class:
    unregister_chrdev_region(mydev,1);
    return -1;

}
/**************************************End of Init Module******************************************/


/***************************************Cleanup Module*********************************************/
static void __exit charDev_fops_exit(void) 
{
    printk(KERN_ALERT "Inside %s Module\n", __FUNCTION__);
    printk(KERN_ALERT "Removing charDev_fops from Kernel\n");
    // kfree(kernel_buffer/*Kbuff*/);
    device_destroy(dev_class,mydev);                // To destroy char device 
    class_destroy(dev_class);                       // To destroy device class
    cdev_del(&my_cdev);                             // To remove charDevice from the system
    unregister_chrdev_region(mydev,1);              // To unregister Device Number 
    pr_info("Device Driver removed... Successfully\n");
}
/*************************************End of Cleanup Module****************************************/

module_init(charDev_fops_init);         // driver initialization entry point
module_exit(charDev_fops_exit);         // driver exit entry point