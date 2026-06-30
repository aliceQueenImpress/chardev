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
#include "asm/uaccess.h"
#include "chardev.h"
#include "ioctl.h"

#include "linux/export.h"
#include "linux/fs.h"
#include "linux/gfp_types.h"
#include "linux/types.h"

static struct fan_device *device_data;

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

    pr_alert("set profile: speed=%d | auto_mode=%s | profile='%s'\n",speed,(auto_mode)?"on":"off",profile);

    write_lock(&fan_data_wrap->lock);

    fan_data_wrap->config.speed=speed;
    fan_data_wrap->config.auto_mode=auto_mode;
    strscpy(fan_data_wrap->config.profile, profile, sizeof(fan_data_wrap->config.profile));

    write_unlock(&fan_data_wrap->lock);

    pr_info("new profile setted on success\n");

    return 0;
}

static long fan_unlocked_ioctl(struct file * fops,unsigned int nr,unsigned long arg){

    struct fan_device * device_data = fops->private_data;
    struct fan_chardev_struct data ;
    int ret =0;

    //checkpoint
    if (_IOC_TYPE(nr)!= IOC_MAGIC){
        return -ENOTTY;
    }

    if (_IOC_NR(nr) > IOCTL_VAL_MAXNR){
        return -ENOTTY;
    }

    if (!access_ok((void __user *)arg, _IOC_SIZE(nr) )){
        return -EFAULT;
    }

    switch (nr) {
        case FAN_IOCTL_RESET:
            ret = set_profile(device_data, DEFLT_SPEED, DEFLT_AUTO_MODE, DEFLT_PROFILE);
            if (ret){
                return ret; //on error;
            }
        break;

        case FAN_IOCTL_GET_CONFIG:
            read_lock(&device_data->lock);
            data = device_data->config;
            read_unlock(&device_data->lock);

            if(copy_to_user((int __user *)arg,&data,sizeof(data))){
                pr_alert("copy data to user failed\n");
                return -EFAULT;
            }
        break;

        case FAN_IOCTL_SET_CONFIG:
            if(copy_from_user(&data,(int __user *)arg,sizeof(struct fan_chardev_struct))){
                return -EFAULT;
            }

            if(data.speed<0 || data.speed>100){
                pr_err("speed  valaue out of bound with value :%d\n",data.speed);
                return -EINVAL;
            }

            ret = set_profile(device_data,data.speed,data.auto_mode,data.profile);

            if (ret){
                return ret; //on error;
            }

        break;

        default: pr_warn("invalid value\n");
            return -ENOTTY;
    }

    return 0;
}

static  int fan_open(struct inode *inode, struct file* fops){
    //initialization of fan data by default:
    pr_alert("%s call.\n",__func__);

    fops->private_data = device_data;

    pr_info("%s char device opened successfully...\n",DRIVER_NAME);

    return 0;
}

static ssize_t fan_read(struct file * fops,char __user * buffer, size_t count, loff_t * offset){
    //TODO: implement the complete for any eg: cat /dev/fan ops
    pr_info("try to read in %s\n",__func__);

    return 0;

    /*static ssize_t fan_read(struct file *fops, char __user *buffer, size_t count, loff_t *offset) {
    struct fan_device *dev = fops->private_data;
    struct fan_chardev_struct local_config;
    char kernel_buf[128];
    int len;

    // 1. Si l'utilisateur a déjà tout lu (offset > 0), on signale la fin du fichier (EOF)
    if (*offset > 0) {
        return 0;
    }

    // 2. On récupère les données sous verrou de lecture de manière ultra-rapide
    read_lock(&dev->lock);
    local_config = dev->config;
    read_unlock(&dev->lock);

    // 3. On formate une jolie chaîne de caractères textuelle
    len = scnprintf(kernel_buf, sizeof(kernel_buf),
                    "Status: Vitesse=%d%% | Mode=%s | Profil='%s'\n",
                    local_config.speed,
                    local_config.auto_mode ? "Auto" : "Manuel",
                    local_config.profile);

    // 4. Si l'utilisateur demande moins d'octets que ce qu'on a préparé, on s'aligne
    if (count < len) {
        len = count;
    }

    // 5. On envoie le texte vers l'espace utilisateur
    if (copy_to_user(buffer, kernel_buf, len)) {
        return -EFAULT;
    }

    // 6. On met à jour l'offset pour que le prochain appel sache qu'on a déjà écrit
    *offset += len;

    // On retourne le nombre d'octets lus
    return len;
}*/
}

static int fan_release (struct inode * inode, struct file *fops){

    pr_alert("%s call.\n",__func__);

    //don't free memory here for multi-open secure

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
 int ret = 0;

    // CORRECTION : Allocation unique au chargement du module
    device_data = kmalloc(sizeof(struct fan_device), GFP_KERNEL);
    if (!device_data) {
        return -ENOMEM;
    }

    rwlock_init(&device_data->lock);

    ret = set_profile(device_data, DEFLT_SPEED, DEFLT_AUTO_MODE, DEFLT_PROFILE);
    if (ret) {
        kfree(device_data);
        return ret;
    }

    // C'est ici que tu mettras ton register_chrdev / cdev_add plus tard !

    pr_info("%s char device successfully load...\n", DRIVER_NAME);
    return 0;
}

static void __exit fan_exit(void){
    if (device_data) {
        kfree(device_data);
    }
    pr_info("%s unload from system..\n", DRIVER_NAME);
}

module_init(fan_init);
module_exit(fan_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fan Controller Character Device Driver");
