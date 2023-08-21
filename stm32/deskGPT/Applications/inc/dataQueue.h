#ifndef __DATAQUEUE_H__
#define __DATAQUEUE_H__

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include<string.h>

#define DATA_SIZE 550   //128
#define DATA_QUEUE_SIZE 5   //10
#define AUDIO_DATA_SIZE 32 //32
#define AUDIO_DATA_BUF_SIZE 512
#define AUDIO_DATA_QUEUE_SIZE 512 //512

typedef struct __dataPack
{
    uint8_t data[DATA_SIZE];
    uint8_t id;
} dataPack_t;

typedef struct __dataQueue
{
    dataPack_t dataPack[DATA_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t size;
} dataQueue_t;

typedef struct __audioDataQueue
{
    uint32_t data[AUDIO_DATA_QUEUE_SIZE][AUDIO_DATA_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t size;
} audioDataQueue_t;

void dataQueueInit(dataQueue_t *pdataQueue);
void pcDataQueueInit(void);
void audioDataQueueInit(void);
uint8_t dataQueueIsEmpty(dataQueue_t *pdataQueue);
uint8_t pcDataQueueIsEmpty(void);
uint8_t audioDataQueueIsEmpty(void);
uint8_t dataQueueIsFull(dataQueue_t *pdataQueue);
uint8_t pcDataQueueIsFull(void);
uint8_t audioDataQueueIsFull(void);
uint8_t dataQueuePush(dataQueue_t *pdataQueue, uint8_t *data, uint32_t len);
uint8_t pcDataQueuePush(uint8_t *data, uint32_t len);
// uint8_t audioDataQueuePush32(uint32_t *data, uint32_t len);
// uint8_t audioDataQueuePush16(uint16_t *data, uint32_t len);
// uint8_t audioDataQueuePush8(uint8_t *data, uint32_t len);
uint8_t audioDataQueuePush8(uint8_t *data);
uint8_t dataQueuePop(dataQueue_t *pdataQueue, uint8_t *data, uint32_t len);
uint8_t pcDataQueuePop(uint8_t *data, uint32_t len);
// uint32_t audioDataQueuePop();
uint8_t audioDataQueuePop(uint8_t *data);

#endif // __DATAQUEUE_H__
