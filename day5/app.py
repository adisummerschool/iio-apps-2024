import math
from threading import Event, Thread
from queue import Queue
from time import sleep
from pynput import keyboard
import iio 

URI = "ip:10.76.84.236"
DEV_NAME = "ad5592r_s"

def frontend_thread_func(movement):
    pass


def init_device():
    ctx = iio.Context(URI) 
    if not ctx: 
        print("cannot get ctx")
    
    device = ctx.find_device(DEV_NAME)
    if not device: 
        print("cannot get device")

    return device 


def get_data(dev):
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    channels = []
    values = []
    for i in range(6):
        channel = dev.find_channel(f"voltage{i}", False)
        if not channel: 
            print("cannot get channel")
        else:
            channels.append(channel)
        values.append(channel.attrs['raw'].value)
        print("{0} {1}".format(channel.id, values[i]))

    data['x']['+'] = values[0]
    data['x']['-'] = values[1]
    data['y']['+'] = values[2]
    data['y']['-'] = values[3]
    data['z']['+'] = values[4]
    data['z']['-'] = values[5]

    print(data)

    return data


def get_roll_pitch(data):
    
    x_accel = float(data['x']['+']) - float(data['x']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    z_accel = float(data['z']['+']) - float(data['z']['-'])

    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi
    return roll, pitch


def get_movement(roll, pitch):
    pass


def start_iio(device):
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
   # movement = get_movement(roll, pitch)
    print("roll: ", roll)
    print("pitch: ", pitch)
    #print(roll, pitch)
    # simulation loop


def backend_thread_func(movement):
    dev = init_device()
    
    while not exit_event.is_set():
        if start_event.is_set():
            #print("running...")
            sleep(1)
            movement.put(start_iio(dev))
        start_event.wait()
        

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
        return False 
    
    return True


if __name__ == '__main__':

    start_event = Event() 
    exit_event = Event()

    init_movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movement_queue = Queue()
    movement_queue.put(init_movement)

    frontend_thread = Thread(target=frontend_thread_func, args=(movement_queue,), daemon=True)
    # frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue,), daemon=True)
    # backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress) 
    listener.daemon = True 
    listener.start() 

    listener.join()
    # frontend_thread.join()
    # backend_thread.join() 
