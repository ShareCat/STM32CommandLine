

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H
#define __BUTTON_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "string.h"


//According to your need to modify the constants.
#define TICKS_INTERVAL    20                        /* 按键任务调度周期，单位ms */
#define DEBOUNCE_TICKS    2                         /* 消抖周期，MAX 8 */
#define SHORT_TICKS       (300 / TICKS_INTERVAL)    /* 短按时间300ms */
#define LONG_TICKS        (10000 / TICKS_INTERVAL)  /* 长按时间10s */


typedef void (*BtnCallback)(void*);

typedef enum {
    PRESS_DOWN = 0,
    PRESS_UP,
    PRESS_REPEAT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS_START,
    LONG_PRESS_HOLD,
    number_of_event,
    NONE_PRESS
} PressEvent;

typedef struct Button {
    uint16_t ticks;
    uint8_t  repeat : 4;
    uint8_t  event : 4;
    uint8_t  state : 3;
    uint8_t  debounce_cnt : 3;
    uint8_t  active_level : 1;
    uint8_t  button_level : 1;
    uint8_t (*hal_button_Level)(void);
    BtnCallback  cb[number_of_event];
    struct Button* next;
} Button;

extern void button_init(struct Button* handle, uint8_t(*pin_level)(), uint8_t active_level);
extern void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);
extern PressEvent get_button_event(struct Button* handle);
extern int button_start(struct Button* handle);
extern void button_stop(struct Button* handle);
extern void button_ticks(void);



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/




#endif /* __BUTTON_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
