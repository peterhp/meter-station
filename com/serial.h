#ifndef _QPT_HEADER_SERIAL_OPERATIONS_H_
#define _QPT_HEADER_SERIAL_OPERATIONS_H_

/**
 * This file provides base settings for serial communications, mainly 
 * baud rate, data bits, parity, stop bits and flow control, which 
 * are needed to connect a serial device. However, serveral options 
 * are only listed here but do not been implimented, which are taged 
 * as reserved, for we have no needs as well as no time at current. 
 * There are 5 IO models in linux, but only blocking and async IO 
 * models can be used. For async model, DO NOT use serial_read to 
 * read data, use a self callback functions for data process instead. 
 * Created by Sora on 2016.05.01.
 *
 * Add timeout for sync read model, and clear sleep within sync read 
 * model as sleep may cause data loss.
 * Updated by Sora on 2016.05.05.
**/

#ifdef __cplusplus
extern "C" {
#endif

// baud rate
#define BAUD_2400		2400
#define BAUD_4800		4800
#define BAUD_9600		9600
#define BAUD_19200		19200
#define BAUD_38400		38400
#define BAUD_57600		57600
#define BAUD_115200		115200
#define BAUD_230400		230400

// data bits
#define DATA_5			5
#define DATA_6			6
#define DATA_7			7
#define DATA_8			8

// parity
#define PARITY_NONE		0
#define PARITY_ODD		1
#define PARITY_EVEN		2
#define PARITY_SPACE	3
#define PARITY_MARK		4 // reserved

// stop bits
#define STOP_ONE		1
#define STOP_TWO		2

// flow control
#define CONTROL_NONE	0
#define CONTROL_HARD	1 // reserved
#define CONTROL_SOFT	2 // reserved
#define CONTROL_BOTH	(CONTROL_HARD | CONTROL_SOFT) // reserved

// io model
#define IO_BLOCK		1
#define IO_NONBLOCK		2 // reserved
#define IO_MULTIPLEX	3 // reserved
#define IO_SIGNAL		4 // reserved
#define IO_ASYNC		5

#include <pthread.h>

typedef int (*pf_data_proc)(const void *, int);

typedef struct serial_context {
	char dev[16];
	int fd;

	// serial port setting
	int baud_rate;
	int data_bits;
	int parity;
	int stop_bits;
	int flow_control;

	// I/O
	int io_model;
	// seconds, ignored in async model
	// set before serial_read
	int i_timeout;

	// used for async io model
	pthread_t rth_id;
	int rth_status;
	int cw_stop; // command word -- stop
	pf_data_proc process_data;
} serial_ctx;

extern void serial_set_default(serial_ctx *sctx);

extern int  serial_open(serial_ctx *sctx);
extern void serial_close(serial_ctx *sctx);

extern int serial_read(const serial_ctx *sctx, void *buf, int len); // for sync io
extern int serial_write(const serial_ctx *sctx, const void *buf, int len);

#ifdef __cplusplus
}
#endif

#endif // serial.h
