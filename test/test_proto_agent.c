#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util/util.h"
#include "proto/agent.h"

static ms_ctx *gen_digit_ctx() {
	byte sid[4] = {0x0, 0x0, 0x0, 0x1};
	byte mid[6] = {0x0, 0x0, 0x0, 0x1, 0x48, 0x2};

	byte dgt[4] = {0x1, 0x9, 0x3, 0x5};

	ms_ctx *msc = (ms_ctx *)malloc(sizeof(ms_ctx));
	memcpy(msc->sid, sid, 4);
	memcpy(msc->wmid, mid, 6);
	msc->type = 0x3;
	msc->dlen = 4;
	msc->data = (byte *)malloc(sizeof(byte) * msc->dlen);
	memcpy(msc->data, dgt, msc->dlen);

	return msc;
}

static void free_ms_ctx(ms_ctx **msc) {
	if (msc && *msc) {
		if ((*msc)->data) {
			free((*msc)->data);
		}
		free(*msc);
		*msc = NULL;
	}
}

void test_pack_modbus() {
	byte buf[32] = {0};
	byte epct[] = {0x1, 0x3, 0x8, 0x1, 0x48, 0x2, 0x3, 
			0x1, 0x9, 0x3, 0x5, 0xC8, 0xF0};
	ms_ctx *msc = gen_digit_ctx();

	modbus_pack(msc, buf, 32);
	assert(!memcmp(epct, buf, sizeof(epct)));

	free_ms_ctx(&msc);
}

