#include <common.h>
#include <kernel.h>
#include <string.h>
#include <apic.h>
#include <timer.h>

const unsigned char icw1 = ((0x10) |
			    EDGE_TRIG |
			    CALL_ADDR_INTERVAL_8 | CASCADE_MODE | ICW4_NEEDED);

/*int temp1, temp2; */

int init_APIC(void)
{
	/* start the initialization sequence */
	outb(0x20, 0x11);
	delay(2000);
	outb(0xA0, 0x11);
	delay(2000);

	/* t3 - t7 of the vector address */
	outb(0x21, 0x20);
	delay(2000);
	outb(0xA1, 0x28);
	delay(2000);

	/* cascade configuration */
	outb(0x21, 0x04);
	delay(2000);
	outb(0xA1, 0x02);	/* slave id */
	delay(2000);

	/* general configuration */
	outb(0x21, 0x01);
	delay(2000);
	outb(0xA1, 0x01);
	delay(2000);

	/* mask all interrupts for now */
	outb(0x21, 0x00);
	delay(2000);
	outb(0xA1, 0x00);	/* all except cascaded one */
	delay(2000);

	return 0;
}

void read_int_request_reg(int *irr_val_master, int *irr_val_slave)
{
	/**irr_val_master = inb (0x21); */
	/*      *irr_val_slave = inb (0xA1); */

	/* send ocw3 to master */
	outb(0x20, 0x02);
	delay(2000);
	*irr_val_master = inb(0x20);
	delay(2000);

	/* and to the slave */
	outb(0xA0, 0x02);
	delay(2000);
	*irr_val_slave = inb(0xA0);

	printk("M_IRR = %x S_IRR = %x\n", *irr_val_master, *irr_val_slave);
}

void read_in_service_reg(int *isr_val_master, int *isr_val_slave)
{
	/* send ocw3 to master */
	outb(0x20, 0x01);
	delay(2000);
	*isr_val_master = inb(0x20);
	delay(2000);

	/* and to the slave */
	outb(0xA0, 0x01);
	delay(2000);
	*isr_val_slave = inb(0xA0);

	printk("M_ISR = %x S_ISR = %x\n", *isr_val_master, *isr_val_slave);
}
