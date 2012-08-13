#include <ganoid/types.h>
#include <kbd-handler.h>
#include <isr.h>
#include <kernel.h>
#include <kbd.h>
#include <clib.h>

void kbd_isr(void);
unsigned char read_byte_from_dat_buf(void);

#define KBD_BUFFER_SIZE 10
static unsigned char key_buffer[KBD_BUFFER_SIZE] = { 0 };

static int head; /* always points to byte to be written */
static int tail; /* points to the last byte of the circular buffer */

static void keyboard_state_machine(unsigned char i, int *o,
				   bool *make, bool *repeat_key,
				   bool *basic_key);

struct keymap {
	int scan_code;
	int value;
};

static int binary_search_key(int scan_code, struct keymap *map,
			     int keymap_length, int *value);

#define KEY_F1 0x105
#define KEY_F2 0x106
#define KEY_F3 0x104
#define KEY_F4 0x10C
#define KEY_F5 0x103
#define KEY_F6 0x10B
#define KEY_F7 0x183
#define KEY_F8 0x10A
#define KEY_F9 0x101
#define KEY_F10 0x109
#define KEY_F11 0x178
#define KEY_F12 0x107

#define KEY_TAB 0x10D
#define KEY_LALT 0x111
#define KEY_LSHIFT 0x112
#define KEY_RSHIFT 0x159
#define KEY_LCTLR 0x114
#define KEY_CAPS 0x158

#define KEY_KP0 0x170
#define KEY_KP1 0x169
#define KEY_KP2 0x172
#define KEY_KP3 0x17A
#define KEY_KP4 0x16B
#define KEY_KP5 0x173
#define KEY_KP6 0x174
#define KEY_KP7 0x16C
#define KEY_KP8 0x175
#define KEY_KP9 0x17D
#define KEY_KPDOT 0x171
#define KEY_KPDIV 0x14A
#define KEY_KPSTAR 0x17C
#define KEY_KPMINUS 0x17B
#define KEY_KPPLUS 0x179
#define KEY_ESC    0x176
#define KEY_NUM    0x177

struct keymap basic_keymap[] = {
	{0x01, KEY_F9},
	{0x03, KEY_F5},
	{0x04, KEY_F3},
	{0x05, KEY_F1},
	{0x06, KEY_F2},
	{0x07, KEY_F12},
	{0x09, KEY_F10},
	{0x0A, KEY_F8},
	{0x0B, KEY_F6},
	{0x0C, KEY_F4},
	{0x0D, '\t'},
	{0x0E, '`'},
	{0x11, KEY_LALT},
	{0x12, KEY_LSHIFT},
	/*      {0x14, KEY_LCTRL},  */
	{0x15, 'Q'},
	{0x16, '1'},
	{0x1A, 'Z'},
	{0x1B, 'S'},
	{0x1C, 'A'},
	{0x1D, 'W'},
	{0x1E, '2'},
	{0x21, 'C'},
	{0x22, 'X'},
	{0x23, 'D'},
	{0x24, 'E'},
	{0x25, '4'},
	{0x26, '3'},
	{0x29, ' '},
	{0x2A, 'V'},
	{0x2B, 'F'},
	{0x2C, 'T'},
	{0x2D, 'R'},
	{0x2E, '5'},
	{0x31, 'N'},
	{0x32, 'B'},
	{0x33, 'H'},
	{0x34, 'G'},
	{0x35, 'Y'},
	{0x36, '6'},
	{0x3A, 'M'},
	{0x3B, 'J'},
	{0x3C, 'U'},
	{0x3D, '7'},
	{0x3E, '8'},
	{0x41, ','},
	{0x42, 'K'},
	{0x43, 'I'},
	{0x44, 'O'},
	{0x45, '0'},
	{0x46, '9'},
	{0x49, '.'},
	{0x4A, '/'},
	{0x4B, 'L'},
	{0x4C, ';'},
	{0x4D, 'P'},
	{0x4E, '-'},
	{0x52, '\''},
	{0x54, '['},
	{0x55, '='},
	{0x58, KEY_CAPS},
	{0x59, KEY_RSHIFT},
	{0x5A, '\n'},
	{0x5B, ']'},
	{0x5D, '\\'},
	{0x66, '\b'},
	{0x69, KEY_KP1},
	{0x6B, KEY_KP4},
	{0x6C, KEY_KP7},
	{0x70, KEY_KP0},
	{0x71, KEY_KPDOT},
	{0x72, KEY_KP2},
	{0x73, KEY_KP5},
	{0x74, KEY_KP6},
	{0x75, KEY_KP8},
	{0x76, KEY_ESC},
	{0x77, KEY_NUM},
	{0x78, KEY_F11},
	{0x79, KEY_KPPLUS},
	{0x7A, KEY_KP3},
	{0x7B, KEY_KPMINUS},
	{0x7C, KEY_KPSTAR},
	{0x7D, KEY_KP9},
	{0x83, KEY_F7}
};

