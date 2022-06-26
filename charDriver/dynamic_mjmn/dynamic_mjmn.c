#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>             // For dev_t
#include<linux/fs.h>
#include<linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("CharDriver to allocate Major & Minor number dynamically");

// Creating the Device number Dynamically
dev_t mydev = 0;    // Minor number

// Module init function
static int __init charDriver_init(void) {
    
    printk(KERN_ALERT "Inserting charDriver into Kernel\n");

    // Dynamically allocting Major number
    // int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);
    if(alloc_chrdev_region(&mydev,0,1,"myCharDeviceDynamic") < 0) {
        printk(KERN_ERR "Can not allocate Major number\n");
        return -1;
    }

    pr_info("Device number allocated successfully:\n Major = %d Minor = %d\n", MAJOR(mydev), MINOR(mydev));

    return 0;

}

// Module exit function
static void __exit charDriver_exit(void) {

    printk(KERN_ALERT "Removing charDriver from Kernel\n");
    // Free the Device number after use
    unregister_chrdev_region(mydev,1);
    pr_info("Device number De-allocated successfully\n");
}

module_init(charDriver_init);
module_exit(charDriver_exit);

