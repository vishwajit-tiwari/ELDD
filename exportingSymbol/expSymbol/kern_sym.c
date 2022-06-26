#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("Exporting Symbol to Kernel-Symbol-Table so that Other Modules can use them");

// Function need to be exported
/*static*/ int my_add(int a, int b) {
    return (a+b);
}

// Command to export symbol to Kernel-Symbol-Table
EXPORT_SYMBOL_GPL(my_add);

static int kern_sym_init(void) {

    printk(KERN_ALERT "Inserting Kernel Symbol into Kernel\n");
    pr_info("Exporting my_add function into Kernel-Symbol-Table\n");

    return 0;
}

static void kern_sym_exit(void) {

    printk(KERN_ALERT "Removing Kernel Symbol from Kernel\n");
    pr_info("Removing my_add from Kernel-Symbol-Table\n");
}

module_init(kern_sym_init);
module_exit(kern_sym_exit);
