import iio
import matplotlib.pyplot as plt
import array
import numpy as np
import functions

URI = "ip:10.76.84.102"
NO_OF_CHANNELS = 6
SAMPLES_COUNT = 200

ctx = iio.Context(URI)
if not ctx:
    print("cannot get context!\n")
    exit(-1)

dev = ctx.find_device("ad5592r_s")
if not dev:
    print("cannot get device!\n")
    exit(-2)

channels = []
for i in range(NO_OF_CHANNELS):
    channels.append(dev.find_channel(f"voltage{i}"))

for channel in channels:
    channel.enabled = True

buf = iio.Buffer(dev, SAMPLES_COUNT, False)
if not buf:
    print("cannot create buffer!\n")
    exit(-3)

while True:
    buf.refill()
    bytes_read = buf.read()

    actual_data = []
    for i in range(0, len(bytes_read), 2):
        actual_data.append(functions.create_sixteen_bit_number(bytes_read[i + 1], bytes_read[i]))
    
    arr = np.array(actual_data)
    #print(arr.shape)

    xpoz = arr[0::6]
    xneg = arr[1::6]
    ypoz = arr[2::6]
    yneg = arr[3::6]
    zpoz = arr[4::6]
    zneg = arr[5::6]

    X = xpoz - xneg
    Y = ypoz - yneg
    Z = zpoz - zneg

    plt.plot(X, 'r')
    plt.plot(Y, 'g')
    plt.plot(Z, 'b')

    plt.draw()
    plt.pause(1)
    plt.clf()
