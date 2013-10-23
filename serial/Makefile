# Makefile – makefile of our first driver
 
# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
    obj-m := driver.o
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
    KERNEL_SOURCE := /usr/src/linux
    PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

install:
	sudo rmmod 8250_pnp && sudo rmmod 8250 && sudo insmod driver.ko

reinstall:
	sudo rmmod driver && sudo insmod driver.ko
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean

endif
