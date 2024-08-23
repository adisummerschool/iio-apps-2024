import iio
from threading import Thread, Event
from queue import Queue
from pynput import keyboard
from time import sleep, time
import math
import tkinter as tk

URI = "ip:10.76.84.239"
DEVICE = "ad5592r_s"
THR_LOW = 5
THR_HIGH = 45
PWM_TIMEOUT = 0.1

def update_sqaure(canvas, square, time):
    # intensity = int(255 * time)
    # intensity = max(0, min(255, intensity))
    # color = "#{:02x}{:02x}{:02x}".format(0, 0, intensity)
    # canvas.itemconfig(square, fill=color)
    intensity = 255 - int(255 * time)
    color = f'#{intensity:02x}{intensity:02x}ff'
    canvas.itemconfig(square, fill=color)

def frontend_thread_f(movement):
    def update_gui():
        movements = movement.get()
        update_sqaure(canvas, squares['left'], movements['left'])
        update_sqaure(canvas, squares['right'], movements['right'])
        update_sqaure(canvas, squares['front'], movements['front'])
        update_sqaure(canvas, squares['back'], movements['back'])
        root.after(int(PWM_TIMEOUT * 100), update_gui)
    root = tk.Tk()
    canvas = tk.Canvas(root, width = 400, height = 400)
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
        #print("{0} {1}".format(chan.id,values[i]))
    data['x']['+'] = values[0]
    data['x']['-'] = values[1]
    data['y']['+'] = values[2]
    data['y']['-'] = values[3]
    data['z']['+'] = values[4]
    data['z']['-'] = values[5]
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
            movement['right'] = roll/60 - THR_LOW/60
    elif (-roll > THR_LOW):
        if (-roll > THR_HIGH):
            movement['left'] = 1
        else:
            movement['left'] = abs(roll/60) - THR_LOW/60
    else:
        movement['right'] = 0
        movement['left'] = 0
    if (pitch > THR_LOW):
        if (pitch > THR_HIGH):
            movement['back'] = 1
        else:
            movement['back'] = pitch/60 - THR_LOW/60
    elif (-pitch > THR_LOW):
        if (-pitch > THR_HIGH):
            movement['front'] = 1
        else:
            movement['front'] = abs(pitch/60) - THR_LOW/60
    else:
        movement['front'] = 0
        movement['back'] = 0
    return movement
def key_press(key, on_time):
    kn_controller = keyboard.Controller()
    kn_controller.press(key)
    sleep(on_time)
    kn_controller.release(key)
def start_iio(device):
    start = time()
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    #print(roll, pitch)
    movement = get_movement(roll, pitch)
    iio_timeout = time() - start
    on_time = -1
    for move, key in [('right', 'd'), ('left', 'a'), ('back', 's'), ('front', 'w')]:
        value = movement[move]
        if value > 0:
            on_time = value*PWM_TIMEOUT
            thread = Thread(target = key_press, args=(key, on_time))
            thread.daemon = True
            thread.start()
        if on_time != -1:
            sleep(PWM_TIMEOUT)
    return movement
def backend_thread_f(movement):
    device = init_device()
    while not exit_event.is_set():
        if start_event.is_set():
            #print("running")
            #sleep(1)
            movement.put(start_iio(device))
        start_event.wait()
def on_keypress(key):
    if key == keyboard.Key.ctrl_l: 
        if start_event.is_set():
            start_event.clear()
            print("paused")
            print(key)
        else:
            start_event.set()
            print("started")
    elif key == keyboard.Key.esc:
        start_event.set()
        exit_event.set()
        print("exited")
        return False
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
    #frontend_thread.join()
    #backend_thread.join()