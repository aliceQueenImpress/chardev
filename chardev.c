/**
 * @file cha rdev.c
 * @author alice (jdj17180@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-06-30
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/module.h>
#include <asm/errno.h>

#include "chardev.h"
#include "ioctl.h"


static int  __init fan_init(void){
    return 0;
}

static void __exit fan_exit(void){

}


module_init(fan_init);
module_exit(fan_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("");
