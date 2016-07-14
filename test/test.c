#include <stdio.h>

#include "test.h"

int main(int argc, char *argv[]) {
	printf("Unit test starts.\n");

	test_crc16();
	test_check_crc16();
	
	test_pack_modbus();
	test_unpack_modbus();

	printf("Finish unit test.\n");

	return 0;
}
