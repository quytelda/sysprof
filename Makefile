ccflags-y := -std=gnu99 -Wno-declaration-after-statement

obj-m += sysprof.o
sysprof-objs := module/sysprof.o module/data/netfilter.o module/shmem.o module/register.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

gather:
	gcc -o gather -Wall -lm daemon/gather.c
