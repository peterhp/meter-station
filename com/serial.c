#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>

#include "serial.h"

#define LEN_FRAME_RD 128

#define THREAD_STATUS_STOP	0
#define THREAD_STATUS_RUN	1

void serial_set_default(serial_ctx *sctx) {
	if (!sctx) {
		return ;
	}

	// 115200 8N1
	sctx->baud_rate = BAUD_115200;
	sctx->data_bits = DATA_8;
	sctx->parity = PARITY_NONE;
	sctx->stop_bits = STOP_ONE;
	sctx->flow_control = CONTROL_NONE;

	// blocking I/O model
	sctx->io_model = IO_BLOCK;
	sctx->i_timeout = 2; // 2s

	// default data process routine
	sctx->process_data = NULL;

	// thread parameters for async io model
	sctx->rth_id = 0;
	sctx->rth_status = THREAD_STATUS_STOP;
	sctx->cw_stop = 0;
}

static inline void tc_set_baud_rate(struct termios *opt, int baud_rate) {
	int tcbaud = B9600; // default baud rate 9600
	switch (baud_rate) {
		case BAUD_2400:   tcbaud = B2400;   break;
		case BAUD_4800:   tcbaud = B4800;   break;
		case BAUD_9600:   tcbaud = B9600;   break;
		case BAUD_19200:  tcbaud = B19200;  break;
		case BAUD_38400:  tcbaud = B38400;  break;
		case BAUD_57600:  tcbaud = B57600;  break;
		case BAUD_115200: tcbaud = B115200; break;
		case BAUD_230400: tcbaud = B230400; break;
	}
	cfsetispeed(opt, tcbaud);
	cfsetospeed(opt, tcbaud);
}

static inline void tc_set_data_bits(struct termios *opt, int data_bits) {
	int tcbits = CS8; // default data bits 8
	switch (data_bits) {
		case DATA_5: tcbits = CS5; break;
		case DATA_6: tcbits = CS6; break;
		case DATA_7: tcbits = CS7; break;
		case DATA_8: tcbits = CS8; break;
	}
	opt->c_cflag &= ~CSIZE;
	opt->c_cflag |= tcbits;
}

static inline void tc_set_parity(struct termios *opt, int parity) {
	switch (parity) {
		case PARITY_NONE: 
			opt->c_cflag &= ~PARENB;
			opt->c_iflag &= ~(INPCK | ISTRIP);
			break;
		case PARITY_ODD: 
			opt->c_cflag |= PARENB;
			opt->c_cflag |= PARODD;
			opt->c_iflag |= (INPCK | ISTRIP);
			break;
		case PARITY_EVEN: 
			opt->c_cflag |= PARENB;
			opt->c_cflag &= ~PARODD;
			opt->c_iflag |= (INPCK | ISTRIP);
			break;
		case PARITY_SPACE:
			opt->c_cflag &= ~PARENB;
			opt->c_iflag |= (INPCK | ISTRIP);
			break;
		default: // default parity none
			opt->c_cflag &= ~PARENB;
			opt->c_iflag &= ~(INPCK | ISTRIP);
	}
}

static inline void tc_set_stop_bits(struct termios *opt, int stop_bits) {
	switch (stop_bits) {
		case STOP_ONE: opt->c_cflag &= ~CSTOPB; break;
		case STOP_TWO: opt->c_cflag |=  CSTOPB; break;
		default:       opt->c_cflag &= ~CSTOPB; break; // default stop bits 1
	}
}

static inline void tc_set_flow_control(struct termios *opt, int flow_control) {
	switch (flow_control) {
		case CONTROL_NONE:
			opt->c_cflag &= ~CRTSCTS;
			opt->c_iflag &= ~(IXON | IXOFF | IXANY);
			break;
		case CONTROL_HARD: // reserved, not used
			opt->c_cflag |= CRTSCTS;
			opt->c_iflag &= ~(IXON | IXOFF | IXANY);
			break;
		case CONTROL_SOFT: // reserved, not used
			opt->c_cflag &= ~CRTSCTS;
			opt->c_iflag |= (IXON | IXOFF | IXANY);
			break;
		case CONTROL_BOTH: // reserved, not used
			opt->c_cflag |= CRTSCTS;
			opt->c_iflag |= (IXON | IXOFF | IXANY);
			break;
		default: // default flow control none
			opt->c_cflag &= ~CRTSCTS;
			opt->c_iflag &= ~(IXON | IXOFF | IXANY);
	}
}

static inline void tc_set_io_control(struct termios *opt, int io_model) {
	switch (io_model) {
		case IO_BLOCK:
			opt->c_cc[VMIN] = 0;
			opt->c_cc[VTIME] = 5;
			break;
		case IO_NONBLOCK:
			opt->c_cc[VMIN] = 0;
			opt->c_cc[VTIME] = 0;
			break;
		case IO_MULTIPLEX:
			opt->c_cc[VMIN] = 1;
			opt->c_cc[VTIME] = 0;			
			break;
		case IO_SIGNAL:
			opt->c_cc[VMIN] = 1;
			opt->c_cc[VTIME] = 0;
			break;
		case IO_ASYNC:
			opt->c_cc[VMIN] = 0;
			opt->c_cc[VTIME] = 20;
			break;
		default:
			opt->c_cc[VMIN] = 0;
			opt->c_cc[VTIME] = 5;
			break;
	}
}

