#ifndef FANDEV_H
#define FANDEV_H

#include <linux/spinlock.h>
#include "chardev.h"

struct fan_device{
    struct fan_chardev_struct config;
    rwlock_t lock;
};

#endif
