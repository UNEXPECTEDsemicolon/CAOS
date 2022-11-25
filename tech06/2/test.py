from random import randint, choice
from string import ascii_lowercase, whitespace
from sys import stderr

n = randint(1, 255)
# n = 255
a = ['a'*randint(1, 4096) for _ in range(n)]
for x in a:
    print(x, end=choice(whitespace))
print(n, file=stderr)