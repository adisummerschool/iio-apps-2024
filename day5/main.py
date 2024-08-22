from threading import Event, Thread
from queue import Queue
from pynput import keyboard

import iio
from time import sleep, time
import math

URI = "ip:10.76.84.243"
DEVICE = "iio:device0"

PWM_TIMEOUT = 0.4

def frontend_thread(movement):
    pass

def init_device():
    ctx = iio.Context(URI)

    if not ctx:
        print("No context")

    device = ctx.find_device(DEVICE)

    if not device:
        print("no dev")

    return device

def get_data(device):
    axes = {'x', 'y', 'z'}
    data = {
        'x' : {'-': int, '+': int},
        'y' : {'-': int, '+': int},
        'z' : {'-': int, '+': int},
    }

    # get data
    # data['x']['-'] = value

    channels = []
    values = []

    for i in range(6):
        chan = device.find_channel(f"voltage{i}")
        values.append(chan.attrs['raw'].value)
        #sleep(1)

    data['x']['+'] = values[0]
    data['x']['-'] = values[1]
    data['y']['+'] = values[2]
    data['y']['-'] = values[3]
    data['z']['+'] = values[4]
    data['z']['-'] = values[5]

    return data

def get_roll_pitch(data):

    x_accel = float(data['x']['+']) - float(data['x']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    z_accel = float(data['z']['+']) - float(data['z']['-'])
    
    # Calculate roll and pitch
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi

    return roll, pitch


def num_to_range(num, inMin, inMax, outMin, outMax):
  return outMin + (float(num - inMin) / float(inMax - inMin) * (outMax
                  - outMin))

def get_movement(roll, pitch):
    movement = {'left': 0, 'right' : 0, 'front' : 0, 'back' : 0}

    threshold = 60
   
    if roll > 0 and roll < threshold:
        movement['left'] = abs(num_to_range(roll, 0, threshold, 0, 1))
    elif roll < 0 and roll > -threshold:
        movement['right'] = abs(num_to_range(roll, 0, threshold, 0, 1))
    elif roll >= threshold:
        movement['left'] = 1
    elif roll <= -threshold:
        movement['right'] = 1

    if pitch > 0 and pitch <= threshold:
        movement['back'] = abs(num_to_range(roll, 0, threshold, 0, 1))
    elif pitch < 0 and pitch >= -threshold:
        movement['front'] = abs(num_to_range(roll, 0, threshold, 0, 1))
    elif pitch >= threshold:
        movement['back'] = 1
    elif pitch <= -threshold:
        movement['front'] = 1

    return movement

def key_press(key, my_time):
    kb_controller = keyboard.Controller()

    kb_controller.press(key)
    sleep(my_time)
    kb_controller.release(key) 

def start_iio(device):
    start = time()
    data = get_data(device)
    #print(data)
    roll, pitch = get_roll_pitch(data)
    #print("roll: ", roll, "pitch: ", pitch)
    movement = get_movement(roll, pitch)
    #print("left",  movement['left'], "right", movement['right'])
    #print("front",  movement['front'], "back", movement['back'])
    iio_timeout = time() - start
    #simulation loop

    for move, key in [('front', 'w'), ('back', 's'), ('left', 'a'),('right', 'd')]:
        value = movement[move]
        if value > 0:
            on_time = PWM_TIMEOUT * value
            thread = Thread(target=key_press, args=(key,on_time,))
            thread.daemon = True
            thread.start()

    if on_time != -1:
        sleep(PWM_TIMEOUT - iio_timeout)

    return movement

def backend_thread_func(movement):
    device = init_device()
    #print(device)

    while not exit_event.is_set():
        if start_event.is_set():
           #print("runnint...")
           #sleep(1)
           movement.put(start_iio(device))
        start_event.wait()


def on_keypress(key):

    if key == keyboard.Key.ctrl:
        if start_event.is_set():
            start_event.clear()
            print("pause")
        else :
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

    init_momevent = {'left': 0, 'right' : 0, 'front' : 0, 'back' : 0}
    movement_queue = Queue()
    movement_queue.put(init_momevent)

    frontend_thread = Thread(target=frontend_thread, args=(movement_queue,))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue,))
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    listener.join()
    backend_thread.join()
    frontend_thread.join()