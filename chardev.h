#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/types.h>

struct fan_chardev_struct {
    int32_t speed; //percentage of the speed (0-100)
    int32_t auto_mode; //auto_matic mode 1 = auto, 0 = manual
    char profile[16]; //profile describing
};

#endif /*CHARDEV_H*/
