import subprocess
import os
from random import choice, randint
from string import ascii_lowercase

for _ in range(10000):
    n = randint(10**5, 10**6)
    s = "".join([choice(ascii_lowercase + "\n ") for _ in range(n)])
    with open("input.txt", 'w') as f:
        f.write(s)
    ans = s[::-1]
    res = subprocess.run(["qemu-aarch64-static",
                         "-L", "/home/belov/caos/sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu",
                         os.path.join(os.getcwd(), "main")], input=s.encode("utf-8"),
                         capture_output=True).stdout.decode("utf-8")
    if res != ans:
        print("true", ans[:10])
        print("your", res[:10])
        break
