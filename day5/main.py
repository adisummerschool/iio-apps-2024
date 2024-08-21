from threading import Event, Thread
from queue import Queue
from time import sleep
from pynput import keyboard
import iio
import math

URI = "ip:10.76.84.107"
DEV_NAME = "ad5592r_s"

def frontend_thread_func(movement):
    pass

def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("No contex")
    device = ctx.find_device(DEV_NAME)
    if not device:
        print("No device")
    return device

def start_iio(device):
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    print(roll, pitch)
    #movement = get_movement(roll, pitch)

    #simulation loop

def get_data(device):
    axis = ['x', 'y', 'z']
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }
    # get data
    # data['x']['-'] = value
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
    return roll,pitch

def get_movement(roll,pitch):
    pass


def backend_thread_func(movement):
    device = init_device()
    print(device.name)

    while not exit_event.is_set():
        if start_event.is_set():
            #print("running...")
            movement.put(start_iio(device))
            sleep(1)
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
        return False #daca se termina in False, se termina programul si nu se mai asculta urmatoarele comenzi
    
    return True

if __name__ == '__main__':
    start_event = Event()
    exit_event = Event() #vizibile in ambele threaduri

    init_movement = {'left' : 0, 'right' : 0, 'front' : 0, 'back' : 0}
    movement_queue = Queue()
    movement_queue.put(init_movement)

    frontend_thread = Thread(target=frontend_thread_func, args=(movement_queue,))
    frontend_thread.daemon = True #cand omoram programul, sa fie omorat si acest thread
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue,))
    backend_thread.daemon = True #cand omoram programul, sa fie omorat si acest thread
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    listener.join() #aici se blocheaza programul si daca e False se termina, iar daca e True asculta urmatoarele comenzi(ruleaza in continuare)
    frontend_thread.join() #asteptam sa se termine threadul
    backend_thread.join()
    
