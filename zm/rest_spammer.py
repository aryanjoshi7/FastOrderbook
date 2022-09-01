import requests
import random
import time

while True:
    url = 'http://localhost:25000/send?price={}&side={}&quantity={}'.format(100+random.uniform(-2, 2), int(random.random()>0.5), int(random.uniform(1, 10)))
    x = requests.get(url, verify = False)
    print(x.text)
    time.sleep(0.5)
