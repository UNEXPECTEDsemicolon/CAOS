from sys import argv

with open(argv[1], 'rb') as f:
    data = f.read()

print(*[int.from_bytes(data[4*i:4*(i+1)], 'little', signed=True)
      for i in range(len(data) // 4)])
