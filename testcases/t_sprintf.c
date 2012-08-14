#include <ganoid/types.h>
#include <stdarg.h>
#include <kernel.h>
#include <vga.h>
#include <clib.h>
#include <string.h>

#include <tests/t_sprintf.h>

int Test_sprintf(int param1 __attribute__ ((unused)),
		 int param2 __attribute__ ((unused)),
		 void *ptr __attribute__ ((unused)))
{
	/*  int i = -12345; */

	/*  const char teststring1[] = "Welcome to Ufeox!"; */
	const char teststring2[] = "Unix";
	/*  const char teststring3[] = "Power Management"; */

	char buffer[100];
	vga_puts("\nsprintf test::\n\n");

	/* ============================================================ */
	/* %u */
	/* ------- */
	sprintf(buffer, "My name is |%*d|\n", 100);
	vga_puts(buffer);
	/* ============================================================ */

	/* ============================================================ */
	/* all together */
	/* ------- */
	sprintf(buffer, "My name is %s\n"
		"My age is %d\n" "100 in hex is 0x%x", teststring2, 20, 100);
	vga_puts(buffer);
	/* ============================================================ */

	return 0;
}
