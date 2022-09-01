import requests
import random
import time
i = 0
while True:
    #url = 'http://localhost:25000/send?price={}&side={}&quantity={}'.format(100+random.uniform(-2, 2), int(random.random()>0.5), int(random.uniform(1, 10)))
    url = "https://api.krnl.finance/etf_model?key=1234" #"http://localhost:5000/etf_model?key=1234"
    x = requests.get(url)
    print(i)
    print(x.json())
    i+=1
    time.sleep(0.1)
