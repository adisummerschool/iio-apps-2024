import iio

import matplotlib.pyplot as plt
import numpy as np
import array
import functii

URI = "ip:10.76.84.239"

ctx = iio.Context(URI)
if not ctx:
    print("No context")
    exit(0)

dev = ctx.find_device("ad5592r_s")
if not dev:
    print("No device")
    exit(0)

#iio.DeviceAttr._write("en", "1")

channels = []
for i in range(6):
    channels.append(dev.find_channel(f"voltage{i}"))
    if not channels[i]:
        print("No channel")


for chn in channels:
    chn.enabled = True

buf = iio.Buffer(dev, 10, False)

while True:
    buf.refill()
    bytes_buf = buf.read()

    actual_data = []

    for i in range (0, len(bytes_buf), 2):
        sixteen_bit_number = functii.sixteen_bit_to_int(i, bytes_buf)
        actual_data.append(sixteen_bit_number)
    
    arr = np.array(actual_data)
    print(arr.shape)

    xpoz = np.array(actual_data[0::6])
    xneg = np.array(actual_data[1::6])
    ypoz = np.array(actual_data[2::6])
    yneg = np.array(actual_data[3::6])
    zpoz = np.array(actual_data[4::6])
    zneg = np.array(actual_data[5::6])

    X = xpoz - xneg
    Y = ypoz - yneg
    Z = zpoz - zneg

    plt.plot(X, label='X', color='red')
    plt.plot(Y, label='Y', color='green')
    plt.plot(Z, label='Z', color='blue')

    plt.draw()
    plt.pause(1)
    plt.clf()




functii.functia_florina()

#source .venv/bin/activate
#python3 plot.py
