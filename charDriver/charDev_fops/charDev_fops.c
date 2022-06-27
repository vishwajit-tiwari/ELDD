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
#include<linux/fs.h>                // for inode structure & File operations
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("A Module Prog for cdev struct & file operation");


dev_t mydev = 0;                        // Creating Device Number
static struct class *dev_class;         // Return type of class_create (struct pointer)
static struct cdev my_cdev;             // For charDevice registration

// Function prototypes
static int      __init charDev_fops_init(void);
static void     __exit charDev_fops_exit(void);
static int      charDev_open(struct inode *inode, struct file *file);
static int      charDev_release(struct inode *inode, struct file *file);
static ssize_t  charDev_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  charDev_write(struct file *filp, const char *buf, size_t len, loff_t * off);


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
static int charDev_open(struct inode *inode, struct file *file) {
    pr_info("Driver Open function called....!!!\n");
    return 0;
}

/**
 * @brief This function will be called when we close the Device file
 * 
 */
static int charDev_release(struct inode *inode, struct file *file) {
    pr_info("Driver Release function called......!!!\n");
    return 0;
}

/**
 * @brief This function will be called when we Write to the Device file
 * 
 */
static ssize_t charDev_write(struct file *filp, const char *buf, size_t len, loff_t *off) {
    pr_info("Driver Write function called.........!!!\n");

    unsigned long result1;
    ssize_t retval;
    char Kbuff[100];

    printk(KERN_ALERT "This is the kernel write call...Inside %s call\n", __FUNCTION__);

    // Access to user space data
    result1 = copy_from_user((void *)Kbuff, (char *)buf, len);
    if(result1 == 0) {
        pr_info("Message from user app >>>%s<<<\n",Kbuff);
        pr_info("Data received completely\n");
        retval = len;
        return retval;
    }
    else if(result1 > 0) {
        pr_info("Message from user app >>>%s<<<\n", Kbuff);
        pr_info("only some part of data is received\n");
        retval = (len-result1);
        return retval;
    }
    else {
        pr_info("Error in write in kernel space\n");
        retval = -EFAULT;
        return retval;
    }

    return len;
}

/**
 * @brief This function will be called when we Read through Device File
 * 
 */
static ssize_t charDev_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    pr_info("Driver Read function called...........!!!\n");

    unsigned long result;
    ssize_t retval;
    char Kbuff[100];

    printk(KERN_ALERT "This is the kernel read call...Inside %s call\n", __FUNCTION__);

    // Access to user space data
    result = copy_to_user((void *)buf, (const void *)Kbuff, sizeof(Kbuff));
    if(result == 0) {
        pr_info("Message to user app >>>%s<<<\n",buf);
        pr_info("Data sent completely\n");
        retval = len;
        return retval;
    }
    else if(result > 0) {
        pr_info("Message to user app >>>%s<<<\n", buf);
        pr_info("only some part of data has been sent\n");
        retval = (len-result);
        return retval;
    }
    else {
        pr_info("Error in Read in kernel space\n");
        retval = -EFAULT;
        return retval;
    }

    return len;
}


/***********************************Start of Init Module**********************************************/
static int __init charDev_fops_init(void) {

    printk(KERN_ALERT "Inserting charDev_fops into Kernel\n");
    /**
     * @brief Allocating Major number Dynamically
     * int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);
     */
    if(alloc_chrdev_region(&mydev,0,1,"charDev_fops") < 0) {
        pr_err("Cannot allocate Major number\n");
        return -1;
    }

    pr_info("Device Number allocated successfully:\n Major = %d Minor = %d\n", MAJOR(mydev), MINOR(mydev));

    // Creating cdev structure
    cdev_init(&my_cdev, &fops);

    // Adding charDevice to the system
    if((cdev_add(&my_cdev,mydev,1)) < 0) {
        pr_err("Cannot add device to the syste\n");
        return -1;
    }

    // Creating struct Class for Device
    if((dev_class = class_create(THIS_MODULE,"charDev_class")) == NULL) {
        printk(KERN_ERR "Cannot create the struct class for device\n");
        goto r_class;
    }

    /**
     * @brief Creating Device File
     * struct device *device_create (struct *class, struct device *parent, 
     *                               dev_t dev, void * drvdata, const char *fmt, ...);
     */
    if((device_create(dev_class,NULL,mydev,NULL,"charDev_device")) == NULL) {
        pr_err("Cannot create the Device\n");
        goto r_device;
    }

    pr_alert("Kernel Module Inserted Successfully\n");

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
static void __exit charDev_fops_exit(void) {

    printk(KERN_ALERT "Removing charDev_fops from Kernel\n");
    cdev_del(&my_cdev);                             // To remove charDevice from the system
    device_destroy(dev_class,mydev);                // To destroy char device 
    class_destroy(dev_class);                       // To destroy device class
    unregister_chrdev_region(mydev,1);              // To unregister Device Number 
    pr_info("Kernel Module Removed Successfully\n");
}
/*************************************End of Cleanup Module****************************************/

module_init(charDev_fops_init);         // driver initialization entry point
module_exit(charDev_fops_exit);         // driver exit entry point