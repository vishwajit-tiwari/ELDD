#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/types.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("Module Program for static Device File");

// Creating Device Number Dynamically
dev_t mydev = 0;

// Module Entry
static int __init staticDevFile_init(void) {

    printk(KERN_ALERT "Inserting Module into Kernel\n");
    
    // Allocating Major number Dynamically
    if(alloc_chrdev_region(&mydev,0,1,"myCharDevice") < 0) {
        printk(KERN_ERR "Cannot allocate Major number\n");
        return -1;
    }

    printk(KERN_INFO "Device Number allocated successfully:\n Major = %d Minor = %d\n", MAJOR(mydev), MINOR(mydev));

    return 0;
}
static void __exit staticDevFile_exit(void) {

    printk(KERN_ALERT "Removing Module from Kernel\n");
    // De-allocating Major Minor number 
    unregister_chrdev_region(mydev,1);
    pr_info("De-allocating Major Minor number\n");
}

module_init(staticDevFile_init);
module_exit(staticDevFile_exit);
