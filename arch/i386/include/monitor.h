#ifndef MONITOR_H_
#define MONITOR_H_


#define VGA_CONTROLLER_CURSORLOC_PORT  0x3D4
#define VGA_CONTROLLER_DATA_ADDR 

// print single character.
void monitor_put(char c);

// clear the screen.
void monitor_clear ();
 
// write a string to the display.
void monitor_write (char *c);

#endif // MONITOR_H_
