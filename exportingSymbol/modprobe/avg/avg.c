#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

#include "../myHeader.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishwajit Tiwari");

static int a = 20;
static int b = 10;

// Take Command Line input
module_param(a,int,S_IRUGO);
module_param(b,int,S_IRUGO);

static int avg_init(void) {

    printk(KERN_ALERT "Inserting avgModule into Kernel\n");
    pr_info("Sum of %d & %d is  = %d\n", a, b, my_add(a,b));
    pr_info("Diff of %d & %d is = %d\n", a, b, my_sub(a,b));
    
    return 0;
}

static void avg_cleanup(void) {

    printk(KERN_ALERT "Removing avgModule from Kernel\n");

}

module_init(avg_init);
module_exit(avg_cleanup);