import iio
from threading import Thread, Event
from queue import Queue
from pynput import keyboard
from time import sleep
import math

URI = "ip:10.76.84.239"
DEVICE = "ad5592r_s"


def frontend_thread_f(movement):
    pass

def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("No context")
        exit(0)
    
    device = ctx.find_device(DEVICE)
    if not device:
        print("No device")
        exit(0)

    return device

def get_data(device):
    data = {
        'x':{'-': int, '+': int},
        'y':{'-': int, '+': int},
        'z':{'-': int, '+': int},
    }
    channels = []
    values = []
    for i in range(6):
        chan = device.find_channel(f"voltage{i}")
        values.append(chan.attrs['raw'].value)
        print("{0} {1}".format(chan.id,values[i]))
    data['x']['+'] = values[0]
    data['x']['-'] = values[1]
    data['y']['+'] = values[2]
    data['y']['-'] = values[3]
    data['z']['+'] = values[4]
    data['z']['-'] = values[5]

    return data

def get_roll_pitch(axis_data):
    x_accel = float(axis_data['x']['+']) - float(axis_data['x']['-'])
    y_accel = float(axis_data['y']['+']) - float(axis_data['y']['-'])
    z_accel = float(axis_data['z']['+']) - float(axis_data['z']['-'])
    # Calculate roll and pitch
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi

    return roll, pitch


def start_iio(device):
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    print(roll, pitch)
    #movement = get_movement(roll, pitch)



def backend_thread_f(movement):
    device = init_device()

    while not exit_event.is_set():
        if start_event.is_set():
            print("running")
            sleep(1)
            movement.put(start_iio(device))
        start_event.wait()
          

def on_keypress(key):
    if key == keyboard.Key.ctrl:
        if start_event.is_set():
            start_event.clear()
            print("started")
        else:
            start_event.set()
            print("stoped")
    elif key == keyboard.Key.esc:
        start_event.set()
        exit_event.set()
        print("exited")
        return False
    
    print(key)

    return True
        
    
    

if __name__ == '__main__':

    start_event = Event()
    exit_event = Event()

    init_movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movement_queue = Queue()
    movement_queue.put(init_movement)

    frontend_thread = Thread(target = frontend_thread_f, args=(movement_queue,))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target = backend_thread_f, args=(movement_queue,))    
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()


    listener.join()
    frontend_thread.join()
    backend_thread.join()


    




