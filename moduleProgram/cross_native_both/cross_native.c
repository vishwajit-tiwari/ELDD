/**
 * @file cross_native.c
 * @author Vishwajit Tiwari (tvishwajit@cdac.in)
 * @brief  A simple module program supporting both native & cross.
 * @version 0.1
 * @date 2023-01-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_AUTHOR("Vishwajit Tiwari");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module program supporting both native & cross");
MODULE_INFO(board, "RASPBERRY PI 4");

static int entrymodule_init(void)
{
    printk(KERN_ALERT "Inside %s Module:\n", __FUNCTION__);
    pr_info("Inserting Module into Kernel\nHello!!!");
    return 0;
}
static void exitmodule_exit(void)
{
    printk(KERN_ALERT "Inside %s Module:\n", __FUNCTION__);
    pr_info("Removing Module from Kernel\nByee!!!!!");
}

module_init(entrymodule_init);           //Insert module into Kernel
module_exit(exitmodule_exit);            //Remove module from Kernel
