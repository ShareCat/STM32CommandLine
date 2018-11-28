/**
  ******************************************************************************
  * @file:      secure_boot.c
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-11-28
  * @brief:     secure boot
  * @attention:
  ******************************************************************************
  */


#include <stdint.h>
#include <string.h>

#include "./secure_boot.h"

#include "stm32f10x_flash.h"



#ifndef True
#define True    1
#endif

#ifndef False
#define False   0
#endif


#define SECURE_BOOT_DBG     /* debug */


#define CHIP_ID_LEN 12


#define SECURE_NUM          1
#define SECURE_BOOT_ADDR    0x08000100
const uint32_t SECURE_BOOT_VAL1 __attribute__((at(SECURE_BOOT_ADDR))) = 0xFFFFFFFF;
const uint32_t SECURE_BOOT_VAL2 __attribute__((at(SECURE_BOOT_ADDR + 4))) = 0xFFFFFFFF;
const uint32_t SECURE_BOOT_VAL3 __attribute__((at(SECURE_BOOT_ADDR + 8))) = 0xFFFFFFFF;


/**
  * @brief  get the ID of MCU
  * @param  id
  * @param  id length
  * @retval null
  */
void get_chip_id(uint8_t *id, uint8_t len)
{
    uint8_t *addr = (uint8_t *)0X1FFFF7E8;

    memcpy(id, addr, len);

#ifdef SECURE_BOOT_DBG
    uint8_t i;

    printf("chip_id: ");
    for (i = 0; i < CHIP_ID_LEN; i++) {
        printf("%02x ", id[i]);
    }
    printf("\r\n\r\n");
#endif

#if 0
    id[0] = *(volatile uint32_t *)(0X1FFFF7F0);  // higher byte
    id[1] = *(volatile uint32_t *)(0X1FFFF7EC);  // 
    id[2] = *(volatile uint32_t *)(0X1FFFF7E8);  // lower byte
#endif
}


/**
  * @brief  get the flash size of MCU
  * @param  size
  * @retval null
  */
void get_chip_flash_size(uint16_t *size)
{
    *size = *(volatile uint16_t *)(0X1FFFF7E0);

#ifdef SECURE_BOOT_DBG
    printf("chip flash size = %d KB \r\n", *size);
#endif
}


/**
  * @brief  write the encrypted id to flash
  * @param  addr
  * @param  buff
  * @param  len
  * @retval null
  */
static void write_to_flash(uint32_t addr, uint8_t *buff, uint16_t len)
{
    uint32_t *data = (uint32_t *)buff;
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    if (0 != (len % 4)) return;

    /* unlock the flash */
    FLASH_Unlock();

    /* clear the mask */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    while((len > 0) && (FLASHStatus == FLASH_COMPLETE)) {
        FLASHStatus = FLASH_ProgramWord(addr, *data);
        data++;
        addr += 4;
        len -= 4;
    }

    /* lock the flash */
    FLASH_Lock();
}


/**
  * @brief  get the encrypted id
  * @param  chip_id
  * @param  secure_id
  * @param  len
  * @retval null
  */
static void get_secure_id(uint8_t *chip_id, uint8_t *secure_id, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < len; i++) {
        if (0 == (i % 2)) {
            secure_id[i] = chip_id[i] + SECURE_NUM;
        } else {
            secure_id[i] = (~chip_id[i]) - SECURE_NUM;
        }
    }

#ifdef SECURE_BOOT_DBG
    printf("secure_id: ");
    for (i = 0; i < CHIP_ID_LEN; i++) {
        printf("%02x ", secure_id[i]);
    }
    printf("\r\n\r\n");
#endif
}


/**
  * @brief  first boot, so write the encrypted id to flash
  * @param  null
  * @retval null
  */
static void first_boot_encrypt(void)
{
    uint8_t chip_id[CHIP_ID_LEN];
    uint8_t secure_id[CHIP_ID_LEN];

    get_chip_id(chip_id, CHIP_ID_LEN);

    get_secure_id(chip_id, secure_id, CHIP_ID_LEN);

    write_to_flash(SECURE_BOOT_ADDR, secure_id, CHIP_ID_LEN);

#if 0
    uint8_t i;
    uint8_t secure_id_temp[CHIP_ID_LEN];

    memcpy(secure_id_temp, SECURE_BOOT_ADDR, CHIP_ID_LEN);
    printf("first_boot_encrypt: ");
    for (i = 0; i < CHIP_ID_LEN; i++) {
        printf("%02x ", secure_id_temp[i]);
    }
    printf("\r\n\r\n");
#endif
}


/**
  * @brief  not the first boot, so compare the encrypted id
  * @param  null
  * @retval null
  */
static uint8_t boot_encrypt_check(void)
{
    uint8_t secure;
    uint8_t chip_id[CHIP_ID_LEN];
    uint8_t secure_id[CHIP_ID_LEN];
    uint8_t secure_id_temp[CHIP_ID_LEN];

    get_chip_id(chip_id, CHIP_ID_LEN);

    get_secure_id(chip_id, secure_id, CHIP_ID_LEN);

    memcpy(secure_id_temp, (uint8_t *)SECURE_BOOT_ADDR, CHIP_ID_LEN);

    /*
        compare the encrypted id
    */
    if (0 != memcmp(secure_id, secure_id_temp, CHIP_ID_LEN)) {
        /* not secure */
        secure = SECURE_FALSE;
        printf("secure boot fail \r\n");
    } else {
        /* secure */
        secure = SECURE_TRUE;
        printf("secure boot success \r\n");
    }

#if 0
    uint8_t i;

    memcpy(secure_id_temp, SECURE_BOOT_ADDR, CHIP_ID_LEN);
    printf("boot_encrypt_check: ");
    for (i = 0; i < CHIP_ID_LEN; i++) {
        printf("%02x ", secure_id_temp[i]);
    }
    printf("\r\n\r\n");
#endif

    return secure;
}


/**
  * @brief  secure boot check
  * @param  null
  * @retval null
  */
uint8_t secure_boot_check(void)
{
    uint32_t *addr;
    uint8_t first_boot = True;
    uint8_t secure = SECURE_TRUE;

    addr = (uint32_t *)&SECURE_BOOT_VAL1;
    if (0xffffffff != *addr) {
        first_boot = False;
    }

    addr = (uint32_t *)&SECURE_BOOT_VAL2;
    if (0xffffffff != *addr) {
        first_boot = False;
    }

    addr = (uint32_t *)&SECURE_BOOT_VAL3;
    if (0xffffffff != *addr) {
        first_boot = False;
    }

    if(True == first_boot) {
        first_boot_encrypt();
    } else {
        secure = boot_encrypt_check();
    }

    return secure;
}


/*********************************************END OF FILE**********************/
