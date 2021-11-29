// Thanks to https://studenti.fisica.unifi.it/~carla/manuali/linux_device_driver_tutorial.pdf

#include <linux/errno.h> /* error codes */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/fs.h> /* everything... */
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/types.h> /* size_t */
#include <linux/uaccess.h> /* copy_from/to_user */

/* License specification */
MODULE_LICENSE("Dual BSD/GPL");

/* Global variables of the driver */
/* Major number */
const int simple_driver_major = 60; /* local/experimental use */
const int simple_driver_minor = 0;
/* Buffer to store data */
char *simple_driver_buffer;
/* Buffer size */
const size_t simple_driver_buffer_size = sizeof(uint64_t);

/* Function declarations */
int simple_driver_init(void);
void simple_driver_exit(void);
int simple_driver_open(struct inode *inode, struct file *filp);
int simple_driver_release(struct inode *inode, struct file *filp);
ssize_t simple_driver_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t simple_driver_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

/* Structure that declares the usual file */
/* access functions */
struct file_operations simple_driver_fops = {
  read: simple_driver_read,
  write: simple_driver_write,
  open: simple_driver_open,
  release: simple_driver_release
};

/* Function implementations */
int simple_driver_init(void)
{
	int result;

	/* Register device */
	result = register_chrdev(simple_driver_major, "simple_driver", &simple_driver_fops);
	if (result < 0) 
	{
		printk("simple_driver: cannot obtain major number %d\n", simple_driver_major);    
		return result;
	}

	/* Allocating memory for the buffer */
	simple_driver_buffer = kmalloc(1, GFP_KERNEL); 
	if (!simple_driver_buffer) { 
		result = -ENOMEM;
		goto FAIL; 
	} 
	memset(simple_driver_buffer, 0, simple_driver_buffer_size);
	printk("simple_driver: Hello there!\n");
	return 0;

FAIL:
	simple_driver_exit();
	return result;
}

void simple_driver_exit(void)
{
	/* Unregister device */
	unregister_chrdev(simple_driver_major, "simple_driver");

	/* Free memory buffer */
	if(simple_driver_buffer)
		kfree(simple_driver_buffer);

	printk("simple_driver: Goodbye!\n");
}

int simple_driver_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int simple_driver_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t simple_driver_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	copy_to_user(buf, simple_driver_buffer, simple_driver_buffer_size);
	printk("simple_driver: read done\n");

	/* Changing reading position as best suits */ 
	if (*f_pos == 0)
	{ 
		*f_pos += simple_driver_buffer_size;
		return simple_driver_buffer_size;
	}
	else
	{ 
		return 0; 
	}
}

ssize_t simple_driver_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	if(count != simple_driver_buffer_size)
	{
		printk("simple_driver: trying to write %ld bytes, %ld required\n",
			count, simple_driver_buffer_size);
		return count;
	}

	copy_from_user(simple_driver_buffer, buf, simple_driver_buffer_size);
	printk("simple_driver: write done\n");

	return count;
}

module_init(simple_driver_init);
module_exit(simple_driver_exit);
