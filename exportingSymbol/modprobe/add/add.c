#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishwajit Tiwari");

/*static*/ int my_add(int a,int b) {
    return (a+b);
}
/*static*/ int my_sub(int a,int b) {
    return (a-b);
}

EXPORT_SYMBOL_GPL(my_add);
EXPORT_SYMBOL_GPL(my_sub);

static int add_init(void) {

    printk(KERN_ALERT "Inserting addModule into Kernel\n");

    return 0;
}

static void add_cleanup(void) {

    printk(KERN_ALERT "Removing addModule from Kernel\n");

}

module_init(add_init);
module_exit(add_cleanup);