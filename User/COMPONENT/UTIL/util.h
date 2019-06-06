

#ifndef __APP_UTIL_H
#define __APP_UTIL_H

#include <stdint.h>


/* 大小端转换宏 */
#define BIG_LITTLE_SWAP16(A)        ((((uint16_t)(A) & 0xff00) >> 8)        \
                                    | (((uint16_t)(A) & 0x00ff) << 8))

#define BIG_LITTLE_SWAP32(A)        ((((uint32_t)(A) & 0xff000000) >> 24)   \
                                    | (((uint32_t)(A) & 0x00ff0000) >> 8)   \
                                    | (((uint32_t)(A) & 0x0000ff00) << 8)   \
                                    | (((uint32_t)(A) & 0x000000ff) << 24))


extern void my_str_remove_blank(char *str);
extern int my_atoi(char *str);
extern char* my_itoa(int value, char* string, int radix);


#endif /* __APP_UTIL_H */
