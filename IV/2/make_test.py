#!/usr/bin/python


# with open("input.txt", 'wb') as f:
#     lst = [randint(0, 10) for i in range(randint(0, 0))]
#     print(*lst)
#     for x in lst:
#         f.write(x.to_bytes(4, 'little', signed=True))


import subprocess
import os
from random import choice, randint
from string import ascii_lowercase
from sys import argv

for _ in range(10000):
    lst = [randint(-10, 10) for i in range(10**7)]
    # lst = list(map(int, "4 8 6 9 9 10 2".split()))
    with open("input.txt", 'wb') as f:
        for x in lst:
            f.write(x.to_bytes(4, 'little', signed=True))
    ans = sorted(lst)
    # print(*lst)
    break
    raw_res = subprocess.run([os.path.join(os.getcwd(), './2.2'), "input.txt"])

    with open("input.txt", 'rb') as f:
        data = f.read()
    res = [int.from_bytes(data[4*i:4*(i+1)], 'little', signed=True)
           for i in range(len(data) // 4)]

    if not all(map(lambda x: x[0] == x[1], zip(ans, res))):
        print(*lst)
        print("true", ans[:10])
        print("your", res[:10])
        break
