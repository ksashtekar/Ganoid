#ifndef _KBD_HANDLER_H
#define _KBD_HANDLER_H



int init_kbd_handler (void);
void flush_kbd_buffer (void);
int read_key (int *key, bool *printable);


#endif
