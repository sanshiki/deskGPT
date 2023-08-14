#ifndef __DATAQUEUE_H__
#define __DATAQUEUE_H__

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"

#define DATA_SIZE 128
#define DATA_QUEUE_SIZE 10

typedef struct __dataPack{
    uint8_t data[DATA_SIZE];
    uint8_t id;
} dataPack_t;

typedef struct __dataQueue{
    dataPack_t dataPack[DATA_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t size;
} dataQueue_t;

void dataQueueInit(dataQueue_t *pdataQueue);
void pcDataQueueInit(void);
uint8_t dataQueueIsEmpty(dataQueue_t *pdataQueue);
uint8_t pcDataQueueIsEmpty(void);
uint8_t dataQueueIsFull(dataQueue_t *pdataQueue);
uint8_t pcDataQueueIsFull(void);
uint8_t dataQueuePush(dataQueue_t *pdataQueue, uint8_t *data, uint32_t len);
uint8_t pcDataQueuePush(uint8_t *data, uint32_t len);
uint8_t dataQueuePop(dataQueue_t *pdataQueue, uint8_t *data, uint32_t len);
uint8_t pcDataQueuePop(uint8_t *data, uint32_t len);

#endif // __DATAQUEUE_H__
