#include <stdio.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "com/monitor.h"
#include "proto/agent.h"
#include "data/wm.h"

static smt_ctx smtc;

static int data_proc(void *data, int len) {
	ms_ctx msc;
	byte reply[64] = {0};
	int rep_len = 0;

	agent_unpack(&msc, data, len, PROTO_MODBUS);

	// prepare data
	cmd_do(&msc);
	
	rep_len = agent_pack(&msc, reply, 64, PROTO_MODBUS);

	serial_write(&smtc.sctx, reply, rep_len);

	return 0;
}

static int proc_stop = 0;
static void sig_proc_stop(int signo) {
	if (SIGINT == signo || SIGQUIT == signo) {
		proc_stop = 1;
	}
}

static int sig_init_callback() {
	if (signal(SIGINT, sig_proc_stop) == SIG_ERR) {
		return -1;
	}
	if (signal(SIGQUIT, sig_proc_stop) == SIG_ERR) {
		return -2;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	sig_init_callback();

	printf("Start!\n");

	smt_init(&smtc, "/dev/ttyUSB0", data_proc);
	smt_start(&smtc);
	while (!proc_stop) {
		sleep(1);
	}
	smt_stop(&smtc);

	printf("End!\n");

	return 0;
}

