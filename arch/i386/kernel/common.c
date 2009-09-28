#include <common.h>

void outb (unsigned short port, unsigned char value)
{
  asm volatile ("outb %0, %1": : "a"(value), "dN"(port));
}

unsigned char inb (unsigned short port)
{
  unsigned char ret;
  asm volatile ("inb %1, %0": "=a"(ret): "dN"(port));
  return ret;
}

unsigned short inw (unsigned short port)
{
  unsigned short ret;
  asm volatile ("inw %1, %0": "=a"(ret): "dN"(port));
  return ret;
}
