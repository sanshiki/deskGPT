def byte2int(byte_data):
    # flag = 1
    # if byte_data[0] == 255:
    #     byte_data[0] = 0
    #     flag = -1
    # return int.from_bytes(byte_data, byteorder='big', signed=True)  # 大端模式，有符号数
    return int.from_bytes(byte_data, byteorder='little', signed=True) # 小端模式，有符号数

def int2byte(int_data):
    return int_data.to_bytes(2, byteorder='little', signed=True)

#建立有符号24位和16位之间的映射关系，例如-32768对应-8388608
def byte24to16(byte_data):
    float24_data = float(byte2int(byte_data))
    float16_data = float24_data / 256
    return int2byte(int(float16_data))



#对字节值进行处理，如果是补码形式，转为原码，返回原码
def byteCheck(byte_data):
    origin_data = b''
    if byte_data[0] == 255:
        for i in byte_data:
            origin_data += bytes([255 - i])
        #把origin_data的第一个字节改成ff，以表示负数

        return origin_data
    else:
        return byte_data