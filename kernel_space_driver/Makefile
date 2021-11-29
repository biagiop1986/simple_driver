LINUX_SOURCES=/usr/src/linux-headers-$(shell uname -r)
obj-m := simple_driver.o

all:
	make -C $(LINUX_SOURCES) M=$(PWD) modules
clean:
	make -C $(LINUX_SOURCES) M=$(PWD) clean
	
