

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


typedef enum bool_ {
    false,
    true
} bool;


extern int big_little_endian_check(void);
extern int str_is_digit(char *p_str);
extern int strcmpnum(char *s, char *t);
extern void my_delete_char(char str[], char target);
extern void my_str_remove_blank(char *str);
extern void my_hex_to_str(unsigned char *pHex, unsigned char *pAscii, int nLen);
extern int my_str_to_hex(char *str, unsigned char *out, unsigned int *outlen);
extern void my_str_to_upper(char* str);
extern void my_str_to_lower(char* str);
extern int my_htoi(char s[]);
extern int my_atoi(char *str);
extern char* my_itoa(int value, char* string, int radix);


#endif /* __APP_UTIL_H */
