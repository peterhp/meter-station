#ifndef _QPT_HEADER_SERIAL_MONITOR_H_
#define _QPT_HEADER_SERIAL_MONITOR_H_

#include <pthread.h>

#include "serial.h"
#include "util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct serial_monitor_context {
	serial_ctx sctx;

	pthread_t r_tid; 	// read thread id
	volatile int r_run; // read thread status

	int (*proc_data)(void *data, int len);
	pthread_t m_tid;	// monitor thread
	volatile int m_run; // monitor status
	
	byte data[1024];
	int dlen;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
} smt_ctx;

extern void smt_init(smt_ctx *smtc, const char *dev, 
		int (*proc_data)(void *data, int len));

extern int smt_start(smt_ctx *smtc);
extern void smt_stop(smt_ctx *smtc);

#ifdef __cplusplus
}
#endif

#endif // monitor.h
