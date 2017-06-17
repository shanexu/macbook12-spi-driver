obj-m += applespi.o
obj-m += appletb.o
obj-m += debug_acpi.o

KVERSION := $(KERNELRELEASE)
ifeq ($(origin KERNELRELEASE), undefined)
KVERSION := $(shell uname -r)
endif
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

test: all
	sync
	-rmmod applespi
	insmod ./applespi.ko

debug: all
	sudo insmod ./debug_acpi.ko
	sudo rmmod debug_acpi
	dmesg | grep debug_acpi > debug.out
	@echo "output in debug.out"
