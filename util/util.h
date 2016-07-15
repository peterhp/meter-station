#ifndef _QPT_HEADER_UTILITY_H_
#define _QPT_HEADER_UTILITY_H_

#include "type_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

extern uint16 crc16(uint16 crc, const uint8 *data, unsigned int len);
extern int crc16_check(uint16 crc_init, const uint8 *data, 
		unsigned int len, const uint8 crc_code[2]);

#ifdef __cplusplus
}
#endif

#endif // util.h
