#include <stdio.h>

#include <errno.h>
#include <signal.h>

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

	printf("Start\n");

	while (!proc_stop) {

	}

	printf("End.\n");

	return 0;
}

