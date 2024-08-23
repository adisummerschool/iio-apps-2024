from threading import Event, Thread
from queue import Queue
from pynput import keyboard

import iio
from time import sleep, time
import math
import tkinter as tk

URI = "ip:10.76.84.243"
DEVICE = "iio:device0"
 
PWM_TIMEOUT = 0.2

def update_square(canvas, square, time):
    intensity = 255 - max(min(int(time * 255), 255), 0)

    hex_intensity = hex(intensity)                                              # convert int to hex
    string_color = hex_intensity.split("x", 1)
    if intensity < 16:                                                          # make the color
        color = '#' + '0' + string_color[1] + '0' + string_color[1]  + 'ff'
    else:
        color = '#' + string_color[1] + string_color[1]  + 'ff'              
    #print(color)
    canvas.itemconfig(square, fill=color)

def frontend_thread_func(movement):
    def update_gui():
        movements = movement.get()
        update_square(canvas, squares['left'], movements['left'])
        update_square(canvas, squares['right'], movements['right'])
        update_square(canvas, squares['front'], movements['front'])
        update_square(canvas, squares['back'], movements['back'])

        root.after(int( PWM_TIMEOUT * 1000), update_gui)

    root = tk.Tk()
    canvas = tk.Canvas(root, width=400, height=400, background='#808080')
    canvas.pack()
    
    sq_size = 100
    squares = {
        'left': canvas.create_rectangle(50, 150, 50 + sq_size, 150 + sq_size, fill='white'),
        'right': canvas.create_rectangle(250, 150, 250 + sq_size, 150 + sq_size, fill='white'),
        'front': canvas.create_rectangle(150, 50, 150 + sq_size, 50 + sq_size, fill='white'),
        'back': canvas.create_rectangle(150, 250, 150 + sq_size, 250 + sq_size, fill='white'),
    }

    update_gui()
    root.mainloop()

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

    on_time = -1
    for move, key in [('front', 'w'), ('back', 's'), ('left', 'a'),('right', 'd')]:
        value = movement[move]
        if value > 0:
            on_time = PWM_TIMEOUT * value
            thread = Thread(target=key_press, args=(key,on_time,))
            thread.daemon = True
            thread.start()

    if on_time != -1:
        sleep(max(PWM_TIMEOUT - iio_timeout, 0))

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

    if key == keyboard.Key.ctrl_l:
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

    frontend_thread = Thread(target=frontend_thread_func, args=(movement_queue,))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue,))
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    listener.join()
    #backend_thread.join()
    #frontend_thread.join()