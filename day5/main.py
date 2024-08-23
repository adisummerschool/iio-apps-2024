from threading import Event, Thread
from queue import Queue
from time import sleep, time
from pynput import keyboard
import tkinter as tk
import math
import iio

URI                     = "ip:10.76.84.245"
DEVICE_NAME             = "ad5592r_s"
CHANNEL_ATTRIBUTE       = 'raw'
CHANNELS_NUMBER         = 6
MIN_THRESHOLD           = 10
MAX_THRESHOLD           = 60
PWM_TIMEOUT             = 0.2
SQUARE_DIMESION         = 100
SQUARE_BORDER_WIDTH     = 3
SQUARE_OUTLINE          = "black"
SQUARE_INITIAL_FILL     = "white"
SQUARE_MAX_FILL         ="#00FF00"
ZERO_PAIR               = "00"
HEX_FORMAT              = '02x'
NUMBER_SIGH             = "#"
MAX_COLOR_NUMBER        = 255
COLOR_UPPER_LIMIT       = 200

window = None
canvas = None
squares = {}

def create_window():
    global window, canvas, squares

    window = tk.Tk()
    window.title('Input strenght')

    window_height = 600
    window_width = 900

    canvas = tk.Canvas(window, width=window_width, height=window_height, borderwidth=0, highlightthickness=0)

    screen_width = window.winfo_screenwidth()
    screen_height = window.winfo_screenheight()

    x_cordinate = int((screen_width/2) - (window_width/2))
    y_cordinate = int((screen_height/2) - (window_height/2))

    window.geometry("{}x{}+{}+{}".format(window_width, window_height, x_cordinate, y_cordinate))

    sq_f_b_x0 = int((window_width/2) - (SQUARE_DIMESION/2))
    sq_f_y = int((window_height/2) - (SQUARE_DIMESION + SQUARE_DIMESION/2))
    sq_b_y = int((window_height/2) + (SQUARE_DIMESION/2))

    sq_l_r_y0 = int((window_height/2) - (SQUARE_DIMESION/2))
    sq_l_x = int((window_width/2) - (SQUARE_DIMESION*2))
    sq_r_x = int((window_width/2) + SQUARE_DIMESION)
    
    squares['front'] = canvas.create_rectangle(sq_f_b_x0, sq_f_y, (sq_f_b_x0 + SQUARE_DIMESION), (sq_f_y + SQUARE_DIMESION), outline=SQUARE_OUTLINE, fill=SQUARE_INITIAL_FILL, width=SQUARE_BORDER_WIDTH)
    squares['back'] = canvas.create_rectangle(sq_f_b_x0, sq_b_y, (sq_f_b_x0 + SQUARE_DIMESION), (sq_b_y + SQUARE_DIMESION), outline=SQUARE_OUTLINE, fill=SQUARE_INITIAL_FILL, width=SQUARE_BORDER_WIDTH)
    squares['left'] = canvas.create_rectangle(sq_l_x, sq_l_r_y0, (sq_l_x + SQUARE_DIMESION), (sq_l_r_y0 + SQUARE_DIMESION), outline=SQUARE_OUTLINE, fill=SQUARE_INITIAL_FILL, width=SQUARE_BORDER_WIDTH)
    squares['right'] = canvas.create_rectangle(sq_r_x, sq_l_r_y0, (sq_r_x + SQUARE_DIMESION), (sq_l_r_y0 + SQUARE_DIMESION), outline=SQUARE_OUTLINE, fill=SQUARE_INITIAL_FILL, width=SQUARE_BORDER_WIDTH)

    canvas.pack()

    window.mainloop()

def update_window(movement):
    global window, canvas, squares

    if not movement.empty():
        step = movement.get()
        
        frontCode = SQUARE_INITIAL_FILL
        backCode = SQUARE_INITIAL_FILL
        rightCode = SQUARE_INITIAL_FILL
        leftCode = SQUARE_INITIAL_FILL

        if step['front'] > 0:
            frontCode = NUMBER_SIGH + ZERO_PAIR + format(MAX_COLOR_NUMBER - int(step['front'] * COLOR_UPPER_LIMIT), HEX_FORMAT).upper() + ZERO_PAIR
        if step['back'] > 0:
            backCode = NUMBER_SIGH + ZERO_PAIR + format(MAX_COLOR_NUMBER - int(step['back'] * COLOR_UPPER_LIMIT), HEX_FORMAT).upper() + ZERO_PAIR
        if step['right'] > 0:
            rightCode = NUMBER_SIGH + ZERO_PAIR + format(MAX_COLOR_NUMBER - int(step['right'] * COLOR_UPPER_LIMIT), HEX_FORMAT).upper() + ZERO_PAIR
        if step['left'] > 0:
            leftCode = NUMBER_SIGH + ZERO_PAIR + format(MAX_COLOR_NUMBER - int(step['left'] * COLOR_UPPER_LIMIT), HEX_FORMAT).upper() + ZERO_PAIR
        
        canvas.itemconfig(squares['front'], fill=frontCode)
        canvas.itemconfig(squares['back'], fill=backCode)
        canvas.itemconfig(squares['right'], fill=rightCode)
        canvas.itemconfig(squares['left'], fill=leftCode)

    window.update()

