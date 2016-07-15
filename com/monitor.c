#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "serial.h"
#include "monitor.h"

static void *thread_listener(void *arg) {
	smt_ctx *smtc = (smt_ctx *)arg;

	byte buf[1024];
	int rlen = 0;

	while (smtc->r_run) {
		rlen = serial_read(&smtc->sctx, buf, 1024);
		if (rlen <= 0) {
			continue;
		}

		pthread_mutex_lock(&smtc->mutex);
		memcpy(smtc->data, buf, rlen);
		smtc->dlen = rlen;
		pthread_cond_signal(&smtc->cond);
		pthread_mutex_unlock(&smtc->mutex);
	}

	return NULL;
}

static void *thread_monitor(void *arg) {
	smt_ctx *smtc = (smt_ctx *)arg;

	byte buf[1024];
	int len = 0;

	while (smtc->m_run) {
		// wait signal
		pthread_mutex_lock(&smtc->mutex);
		pthread_cond_wait(&smtc->cond, &smtc->mutex);
		memcpy(buf, smtc->data, smtc->dlen);
		len = smtc->dlen;
		pthread_mutex_unlock(&smtc->mutex);

		// data proc
		(*smtc->proc_data)(buf, len);
	}

	return NULL;
}

void smt_init(smt_ctx *smtc, const char *dev, 
		int (*proc_data)(void *data, int len)) {
	memset(smtc, 0, sizeof(smt_ctx));

	// init serial context
	strcpy(smtc->sctx.dev, dev);
	serial_set_default(&smtc->sctx);

	// set data proc function
	smtc->proc_data = proc_data;
}

int smt_start(smt_ctx *smtc) {
	// open serial
	if (!serial_open(&smtc->sctx)) {
		return -1;
	}

	// init mutex and cond
	pthread_mutex_init(&smtc->mutex, NULL);
	pthread_cond_init(&smtc->cond, NULL);

	// serial listener thread
	smtc->r_run = 1;
	pthread_create(&smtc->r_tid, NULL, thread_listener, smtc);

	// monitor thread
	smtc->m_run = 1;
	pthread_create(&smtc->m_tid, NULL, thread_monitor, smtc);

	return 0;
}

void smt_stop(smt_ctx *smtc) {
	// stop listener and monitor
	smtc->m_run = 0;
	smtc->r_run = 0;
	pthread_join(smtc->m_tid, NULL);
	pthread_join(smtc->r_tid, NULL);

	pthread_mutex_destroy(&smtc->mutex);
	pthread_cond_destroy(&smtc->cond);

	serial_close(&smtc->sctx);
}

