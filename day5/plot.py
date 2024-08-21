import iio
import matplotlib.pyplot as plt
import array
import numpy as np
import functii
import csv

URI = "ip:10.76.84.254"

ctx = iio.Context(URI)
if not ctx:
   print("Context error")

dev = ctx.find_device("iio:device0")
if not dev:
   print("Dev error")

#iio.DeviceAttr._write(en","1")

channels = []

for i in range(6):
   channels.append(dev.find_channel(f"voltage{i}"))

for chn in channels:
    chn.enabled = True

buf = iio.Buffer(dev, 32)

shape_array = []

plt.ion()

while True:
   #  functii.functie_radu_maris()
    buf.refill()
    bytes_buf = buf.read()

    actual_data = []
    for i in range(0, len(bytes_buf), 2):
       sixteen_bit_nr = functii.sixteen_bit(i, bytes_buf)
       actual_data.append(sixteen_bit_nr)
    
    shape_array = np.array(actual_data)
    # print(shape_array.shape)

    xpoz = np.array(actual_data[0::6])
    xneg = np.array(actual_data[1::6])
    ypoz = np.array(actual_data[2::6])
    yneg = np.array(actual_data[3::6])
    zpoz = np.array(actual_data[4::6])
    zneg = np.array(actual_data[5::6])

    X = xpoz - xneg
    Y = ypoz - yneg
    Z = zpoz - zneg

    plt.plot(X, 'r')
    plt.plot(Y, 'b')
    plt.plot(Z, 'g')

    plt.draw()
    plt.pause(0.1)

    plt.show()
    plt.clf()