static inline void serial_set_opt(serial_ctx *sctx) {
	struct termios tcopt;
	tcgetattr(sctx->fd, &tcopt);

	tc_set_baud_rate(&tcopt, sctx->baud_rate);
	tc_set_data_bits(&tcopt, sctx->data_bits);
	tc_set_parity(&tcopt, sctx->parity);
	tc_set_stop_bits(&tcopt, sctx->stop_bits);
	tc_set_flow_control(&tcopt, sctx->flow_control);

	tcopt.c_cflag |= (CLOCAL | CREAD);

	// raw model with no echo
	tcopt.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
	tcopt.c_oflag &= ~OPOST;

	// no special actions on CR and NL
	tcopt.c_iflag &= ~(IGNBRK | BRKINT | INLCR | IGNCR | ICRNL);
	tcopt.c_oflag &= ~(OCRNL | ONLCR | ONOCR | ONLRET);

	tc_set_io_control(&tcopt, sctx->io_model);

	tcflush(sctx->fd, TCIOFLUSH);

	tcsetattr(sctx->fd, TCSANOW, &tcopt);
}

static inline void serial_set_io_model(serial_ctx *sctx) {
	int flags = fcntl(sctx->fd, F_GETFL);

	if (IO_NONBLOCK == sctx->io_model) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}

	fcntl(sctx->fd, F_SETFL, flags);
}

static int serial_rthread_create(serial_ctx *sctx);
static int serial_rthread_destroy(serial_ctx *sctx);

int serial_open(serial_ctx *sctx) {
	if (!sctx || strlen(sctx->dev) <= 0) { // check param validality
		return -1;
	}

	sctx->fd = open(sctx->dev, O_RDWR | O_NOCTTY);
	if (-1 == sctx->fd) { // fail to open serial port
		return -2;
	}

	if (!isatty(sctx->fd)) { // not a tty
		close(sctx->fd);
		return -3;
	}

	// set serial io model
	serial_set_io_model(sctx);
	// set serial options
	serial_set_opt(sctx);

	if (IO_ASYNC == sctx->io_model) {
		if (serial_rthread_create(sctx) != 0) {
			return -4;
		}
	}

	return 0;
}

void serial_close(serial_ctx *sctx) {
	if (IO_ASYNC == sctx->io_model) {
		serial_rthread_destroy(sctx);
	}

	close(sctx->fd);
}

int serial_read(const serial_ctx *sctx, void *buf, int len) {
	int rlen = 0, rcnt = 0;
	int bread = 0;
	int cretry = sctx->i_timeout * 10 / 5;

	if (IO_BLOCK != sctx->io_model) { // only for blocking io now
		return 0;
	}

	while (rcnt < len) {
		// block until data received or timeout
		rlen = read(sctx->fd, buf + rcnt, len - rcnt);

		if (rlen > 0) { // read ok
			bread = 1;
			rcnt += rlen;
		} else if (0 == rlen) {
			if (bread) { // no more data
				break;
			} else if (cretry) { // no data read, retry
				cretry--;
			} else { // timeout
				return -2;
			}
		} else { // read error
			return -1;
		}
	}
	
	return rcnt;
}

int serial_write(const serial_ctx *sctx, const void *buf, int len) {
	int wlen = 0, wcnt = 0;
	while (wcnt < len) {
		wlen = write(sctx->fd, buf + wcnt, len - wcnt);
		if (wlen < 0) { // error occurs
			return -1;
		}
		wcnt += wlen;
	}

	return wcnt;
}

static void *serial_rthread_proc(void *arg) {
	serial_ctx *sctx = (serial_ctx *)arg;

	unsigned char frame[LEN_FRAME_RD]; // frame buffer
	int rlen = 0;

	while (!sctx->cw_stop) {
		// read all if any data exists
		while ((rlen = read(sctx->fd, frame, LEN_FRAME_RD)) > 0) {
			if (sctx->process_data) {
				(*sctx->process_data)(frame, rlen); // for self data process
				continue;
			}
		}
	}

	return NULL;
}

static int serial_rthread_create(serial_ctx *sctx) {
	int ret = 0;

	if (THREAD_STATUS_RUN == sctx->rth_status) { // read thread already exists
		return 1;
	}

	sctx->cw_stop = 0;

	ret = pthread_create(&sctx->rth_id, NULL, serial_rthread_proc, sctx);
	if (0 == ret) {
		sctx->rth_status = THREAD_STATUS_RUN;
	}

	return ret;
}

static int serial_rthread_destroy(serial_ctx *sctx) {
	if (THREAD_STATUS_STOP == sctx->rth_status) {
		return 1;
	}

	// change command word to stop
	sctx->cw_stop = 1;

	// wait until read thread exits
	pthread_join(sctx->rth_id, NULL);

	sctx->rth_status = THREAD_STATUS_STOP;

	return 0;
}

