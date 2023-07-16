str1 = "fffffc3e"
str2 = "-ffffc3e"

b1 = b'\xff\xff\xfc\x3e'

#取b1的反码
b2 = b''
for i in b1:
    b2 += bytes([255 - i])

print(b1)
print(b2)