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
    

#修改字节中的指定位
def setbit(byte, bit, value=1):
    if type(byte) == str:
        byte = int(byte, 16)
    elif type(byte) == bytes:
        byte = int.from_bytes(byte, byteorder='big', signed=False)

    result = 0
    
    if value == 0:
        result = byte & ~(1 << bit)
    else:
        result = byte | (1 << bit)

    return result.to_bytes(1, byteorder='big', signed=False)

#修改字节
def setbyte(byte, idx, value):
    bytearr = bytearray(byte)
    if type(value) == str:
        value = int(value, 16)
    elif type(value) == bytes:
        value = int.from_bytes(value, byteorder='big', signed=False)
    bytearr[idx] = value
    return bytes(bytearr)
