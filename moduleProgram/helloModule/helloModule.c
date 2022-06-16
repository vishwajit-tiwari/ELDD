#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

MODULE_AUTHOR("Vishwajit Tiwari");
MODULE_LICENSE("GPL");          // Tells Module bear free License
MODULE_DESCRIPTION("Hello World Module");

static int helloModule_init(void) {

    printk(KERN_ALERT "Hello World!\nInserting Module into Kernel\n");
    return 0;
}

static void helloModule_exit(void) {
    
    printk(KERN_ALERT"Good bye...\nRemoving Module from Kernel\n");
}

module_init(helloModule_init);  // Insert Module into Kernel
module_exit(helloModule_exit);  // Remove Module from Kernel