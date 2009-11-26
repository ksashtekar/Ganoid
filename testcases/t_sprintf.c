#include <ktypes.h>
#include <stdarg.h>
#include <kernel.h>
#include <vga.h>
#include <clib.h>
#include <string.h>

#include <tests/t_sprintf.h>



int Test_sprintf (int param1, int param2, void *ptr)
{
  int i = -12345;

  const char teststring1[] = "Welcome to Ufeox!";
  const char teststring2[] = "Unix";
  const char teststring3[] = "Power Management";

  char buffer[100];
  vga_puts ("\nsprintf test::\n\n");

  /*
  // ============================================================
  // Test %d
  // -------
  //i = 214748364;
  //i = 999999996;
  //= 1000000000;
  //i = 0x7ffffff7;
  i = -10;
  while (i != -11) {//i != 0){
    sprintf (buffer, "%d\n", i);
    vga_puts (buffer);
    if (atoi (buffer) != i){ // check if the conversion is correct
      vga_puts (buffer);
      vga_puts ("- sprintf error\n");
      while (1);
      }
    //vga_puts ("...Ok \n");
    delay ();
    i++; 
  }
  // ============================================================
  */

  /*
  // ============================================================
  // Test %s
  // -------
  sprintf (buffer, "\n\t\t%s\n\n-This Operating System is based on %s\n"
                               "-It's speciality is %s\n", teststring1, teststring2, teststring3);
  vga_puts (buffer);
  // ============================================================
  */

  /*
  // ============================================================
  // %x test
  // -------
  //sprintf (buffer, "hex string : %x\n", 0x55);
  //vga_puts (buffer);
  // ============================================================
  */

  // ============================================================
  // %u
  // -------
  sprintf (buffer, "My name is |%*d|\n", 100);
  vga_puts (buffer);
  // ============================================================
  while (1);

  // ============================================================
  // all together
  // -------
  sprintf (buffer, "My name is %s\n"
	   "My age is %d\n"
	   "100 in hex is 0x%x", teststring2, 20, 100);
  vga_puts (buffer);
  // ============================================================
  while (1);

  return 0;
}
