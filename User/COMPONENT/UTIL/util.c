#include "./util.h"
#include <string.h>


union endian_node
{
    unsigned int m;
    char c;
};

int big_little_endian_check(void)
{
    union endian_node data;
    int rtn;

    data.m = 0X12345678;
    //printf("0x%02x \n", data.c);
    if (0X78 == data.c) {
        //printf("Little-Endian \n");
        rtn = 0;
    } else if (0X12 == data.c) {
        //printf("Big-Endian \n");
        rtn = 1;
    }

    return rtn;
}


/**
  * @brief  判断字符串是否是全数字
  * @param  p_str 要判断的字符串
  * @retval 1 字符串是全数字
  */
int str_is_digit(char *p_str)
{
    int rtn = 0;

    if (NULL == p_str) return -1;

    if (strspn(p_str, "0123456789") == strlen(p_str)) {
        /* 全数字 */
        rtn = 1;
    }

    return rtn;
}


/**
  * @brief  比较两个字符串数字大小
  * @param  char * 要比较的字符串数字
  * @retval 0相等，大于0表示s比t大，小于0表示s比t小
  */
int strcmpnum(char *s, char *t)
{
    if (strlen(s) == strlen(t)) {
        return strcmp(s , t);
    } else {
        return strlen(s) - strlen(t);
    }
}


/**
  * @brief  将字符串中所有的target字符去掉
  * @param  str 要转化的字符串
  * @retval null
  */
void my_delete_char(char str[], char target)
{
    int i, j;

    for(i = j = 0; str[i]!='\0'; i++){
        if(str[i] != target){
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}


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
  * @brief  将一个十六进制字节串转换成ASCII码表示的十六进制字符串
  * @param  pHex    十六进制数字节串首地址
  * @retval 整数
  */
void my_hex_to_str(unsigned char *pHex, unsigned char *pAscii, int nLen)
{
    unsigned char Nibble[2];
    unsigned int i,j;
    for (i = 0; i < nLen; i++){
        Nibble[0] = (pHex[i] & 0xF0) >> 4;
        Nibble[1] = pHex[i] & 0x0F;
        for (j = 0; j < 2; j++){
            if (Nibble[j] < 10){            
                Nibble[j] += 0x30;
            }
            else{
                if (Nibble[j] < 16)
                    Nibble[j] = Nibble[j] - 10 + 'A';
            }
            *pAscii++ = Nibble[j];
        }               // for (int j = ...)
    }           // for (int i = ...)
}


/**
  * @brief  将十六进制的字符串转换为十六进制数组
  * @param  str 要转化的字符串
  * @retval 整数
  */
int my_str_to_hex(char *str, unsigned char *out, unsigned int *outlen)
{
    char *p = str;
    char high = 0, low = 0;
    int tmplen = strlen(p), cnt = 0;
    tmplen = strlen(p);
    while(cnt < (tmplen / 2))
    {
        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
        p ++;
        cnt ++;
    }
    if(tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
    
    if(outlen != 0) *outlen = tmplen / 2 + tmplen % 2;
    return tmplen / 2 + tmplen % 2;
}


/**
* 此函数可以将所有小写字母转换为大写字母。
*/
void my_str_to_upper(char* str)
{
    char* curr = str;

    //若字符串未结束
    while(*curr != '\0') {
        //判别是否为字母 isalpha ()
        //判别是否为小写 islower() 
        // 以上均为ctype.h提供的函数
        if(isalpha(*curr) && islower(*curr)) {
            //修改字母
            *curr = toupper(*curr);
        }
        //移动指针，判别下一个字母
        curr++;
    }
}


/**
* 此函数可以将所有大写字母转换为小写字母。
*/
void my_str_to_lower(char* str)
{
    char* curr = str;

    while(*curr != '\0') {
        //isupper() 判断字母是否为大写
        if(isalpha(*curr)&&isupper(*curr)) {
            *curr=tolower(*curr);
        }
        curr++;
    }
}


/**
* 此函数可以将一个大写字母转换为小写字母。
*/
char my_tolower(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    } else {
        return c;
    }
}


/**
  * @brief  将十六进制字符串转化成整数
  * @param  str 要转化的字符串
  * @retval 整数
  */
int my_htoi(char s[])
{
    int i;
    int n = 0;

    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) {
        i = 2;
    } else {
        i = 0;
    }

    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i) {
        if (my_tolower(s[i]) > '9') {
            n = 16 * n + (10 + my_tolower(s[i]) - 'a');
        } else {
            n = 16 * n + (my_tolower(s[i]) - '0');
        }
    }
    return n;
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
