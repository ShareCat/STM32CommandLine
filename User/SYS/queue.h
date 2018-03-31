#ifndef __QUEUE_H
#define	__QUEUE_H

#include <stdint.h>

#define True  1
#define False 0
#define NULL  0

enum {
    ZERO,
    NUM8  = 8,
    NUM16 = 16,
    NUM32 = 32
};



#define ENABLE_ALL_IRQ()    __enable_irq()     /* disable all interrupt */
#define DISABLE_ALL_IRQ()   __disable_irq()    /* enable all interrupt */



extern uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_in(uint16_t *Front,uint16_t *Rear,uint8_t *PBase,uint16_t Len,uint8_t *PData);
extern uint8_t queue_out(uint16_t *Front,uint16_t *Rear,uint8_t *PBase,uint16_t Len,uint8_t *PData);

//queue init.
#define QueueInit(q)         queue_init((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//queue full?
#define QueueFull(q)         queue_full((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//queue empty?
#define QueueEmpty(q)        queue_empty((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//put pdata in queue
#define QueueIn(q, pdata)    queue_in((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)
//get data from queue
#define QueueOut(q, pdata)   queue_out((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)

//8 Bytes Queue Struct
typedef struct queue8 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[8+1];
} QUEUE8_TYPE;

//16 Bytes Queue Struct
typedef struct queue16 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[16+1];
} QUEUE16_TYPE;

//32 Bytes Queue Struct
typedef struct queue32 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[32+1];
} QUEUE32_TYPE;

//64 Bytes Queue Struct
typedef struct queue64 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[64+1];
} QUEUE64_TYPE;

//128 Bytes Queue Struct
typedef struct queue128 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[128+1];
} QUEUE128_TYPE;

//256 Bytes Queue Struct
typedef struct queue256 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[256+1];
} QUEUE256_TYPE;

#endif /* __QUEUE_H */

