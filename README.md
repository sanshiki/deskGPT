# 嵌入式高级实验作业：智能桌宠deskGPT

## 项目介绍

### 简介

本项目是浙江大学嵌入式高级实验课程项目，内容为基于stm32的AI智能桌宠deskGPT，能够实现识别用户语音，并转为文字，通过基于chatGPT3.5的大语言模型AI，实现对用户语音的智能回复，并合成语音，通过扬声器播放。

### 模块介绍

#### 下位机

下位机MCU我们采用stm32f407zgt6，使用cubemx+keil5进行编程。录音以及播放模块采用vs1053模块。

#### 上位机

上位机程序我们使用python3.11.4，语音识别（iat）与语音合成（tts）我们采用科大讯飞的web接口，AI模块我们使用的是chatanywhere的开源项目，参考https://github.com/chatanywhere/GPT_API_free.git。

## 依赖安装

### 安装方法

```bash
pip install <package-name>
```

**注意：pywt的安装方法为 `pip install PyWavelets`**

### python第三方库

websocket、numpy、pyusb、openai、pywt、scipy

## 更新日志

23.7.14：建立项目仓库

嵌入式部分：基本参照https://blog.csdn.net/weixin_44226356/article/details/125153210?ops_request_misc=&request_id=&biz_id=102&utm_term=cubemx%20i2s&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-8-125153210.142^v88^insert_down38v5,239^v2^insert_chatgpt&spm=1018.2226.3001.4187

经测试发现usb_cdc在发送时字节会翻转，故写了一个byteSwap函数

上位机serial：主要是使用了python自带的pyserial库。安装：

```bash
pip install pyserial
```

串口封装为usb_cdc库。

UI部分使用tkinter，还未完成

文件读写封装为fileIO库，主要用的就是pcmFileManager

**已实现的功能**：usb_cdc串口收发，i2s收发，保存为pcm文件

**待解决的问题**：pcm文件的数据不知道正不正确，经过网上找的pcm播放器放出来全是噪音，无法做进一步的滤波处理。

相关csdn链接：
example:https://blog.csdn.net/Laura_Luo/article/details/117779052?spm=1001.2014.3001.5506
usb_cdc:https://blog.csdn.net/zhanglifu3601881/article/details/89441412?spm=1001.2014.3001.5506
inmp441:https://blog.csdn.net/qq_15181569/article/details/131224833?spm=1001.2014.3001.5506
vm8978:https://blog.csdn.net/banzhuan133/article/details/78610883/?spm=1001.2014.3001.5506
i2s:https://blog.csdn.net/Q_Lee/article/details/131217760?spm=1001.2014.3001.5506


23.7.20

usb通信改为使用pyusb库

解决pcm数据格式问题，成功播放出inmp441的录音，但是噪声较大。

尝试过采用带通滤波、小波变换等滤波方法，最后采用小波变换。

经过测试，usb传输数据遗漏严重，暂时不知道如何解决。

科大讯飞的iat、tts模块以及chatGPT api接入均已完成。


23.8：完善项目

解决了usb传输数据遗漏的问题，从单次发送少量数据，快速发送的模式改为单次发送大量数据，降低发送频率，成功解决了数据发送的问题。

基于此，为上下位机编写了数据包以及解包方式。数据包包括用于检测数据的数据头、下位机控制字以及音频数据。

inmp441效果不好，改为使用vs1053，参考正点原子的原代码进行驱动。

成功实现vs1053的录音和播放问题，并且成功串联从录音、iat、tts到播放的模块。

存在问题：vs1053在录音时会播放录到的音频，并且噪声很大。第一次上电时无法正常工作。


23.9

查资料发现，可以通过修改vs1053的SCI_VOL寄存器来使得录音时不播放，但是正点原子例程的atk_mo1053_set_volume修改不了。

后来发现通过修改sram可以实现。参考博客：http://www.openedv.com/posts/list/14900.htm

在上位机初始化之后，提前发送几个进入录音模式的空数据包，能够解决第一次上电无法工作的问题。

成功串联chatGPT模块，并且使程序流程可以循环。

存在问题：合成的语音没有电流声，但是播放时存在电流声。初步猜测是传输间隔的问题。目前的传输间隔由sleep控制，过于粗糙。

Emoji文件夹需要使用opencv编译，能够产生二进制视频文件(240*240*3).  
