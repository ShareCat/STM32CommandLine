/**
  ******************************************************************************
  * @file:      LHD8006_app_conf.h
  * @author:    Cat  
  * @version:   V1.0  
  * @date:      2018-3-1
  * @brief:     APP程序条件编译控制，所有有关主机特性或是分支的条件编译
                都集中到这个文件，方便统一配置管理
  * @attention: 编译前一定要确认配置是否正确
  ******************************************************************************
  */


#include <stdint.h>

  
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LHD8006_APP_CONF_H
#define __LHD8006_APP_CONF_H



/* ------------------------------------------------ APP宏定义，发布就必须定义 */

/* 0x08008000是APP起始地址，0x08000000到0x08008000是bootloader占32KB */
#define APP_ADDR    ((uint32_t)0x08008000)

/* 定义本工程为APP程序，需要设置中断向量偏移，Keil软件里面也要修改Flash起始地址为FLASH_APP_ADDR */
//#define APP_PROJECT


/* cm_backtrace(针对 ARM Cortex-M 系列 MCU 的错误代码自动追踪、定位，
    错误原因自动分析的开源库，链接：
    https://github.com/armink/CmBacktrace)，如果使用则定义 */
#define USING_CM_BACKTRACE


/* --------------------------------------------------------- 命令行调试宏定义 */

#define NXP_CLI_DEBUG               /* NXP命令行调试宏定义开关 */
#define FATFS_CLI_DEBUG             /* FATFS命令行调试宏定义开关 */


/* ----------------------------------------------------------- 模块调试宏定义 */

//#define APP_W5500_DEBUG             /* app_w5500文件调试 */
//#define APP_OTA_DEBUG               /* OTA调试 */



#endif /* __LHD8006_APP_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
