from moudule import bytetools

str1 = "fffffc3e"
str2 = "-ffffc3e"

# b1 = b'\xff\x7f'
b1 = b'\x00\x80'

def byte2int(byte_data):
    # flag = 1
    # if byte_data[0] == 255:
    #     byte_data[0] = 0
    #     flag = -1
    # return int.from_bytes(byte_data, byteorder='big', signed=True)  # 大端模式，有符号数
    return int.from_bytes(byte_data, byteorder='little', signed=True) # 小端模式，有符号数

# print(byte2int(b1))

# print((bytetools.setbit(b'\x00',7,1)))

# #取b1的反码
# b2 = b''
# for i in b1:
#     b2 += bytes([255 - i])

# print(b1)
# print(b2)

# print(bytetools.setbyte(b1,1,b'\x00'))

# b_arr = bytearray(b1)
# print(b_arr)
# b_arr[0] = 255
# print(b_arr)

b1 = b'\x00\x00\x00\x80'
b2 = b'\xff\x80'

b3 = bytetools.setbytes(b1,0,b2)
print(b3)