import iio
import matplotlib.pyplot as plt
import array
import numpy as np
import functions

DEVICE_NAME = "ad5592r_s"
URI = "ip:10.76.84.104"
ctx = iio.Context(URI)

if not ctx:
    print("Cannot find context")

dev = ctx.find_device(DEVICE_NAME)
if not dev:
    print("Cannot find device")

dev._device.enabled = True

#channel_names = ["voltage" + str(i) for i in range(6)]
#print(channel_names)
#channels = [dev.find_channel(x) for x in channel_names]

channels = []
for i in range(6):
    channels.append(dev.find_channel(f"voltage{i}"))

for channel in channels:
    channel.enabled = True

buf = iio.Buffer(dev, 10, False)
while True:
    buf.refill()
    bytes_buf = buf.read()

    actual_data = []
    for i in range(0, len(bytes_buf), 2):
        sixteen_bit_number = functions.sixteen_bit_i(i, bytes_buf)
        actual_data.append(sixteen_bit_number)
        #print(sixteen_bit_number)

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

    #xpoz_avg = xpoz.sum()/(xpoz.shape[0])
    #print(xpoz_avg)

    plt.plot(X, 'r')
    plt.plot(Y, 'b')
    plt.plot(Z, 'g')

    plt.draw()
    plt.pause(0.1)
    plt.clf()

functions.fun()
