#include "./util.h"


/**
  * @brief  将字符串中所有的空格字符去掉
  * @param  str 要转化的字符串
  * @retval null
  */
void my_str_remove_blank(char *str)
{
    char *c;//指向字符串的字符处
	   while (*str != ' ') {
		      if (*str == '\0') {
			         //字符串中没有空格,直接返回
			         return;
		      }
		      str++;
	   }//将blank指针指向第一个空格位置
    //让c指针指向出现空格后的第一个字符串
    c = str + 1;
    while (*c == ' ') {
        c++;
    }
    while (*c != '\0') {
        if (*c != ' ') {
            //将空格换为后面出现的字符，换过去的字符置为空格
            *str = *c;
            *c = ' ';
            str++;
        }
        c++;
    }
    *str = '\0';
}


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
