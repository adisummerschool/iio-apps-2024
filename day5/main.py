from time import sleep
import iio
import math
import numpy as np
from pynput import keyboard
from threading import Event, Thread
from queue import Queue

DEVICE_NAME = "ad5592r_s"
URI = "ip:10.76.84.104"

#device acquisition from context
def init_device():
    ctx = iio.Context(URI)

    if not ctx:
        print("Cannot find context")
 
    dev = ctx.find_device(DEVICE_NAME)
    if not dev:
        print("Cannot find device")

    dev._device.enabled = True
    return dev
   

#data acquisition
def get_data(dev):
    axes = {"x+", "x-", "y+", "y-", "z+", "z-"}
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    channels = []
    for i in range(6):
        channels.append(dev.find_channel(f"voltage{i}"))

    data['x']['+'] = channels[0].attrs['raw'].value
    data['x']['-'] = channels[1].attrs['raw'].value
    data['y']['+'] = channels[2].attrs['raw'].value
    data['y']['-'] = channels[3].attrs['raw'].value
    data['z']['+'] = channels[4].attrs['raw'].value
    data['z']['-'] = channels[5].attrs['raw'].value

    return data

#calculate roll and pitch from acceleration
def get_roll_pitch(axis_data):
    x_accel = float(axis_data['x']['+']) - float(axis_data['x']['-'])
    y_accel = float(axis_data['y']['+']) - float(axis_data['y']['-'])
    z_accel = float(axis_data['z']['+']) - float(axis_data['z']['-'])

    # Calculate roll and pitch
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi

    return roll, pitch

#determine movement pwm sequence from roll and pitch values
def get_movement(roll, pitch):
    pass

#return new movement data
def start_iio(dev):
    data = get_data(dev)
    roll, pitch = get_roll_pitch(data)
    movement = get_movement(roll, pitch)

    print(roll, pitch)

    #simulation loop
    #controller = keyboard.Controller()
    #controller.press(key=keyboard.Key.up)
    #controller.release(key=keyboard.Key.up)

    return movement
  


#GUI thread
def frontend_thread_func(movement):
    pass

#movement simulation thread
def backend_thread_func(movement):
    dev = init_device()

    while not exit_event.is_set():
        if start_event.is_set():
            movement.put(start_iio(dev))
        start_event.wait()


#keypress listener
def on_keypress(key):
    if key == keyboard.Key.ctrl:
        if start_event.is_set():
            start_event.clear()
            print("paused")
        else:
            start_event.set()
            print("started")
    elif key == keyboard.Key.esc:
        start_event.set()
        exit_event.set()
        print("exit")
        #listener-ul returneaza False => se termina thread-ul
        return False
    
    return True
        

#application entry point
if __name__ == '__main__':
    start_event = Event()
    exit_event = Event()

    #dictionary for keypresses
    init_memevent = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movement_queue = Queue()
    movement_queue.put(init_memevent)

    frontend_thread = Thread(target=frontend_thread_func, args=(movement_queue, ))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue, ))
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()
       
    listener.join()
    frontend_thread.join()
    backend_thread.join()
