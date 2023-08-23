import json
data = {
    'name' : 'myname',
    'age' : 100,
}
json_str = json.dumps(data)
print(json_str)
print(type(json_str))
print(json.loads(json_str))
print(type(json.loads(json_str)))
print(json.loads(json_str)['name'])