/*
int extended_keymap[] = {
	0x1F, 0x14, 0x27, 0x11, 0x2F, 0x70, 0x6C, 0x7D, 0x71, 0x69, 0x7A, 0x75, 0x6B, 0x72, 0x74, 0x4A, 0x5A, 0x37, 0x3F, 0x5E, 0x4D, 0x15, 0x3B, 0x34, 0x23, 0x32, 0x21, 0x50, 0x48, 0x2B, 0x40, 0x10, 0x3A, 0x38, 0x30, 0x28, 0x20, 0x18};
*/

/*/================ Extended keymap ========================= */

#define KEY_MM_NEXT_TRACK 0x24D
#define KEY_MM_PREVIOUS_TRACK  0x215
#define KEY_MM_STOP 0x23B
#define KEY_MM_PLAY_PAUSE 0x234
#define KEY_MM_MUTE 0x223
#define KEY_MM_VOLUME_UP 0x232
#define KEY_MM_VOLUME_DOWN 0x221
#define KEY_MM_MEDIA_SELECT 0x250
#define KEY_MM_E_MAIL 0x248
#define KEY_MM_CALCULATOR 0x22B
#define KEY_MM_MY_COMPUTER 0x240
#define KEY_MM_WWW_SEARCH 0x210
#define KEY_MM_WWW_HOME 0x23A
#define KEY_MM_WWW_BACK 0x238
#define KEY_MM_WWW_FORWARD 0x230
#define KEY_MM_WWW_STOP 0x228
#define KEY_MM_WWW_REFRESH 0x220
#define KEY_MM_WWW_FAVORITES 0x218

#define KEY_ACPI_POWER 0x237
#define KEY_ACPI_SLEEP 0x23F
#define KEY_ACPI_WAKE 0x25E

#define KEY_LGUI 0x21F
#define KEY_RCTRL 0x214
#define KEY_RGUI 0x227
#define KEY_RALT 0x211
#define KEY_APPS 0x22F
#define KEY_INSERT 0x270
#define KEY_HOME 0x26C
#define KEY_PGUP 0x27D
#define KEY_DELETE 0x271
#define KEY_END 0x269
#define KEY_PGDN 0x27A
#define KEY_UPARROW 0x275
#define KEY_LARROW 0x26B
#define KEY_DARROW 0x272
#define KEY_RARROW 0x274
#define KEY_KPFORWARDSLASH 0x24A
#define KEY_KPENTER 0x25A

struct keymap extended_keymap[] = {
	{0x10, KEY_MM_WWW_SEARCH},
	{0x11, KEY_RALT},
	{0x14, KEY_RCTRL},
	{0x15, KEY_MM_PREVIOUS_TRACK},
	{0x18, KEY_MM_WWW_FAVORITES},
	{0x1F, KEY_LGUI},
	{0x20, KEY_MM_WWW_REFRESH},
	{0x21, KEY_MM_VOLUME_DOWN},
	{0x23, KEY_MM_MUTE},
	{0x27, KEY_RGUI},
	{0x28, KEY_MM_WWW_STOP},
	{0x2B, KEY_MM_CALCULATOR},
	{0x2F, KEY_APPS},
	{0x30, KEY_MM_WWW_FORWARD},
	{0x32, KEY_MM_VOLUME_UP},
	{0x34, KEY_MM_PLAY_PAUSE},
	{0x37, KEY_ACPI_POWER},
	{0x38, KEY_MM_WWW_BACK},
	{0x3A, KEY_MM_WWW_HOME},
	{0x3B, KEY_MM_STOP},
	{0x3F, KEY_ACPI_SLEEP},
	{0x40, KEY_MM_MY_COMPUTER},
	{0x48, KEY_MM_E_MAIL},
	{0x4A, KEY_KPFORWARDSLASH},
	{0x4D, KEY_MM_NEXT_TRACK},
	{0x50, KEY_MM_MEDIA_SELECT},
	{0x5A, KEY_KPENTER},
	{0x5E, KEY_ACPI_WAKE},
	{0x69, KEY_END},
	{0x6B, KEY_LARROW},
	{0x6C, KEY_HOME},
	{0x70, KEY_INSERT},
	{0x71, KEY_DELETE},
	{0x72, KEY_DARROW},
	{0x74, KEY_RARROW},
	{0x75, KEY_UPARROW},
	{0x7A, KEY_PGDN},
	{0x7D, KEY_PGUP}
};

