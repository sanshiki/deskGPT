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
