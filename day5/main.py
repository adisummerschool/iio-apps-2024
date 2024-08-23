import math
import iio
#from iio import Context
import tkinter as tk

from time import sleep, time
from threading import Event, Thread
from queue import Queue
from pynput import keyboard

URI = "ip:10.76.84.244"
THR_HIGH = 45
THR_LOW = 5
PWM_TIMEOUT = 0.2

def update_square(canvas, square, time):
    # print(time)
    intensity = 255 - max(0, min(int(time * 255), 255))
    intensity_str = str(hex(intensity))
    if time == 0 :
        color = '#ffffff'
    elif intensity < 16 :
        intensity_str = intensity_str[1:]
        intensity_final =  intensity_str.replace('x', '0')
        color = '#' + intensity_final + intensity_final + 'ff'
    else:   
        intensity_final =  intensity_str.replace('0x', '')
        color = '#' + intensity_final + intensity_final + 'ff'
    # print(color)
    canvas.itemconfig(square, fill=color)

def frontend_thread_func(movement):
    def update_gui():
        movements = movement.get()
        update_square(canvas, squares['left'], movements['left'])
        update_square(canvas, squares['right'], movements['right'])
        update_square(canvas, squares['up'], movements['back'])
        update_square(canvas, squares['down'], movements['front'])

        root.after(int(PWM_TIMEOUT * 100), update_gui)

    root = tk.Tk()
    canvas = tk.Canvas(root, width=400, height=400)
    canvas.pack()

    sq_size = 100
    squares = {
        'left': canvas.create_rectangle(50, 150, 50 + sq_size, 150 + sq_size, fill='white'), 
        'right': canvas.create_rectangle(250, 150, 250 + sq_size, 150 + sq_size, fill='white'), 
        'up': canvas.create_rectangle(150, 50, 150 + sq_size, 50 + sq_size, fill='white'), 
        'down': canvas.create_rectangle(150, 250, 150 + sq_size, 250 + sq_size, fill='white'), 
    }

    update_gui()
    root.mainloop()

def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("No Context!")
        return -1
    
    device = ctx.find_device("iio:device0")
    if not device:
        print("No device!")
        return -1
    
    return device

def get_data(device):
    axes = ['x', 'y', 'z']
    data = {
        'x' : {'-': int, '+': int},
        'y' : {'-': int, '+': int},
        'z' : {'-': int, '+': int},
    }

    index = 0
    #get data
    for i in axes:
        chan_pos = device.find_channel(f"voltage{index}")
        chan_neg = device.find_channel(f"voltage{index+1}")

        data[i]['+'] = chan_pos.attrs['raw'].value
        data[i]['-'] = chan_neg.attrs['raw'].value
        
        index += 2
    return data

def get_roll_pitch(axis_data):
    x_accel = float(axis_data['x']['+']) - float(axis_data['x']['-'])
    y_accel = float(axis_data['y']['+']) - float(axis_data['y']['-'])
    z_accel = float(axis_data['z']['+']) - float(axis_data['z']['-'])
    
    # Calculate roll and pitch
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi

    return roll, pitch

def get_movement(roll, pitch):
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}

    if roll < 0 :
        if abs(roll) < THR_LOW:
            movement['left'] = 0
        elif abs(roll) > THR_HIGH:
            movement['left'] = 1
        else:
            movement['left'] = abs(roll) / THR_HIGH
    else:
        if roll < THR_LOW:
            movement['right'] = 0
        elif roll > THR_HIGH:
            movement['right'] = 1
        else:
            movement['right'] = abs(roll) / THR_HIGH

    if pitch < 0 :
        if abs(pitch) < THR_LOW:
            movement['back'] = 0
        elif abs(pitch) > THR_HIGH:
            movement['back'] = 1
        else:
            movement['back'] = abs(pitch) / THR_HIGH
    else:
        if pitch < THR_LOW:
            movement['front'] = 0
        elif pitch > THR_HIGH:
            movement['front'] = 1
        else:
            movement['front'] = abs(pitch) / THR_HIGH        
    return movement

def key_press(key, time):
    kb_controller = keyboard.Controller()

    kb_controller.press(key)
    sleep(time)
    kb_controller.release(key)

def start_iio(device):
    start = time()
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    movement = get_movement(roll, pitch)
    iio_timeout = time() - start

    on_time = -1
    for move, keyt in [('left', 'a'), ('right', 'd'), ('front', 'w'), ('back', 's')]:
        value = movement[move]
        if value > 0 :
            on_time = PWM_TIMEOUT * value
            thread = Thread(target=key_press, args=(keyt, on_time))
            thread.daemon = True
            thread.start() 

    if on_time != -1:
        sleep(PWM_TIMEOUT)

    return movement
    # simulation loop

def backend_thread_func(movement):
    device = init_device()
    while not exit_event.is_set():
        if start_event.is_set():
            movement.put(start_iio(device))

        start_event.wait()

def on_keypress(key):
    if key == keyboard.Key.ctrl_l:
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
    # frontend_thread.join()
    # backend_thread.join()