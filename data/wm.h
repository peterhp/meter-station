#ifndef _QPT_HEADER_WATERMETER_MAIN_DATA_H_
#define _QPT_HEADER_WATERMETER_MAIN_DATA_H_

#include "util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int cmd_do(ms_ctx *msc);

extern int get_digits(byte *digits, const byte *mid);
extern int get_image(byte *img, const byte *mid);

extern int save_options(const char *file, const byte *dat, int len);

#ifdef __cplusplus
}
#endif

#endif // wm.h
