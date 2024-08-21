import iio
import matplotlib.pyplot as plt
import array
import numpy as np
import functii

URI = "ip:10.76.84.236"
ctx = iio.Context(URI)
if not ctx: 
    print("cannot get ctx")
 
DEV_NAME = "ad5592r_s"
dev = ctx.find_device(DEV_NAME)
if not dev: 
    print("cannot get device")

n = 6 
channels = []
for i in range(n):
    new_channel = dev.find_channel(f"voltage{i}", False)
    if not new_channel: 
        print("cannot get channel")
    else:
        channels.append(new_channel)

for chn in channels:
    chn.enabled = True

SAMPLES_CNT = 32
buf = iio.Buffer(dev, SAMPLES_CNT, False)

while True: 
    buf.refill()
    bytes_read = buf.read()
    
    actual_data = []

    for i in range(0, len(bytes_read), 2):
        sixteen_bits_number = functii.sixteen_bit(i, bytes_read)
        actual_data.append(sixteen_bits_number)
    
    x_poz = np.array(actual_data[0::6])
    x_neg = np.array(actual_data[1::6])
    y_poz = np.array(actual_data[2::6])
    y_neg = np.array(actual_data[3::6])
    z_poz = np.array(actual_data[4::6])
    z_neg = np.array(actual_data[5::6])

    X = x_poz - x_neg
    Y = y_poz - y_neg
    Z = z_poz - z_neg

    plt.plot(X, 'r')
    plt.plot(Y, 'm')
    plt.plot(Z, 'y')

    plt.draw()
    plt.pause(1)
    plt.clf()
    



