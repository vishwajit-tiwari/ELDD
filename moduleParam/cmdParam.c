#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
/*#include<linux/moduleparam.h>*/


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishwajit Tiwari");
MODULE_DESCRIPTION("Module Parameter from Command Line");

// Variables to store command line arguments
static char *charvar = "module";
static int intvar = 10;
static int array[20];
// static bool statement = TRUE;

// Macro to enable command line arguments
module_param(charvar,charp,S_IRUGO);
module_param(intvar,int,S_IRUGO);
// module_param_array(name,type,number,permission);
module_param_array(array,int,NULL,S_IRUGO);
// module_param(statement,bool,S_IRUGO);

// Module Entry
static int cmdParam_init(void) {

    printk(KERN_ALERT "Inserting cmdParam into Kernel\n");
    pr_info("We are in init function\n");
    pr_info("The value of charvar is %s\n", charvar);
    pr_info("The value of intvar = %d\n", intvar);
    // pr_info("The Value of boolean = %s", statement);

    return 0;

}

// Module Exit
static void cmdParam_exit(void) {

    printk(KERN_ALERT "Removing cmdParam from Kernel\n");
}

module_init(cmdParam_init);     // Insert Module into Kernel
module_exit(cmdParam_exit);     // Remove Module from Kernel