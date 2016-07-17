#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/util.h"
#include "proto/agent.h"
#include "wm.h"

int cmd_do(ms_ctx *msc) {
	int ret = 0;
	
	switch (msc->type) {
		case FC_DIGIT:
			ret = get_digits(msc->data, msc->wmid);
			msc->dlen = ret;
			break;

		case FC_IMAGE:
			ret = get_image(msc->data, msc->wmid);
			msc->dlen = ret;
			break;

		case FC_OPTION:
			ret = save_options("/etc/wm.dat", msc->data, msc->dlen);
			break;
	}

	return ret;
}

