# Makefile - makefile of our first driver

# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.

ifneq (${KERNELRELEASE},)
	obj-m	+=	netMalloc_mod.o
	netMalloc_mod-objs	:= netMalloc.o

# Otherwise we were called derectly from the command line.
# Invoke the kernel build system.
else
	KERNEL_SOURCE := /usr/src/linux-headers-3.2.0-4-486/
	PWD := $(shell pwd)

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
install:
	sudo insmod netMalloc_mod.ko
unstall:
	sudo rmmod netMalloc_mod

reinstall: unstall install

cleann:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
re: clean default

endif