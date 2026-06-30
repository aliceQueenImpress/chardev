#ifndef IOCTL_H
#define IOCTL_H

#include "chardev.h"
#include <linux/ioctl.h>

#define DRIVER_NAME "FAN"

#define IOCTL_VAL_MAXNR 3

#define IOC_MAGIC '\x76' //'v'

#define FAN_IOCTL_RESET         _IO(IOC_MAGIC,0)
#define FAN_IOCTL_SET_CONFIG    _IOW(IOC_MAGIC,1,struct fan_chardev_struct )
#define FAN_IOCTL_GET_CONFIG    _IOR(IOC_MAGIC,2,struct fan_chardev_struct)


#endif /*IOCTL_H*/
