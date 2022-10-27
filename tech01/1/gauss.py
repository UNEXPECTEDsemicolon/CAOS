#!/usr/bin/python

import numpy as np
from random import randint, uniform
import os
import subprocess


for _ in range(10000):
    n = 5
    m = 10
    M = np.array([np.array([uniform(-m, m) for i in range(n)]) for j in range(n)])
    V = np.array([uniform(-m, m) for i in range(n)])
    if (abs(np.linalg.det(M)) < 0.000001):
        continue
    with open("input.txt", 'w') as f:
        for i in range(n):
            print(*M[i], V[i], file=f, sep=',')
    ans = np.linalg.solve(M, V)
    res = subprocess.run([os.path.join(os.getcwd(), "2.sh"), "input.txt"],
                         capture_output=True).stdout.decode("utf-8").split('\n')[:-1]
    res = np.array(list(map(float, res)))
    if not (abs(res - ans) < 1e-10).all():
        print("true", *ans)
        print("your", *res)
        break
