#include "dataQueue.h"

dataQueue_t pcDataQueue;

void dataQueueInit(dataQueue_t *pdataQueue)
{
    pdataQueue->head = 0;
    pdataQueue->tail = 0;
    pdataQueue->size = 0;
}

void pcDataQueueInit(void)
{
    dataQueueInit(&pcDataQueue);
}

uint8_t dataQueueIsEmpty(dataQueue_t *pdataQueue)
{
    return (pdataQueue->size == 0);
}

uint8_t pcDataQueueIsEmpty(void)
{
    return (pcDataQueue.size == 0);
}

uint8_t dataQueueIsFull(dataQueue_t *pdataQueue)
{
    return (pdataQueue->size == DATA_QUEUE_SIZE);
}

uint8_t pcDataQueueIsFull(void)
{
    return (pcDataQueue.size == DATA_QUEUE_SIZE);
}

uint8_t dataQueuePush(dataQueue_t *pdataQueue, uint8_t *data, uint32_t len)
{
    if(dataQueueIsFull(pdataQueue))
    {
        return 0;
    }
    for(int i=0;i<len;i++)
    {
        pdataQueue->dataPack[pdataQueue->tail].data[i] = data[i];
    }
    pdataQueue->dataPack[pdataQueue->tail].id = pdataQueue->tail;
    pdataQueue->tail = (pdataQueue->tail + 1) % DATA_QUEUE_SIZE;
    pdataQueue->size++;
    return 1;
}

uint8_t pcDataQueuePush(uint8_t *data, uint32_t len)
{
    return dataQueuePush(&pcDataQueue, data, len);
}

uint8_t dataQueuePop(dataQueue_t *pdataQueue, uint8_t *data, uint32_t len)
{
    if(dataQueueIsEmpty(pdataQueue))
    {
        return 0;
    }
    for(int i=0;i<len;i++)
    {
        data[i] = pdataQueue->dataPack[pdataQueue->head].data[i];
    }
    pdataQueue->head = (pdataQueue->head + 1) % DATA_QUEUE_SIZE;
    pdataQueue->size--;
    return 1;
}

uint8_t pcDataQueuePop(uint8_t *data, uint32_t len)
{
    return dataQueuePop(&pcDataQueue, data, len);
}
