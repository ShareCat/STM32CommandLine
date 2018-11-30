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


/*
    TODO:
    单独使用chip ID加密不是很完美，破解者可以读取2个chip的flash进行对比，就能找
    到不一样的地方，这个地方正好是加密的密码，如果破解者将这些密码清空（0XFF），
    相当于就破解成功了。对于这个隐患可以在flash的另一个位置设置一个const变量，初
    始值也是0xFFFFFFFF，第一次运行时候，赋值为magic number，以后每次上电除了要判
    断密码正确，还要判断magic number是否正确，这样就能防止上述的隐患，因为每个
    chip的flash文件对比时候，只知道密码不一样，当把密码清空（0XFF）后上电，也不
    行，因为这个时候magic number正确，表示不是真正的第一次上电。
*/
#define SECURE_MAGIC_NUM        0x2E534543
#define SECURE_MAGIC_NUM_ADDR   0x08000300

const uint8_t secure_magic_num __attribute__((at(SECURE_MAGIC_NUM_ADDR))) = 0xFFFFFFFF;


#define SECURE_NUM              3
#define SECURE_ID_LEN           24
#define SECURE_ID_ADDR          0x08000400

const uint8_t secure_id[SECURE_ID_LEN] __attribute__((at(SECURE_ID_ADDR))) = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};



/**
  * @brief  get the ID of MCU
  * @param  id
  * @param  id length
  * @retval null
  */
void get_chip_id(uint8_t *id, uint8_t chip_id_len)
{
    /* 0X1FFFF7E8 ---> chip ID addr. of STM32F103 */
    uint8_t *addr = (uint8_t *)0X1FFFF7E8;

    memcpy(id, addr, chip_id_len);

#ifdef SECURE_BOOT_DBG
    uint8_t i;

    printf("chip_id: ");
    for (i = 0; i < chip_id_len; i++) {
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
static void get_secure_id(uint8_t *chip_id, uint8_t chip_id_len, uint8_t *secure_id, uint8_t secure_id_len)
{
    uint8_t i;
    uint8_t *addr = secure_id;

    if ((chip_id_len * 2) != secure_id_len) {
        printf("(chip_id_len * 2) != secure_id_len");
        return;
    }

    for (i = 0; i < chip_id_len; i++) {
        if (0 == (i % 2)) {
            addr[i] = chip_id[i] / SECURE_NUM;
        } else {
            addr[i] = (~chip_id[i]) - SECURE_NUM;
        }
    }

    addr += chip_id_len;
    for (i = 0; i < (secure_id_len - chip_id_len); i++) {
        if (0 == (i % 2)) {
            addr[i] = chip_id[i] + SECURE_NUM;
        } else {
            addr[i] = (~chip_id[i]) * addr[i - 1];
        }
    }

#ifdef SECURE_BOOT_DBG
    printf("secure_id: ");
    for (i = 0; i < secure_id_len; i++) {
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
    uint8_t secure_id[SECURE_ID_LEN];

    get_chip_id(chip_id, CHIP_ID_LEN);

    get_secure_id(chip_id, CHIP_ID_LEN, secure_id, SECURE_ID_LEN);

    write_to_flash(SECURE_ID_ADDR, secure_id, SECURE_ID_LEN);

#if 0
    uint8_t i;
    uint8_t secure_id_temp[SECURE_ID_LEN];

    memcpy(secure_id_temp, SECURE_ID_ADDR, SECURE_ID_LEN);
    printf("first_boot_encrypt: ");
    for (i = 0; i < SECURE_ID_LEN; i++) {
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
    uint8_t secure_id[SECURE_ID_LEN];
    uint8_t secure_id_temp[SECURE_ID_LEN];

    get_chip_id(chip_id, CHIP_ID_LEN);

    get_secure_id(chip_id, CHIP_ID_LEN, secure_id, SECURE_ID_LEN);

    memcpy(secure_id_temp, (uint8_t *)SECURE_ID_ADDR, SECURE_ID_LEN);

    /*
        compare the encrypted id
    */
    if (0 != memcmp(secure_id, secure_id_temp, SECURE_ID_LEN)) {
        /* not secure */
        secure = SECURE_FALSE;
        //printf("secure boot fail \r\n");
    } else {
        /* secure */
        secure = SECURE_TRUE;
        //printf("secure boot success \r\n");
    }

#if 0
    uint8_t i;

    memcpy(secure_id_temp, SECURE_ID_ADDR, SECURE_ID_LEN);
    printf("boot_encrypt_check: ");
    for (i = 0; i < SECURE_ID_LEN; i++) {
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
    uint8_t i;
    uint8_t *addr = (uint8_t *)SECURE_ID_ADDR;
    uint8_t first_boot = True;
    uint8_t secure = SECURE_TRUE;

    for (i = 0; i < SECURE_ID_LEN; i++) {
        if (0xff != addr[i]) {
            first_boot = False;
            break;
        }
    }

    if (True == first_boot) {
        first_boot_encrypt();

#ifdef SECURE_BOOT_DBG
        printf("first_boot_encrypt \r\n");
#endif

    } else {
        secure = boot_encrypt_check();

#ifdef SECURE_BOOT_DBG
        printf("boot_encrypt_check \r\n");
#endif
    }

    return secure;
}


/*********************************************END OF FILE**********************/
