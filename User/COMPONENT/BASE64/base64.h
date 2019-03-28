#ifndef _BASE64_H__
#define _BASE64_H__

char * base64_encode(  unsigned char * bindata, char * base64, int binlength );
int base64_decode(  char * base64, unsigned char * bindata );

#endif