struct keymap testmap[] = {
	{12, 0},
	{14, 0},
	{17, 0},
	{19, 0},
	{34, 0}
};

int init_kbd_handler(void)
{
	int r;
	add_isr_handler(KBD_INTERRUPT, kbd_isr);
	r = init_kbd();
	enable_keyboard_int();
	return r;
}

void kbd_isr()
{
	/*unsigned char scan_code; */
	key_buffer[head] = read_byte_from_dat_buf();

	head++;
	if (KBD_BUFFER_SIZE == head) {
		/*printk ("kbd buff overflow"); */
		head = 0;
	}
	if (head == tail)
		tail++;
	if (KBD_BUFFER_SIZE == tail) {
		tail = 0;
	}

	/*
	   printk ("--------- Array ----------\n");
	   printk ("head = %d\ntail = %d\n", head, tail);
	   for (int i = 0; i<KBD_BUFFER_SIZE; i++) {
	   printk ("0x%2x\n", key_buffer[i]);
	   }
	   printk ("--------------------------\n");
	 */

}

void flush_kbd_buffer()
{
	ENTER_CRITICAL_SECTION;

	head = 0;
	tail = 0;

	EXIT_CRITICAL_SECTION;
}

int read_key(int *key, bool * printable)
{
	int scan_code = 0;
	*printable = true;
	ENTER_CRITICAL_SECTION;

	/*
	   unsigned char testarr[]={0x1C, 0x1C, 0xF0, 0x1C, 0xE0, 0x1F, 0xE0,
	   0x1F, 0xE0, 0x00, 0xF0, 0x1F};
	   int o, make;

	   for (unsigned int i = 0; i < sizeof(testarr); i++){
	   keyboard_state_machine(testarr[i], &o, &make);
	   if (o) {
	   printk ("Key: %2x", o);
	   if (make)
	   printk ("  :: MAKE\n");
	   else
	   printk ("  :: BREAK\n");
	   }
	   else
	   printk ("KEY IGNORED\n");
	   }
	   printk ("\n PARSE COMPLETE");
	   while (1){}
	 */

	/* parse loop */
	bool make = false;	/*true; */
	bool repeat_key = 0;
	bool basic_key = 0;
	for (; tail != head;) {
		keyboard_state_machine(key_buffer[tail], &scan_code,
				       &make, &repeat_key, &basic_key);
		tail++;
		tail = (tail % KBD_BUFFER_SIZE);

		if (repeat_key)	/* a repeat key is also accepted */
			break;
		if (make)	/* send a key only when we have a break. */
			break;
		/*              else if (scan_code)  */
		/*printk ("%2x received but IGNORED\n", scan_code); */

		make = 0;
		repeat_key = 0;
		scan_code = 0;
	}

	EXIT_CRITICAL_SECTION;

	int val;
	int r;
	if (make || repeat_key) {
		/*printk ("%2x", scan_code); */

		if (basic_key) {
			r = binary_search_key(scan_code, basic_keymap,
					      sizeof(basic_keymap) /
					      sizeof(struct keymap), &val);
			*key = val;
			if (0 == r)
				if (val & 0x100) {
					/* b'coz we hav used such an encoding */
					*printable = false;
				} else {
					*printable = true;
				}
			else
				return r;

		} else {
			r = binary_search_key(scan_code, extended_keymap,
					      sizeof(extended_keymap) /
					      sizeof(struct keymap), &val);
			*key = val;
			if (0 == r)
				if (val & 0x200) {
					/* b'coz we hav used such an encoding */
					*printable = false;
				} else {
					*printable = true;
				}
			else
				return r;
		}
		/*
		   if (val & 0x100)
		   printk ("%c", val);
		 */
		return 0;

	} else
		return -1;

}

enum {
	STATE_INIT,
	STATE_BASIC_BREAK_EXPEC,
	STATE_BASIC_BREAK_ES,
	STATE_EXT_MAKE_ES,
	STATE_EXT_BREAK_EXPEC,
	STATE_EXT_BREAK_ES_1,
	STATE_EXT_BREAK_ES_2,
	STATE_IGNORE
};

/*
 * Our strategy would be to ignore the keys which are ambiguous. Basically
 * we will not try to guess the key.
 * 
 * if key is to be returned then the *o wil have non-zero value. if the *o
 * has a non-zero value then the *make can be analysed to find out whether
 * it was a make or a break.
 */
