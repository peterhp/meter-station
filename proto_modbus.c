#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "proto_agent.h"

int modbus_pack(const ms_ctx *msc, byte *buf, int len) {
	int pktlen = 0;

	assert(msc);

	pktlen = 9 + msc->dlen;
	if (!buf) {
		return pktlen;
	} else if (pktlen > len) {
		return -1;
	}

	buf[0] = msc->sid[3];
	buf[1] = (byte)msc->type;
	buf[2] = (byte)msc->dlen + 4;
	memcpy(buf + 3, msc->wmid + 3, 3);
	buf[6] = 0x3;
	memcpy(buf + 7, msc->data, msc->dlen);

	uint16 wcrc = crc16(0xFFFF, buf, (unsigned int)(7 + msc->dlen));
	memcpy(buf + 7 + msc->dlen, &wcrc, 2);

	return pktlen;
}

