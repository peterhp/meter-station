#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "util.h"

void test_crc16() {
	byte data1[] = {0x1, 0x3, 0x1, 0x48, 0x0, 0x2};
	byte data2[] = {0x1, 0x3, 0x4, 0x41, 0x83, 0xC3, 0x6A};

	uint16 crc1 = crc16(0xFFFF, data1, sizeof(data1));
	uint16 crc2 = crc16(0xFFFF, data2, sizeof(data2));

	assert(0xE145 == crc1);
	assert(0x38CF == crc2);
}

