import iio
import matplotlib.pyplot as plt
import numpy as np
import functii as f

URI = "ip:10.76.84.245"
DEVICE_NAME = "ad5592r_s"
CHANNEL_ATTRIBUTE = "raw"
CHANNELS_NUMBER = 6
SAMPLE_COUNT = 32
CYCLIC_STATUS = False

ctx = iio.Context(URI)
if not ctx:
    print("Context unavailable!!!\n")
    SystemExit
print("Connection established!\n")

device = ctx.find_device(DEVICE_NAME)
if not device:
    print("No device with name \"" + DEVICE_NAME + "\"")
    SystemExit

channels = []

for i in range(CHANNELS_NUMBER):
    channels.append(device.find_channel(f"voltage{i}"))

for ch in channels:
    ch.enabled = True

buffer = iio.Buffer(device, SAMPLE_COUNT, CYCLIC_STATUS)
while True:
    try:
        buffer.refill()
        bytes_buffer = buffer.read()

        data = []
        for i in range(0, len(bytes_buffer), 2):
            sixteen_bit_number = f.sixteen_bit(i, bytes_buffer)
            data.append(sixteen_bit_number)

        print(np.array(data).shape)

        xpoz = np.array(data[0::6])
        xneg = np.array(data[1::6])
        ypoz = np.array(data[2::6])
        yneg = np.array(data[3::6])
        zpoz = np.array(data[4::6])
        zneg = np.array(data[5::6])

        x = xpoz - xneg
        y = ypoz - yneg
        z = zpoz - zneg

        plt.ion()
        plt.plot(x, 'r')
        plt.plot(y, 'b')
        plt.plot(z, 'g')

        plt.draw()
        plt.pause(0.1)
        plt.clf()

        f.functie_matei()
    except KeyboardInterrupt:
        break


print("\nDONE")