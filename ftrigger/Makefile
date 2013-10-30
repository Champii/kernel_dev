# Makefile – makefile of our first driver

# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
    obj-m 		+=	ftrigger_mod.o
    ftrigger_mod-objs	:= 	protocole.o 	\
    				socket_helper.o	\
				ftrigger.o 			\
				str_to_word_tab.o

# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
    KERNEL_SOURCE := /usr/src/linux
    PWD := $(shell pwd)

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

install:
	sudo insmod ftrigger_mod.ko && sudo mount -t ftrigger none /mnt/ftrigger

uninstall:
	sudo umount /mnt/ftrigger; sudo rmmod ftrigger_mod

reinstall: uninstall install

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean

re: clean default

endif
