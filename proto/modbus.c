#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util/util.h"
#include "agent.h"

int modbus_unpack(ms_ctx *msc, const byte *data, int len) {
	memset(msc, 0, sizeof(ms_ctx));

	if (len < 8) {
		return -1;
	}

	if (!crc16_check(0xFFFF, data, 6, data + 6)) {
		return -2;
	}

	msc->sid[3] = data[0];
	msc->type = data[2];
	memcpy(msc->wmid + 3, data + 3, 3);

	return 0;
}

int modbus_pack(const ms_ctx *msc, byte *buf, int len) {
	int pktlen = 0;

	pktlen = 9 + msc->dlen;
	if (!buf) {
		return pktlen;
	} else if (pktlen > len) {
		return -1;
	}

	buf[0] = msc->sid[3];
	buf[1] = 0x3;
	buf[2] = (byte)msc->dlen + 4;
	buf[3] = (byte)msc->type;
	memcpy(buf + 4, msc->wmid + 3, 3);
	memcpy(buf + 7, msc->data, msc->dlen);

	uint16 wcrc = crc16(0xFFFF, buf, (unsigned int)(7 + msc->dlen));
	memcpy(buf + 7 + msc->dlen, &wcrc, 2);

	return pktlen;
}

