import subprocess
import os
from random import choice, randint
from string import ascii_lowercase
from sys import argv

for _ in range(10000):
    n = randint(100, 1000)
    s = ["".join(choice(ascii_lowercase)
                 for _ in range(randint(100, 1000))) for i in range(n)]
    with open("input.txt", 'w') as f:
        f.write('\n'.join(s))
    ans = s[::-1]
    raw_res = subprocess.run([os.path.join(os.getcwd(), argv[1])], input=('\n'.join(s)).encode("utf-8"),
                         capture_output=True)
    if (raw_res.returncode != 0):
        print("WOOOOW! Non-zero return code")
        print("true", ans[:10])
        with open("input.txt", 'w') as f:
            f.write(s)
    res = raw_res.stdout.decode("utf-8").split('\n')
    if not all(map(lambda x: x[0] == x[1], zip(ans, res))):
        print("true", ans[:10])
        print("your", res[:10])
        break
