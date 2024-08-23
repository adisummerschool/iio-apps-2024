import iio
import math
from time import sleep, time
from threading import Event, Thread
from queue import Queue
from pynput import keyboard
import tkinter as tk
import sys

URI = "ip:10.76.84.102"
DEVICE_NAME = "ad5592r_s"
DEGREES_HIGH_LIMIT = 50
DEGREES_LOW_LIMIT = 5
PWM_TIMEOUT = 0.1

def update_square(canvas, square, time):
    intensity = min(255, max(0, int(time * 255)))
    
    red = 255
    green = 255 - intensity
    blue = 255 - intensity
    
    color = f'#{red:02x}{green:02x}{blue:02x}'
    canvas.itemconfig(square, fill=color)

def frontend_thread_func(movement):
    def update_gui():
        movements = movement.get()
        update_square(canvas, squares['left'], movements['left'])
        update_square(canvas, squares['right'], movements['right'])
        update_square(canvas, squares['front'], movements['front'])
        update_square(canvas, squares['back'], movements['back'])

        root.after(int(PWM_TIMEOUT * 1000), update_gui)

    def close_gui(event=None):
        root.destroy()

    root = tk.Tk()
    canvas = tk.Canvas(root, width=400, height=400)
    canvas.pack()

    sq_size = 100
    squares = {
        'left': canvas.create_rectangle(50, 150, 50 + sq_size, 150 + sq_size, fill='white'), 
        'right': canvas.create_rectangle(250, 150, 250 + sq_size, 150 + sq_size, fill='white'), 
        'front': canvas.create_rectangle(150, 50, 150 + sq_size, 50 + sq_size, fill='white'), 
        'back': canvas.create_rectangle(150, 250, 150 + sq_size, 250 + sq_size, fill='white'), 
    }
    root.bind('<Escape>', close_gui)

    update_gui()
    root.mainloop()

def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("cannot get context!\n")
        exit(-1)

    dev = ctx.find_device(DEVICE_NAME)
    if not dev:
        print("cannot get device!\n")
        exit(-2)
    return dev

def get_data(device):
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    # get data

    data['x']['+'] = device.find_channel("voltage0").attrs['raw'].value
    data['x']['-'] = device.find_channel("voltage1").attrs['raw'].value
    data['y']['+'] = device.find_channel("voltage2").attrs['raw'].value
    data['y']['-'] = device.find_channel("voltage3").attrs['raw'].value
    data['z']['+'] = device.find_channel("voltage4").attrs['raw'].value
    data['z']['-'] = device.find_channel("voltage5").attrs['raw'].value

    return data

def get_roll_pitch(data):
    x_accel = float(data['x']['+']) - float(data['x']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    z_accel = float(data['z']['+']) - float(data['z']['-'])
    
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi

    return roll, pitch

def get_movement(roll, pitch):
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}

    if pitch < -DEGREES_LOW_LIMIT:
        if pitch < -DEGREES_HIGH_LIMIT:
            pitch = -DEGREES_HIGH_LIMIT
        movement['front'] = -pitch / DEGREES_HIGH_LIMIT
    elif pitch > DEGREES_LOW_LIMIT:
        if pitch > DEGREES_HIGH_LIMIT:
            pitch = DEGREES_HIGH_LIMIT
        movement['back'] = pitch / DEGREES_HIGH_LIMIT
    
    if roll < -DEGREES_LOW_LIMIT:
        if roll < -DEGREES_HIGH_LIMIT:
            roll = -DEGREES_HIGH_LIMIT
        movement['right'] = -roll / DEGREES_HIGH_LIMIT
    elif roll > DEGREES_LOW_LIMIT:
        if roll > DEGREES_HIGH_LIMIT:
            roll = DEGREES_HIGH_LIMIT
        movement['left'] = roll / DEGREES_HIGH_LIMIT

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

    #simulation loop

    on_time = -1
    for move, key in zip(movement, ['a', 'd', 'w', 's']):
        value = movement[move]
        if value > 0:
            on_time = PWM_TIMEOUT * value
            thread = Thread(target=key_press, args=(key, on_time,))
            thread.daemon = True
            thread.start()
    
    if on_time != -1 and PWM_TIMEOUT - iio_timeout > 0:
            sleep(PWM_TIMEOUT - iio_timeout)

    return movement

    #print(f"Left: {movement['left']:.2f}                         ")
    #print(f"Right: {movement['right']:.2f}                       ")
    #print(f"Front: {movement['front']:.2f}                       ")
    #print(f"Back: {movement['back']:.2f}                         ")

    #sys.stdout.write("\033[A")
    #sys.stdout.write("\033[A")
    #sys.stdout.write("\033[A")
    #sys.stdout.write("\033[A")
    #sys.stdout.flush()

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

    frontend_thread.join()
    backend_thread.join()
    listener.join()