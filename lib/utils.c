#include <kernel.h>
#include <utils.h>

void print_human_readable_size(unsigned size)
{
	const unsigned KB = 1024;
	const unsigned MB = 1048576;
	const unsigned GB = 1073741824;

	if (size < KB)
		printk("%d bytes\n", size);
	else if (size < MB)
		printk("%d KB\n", size / KB);
	else if (size < GB)
		printk("%d MB\n", size / MB);
	else
		printk("%d GB\n", size / GB);
}
