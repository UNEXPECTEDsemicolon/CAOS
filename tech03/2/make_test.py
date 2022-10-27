from random import randint


with open("input.txt", 'wb') as f:
    lst = [randint(-10, 10) for i in range(randint(1, 10))]
    print(*lst)
    for x in lst:
        f.write(x.to_bytes(4, 'little', signed=True))