static void keyboard_state_machine(unsigned char i, int *o,
				   bool *make, bool *repeat_key,
				   bool *basic_key)
{
	*o = 0;
	*repeat_key = false;
	*make = false;
	*basic_key = false;

	/*
	 * Following are the problem cases which need to be analysed
	 * further:
	 * 1. in STATE_INIT there is 0xF0 and it is actually 0xF0 from
	 * break code.
	 */

	static int state = STATE_INIT;

	switch (state) {
	case STATE_INIT:
		/*printk ("<<< STATE_INIT >>>\n"); */
		if (i == 0xF0) {	/* ignore */
			/* ignore the next key after this */
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		if (i == 0xE0) {
			state = STATE_EXT_MAKE_ES;
			goto STATE_MACHINE_EXIT;
		}
		*o = i;
		*make = true;
		*basic_key = true;
		state = STATE_BASIC_BREAK_EXPEC;
		break;
	case STATE_BASIC_BREAK_EXPEC:
		/*printk ("<<< STATE_BASIC_BREAK_EXPEC >>>\n"); */
		if (i == 0xF0) {
			state = STATE_BASIC_BREAK_ES;
			goto STATE_MACHINE_EXIT;
		}
		/* repeat key... */
		*o = i;
		*make = true;
		*repeat_key = true;
		*basic_key = true;
		/* keep state unchanged... */
		break;
	case STATE_BASIC_BREAK_ES:
		/*printk ("<<< STATE_BASIC_BREAK_ES >>>\n"); */
		if (i == 0xE0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		if (i == 0xF0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		*o = i;
		*make = false;
		*basic_key = true;
		state = STATE_INIT;
		break;
	case STATE_EXT_MAKE_ES:
		/*printk ("<<< STATE_EXT_MAKE_ES >>>\n"); */
		if (i == 0xE0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		if (i == 0xF0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		*o = i;
		*make = true;
		*basic_key = false;
		state = STATE_EXT_BREAK_EXPEC;
		break;
	case STATE_EXT_BREAK_EXPEC:
		/*printk ("<<< STATE_EXT_BREAK_EXPEC >>>\n"); */
		if (i == 0xE0) {
			state = STATE_EXT_BREAK_ES_1;
			goto STATE_MACHINE_EXIT;
		}
		if (i == 0xF0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		state = STATE_INIT;
		break;
	case STATE_EXT_BREAK_ES_1:
		/*printk ("<<< STATE_EXT_BREAK_ES_1 >>>\n"); */
		if (i == 0xF0) {
			state = STATE_EXT_BREAK_ES_2;
			goto STATE_MACHINE_EXIT;
		}
		if (i == 0xE0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		/* repeat key... */
		*o = i;
		*make = true;
		*repeat_key = false;
		*basic_key = false;
		state = STATE_EXT_BREAK_EXPEC;
		break;
	case STATE_EXT_BREAK_ES_2:
		/*printk ("<<< STATE_EXT_BREAK_ES_2 >>>\n"); */
		if (i == 0xF0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		if (i == 0xE0) {
			state = STATE_IGNORE;
			goto STATE_MACHINE_EXIT;
		}
		*o = i;
		*make = false;
		*basic_key = false;
		state = STATE_INIT;
		break;
	case STATE_IGNORE:
		/*printk ("<<< STATE_IGNORE >>>\n"); */
		state = STATE_INIT;
		break;
	default:
		KERNEL_PANIC;
		break;
	}

STATE_MACHINE_EXIT:
	return;
}

static int binary_search_key(int scan_code, struct keymap *map,
			     int keymap_length, int *value)
{
	int min_index = 0;
	int max_index = keymap_length - 1;
	int median_index;

	if ((scan_code < map[min_index].scan_code) ||
	    (scan_code > map[max_index].scan_code))
		return -1;

	bool found = false;

	for (; !found;) {
		median_index = (min_index + max_index) / 2;

		if (median_index == min_index) {
			/* we have reached a point where there are only two */
			/* elements left */
			if (map[median_index].scan_code == scan_code)
				found = true;
			else if (map[median_index + 1].scan_code == scan_code) {
				median_index++;
				found = true;
			} else
				found = false;

			break;
		}
		/*printk ("{%d, %d, %d}", min_index, median_index, max_index);*/

		if (scan_code > map[median_index].scan_code) {
			/* upper block */
			/*printk ("upper\n"); */
			min_index = median_index;
		} else {
			/* lower block */
			/*printk ("lower\n"); */
			max_index = median_index;
		}
	}

	if (found) {
		/*printk ("Found at index %d\n", median_index); */
		*value = map[median_index].value;
		return 0;
	} else {
		/*printk ("Not found\n"); */
		return -1;
	}
}
