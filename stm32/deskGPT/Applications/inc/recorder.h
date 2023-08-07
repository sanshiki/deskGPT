#ifndef __RECORDER_H__
#define __RECORDER_H__

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "delay.h"
#include "atk_mo1053.h"

 //RIFF块
typedef __packed struct
{
    uint32_t ChunkID;		   	//chunk id;这里固定为"RIFF",即0X46464952
    uint32_t ChunkSize ;		   	//集合大小;文件总大小-8
    uint32_t Format;	   			//格式;WAVE,即0X45564157
}ChunkRIFF ;
//fmt块
typedef __packed struct
{
    uint32_t ChunkID;		   	//chunk id;这里固定为"fmt ",即0X20746D66
    uint32_t ChunkSize ;		   	//子集合大小(不包括ID和Size);这里为:20.
    uint16_t AudioFormat;	  	//音频格式;0X10,表示线性PCM;0X11表示IMA ADPCM
	uint16_t NumOfChannels;		//通道数量;1,表示单声道;2,表示双声道;
	uint32_t SampleRate;			//采样率;0X1F40,表示8Khz
	uint32_t ByteRate;			//字节速率; 
	uint16_t BlockAlign;			//块对齐(字节); 
	uint16_t BitsPerSample;		//单个采样数据大小;4位ADPCM,设置为4
//	uint16_t ByteExtraData;		//附加的数据字节;2个; 线性PCM,没有这个参数
//	uint16_t ExtraData;			//附加的数据,单个采样数据块大小;0X1F9:505字节  线性PCM,没有这个参数
}ChunkFMT;	   
//fact块 
typedef __packed struct 
{
    uint32_t ChunkID;		   	//chunk id;这里固定为"fact",即0X74636166;
    uint32_t ChunkSize ;		   	//子集合大小(不包括ID和Size);这里为:4.
    uint32_t NumOfSamples;	  	//采样的数量; 
}ChunkFACT;
//data块 
typedef __packed struct 
{
    uint32_t ChunkID;		   	//chunk id;这里固定为"data",即0X61746164
    uint32_t ChunkSize ;		   	//子集合大小(不包括ID和Size);文件大小-60.
}ChunkDATA;

//wav头
typedef __packed struct
{ 
	ChunkRIFF riff;	//riff块
	ChunkFMT fmt;  	//fmt块
	//ChunkFACT fact;	//fact块 线性PCM,没有这个结构体	 
	ChunkDATA data;	//data块		 
}__WaveHeader; 

void VS_Load_Patch(uint16_t *patch,uint16_t len);
void recoder_enter_rec_mode(uint16_t agc);
// uint8_t recorder_play(void);



#endif // __RECORDER_H__