from iio import Context, DeviceAttr, Buffer
import matplotlib.pyplot as plt
import array
import numpy as np
import functii

URI = "ip:10.76.84.252"

ctx = Context(URI)
if not ctx:
    print("No context")

dev = ctx.find_device("iio:device0")
if not dev:
    print("No device")

channels = []
for i in range(6):
    channels.append(dev.find_channel(f"voltage{i}"))

for chan in channels:
    chan.enabled = True

buf = Buffer(dev, 32, False)
while (True):
    buf.refill()
    bytes_buf = buf.read()
    actual_data = functii.actual_data(bytes_buf)
    print(np.array(actual_data).shape)

    xpoz=np.array(actual_data[0::6])
    xneg=np.array(actual_data[1::6])
    ypoz=np.array(actual_data[2::6])
    yneg=np.array(actual_data[3::6])
    zpoz=np.array(actual_data[4::6])
    zneg=np.array(actual_data[5::6])

    x = xpoz - xneg;
    y = ypoz - yneg;
    z = zpoz - zneg;

    plt.ion()
    plt.plot(x, 'r')
    plt.plot(y, 'g')
    plt.plot(z, 'b')

    plt.draw()
    plt.pause(0.1)
    plt.clf()

functii.functie()
   