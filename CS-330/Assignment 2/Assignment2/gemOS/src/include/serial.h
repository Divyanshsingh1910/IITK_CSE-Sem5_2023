#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <types.h>

/*Definitions taken from specs*/
#define XMTRDY          0x20
#define RCVRDY          0x1

#define DLAB            0x80

#define TXR             0       /*  Transmit register (WRITE) */
#define RXR             0       /*  Receive register  (READ)  */
#define IER             1       /*  Interrupt Enable          */
#define IIR             2       /*  Interrupt ID              */
#define FCR             2       /*  FIFO control              */
#define LCR             3       /*  Line control              */
#define MCR             4       /*  Modem control             */
#define LSR             5       /*  Line Status               */
#define MSR             6       /*  Modem Status              */
#define DLL             0       /*  Divisor Latch Low         */
#define DLH             1       /*  Divisor latch High        */

#define BAUD 9600
extern void serial_write(char *);
extern void serial_read(char *);
extern void serial_init(void);

#endif
