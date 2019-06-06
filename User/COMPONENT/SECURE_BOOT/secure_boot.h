/**
  ******************************************************************************
  * @file:      secure_boot.h
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-11-28
  * @brief:     secure boot
  * @attention:
  ******************************************************************************
  */


#ifndef __SECURE_BOOT_H
#define __SECURE_BOOT_H


#define SECURE_TRUE     1
#define SECURE_FALSE    0


extern uint8_t secure_boot_check(void);


#endif /* __SECURE_BOOT_H */
