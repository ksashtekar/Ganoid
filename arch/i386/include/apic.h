#ifndef APIC_H_
#define APIC_H_


// ----------------------------
// Initialization command word 1
#define ICW4_NEEDED (1 << 0)
#define CASCADE_MODE (0 << 1)
#define CALL_ADDR_INTERVAL_8 (0<<2)
#define EDGE_TRIG (0 << 3)

#define ICW1 ((0x10) | EDGE_TRIG | CALL_ADDR_INTERVAL_8 | CASCADE_MODE | ICW4_NEEDED)
// ----------------------------

int init_APIC (void);
void read_int_request_reg (int *irr_val_master, 
			   int *irr_val_slave);
void read_in_service_reg (int *isr_val_master, 
			  int *isr_val_slave);

#endif
