from time import sleep, time
import iio
import math
from pynput import keyboard
from threading import Event, Thread
from queue import Queue

DEVICE_NAME = "ad5592r_s"
URI = "ip:10.76.84.104"
LOW = 5
HIGH = 45
PWM_TIMEOUT = 0.2

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
    roll = 180 * math.atan2(y_accel, z_accel) / math.pi
    pitch = 180 * math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) / math.pi

    return roll, pitch

#determine movement pwm sequence from roll and pitch values
def get_movement(roll, pitch):
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}

    if (pitch <= LOW and pitch >= -LOW):
        movement['front'] = 0
        movement['back'] = 0
    elif (pitch >= HIGH):
        movement['front'] = 1
        movement['back'] = 0
    elif (pitch <= -HIGH):
        movement['front'] = 0
        movement['back'] = 1
    elif (pitch > LOW and pitch < HIGH):
        movement['front'] = (pitch - LOW) / (HIGH - LOW)
        movement['back'] = 0
    else:
        movement['back'] = (pitch + LOW) / (-HIGH + LOW)
        movement['front'] = 0


    if (roll <= LOW and roll >= -LOW):
        movement['right'] = 0
        movement['left'] = 0
    elif (roll >= HIGH):
        movement['right'] = 1
        movement['left'] = 0
    elif (roll <= -HIGH):
        movement['right'] = 0
        movement['left'] = 1
    elif (roll > LOW and roll < HIGH):
        movement['right'] = (roll - LOW) / (HIGH - LOW)
        movement['left'] = 0
    else:
        movement['left'] = (roll + LOW) / (-HIGH + LOW)
        movement['right'] = 0

    return movement

def key_press_func(key, on_time):
    kb_controller = keyboard.Controller()

    kb_controller.press(key)
    sleep(on_time)
    kb_controller.release(key)

#return new movement data
def start_iio(dev):
    init = time()
    data = get_data(dev)
    roll, pitch = get_roll_pitch(data)
    #print(roll, pitch)
    movement = get_movement(roll, pitch)
    #print(movement)
    fin = time() - init

    #simulation loop
    #thread for each keypress
    on_time = -1
    for move, key in [('front', 'w'), ('back', 's'), ('left', 'a'), ('right', 'd')]:
        value = movement[move]
        if (value > 0):
            on_time = PWM_TIMEOUT * value
            key_thread = Thread(target=key_press_func, args=(key, on_time, ))
            key_thread.daemon = True
            key_thread.start()
            #no join, since this function shouldn't wait for the termination of a key thread
    

    #sleep is called here, because the key threads don't wait for one another AND simultaneous presses are allowed
    if (on_time != -1):
        if (fin < PWM_TIMEOUT):
            sleep(PWM_TIMEOUT - fin)
        else:
            sleep(fin)

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
