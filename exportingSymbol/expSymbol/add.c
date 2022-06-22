#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include "myHeader.h"           // Header file having Prototype of my_add function

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("This Module will use Exported my_add function to do add functionality");

static int a = 10;
static int b = 20;


static int add_init(void) {

    printk(KERN_ALERT "Inserting addModule into Kernel\n");
    pr_info("Using Exported symbol for addition of two numbers\n");
    pr_info("Sum of %d and %d is = %d\n", a, b, my_add(a,b));

    return 0;
}

static void add_exit(void) {

    printk(KERN_ALERT "Removing addModule from Kernel\n");
}

module_init(add_init);
module_exit(add_exit);
