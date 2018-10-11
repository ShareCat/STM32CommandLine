

#include "./util.h"


/**
  * @brief  将字符串转化成整数，带符号
  * @param  str 要转化的字符串
  * @retval 整数
  */
int my_atoi(char *str)
{
    int result = 0;
    int neg_multiplier = 1;

    // Scrub leading whitespace
    while(*str && (
                (*str == ' ') ||
                (*str == '\t')))
        str++;

    // Check for negative
    if(*str && *str == '-') {
        neg_multiplier = -1;
        str++;
    }

    // Do number
    for(; (*str) && (*str >= '0') && (*str <= '9'); str++) {
        result = (result * 10) + (*str - '0');
    }

    return result * neg_multiplier;
}


/**
  * @brief  将整数转化成字符串，带符号
  * @param  value   待转化的整数
  * @param  string  保存转换的字符串结果
  * @param  radix   转换的进制，如2，8，10，16进制
  * @retval 保存转换的字符串地址，0表示错误
  */
char* my_itoa(int value, char* string, int radix)
{
    char tmp[33];
    char* tp = tmp;
    int i;
    unsigned v;
    int sign;
    char* sp;

    if (0 == string) return 0;

    if(radix > 36 || radix <= 1) {
        return 0;
    }

    sign = (radix == 10 && value < 0);

    if(sign)
        v = -value;
    else
        v = (unsigned)value;

    while(v || tp == tmp) {
        i = v % radix;
        v = v / radix;

        if(i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    sp = string;

    if(sign)
        *sp++ = '-';

    while(tp > tmp)
        *sp++ = *--tp;

    *sp = 0;
    return string;
}


#if 0   /* microsoft version, used malloc() func. */

#include <stdlib.h>

char* _itoa(int value, char* string, int radix)
{
    char tmp[33];
    char* tp = tmp;
    int i;
    unsigned v;
    int sign;
    char* sp;

    if(radix > 36 || radix <= 1) {
        __set_errno(EDOM);
        return 0;
    }

    sign = (radix == 10 && value < 0);

    if(sign)
        v = -value;
    else
        v = (unsigned)value;

    while(v || tp == tmp) {
        i = v % radix;
        v = v / radix;

        if(i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    if(string == 0)
        string = (char*)malloc((tp - tmp) + sign + 1);

    sp = string;

    if(sign)
        *sp++ = '-';

    while(tp > tmp)
        *sp++ = *--tp;

    *sp = 0;
    return string;
}

#endif


/*********************************************END OF FILE**********************/
