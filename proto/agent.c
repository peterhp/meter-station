#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/util.h"
#include "agent.h"

extern int modbus_pack(const ms_ctx *msc, byte *buf, int len);
extern int modbus_unpack(ms_ctx *msc, const byte *data, int len);

static int default_pack(const ms_ctx *msc, byte *buf, int len) {
	if (!buf) {
		return msc->dlen;
	} else if (msc->dlen > len) {
		return -1;
	}

	memcpy(buf, msc->data, msc->dlen);

	return msc->dlen;
}

static int default_unpack(ms_ctx *msc, const byte *data, int len) {
	memset(msc, 0, sizeof(ms_ctx));

	if (len > 0) {
		msc->dlen = len;
		memcpy(msc->data, data, len);
	}

	return 0;
}

static struct proto_func {
	int type;
	int (*pack)(const ms_ctx *, byte *, int);
	int (*unpack)(ms_ctx *, const byte *, int);
} pf_tbl[] = {
	{PROTO_NONE, 	default_pack, default_unpack}, 
	{PROTO_MODBUS,  modbus_pack,  modbus_unpack}
};

static int proto_index(int proto_type) {
	int i;
	for (i = ARRAY_SIZE(pf_tbl) - 1; i > 0; --i) {
		if (proto_type == pf_tbl[i].type) {
			break;
		}
	}

	return i;
}

int agent_pack(const ms_ctx *msc, byte *buf, int buf_size, int proto) {
	int index = proto_index(proto);

	return (*pf_tbl[index].pack)(msc, buf, buf_size);
}

int agent_unpack(ms_ctx *msc, const byte *data, int dlen, int proto) {
	int index = proto_index(proto);

	return (*pf_tbl[index].unpack)(msc, data, dlen);
}

