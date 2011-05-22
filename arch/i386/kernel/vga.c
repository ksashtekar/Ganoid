#include <vga.h>
#include <common.h>
#include <ktypes.h>

u8 cursor_x, cursor_y;
const u8  max_x = SCREEN_WIDTH, max_y = SCREEN_HEIGHT;
const int KTabWidth = KTAB_WIDTH;


static void vga_scroll_up (void);

void vga_clearscreen ()
{
  unsigned short *vptr = (unsigned short *)VMEM_BASE;
  unsigned char vchar = ' ';
  unsigned char backcolor = 0;
  unsigned char forecolor = 15;
  unsigned char vattr = (u8)(backcolor << 4) | (forecolor);

  unsigned short vval = (u16)((u16)(vattr << 8) | (vchar));

  int cnt;
  cnt  = max_x * max_y;
  int offset = 0;

  vval = (u16)((vattr << 8) | (vchar));
  for (offset = 0; offset < cnt; offset++)
    *(volatile unsigned short*)(vptr + offset) = vval;
  
  cursor_x = 0;
  cursor_y = 0;
  move_cursor (cursor_x, cursor_y);
}

/** \brief Scroll the screen up
 */
static void vga_scroll_up (void)
{
  unsigned short *vptr = (unsigned short *)VMEM_BASE;
  unsigned char backcolor = 0;
  unsigned char forecolor = 15;
  unsigned char vattr = (u8)(backcolor << 4) | (forecolor);

  unsigned short vval = (u8)(vattr << 8) | (' ');

  int rows, col;
  for (rows = 0; rows < (max_y-1); rows++){
    for (col = 0; col < max_x; col++)
      *(volatile unsigned short*)(vptr + (max_x*rows) + col) = 
	*(volatile unsigned short*)(vptr + (max_x*(rows+1)) + col);
  }

  for (col = 0; col < max_x; col++){
    *(volatile unsigned short*)(vptr + (max_x*(max_y-1) + col)) = vval;
  }

  cursor_x = 0;
  cursor_y = (u8)(max_y - 1);
  move_cursor (cursor_x, cursor_y);

}

void vga_puts (const char *s)
{

  while(*s)
    vga_putc (*s++);
}


void vga_putc (const char c)
{
  unsigned short *vptr = (unsigned short *)VMEM_BASE;
  unsigned char backcolor = 0;
  unsigned char forecolor = 15;
  unsigned char vattr = (u8)((backcolor << 4) | (forecolor));

  unsigned short vval = (u16)((vattr << 8) | (c));

  unsigned int location_offset; 

  if (cursor_y == max_y){
    vga_scroll_up ();
  }

  location_offset = (unsigned)((cursor_y * 80) + cursor_x);


  switch (c){
  case '\n':
    cursor_x = 0;
    cursor_y++;
    if (cursor_y != max_y)
      move_cursor (cursor_x, cursor_y);
    break;
  case '\t':
      cursor_x = (u8)(cursor_x + KTabWidth);
    if (cursor_x >= 80){
      cursor_x = 0;
      cursor_y++;
    }
    if (cursor_y >= max_y)
      vga_scroll_up ();
    else
      move_cursor (cursor_x, cursor_y);
    break;
  case '\r': // carriage return
    cursor_x = 0;
    cursor_y++;
    move_cursor (cursor_x, cursor_y);
    break;
  case '\b':
      if(cursor_x > 0) {
	  cursor_x--;
	  move_cursor(cursor_x, cursor_y);
      }
    break;
  default:
    *(volatile unsigned short*)(vptr + location_offset) = vval;
    cursor_x++;
    if (cursor_x == max_x){
      cursor_x = 0;
      cursor_y++;
    }
    /*
    if (cursor_y >= max_y)
      vga_scroll_up ();
      else*/
    if (cursor_y != max_y)
      move_cursor (cursor_x, cursor_y);
    break;
  }
}


/** \brief Read from the VGA controller register
 * 
 */
int ReadRegister (unsigned short aPortAddressIndex,
		  unsigned char aIndex,
		  unsigned short aPortAddressData,
		  unsigned char *aValue)
{
  outb (aPortAddressIndex, aIndex);
  *aValue  = inb(aPortAddressData);
  return 0;
}

/** \brief Write to a VGA controller register
 * 
 */
int WriteRegister (unsigned short aPortAddressIndex,
		  unsigned char aIndex,
		  unsigned short aPortAddressData,
		  unsigned char aValue)
{
  outb (aPortAddressIndex, aIndex);
  outb (aPortAddressData, aValue);
  return 0;
}

/*
int SetBit (unsigned short aPortAddressIndex,
	    unsigned char aIndex,
	    unsigned short aPortAddressData,
	    unsigned short aBitNumber,
	    bool aValue)
{
  unsigned char val;
  unsigned char mask;
  outb (aPortAddressIndex, aIndex);
  val = inb (aPortAddressData);
  mask = 0x01 << aBitNumber;
  val = (val & ~mask);
  if(aValue) // set the bit if required
    val = (val | mask);
  outb (aPortAddressIndex, aIndex);
  outb (aPortAddressData, val);
  return 0;
}

*/



void move_cursor (unsigned char x_pos, unsigned char y_pos)
{
    unsigned short vram_offset = (u16)((y_pos*80)+x_pos);
  WriteRegister(CRTCTRL_ADDR_REG,
		CRTCTRL_CURSORHIGH_INDEX,
		CRTCTRL_ADDR_REG+1,
		(unsigned char)((vram_offset & 0xFF00) >> 8));
  WriteRegister(CRTCTRL_ADDR_REG,
		CRTCTRL_CURSORLOW_INDEX,
		CRTCTRL_ADDR_REG+1,
		(unsigned char)(vram_offset & 0xFF));
  cursor_x = x_pos;
  cursor_y = y_pos;
}




