#ifndef INIT_H
#define INIT_H


void read_multiboot_information (u32 *multiboot_info_ptr);

int process_1 (void*);
int process_2 (void*);
void idle_task (void);


#endif // INIT_H
