stn1 = "你的回答：好好好"
stn2 = "你的回答："

if stn2 in stn1:
    print("Yes")

    #将stn1中的stn2替换成空
    stn1 = stn1.replace(stn2,"")
    print(stn1)