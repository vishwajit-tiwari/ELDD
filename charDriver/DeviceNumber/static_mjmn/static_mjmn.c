#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("Char Driver For Major & Minor Numbers");

// Creating the Device number with our custom Major & Minor numbner
dev_t mydev = MKDEV(220,0);

// Module init function
static int __init charDriver_init(void) {
    
    printk(KERN_ALERT "Inserting charDriver into Kernel\n");

    // Statically register the Device number
    if(register_chrdev_region(mydev,1,"myCharDevice") < 0) {
        printk(KERN_ERR "Can not allocate Major number\n");
        printk(KERN_INFO "Cannot allocate Major number\n");
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

