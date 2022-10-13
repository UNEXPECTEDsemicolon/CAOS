with open("input.txt", 'wb') as f:
    f.write((-123).to_bytes(4, 'little', signed=True) + (16).to_bytes(4, 'little'))
    f.write((456).to_bytes(4, 'little', signed=True) + (0).to_bytes(4, 'little'))
    f.write((-789).to_bytes(4, 'little', signed=True) +
            (8).to_bytes(4, 'little'))
    # print(123, 8, sep='', file=f, end='')
    # print(456, 0, sep='', file=f, end='')