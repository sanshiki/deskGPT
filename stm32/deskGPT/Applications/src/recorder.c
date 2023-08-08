#include "recorder.h"

//VS1053的WAV录音有bug,这个plugin可以修正这个问题 							    
const uint16_t wav_plugin[40]=/* Compressed plugin */ 
{ 
0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */ 
0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */ 
0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */ 
0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */ 
0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,  
}; 

//vs10xx装载patch.
//patch：patch首地址
//len：patch长度
void VS_Load_Patch(uint16_t *patch,uint16_t len) 
{
	uint16_t i; 
	uint16_t addr, n, val; 	  			   
	for(i=0;i<len;) 
	{ 
		addr = patch[i++]; 
		n    = patch[i++]; 
		if(n & 0x8000U) //RLE run, replicate n samples 
		{ 
			n  &= 0x7FFF; 
			val = patch[i++]; 
			while(n--)atk_mo1053_write_cmd(addr, val);  
		}else //copy run, copy n sample 
		{ 
			while(n--) 
			{ 
				val = patch[i++]; 
				atk_mo1053_write_cmd(addr, val); 
			} 
		} 
	} 	
} 		  	  

//激活PCM 录音模式
//agc:0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍		  
void recoder_enter_rec_mode(uint16_t agc)
{
	//如果是IMA ADPCM,采样率计算公式如下:
 	//采样率=CLKI/256*d;	
	//假设d=0,并2倍频,外部晶振为12.288M.那么Fc=(2*12288000)/256*6=16Khz
	//如果是线性PCM,采样率直接就写采样值 
   	atk_mo1053_write_cmd(SPI_BASS,0x0000);    
 	atk_mo1053_write_cmd(SPI_AICTRL0,8000);	//设置采样率,设置为16Khz
 	atk_mo1053_write_cmd(SPI_AICTRL1,agc);		//设置增益,0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍	
 	atk_mo1053_write_cmd(SPI_AICTRL2,0);		//设置增益最大值,0,代表最大值65536=64X
 	atk_mo1053_write_cmd(SPI_AICTRL3,6);		//左通道(MIC单声道输入)，线性PCM
	atk_mo1053_write_cmd(SPI_CLOCKF,0X2000);	//设置VS10XX的时钟,MULT:2倍频;ADD:不允许;CLK:12.288Mhz
	atk_mo1053_write_cmd(SPI_MODE,0x1804);		//MIC,录音激活    
 	delay_ms(5);					//等待至少1.35ms 
 	VS_Load_Patch((uint16_t*)wav_plugin,40);//VS1053的WAV录音需要patch
}

//初始化WAV头.
void recoder_wav_init(__WaveHeader* wavhead) //初始化WAV头			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//还未确定,最后需要计算
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//大小为16个字节
	wavhead->fmt.AudioFormat=0X01; 		//0X01,表示PCM;0X01,表示IMA ADPCM
 	wavhead->fmt.NumOfChannels=1;		//单声道
 	wavhead->fmt.SampleRate=8000;		//8Khz采样率 采样速率
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//16位,即2个字节
 	wavhead->fmt.BlockAlign=2;			//块大小,2个字节为一个块
 	wavhead->fmt.BitsPerSample=16;		//16位PCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//数据大小,还需要计算  
}

// uint8_t recorder_play(void)
// {
//     u8 res;
// 	u8 key;
// 	u8 rval=0;
// 	__WaveHeader *wavhead=0;
// 	uint32_t sectorsize=0;
// 	FIL* f_rec=0;					//文件		    
//  	DIR recdir;	 					//目录
// 	u8 *recbuf;						//数据内存	 
//  	uint16_t w;
// 	uint16_t idx=0;	    
// 	u8 rec_sta=0;					//录音状态
// 									//[7]:0,没有录音;1,有录音;
// 									//[6:1]:保留
// 									//[0]:0,正在录音;1,暂停录音;
//  	u8 *pname=0;
// 	u8 timecnt=0;					//计时器   
// 	uint32_t recsec=0;					//录音时间
//  	u8 recagc=4;					//默认增益为4 
// 	u8 playFlag=0;					//播放标志
// }