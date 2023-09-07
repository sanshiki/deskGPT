import openai

# openai.log = "debug"
openai.api_key = "sk-Hd9qkPQpyP5u3B4XxGSG3HEdlr41u0TV25HagwImVs1R2Xl8"
openai.api_base = "https://api.chatanywhere.com.cn/v1"
history = ''
cnt=0
answer_now=''
pre_knowledge=''

answer_txt = './config/answer.txt'
# question_txt = './config/question.txt'
pre_knowledge_txt = './config/pre_knowledge.txt'

use_console_input = False


# 非流式响应
# completion = openai.ChatCompletion.create(model="gpt-3.5-turbo", messages=[{"role": "user", "content": "Hello world!"}])
# print(completion.choices[0].message.content)

def gpt_35_api_stream(messages: list):
    global history
    global answer_now
    """为提供的对话消息创建新的回答 (流式传输)

    Args:
        messages (list): 完整的对话消息
        api_key (str): OpenAI API 密钥

    Returns:
        tuple: (results, error_desc)
    """
    try:
        response = openai.ChatCompletion.create(
            model='gpt-3.5-turbo',
            messages=messages,
            stream=True,
        )
        completion = {'role': '', 'content': ''}
        for event in response:
            if event['choices'][0]['finish_reason'] == 'stop':
                # print(f'收到的完成数据: {completion}')
                break
            for delta_k, delta_v in event['choices'][0]['delta'].items():
                #print(f'流响应数据: {delta_k} = {delta_v}')
                completion[delta_k] += delta_v
        messages.append(completion)  # 直接在传入参数 messages 中追加消息
        #history=history+'你的回答：'
        #history=history+completion['content']
        answer_now=completion['content']
        print(answer_now)

        file = open(answer_txt, 'w+', encoding='utf-8')
        file.write(completion['content'])
        return (True, '')
    except Exception as err:
        return (False, f'OpenAI API 异常: {err}')

def add_history_text(text):
    global history
    history=history+text

def pre_knowledge_init():
    global pre_knowledge
    global history
    file = open(pre_knowledge_txt, 'r', encoding='utf-8')
    read_text = file.read()
    pre_knowledge = read_text

    history=pre_knowledge

def send_to_chatGPT(user_input=None):
    global history
    global answer_now
    global cnt
    global pre_knowledge

# if __name__ == '__main__':
    #file = open('question.txt', 'r', encoding='utf-8')
    #read_text = file.read()
    #user_input = read_text

    if  user_input.lower() != '结束对话':
        
        if use_console_input:
            user_input = input('请输入您的问题: ')
        else:
            if user_input == None:
                #报错
                print('user_input is None')
                return
                    
        input_text = user_input
        messages = [{'role': 'user','content': history+'我现在提问的内容：'+input_text},]
        print(gpt_35_api_stream(messages))

        history=history+'我之前提问的内容：'
        add_history_text(input_text)
        history=history+'你的回答：'
        history=history+answer_now

        print(history)
        cnt=cnt+1
    cnt=0