def frontend_thread_func(movement):
    window_thread = Thread(target=create_window,args=())
    window_thread.daemon = True
    window_thread.start()

    while not exit_event.is_set():
        if start_event.is_set():
            update_window(movement)
            
        start_event.wait()

def pressKey_thread_func(key, sleepTime):
    keyInput = keyboard.Controller()
    keyInput.press(key)
    sleep(sleepTime)
    keyInput.release(key)

def get_data(device):
    axis = ['x', 'y', 'z']
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    #get data
    for i in range(CHANNELS_NUMBER):
        channel = device.find_channel(f"voltage{i}")
        
        j = '+'
        if i % 2 == 1:
            j = '-'

        data[axis[int(i / 2)]][j] = int(channel.attrs[CHANNEL_ATTRIBUTE].value)

    return data

def get_roll_pitch(data):
    z_accel = float(data['z']['+']) - float(data['z']['-'])
    y_accel = float(data['y']['+']) - float(data['y']['-'])
    x_accel = float(data['x']['+']) - float(data ['x']['-'])

    # Calculate roll and pitch
    roll = 180 * math.atan2(y_accel, math.sqrt(x_accel*x_accel + z_accel*z_accel))/math.pi
    pitch = 180 * math.atan2(x_accel, math.sqrt(y_accel*y_accel + z_accel*z_accel))/ math.pi

    return roll, pitch

def get_movement(roll, pitch):
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    
    #front
    if roll >= MIN_THRESHOLD and roll <= MAX_THRESHOLD:
        movement['front'] = abs(roll) / MAX_THRESHOLD
    elif roll <= MIN_THRESHOLD:
        movement['front'] = 0
    else:
        movement['front'] = 1

    #back
    if roll <= -MIN_THRESHOLD and roll >= -MAX_THRESHOLD:
        movement['back'] = abs(roll) / MAX_THRESHOLD
    elif roll >= -MIN_THRESHOLD:
        movement['back'] = 0
    else:
        movement['back'] = 1

    #right
    if pitch >= MIN_THRESHOLD and pitch <= MAX_THRESHOLD:
        movement['right'] = abs(pitch) / MAX_THRESHOLD
    elif pitch <= MIN_THRESHOLD:
        movement['right'] = 0
    else:
        movement['right'] = 1

    #left
    if pitch <= -MIN_THRESHOLD and pitch >= -MAX_THRESHOLD:
        movement['left'] = abs(pitch) / MAX_THRESHOLD
    elif pitch >= -MIN_THRESHOLD:
        movement['left'] = 0
    else:
        movement['left'] = 1

    return movement

def start_iio(device):
    start_time = time()

    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    movement = get_movement(roll, pitch)

    keys = {'left': 'a', 'right': 'd', 'front': 'w', 'back': 's'}
    iio_timeout = time() - start_time
    sleep_time = -1

    #simulation loop
    for move in movement:
        value = movement[move]
        if value > 0:
            sleep_time = PWM_TIMEOUT * value
            key_thread = Thread(target=pressKey_thread_func, args=(keys[move],sleep_time,))
            key_thread.daemon = True
            key_thread.start()
    
    if sleep_time != -1:
        sleep_period = PWM_TIMEOUT - iio_timeout
        if sleep_period >= 0:
            sleep(sleep_period)
        else:
            sleep(0)
    
    return movement


def backend_thread_func(movement):
    device = init_device()
    
    while not exit_event.is_set():
        if(start_event.is_set()):
            movement.put(start_iio(device))
        start_event.wait()


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
    
    return True

def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("Context unavailable!!!\n")
        SystemExit

    device = ctx.find_device(DEVICE_NAME)
    if not device:
        print("No device with name \"" + DEVICE_NAME + "\"")
        SystemExit

    return device


if __name__ == '__main__':
    start_event = Event()
    exit_event = Event()

    init_movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movmevent_queue = Queue()
    movmevent_queue.put(init_movement)

    frontend_thread = Thread(target=frontend_thread_func, args=(movmevent_queue,))
    frontend_thread.daemon = True
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movmevent_queue,))
    backend_thread.daemon = True
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    listener.join()
    frontend_thread.join()
    backend_thread.join()
