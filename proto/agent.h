#ifndef _QPT_HEADER_WATERMETER_PROTOCOL_AGENT_H_
#define _QPT_HEADER_WATERMETER_PROTOCOL_AGENT_H_

#include "util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FC_NONE		0x0
#define FC_DIGIT	0x1
#define FC_IMAGE	0x2

typedef struct meter_station_context {
	byte sid[4];	// station id
	byte wmid[6];	// watermeter id
	int type;		// data type (func code)
	byte *data;		// data
	int dlen;		// data length
} ms_ctx;

extern int modbus_pack(const ms_ctx *msc, byte *buf, int len);
extern int modbus_unpack(ms_ctx *msc, const byte *data, int len);

#ifdef __cplusplus
}
#endif

#endif // proto_agent.h
