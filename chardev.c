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

#include <linux/module.h>
#include <linux/version.h>
#include <linux/module.h>
#include <asm/errno.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/spinlock.h>

#include "asm-generic/errno-base.h"
#include "asm-generic/ioctl.h"
#include "chardev.h"
#include "ioctl.h"

#include "linux/export.h"
#include "linux/fs.h"
#include "linux/gfp_types.h"
#include "linux/types.h"

struct fan_chardev_struct fan_data={0};
struct fan_device *fan_data_wrap={0};

static int set_profile(struct  fan_device * fan_data_wrap,int32_t speed,int32_t auto_mode,char * profile){
    //to set profile : default profile

    if (!fan_data_wrap){
        pr_err("null pointer for fan_data_wrap in %s\n",__func__);
        return -EFAULT;
    }

    if(strlen(profile)>15){
        pr_err("profile size out of bound\n");
        return -EINVAL;
    }

    write_lock(&fan_data_wrap->lock);

    fan_data_wrap->config.speed=speed;
    fan_data_wrap->config.auto_mode=auto_mode;
    strscpy(fan_data_wrap->config.profile, profile, sizeof(fan_data_wrap->config.profile));

    write_unlock(&fan_data_wrap->lock);

    pr_info("set profile: speed=%d | auto_mode=%s | profile='%s'\n",speed,(auto_mode)?"on":"off",profile);

    return 0;
}

static long fan_unlocked_ioctl(struct file * fops,unsigned int nr,unsigned long arg){

    //checkpoint
    if (_IOC_TYPE(nr)!= IOC_MAGIC){
        return -ENOTTY;
    }

    if (_IOC_NR(nr) > IOCTL_VAL_MAXNR){
        return -ENOTTY;
    }

    if (_IOC_NR(nr) & _IOC_READ){
        if(!access_ok((void  __user *) arg, _IOC_SIZE(nr))){
            return -EFAULT;
        }
    }

    if (_IOC_DIR(nr) & _IOC_WRITE){
        if (!access_ok((void  __user *) arg, _IOC_SIZE(nr))){
            return -EFAULT;
        }
    }


    switch (nr) {
        case FAN_IOCTL_RESET:
        break;

        case FAN_IOCTL_GET_CONFIG:
        break;

        case FAN_IOCTL_SET_CONFIG:

        break;

        default: pr_warn("invalid value\n");
    }

}

static  int fan_open(struct inode *inode, struct file* fops){
    //initialization of fan data by default:
    struct fan_device *device_data;
    int ret = 0;

    pr_alert("%s call.\n",__func__);

    device_data = kmalloc(sizeof(struct fan_device),GFP_KERNEL);

    if(device_data==NULL){
        ret= -ENOMEM;
        goto out;
    }

    //init the lock
    rwlock_init(&fan_data_wrap->lock);

    //set config on default
    ret = set_profile(fan_data_wrap,DEFLT_SPEED,DEFLT_AUTO_MODE,DEFLT_PROFILE);
    if(ret){
        goto out;
    }

    fops->private_data=device_data;

    pr_info("%s char device initialized successfully...\n",DRIVER_NAME);

out:
    return ret;
}

static ssize_t fan_read(struct file * fops,char __user * buffer, size_t count, loff_t * offset){

}

static int fan_release (struct inode * inode, struct file *fops){

    pr_alert("%s call.\n",__func__);

    if(fops->private_data){
        kfree(fops->private_data);
        fops->private_data=NULL;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open  = fan_open,
    .release = fan_release,
    .read = fan_read,
    .unlocked_ioctl=fan_unlocked_ioctl,
};

static int  __init fan_init(void){



    pr_info("%s char device successfully load...\n",DRIVER_NAME);
    return 0;
}

static void __exit fan_exit(void){
    pr_info("%s unload from system..\n",DRIVER_NAME);
}


module_init(fan_init);
module_exit(fan_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("");
