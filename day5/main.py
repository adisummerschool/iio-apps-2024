from threading import Event, Thread
from queue import Queue
from time import sleep
from pynput import keyboard
import math

import iio

URI = "ip:10.76.84.245"
DEVICE_NAME = "ad5592r_s"
CHANNEL_ATTRIBUTE = 'raw'
CHANNELS_NUMBER = 6

def frontend_thread_func(movement):
    pass

def get_data(device):
    axis = ['x', 'y', 'z']
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    #get data
    
    for i in range(CHANNELS_NUMBER):
        channel = device.find_channel(f"voltage{i}")
        
        j = '+'
        if i % 2 == 1:
            j = '-'

        data[axis[int(i / 2)]][j] = int(channel.attrs[CHANNEL_ATTRIBUTE].value)

    return data

def get_roll_pitch(data):
    x_accel = float(data['x']['+']) - float(data['x']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    z_accel = float(data['z']['+']) - float(data['z']['-'])

    # Calculate roll and pitch
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(x_accel, z_accel) * 180 / math.pi

    return roll, pitch

def get_movement(roll, pitch):
    pass

def start_iio(device):
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    print(roll, pitch)
    #movement = get_movement(roll, pith)

    #simulation loop

def backend_thread_func(movement):
    device = init_device()
    
    while not exit_event.is_set():
        if(start_event.is_set()):
            movement.put(start_iio(device))
            sleep(1)
        start_event.wait()


def on_keypress(key):
    if key == keyboard.Key.space:
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
        return False
    
    return True

def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("Context unavailable!!!\n")
        SystemExit

    device = ctx.find_device(DEVICE_NAME)
    if not device:
        print("No device with name \"" + DEVICE_NAME + "\"")
        SystemExit

    return device


if __name__ == '__main__':
    start_event = Event()
    exit_event = Event()

    init_momevent = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movmevent_queue = Queue()
    movmevent_queue.put(init_momevent)

    frontend_thread = Thread(target=frontend_thread_func, args=(movmevent_queue,))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movmevent_queue,))
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    listener.join()
    frontend_thread.join()
    backend_thread.join()