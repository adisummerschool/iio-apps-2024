from threading import Event, Thread
from queue import Queue
from pynput import keyboard
import iio
import numpy as np
import array
from time import sleep
import time
import math
import tkinter as tk


URI                 =   "ip:10.76.84.253"
DEVICE_NAME         =   "ad5592rs"
ATTR_NAME           =   "raw"
EN 		            =   "en"
THR_LOW             =   7
THR_HIGH            =   50
DEG                 =   43
NO_CH               =   6
PWM_TIMEOUT         =   0.2



def update_square(canvas, square, time):
    intensity = 255 - int(time * 255)
    color = '#'
    hexcod = hex(intensity).replace('0x', '') 
    if (intensity < 16):
        cod = '0'
        cod += hexcod
    else:
        cod  = ''
        cod +=  hexcod
    color += cod
    color += cod
    color += 'ff'
    canvas.itemconfig(square, fill=color)
    
def frontend_thread_func(movement):

    def update_gui():
        step = movement.get()
        update_square(canvas, squares['left'], step['left'])
        update_square(canvas, squares['right'], step['right'])
        update_square(canvas, squares['front'], step['front'])
        update_square(canvas, squares['back'], step['back'])

        root.after(int(PWM_TIMEOUT * 100), update_gui)
    
   
    root = tk.Tk()
    canvas = tk.Canvas(root, width=400, height=400)
    canvas.pack()
    
    sq_size = 100
    squares = {
        'left': canvas.create_rectangle(50, 150, 50 + sq_size, 150 + sq_size, fill = 'white'),
        'right': canvas.create_rectangle(250, 150, 250 + sq_size, 150 + sq_size, fill = 'white'),
        'front': canvas.create_rectangle(150, 50, 150 + sq_size, 50 + sq_size, fill = 'white'),
        'back': canvas.create_rectangle(150, 250, 150 + sq_size, 250 + sq_size,fill = 'white'),
    }
    update_gui()
    root.mainloop()


def init_device():
    #context
    ctx = iio.Context(URI)
    if not ctx:
        print("No context found")
        SystemExit
    else:
        print("Context established")
        
    #device  
    cora = ctx.find_device(DEVICE_NAME)
    if not cora:
        print("No device found")
        SystemExit
    print("Device established")
    return cora

def backend_thread_func(movement):
    cora = init_device()
    while not exit_event.is_set():
        if start_event.is_set():
            movement.put(start_iio(cora))
        start_event.wait()

def get_data(cora):
    axis = ['x', 'y', 'z']
    data = {
        'x':{'-': int, '+': int},
        'y':{'-': int, '+': int},
        'z':{'-': int, '+': int},
    }
    val = []
    for i in range(NO_CH):
        chan = cora.find_channel(f"voltage{i}")
        val.append(chan.attrs[ATTR_NAME].value)
    data['x']['+'] = val[0]
    data['x']['-'] = val[1]
    data['y']['+'] = val[2]
    data['y']['-'] = val[3]
    data['z']['+'] = val[4]
    data['z']['-'] = val[5]
    return data

def get_roll_pitch(data):
    z_accel = float(data['z']['+']) - float(data['z']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    x_accel = float(data['x']['+']) - float(data ['x']['-'])
    roll = 180 * math.atan2(y_accel, math.sqrt(x_accel*x_accel + z_accel*z_accel))/math.pi
    pitch = 180 * math.atan2(x_accel, math.sqrt(y_accel*y_accel + z_accel*z_accel))/ math.pi
    return roll, pitch

def get_movement(roll, pitch):
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    if (roll > THR_LOW):
        if (roll > THR_HIGH):
            movement['right'] = 1
        else:
            movement['right'] = roll/DEG - THR_LOW/DEG
    elif (-roll > THR_LOW):
        if (-roll > THR_HIGH):
            movement['left'] = 1
        else:
            movement['left'] = abs(roll/DEG) - THR_LOW/DEG
    else:
        movement['right'] = 0
        movement['left'] = 0
        
    if (pitch > THR_LOW):
        if (pitch > THR_HIGH):
            movement['back'] = 1
        else:
            movement['back'] = pitch/DEG - THR_LOW/DEG
    elif (-pitch > THR_LOW):
        if (-pitch > THR_HIGH):
            movement['front'] = 1
        else:
            movement['front'] = abs(pitch/DEG) - THR_LOW/DEG
    else:
        movement['front'] = 0
        movement['back'] = 0
    
    return movement

def start_iio(cora):
    
    time_start = time.time()
    data = get_data(cora)
    roll, pitch = get_roll_pitch(data)
    movement = get_movement(roll, pitch)
    time_duration = time.time() - time_start
    
    on_time = -1
    for move, key in [('front', 'w'), ('back', 's'), ('left', 'a'), ('right', 'd')]:
        value = movement[move]
        if (value > 0):
            on_time = PWM_TIMEOUT * value
            thread = Thread(target=key_press, args=(key,on_time))
            thread.daemon = True
            thread.start()
    if (on_time != -1):
        sleep(PWM_TIMEOUT - time_duration)
    return movement
    
def key_press(key, on_time):
    kb_controller = keyboard.Controller()
    kb_controller.press(key)
    sleep(on_time)
    kb_controller.release(key)
    

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
    #frontend_thread.join()
    #backend_thread.join() 
  
