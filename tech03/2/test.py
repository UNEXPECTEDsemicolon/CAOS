import subprocess
import os
from random import choice, randint
from string import ascii_lowercase
from sys import argv

cnt = 0
for _ in range(10000):
    n = randint(1, 10**7) # число элементов
    lst = [randint(-1000, 1000) for i in range(n)]
    with open("input.txt", 'wb') as f:
        for x in lst:
            f.write(x.to_bytes(4, 'little', signed=True))
    ans = sorted(lst)
    raw_res = subprocess.run([os.path.join(os.getcwd(), argv[1]), "input.txt"])
    with open("input.txt", 'rb') as f:
        data = f.read()
    res = [int.from_bytes(data[4*i:4*(i+1)], 'little', signed=True)
           for i in range(len(data) // 4)]
    cnt += 1
    print("\rtest number", cnt, end='')
    if not all(map(lambda x: x[0] == x[1], zip(ans, res))):
        print('\n', *lst)
        print("true", ans[:10])
        print("your", res[:10])
        break
