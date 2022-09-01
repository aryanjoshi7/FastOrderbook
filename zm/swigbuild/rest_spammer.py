import requests
import random
import time

while True:
    url = 'https://ec2-18-222-162-147.us-east-2.compute.amazonaws.com:81/send?price={}&side={}&quantity={}'.format(10+random.uniform(-1, 1), int(random.random()>0.5), int(random.uniform(1, 10)))
    x = requests.get(url, verify = False)
    print(x.text)
    time.sleep(0.1)
