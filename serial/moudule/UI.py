# 串口助手界面
import tkinter as tk


class SerialUI:
    def __init__(self) -> None:
        self.root = tk.Tk()
        self.root.title("串口助手")
        self.root.geometry("600x400")
        self.root.resizable(0, 0)
        self.button = {}
        self.label = {}
        self.entry = {}
        self.check = {}

    def run(self):
        def button1_callback():
            print("button1 pressed")
        
        self.add_button("打开串口", button1_callback, 0, 0)
        self.add_button("关闭串口", None, 0, 1)
        self.add_button("发送", None, 0, 2)
        self.add_label("串口号", 1, 0)
        self.add_label("波特率", 2, 0)
        self.add_label("接收区", 3, 0)
        self.add_label("发送区", 4, 0)
        self.add_entry(1, 1)
        self.add_entry(2, 1)
        self.add_entry(3, 1)
        self.add_entry(4, 1)

        if self.is_button_pressed("打开串口"):
            self.set_label_text("接收区", "打开串口成功")
        else:
            self.set_label_text("接收区", "打开串口失败")

        self.root.mainloop()

    #在指定网格位置添加按钮
    def add_button(self, text, command, row, column):
        button = tk.Button(self.root, text=text, command=command)
        button.grid(row=row, column=column)
        self.button[text] = button

    #在指定网格位置添加标签
    def add_label(self, text, row, column):
        label = tk.Label(self.root, text=text)
        label.grid(row=row, column=column)
        self.label[text] = label

    #在指定网格位置添加输入框
    def add_entry(self, row, column):
        entry = tk.Entry(self.root)
        entry.grid(row=row, column=column)
        self.entry[row] = entry

    #在指定网格位置添加选择框
    def add_checkbutton(self, text, row, column):
        check = tk.Checkbutton(self.root, text=text)
        check.grid(row=row, column=column)
        self.check[text] = check

    #指定按钮是否被按下
    def is_button_pressed(self, text):
        # return self.button[text].instate(['pressed'])
        pass
    
    #获取指定标签的文本
    def get_label_text(self, text):
        return self.label[text].cget("text")
    
    #设置指定标签的文本
    def set_label_text(self, text, value):
        self.label[text].config(text=value)

    #获取指定输入框的文本
    def get_entry_text(self, row):
        return self.entry[row].get()
    
    #设置指定输入框的文本
    def set_entry_text(self, row, value):
        self.entry[row].delete(0, tk.END)
        self.entry[row].insert(0, value)

    #获取指定选择框的状态
    def get_checkbutton_state(self, text):
        return self.check[text].cget("state")
    
    #设置指定选择框的状态
    def set_checkbutton_state(self, text, value):
        self.check[text].config(state=value)