#ifndef __KBD_H_
#define __KBD_H_

#include <types.h>

#define KBD_CTRL_PORT 0x64
#define KBD_DATA_PORT 0x60
#define KBD_CDATA_BIT 0
#define KBD_DDATA_BIT 1
#define KBRD_RESET 0xFE

#define SC_ERROR 0
#define SC_CAPS 58
#define SC_LSFT 42
#define SC_RSFT 54

#define SC_LSFT_REL 170
#define SC_RSFT_REL 182

enum{
       CAPS_ON,
       LSHIFT,
       RSHIFT,
};
static u8 kbd_status; /*Global KBD status */

static const char kbmap_base[128] = {
    0, 27, /*ESC*/ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8, /*Back space*/ 
    '\t', 'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, /*CTRL*/
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0 /*LSHIFT*/, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0 /*RSHIFT*/, 0, 0,
    32,0, /*CAPS*/ 0,0, 0, 0
};
static const char kbmap_upper[128] = {
    0, 27, /*ESC*/ '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8, /*Back space*/ 
    '\t', 'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, /*CTRL*/
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0 /*LSHIFT*/, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0 /*RSHIFT*/, 0, 0,
    32,0, /*CAPS*/ 0,0, 0, 0
};
extern void kbd_read(char *s);
extern void kbd_reboot(void);
#endif
