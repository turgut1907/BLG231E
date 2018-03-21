	sudo insmod ./messagebox.ko
	sudo mknod /dev/messagebox c 247 0
	sudo chmod 0666 /dev/messagebox