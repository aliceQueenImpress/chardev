obj-m += chardev.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: module app

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

app: userspace_test.c
	gcc userspace_test.c -o test_fan

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f test_fan
