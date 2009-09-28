#ifndef VGA_H_
#define VGA_H_

#define VMEM_BASE 0x000B8000

#define TAB_WIDTH 4


#define SEQUENCER_ADDR_REG 0x3C4
#define SEQUENCER_RESET_INDEX 0x00
#define SEQUENCER_CLKMOD_INDEX 0x01

#define CRTCTRL_ADDR_REG  0x3D4
#define CRTCTRL_CURSORHIGH_INDEX 0x0E
#define CRTCTRL_CURSORLOW_INDEX  0x0F

#define ATTRB_MISC_OUTPUT_WR 0x3C2
#define ATTRB_MISC_OUTPUT_RD 0x3CC

void vga_clearscreen (void);


int ReadRegister (unsigned short aPortAddressIndex,
		  unsigned char aIndex,
		  unsigned short aPortAddressData,
		  unsigned char *aValue);

int WriteRegister (unsigned short aPortAddressIndex,
		  unsigned char aIndex,
		  unsigned short aPortAddressData,
		   unsigned char aValue);

void move_cursor (unsigned char x_pos, unsigned char y_pos);
void vga_putc (char c);
void vga_puts (const char *s);

/*
int SetBit (unsigned short aPortAddressIndex,
	    unsigned char aIndex,
	    unsigned short aPortAddressData,
	    unsigned short aBitNumber,
	    int aValue);
*/
#endif  // VGA_H_



