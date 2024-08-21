import iio
import matplotlib.pyplot as plt
import array
import numpy as np
from functii import *

# URI                 =   "ip:10.76.84.236"
URI                 =   "ip:10.76.84.253"
DEVICE_NAME         =   "ad5592rs"
ATTR_NAME           =   "raw"
EN 		            =   "en"
THRESHOLD           =   400
NO_CH               =   6
MAX_ADC             =   4095
VOLTS_PER_LSB       =   2.4 / MAX_ADC
SAMPLE_CNT          =   32

#context
ctx = iio.Context(URI)
if not ctx:
    print("No context found")
    SystemExit
else:
    print("Context established")
    
#device  
cora = ctx.find_device(DEVICE_NAME)
if not cora:
    print("No device found")
    SystemExit
else:
    print("Device established")
    
#channels
channels = []
for i in range(NO_CH):
    channels.append(cora.find_channel(f"voltage{i}"))
for ch in channels:
    ch.enabled = True
    
#buffer
buf = iio.Buffer(cora, SAMPLE_CNT, False)
while True:
    try:
        buf.refill()
        byt = buf.read()
        raw_val = []
        for i in range(0, len(byt), 2):
            nr_16bit = two_bytes(i, byt)
            raw_val.append(nr_16bit)
        
        raw = np.array(raw_val)
        xpoz = raw[0::6]
        xneg = raw[1::6]
        ypoz = raw[2::6]
        yneg = raw[3::6]
        zpoz = raw[4::6]
        zneg = raw[5::6]
        
        x = xpoz - xneg
        y = ypoz - yneg
        z = zpoz - zneg
            
        plt.ion()
        plt.plot(x, 'r')
        plt.plot(y, 'g')
        plt.plot(z, 'b')
        plt.draw()
        plt.pause(0.1)
        plt.clf()
        
    except KeyboardInterrupt:
        print("\nskibidid")
        break
    