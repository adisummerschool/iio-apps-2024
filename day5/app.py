import math
from threading import Event, Thread
from queue import Queue
from time import sleep, time
from pynput import keyboard
import iio 

URI = "ip:10.76.84.236"
DEV_NAME = "ad5592r_s"
ATTR_NAME = "raw"
THRESHOLD = 5
MAX_DEG = 45
PWM_TIMEOUT = 0.1

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
        values.append(channel.attrs[ATTR_NAME].value)

    data['x']['+'] = values[0]
    data['x']['-'] = values[1]
    data['y']['+'] = values[2]
    data['y']['-'] = values[3]
    data['z']['+'] = values[4]
    data['z']['-'] = values[5]

#    print(data)

    return data


def get_roll_pitch(data):
    
    x_accel = float(data['x']['+']) - float(data['x']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    z_accel = float(data['z']['+']) - float(data['z']['-'])

    roll = 180 * math.atan2(y_accel, math.sqrt(x_accel*x_accel + z_accel*z_accel)) / math.pi
    pitch = 180 * math.atan2(x_accel, math.sqrt(y_accel*y_accel + z_accel*z_accel)) / math.pi

    return roll, pitch


def get_movement(roll, pitch):
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    
    roll_percentage = roll / MAX_DEG
    if abs(roll_percentage) > 1: 
        if roll_percentage > 0:
            roll_percentage = 1
        elif roll_percentage < 0:
            roll_percentage = -1


    pitch_percentage = pitch / MAX_DEG
    if abs(pitch_percentage) > 1: 
        if pitch_percentage > 0:
            pitch_percentage = 1
        elif pitch_percentage < 0:
            pitch_percentage = -1

    if abs(roll_percentage) >= (THRESHOLD / MAX_DEG):
        if roll_percentage < 0:
            movement['left'] = -1 * roll_percentage
            movement['right'] = 0
        else:
            movement['left'] = 0
            movement['right'] = roll_percentage 

    if abs(pitch_percentage) >= (THRESHOLD / MAX_DEG) :
        if pitch_percentage < 0:
            movement['front'] = -1 * pitch_percentage
            movement['back'] = 0
        else:
            movement['front'] = 0
            movement['back'] = pitch_percentage 

    # print("roll: ", "{:.2f}".format(roll), "=", "{:.2f}".format(roll_percentage), "/ 1") 
    # print("pitch: ", "{:.2f}".format(pitch), "=", "{:.2f}".format(pitch_percentage), "/ 1") 
    # print("\nleft: ", "{:.2f}".format(movement['left']), "right: ", "{:.2f}".format(movement['right']))
    # print("front: ", "{:.2f}".format(movement['front']), "back: ", "{:.2f}".format(movement['back']))
    # print("---------------------------------")
    return movement


def key_pressed(key, time):
    controller = keyboard.Controller()

    controller.press(key)
    sleep(time)
    controller.release(key)
# 

def start_iio(device):
    start = time()
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    movement = get_movement(roll, pitch)
    iio_timeout = time() - start
    
    on_time = -1
    for move, key in [('front', 'w'), ('back', 's'), ('left', 'a'), ('right', 'd')]:
        value = movement[move]
        if value > 0: 
            on_time = PWM_TIMEOUT * value
            key_thread = Thread(target=key_pressed, args=(key, on_time,), daemon=True)
            key_thread.start()

    if on_time != -1: 
        sleep(PWM_TIMEOUT)#- iio_timeout)

    return movement


def backend_thread_func(movement):
    dev = init_device()
    while not exit_event.is_set():
        if start_event.is_set():
            #sleep(1)
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
    frontend_thread.join()
    backend_thread.join() 
