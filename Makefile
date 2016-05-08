ccflags-y := -std=gnu99 -Wno-declaration-after-statement -I $(src)/include

obj-m += sysprof.o
sysprof-objs := module/sysprof.o module/data/netfilter.o module/shmem.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
