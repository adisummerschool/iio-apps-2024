from threading import Thread, Event
from queue import Queue
from time import sleep, time
from pynput import keyboard
from iio import Context
import math

URI = "ip:10.76.84.252"
device_name ="iio:device0"
sign = lambda x: math.copysign(1, x)
sensitivity = 25
PWM_TIMEOUT = 0.3

def frontend_thread_func(movevement):
    pass

def init_device():
    ctx = Context(URI)
    if not ctx:
        print("No context")
    dev = ctx.find_device("iio:device0")
    if not dev:
        print("No device")
    return dev
    

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
        #print("{0} {1}".format(chan.id,values[i]))
    
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
    roll = 180 * math.atan2(y_accel, math.sqrt(x_accel**2 + z_accel**2))/math.pi
    pitch = 180 * math.atan2(x_accel, math.sqrt(y_accel**2 + z_accel**2))/ math.pi

    return roll, pitch

def get_movement(roll, pitch):
    threshold = 5
    maxangle = 60
    movement={'left': 0, 'right': 0, 'front': 0, 'back': 0, 'sensitivity': 0}

    if roll > threshold and roll < maxangle:
        movement['back'] = ((abs(roll)-threshold) / (maxangle-threshold)) ** (10/sensitivity)
    elif roll > maxangle:
        movement['back'] = 1
    elif roll < -threshold and roll > -maxangle:
        movement['front'] = ((abs(roll)-threshold) / (maxangle-threshold)) ** (10/sensitivity)
    elif roll < -maxangle:
        movement['front'] = 1

    if pitch > threshold and pitch < maxangle:
        movement['left'] = ((abs(pitch)-threshold) / (maxangle-threshold)) ** (10/sensitivity) 
    elif pitch > maxangle:
        movement['left'] = 1 
    elif pitch < -threshold and pitch > -maxangle:
        movement['right'] = ((abs(pitch)-threshold) / (maxangle-threshold)) ** (10/sensitivity)
    elif pitch < -maxangle:
        movement['right'] = 1

    movement['sensitivity'] = sensitivity

    return movement

def key_press(key, time_, kb_controller):
    kb_controller.press(key)
    sleep(time_)
    kb_controller.release(key)

def start_iio(device):
    start=time()
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    movement =  get_movement(roll, pitch)
    iio_timeout = time() - start
    on_time = -1
    kb_controller = keyboard.Controller()

    for move, key in [('left', 'a'), ('right', 'd'), ('front', 'w'), ('back', 's')]:
        value = movement[move]
        #kb_controller.release(key)
        if(value > 0):
            on_time = value * PWM_TIMEOUT
            if(key in ['left', 'right']):
                thread = Thread(target=key_press, args=(key, on_time, kb_controller))
            else:
                thread = Thread(target=key_press, args=(key, on_time, kb_controller))
            thread.daemon = True
            thread.start()
    if(on_time != -1):
        sleep(PWM_TIMEOUT-iio_timeout)
    return movement
    
    #simulation loop
    
def backend_thread_func(movement):
    device = init_device()

    while not exit_event.is_set():
        if start_event.is_set():
            movement.put(start_iio(device))
        start_event.wait()

def on_keypress(key):
    global sensitivity
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
    elif key == keyboard.Key.page_up and sensitivity < 50:
        sensitivity += 1
        print("Sensitivity: ", sensitivity)
    elif key == keyboard.Key.page_down and sensitivity > 1:
        sensitivity -= 1  
        print("Sensitivity: ", sensitivity)
    return True

if __name__ == '__main__':
    start_event = Event()
    exit_event = Event()
    init_movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movement_queue = Queue()
    movement_queue.put(init_movement)
    frontend_thread = Thread(target=frontend_thread_func, args=(movement_queue,))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue,))
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    frontend_thread.join()
    backend_thread.join()
    listener.join()
