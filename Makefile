obj-m := scull.o
KERNEL_DIR := /usr/src/linux-3.9.5/
PWD := $(shell pwd)
all:
	make -C  $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	rm *.o *.ko
