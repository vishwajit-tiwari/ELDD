/**
 * @file ioctlDriver.c
 * @author Vishwajit Kumar Tiwari (tvishwajit@cdac.in)
 * @brief A simple IOCTL driver to Communicate with User Space Application
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/**
 * @brief IOCTL is referred to as Input and Output Control, 
 * which is used to talking to device drivers.
 * This system call, is available in most driver categories. 
 * The major use of this is in case of handling some specific operations of a device
 * for which the kernel does not have a system call by default.
 * 
 * Some real-time applications of ioctl are Ejecting the media from a “cd” drive, 
 * change the Baud Rate of Serial port, Adjust the Volume, Reading or Writing device registers, etc. 
 * We already have the write and read function in our device driver. But it is not enough for all cases.
 */

/*Steps involved in IOCTL

There are some steps involved to use IOCTL.

    Create IOCTL command in driver
    Write IOCTL function in the driver
    Create IOCTL command in a Userspace application
    Use the IOCTL system call in a Userspace
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
#include<linux/ioctl.h>             // for IOCTL operations

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("A CharDriver Prog for IOCTL implementation");

/*Creating IOCTL Command In Driver*/
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

/*For name of driver & memory size*/
#define NAME RW_CharDriver
#define MEM_SIZE 1024

uint8_t *kernel_buffer;                 // for kmalloc()
int32_t value = 0;                      // for IOCTL

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
static long     charDev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


// File Operations Structure
struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = charDev_read,
    .write          = charDev_write,
    .open           = charDev_open,
    .unlocked_ioctl = charDev_ioctl,
    .release        = charDev_release,

};

/*This function will be called when we open the Device file*/
static int charDev_open(struct inode *pinode, struct file *pfile) 
{
    pr_alert("Driver Open function called....!!!\n");
    printk(KERN_INFO "Device file opened....!!!\n");
    return 0;
}

/*This function will be called when we close the Device file*/
static int charDev_release(struct inode *pinode, struct file *pfile) 
{
    pr_alert("Driver Release function called......!!!\n");
    printk(KERN_INFO "Device File Closed...!!!\n");
    
    return 0;
}

/*This function will be called when we Write to the Device file*/
static ssize_t charDev_write(struct file *filp, const char *Ubuf, size_t len, loff_t *offp) 
{
    printk(KERN_ALERT "This is the kernel write call...Inside %s call\n", __FUNCTION__);
    pr_info("Driver Write function called.........!!!\n");
    
    return len; 
}

/*This function will be called when we Read through Device File*/
static ssize_t charDev_read(struct file *filp, char __user *Ubuf, size_t len, loff_t *offp) 
{
    printk(KERN_ALERT "This is the kernel read call...Inside %s call\n", __FUNCTION__);
    pr_info("Driver Read function called...........!!!\n");

    return 0;
}

/*This function will be called when we write IOCTL on the Device file*/
static long charDev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd) 
    {
        case WR_VALUE:
            if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
            {
                pr_err("Data Write : Err!\n");
            }
            pr_info("Value = %d\n", value);
            break;

        case RD_VALUE:
            if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
            {
                pr_err("Data Read : Err!\n");
            }
            break;

        default:
            pr_info("Default\n");
            break;
    }
    
    return 0;

}


/**********************************Start of Init Module****************************************/
static int __init charDev_fops_init(void) 
{
    printk(KERN_ALERT "Inside %s Module\n", __FUNCTION__);
    printk(KERN_INFO "Inserting Device Driver into Kernel\n");
    
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
        pr_err("Cannot add device to the system\n");
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

    pr_info("Device Driver Inserted... Successfully\n");

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
    printk(KERN_INFO "Removing charDev_fops from Kernel\n");
    device_destroy(dev_class,mydev);                // To destroy char device 
    class_destroy(dev_class);                       // To destroy device class
    cdev_del(&my_cdev);                             // To remove charDevice from the system
    unregister_chrdev_region(mydev,1);              // To unregister Device Number 
    pr_info("Device Driver removed... Successfully\n");
}
/*************************************End of Cleanup Module****************************************/

module_init(charDev_fops_init);         // driver initialization entry point
module_exit(charDev_fops_exit);         // driver exit entry point