from threading import Event, Thread
from queue import Queue
from time import sleep, time
from pynput import keyboard
import iio
import math
import tkinter as tk


URI = "ip:10.76.84.107"
DEV_NAME = "ad5592r_s"
CHANNELS_NUMBER = 6
PWM_TIMEOUT = 0.1
LOW = 5
HIGH = 45

# def rgb2hex(r,g,b):
#     return f'#{r:02x}{g:02x}{b:02x}'

def update_square(canvas, square, time):
    intensity = int(time * 255)
    #color = '#0000ff'
    r = 255 - intensity
    g = 255 - intensity
    b = 255
    

    color = f'#{r:02x}{g:02x}{b:02x}'
    print(color)
    canvas.itemconfig(square, fill=color)

def frontend_thread_func(movement):

    def update_gui():
        movements = movement.get()
        update_square(canvas, squares['left'], movements['left'])
        update_square(canvas, squares['right'], movements['right'])
        update_square(canvas, squares['front'], movements['front'])
        update_square(canvas, squares['back'], movements['back'])

        root.after(int(PWM_TIMEOUT*100), update_gui)

    root = tk.Tk()
    canvas = tk.Canvas(root, width=400, height=400)
    canvas.pack()

    sq_sz = 100
    squares = {
        'left': canvas.create_rectangle(50, 150, 50 + sq_sz, 150 + sq_sz, fill='white'),
        'right':canvas.create_rectangle(250, 150, 250 + sq_sz, 150 + sq_sz, fill='white'),
        'front':canvas.create_rectangle(150, 50, 150 + sq_sz, 50 + sq_sz, fill='white'),
        'back':canvas.create_rectangle(150, 250, 150 + sq_sz, 250 + sq_sz, fill='white'),
    }

    update_gui()
    root.mainloop() #"tine" gui-ul in viata, daca nu facem in int. update_gui ceva ca sa se mai actualizeze, atunci nu se va actualiza


def init_device():
    ctx = iio.Context(URI)
    if not ctx:
        print("No contex")
    device = ctx.find_device(DEV_NAME)
    if not device:
        print("No device")
    return device

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

    #print(f"Roll: {roll}, Pitch: {pitch}")
    #print(f"Left: {movement['left']}, Right: {movement['right']}, Front: {movement['front']}, Back: {movement['back']}")

    on_time = -1
    for move,key in [('back','s'),('front','w'),('right','d'),('left','a')]:
        value = movement[move]
        if value>0:
            on_time = PWM_TIMEOUT * value
            thread = Thread(target=key_press, args=(key,on_time,))
            thread.daemon = True
            thread.start()

    if on_time != -1 and PWM_TIMEOUT-iio_timeout>0:
        sleep(PWM_TIMEOUT-iio_timeout)
    
    return movement


def get_data(device):
    axis = ['x', 'y', 'z']
    data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }
    # get data
    # data['x']['-'] = value
    values = []
    for i in range(CHANNELS_NUMBER):
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
    roll = math.atan2(y_accel, z_accel) * 180 / math.pi
    pitch = math.atan2(-x_accel, math.sqrt(y_accel ** 2 + z_accel ** 2)) * 180 / math.pi
    return roll,pitch

#determine movement pwm sequence from roll and pitch values
# def get_movement(roll, pitch):
#     movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
#     if (pitch <= LOW and pitch >= -LOW):
#         movement['front'] = 0
#         movement['back'] = 0
#     elif (pitch >= HIGH):
#         movement['front'] = 1
#         movement['back'] = 0
#     elif (pitch <= -HIGH):
#         movement['front'] = 0
#         movement['back'] = 1
#     elif (pitch > LOW and pitch < HIGH):
#         movement['front'] = (pitch - LOW) / (HIGH - LOW)
#         movement['back'] = 0
#     else:
#         movement['back'] = (pitch + LOW) / (-HIGH + LOW)
#         movement['front'] = 0
#     if (roll <= LOW and roll >= -LOW):
#         movement['right'] = 0
#         movement['left'] = 0
#     elif (roll >= HIGH):
#         movement['right'] = 1
#         movement['left'] = 0
#     elif (roll <= -HIGH):
#         movement['right'] = 0
#         movement['left'] = 1
#     elif (roll > LOW and roll < HIGH):
#         movement['right'] = (roll - LOW) / (HIGH - LOW)
#         movement['left'] = 0
#     else:
#         movement['left'] = (roll + LOW) / (-HIGH + LOW)
#         movement['right'] = 0
#     return movement

# def get_movement(roll,pitch):
    # movement = {'left' : 0, 'right' : 0, 'front' : 0, 'back' : 0}
    # if (roll > THR_LOW):
    #     if (roll > THR_HIGH):
    #         movement['right'] = 1
    #     else:
    #         movement['right'] = roll/60 - THR_LOW/60
    # elif (-roll > THR_LOW):
    #     if (-roll > THR_HIGH):
    #         movement['left'] = 1
    #     else:
    #         movement['left'] = abs(roll/60) - THR_LOW/60
    # else:
    #     movement['right'] = 0
    #     movement['left'] = 0
    # if (pitch > THR_LOW):
    #     if (pitch > THR_HIGH):
    #         movement['back'] = 1
    #     else:
    #         movement['back'] = pitch/60 - THR_LOW/60
    # elif (-pitch > THR_LOW):
    #     if (-pitch > THR_HIGH):
    #         movement['front'] = 1
    #     else:
    #         movement['front'] = abs(pitch/60) - THR_LOW/60
    # else:
    #     movement['front'] = 0
    #     movement['back'] = 0
    # return movement
def get_movement(roll, pitch):
    movement = {'left' : 0, 'right' : 0, 'front' : 0, 'back' : 0}
    lf=0
    rg=0
    fr=0
    b=0
    if pitch<5 and pitch>-5:
        if roll<5 and roll>-5:
            lf=0
            rg=0
            fr=0
            b=0
        elif roll>60 and roll<180:
            lf=1
        elif roll<-60 and roll>-180:
            rg=1
        elif (roll>=5 and roll<=60) :
            lf = roll/60.0
        elif (roll<=-5 and roll>=-60) :
            rg = abs(roll)/60.0
    elif pitch>40 and pitch<180:
        b=1
        if roll<5 and roll>-5:
            lf=0
            rg=0
            fr=0
        elif roll>60 and roll<180:
            lf=1
        elif roll<-60 and roll>-180:
            rg=1
        elif (roll>=5 and roll<=60) :
            lf = roll/60.0
        elif (roll<=-5 and roll>=-60) :
            rg = abs(roll)/60.0
    elif pitch<-40 and pitch>-180:
        fr=1
        if roll<5 and roll>-5:
            lf=0
            rg=0
            b=0
        elif roll>60 and roll<180:
            lf=1
        elif roll<-60 and roll>-180:
            rg=1
        elif (roll>=5 and roll<=60) :
            lf = roll/60.0
        elif (roll<=-5 and roll>=-60) :
            rg = abs(roll)/60.0
    elif pitch>=5 and pitch<=40:
        b=pitch/40.0
        if roll<5 and roll>-5:
            lf=0
            rg=0
            fr=0
        elif roll>60 and roll<180:
            lf=1
        elif roll<-60 and roll>-180:
            rg=1
        elif (roll>=5 and roll<=60) :
            lf = roll/60.0
        elif (roll<=-5 and roll>=-60) :
            rg = abs(roll)/60.0
    elif pitch<=-5 and pitch>=-40:
        fr=abs(pitch)/40.0
        if roll<5 and roll>-5:
            lf=0
            rg=0
            b=0
        elif roll>60 and roll<180:
            lf=1
        elif roll<-60 and roll>-180:
            rg=1
        elif (roll>=5 and roll<=60) :
            lf = roll/60.0
        elif (roll<=-5 and roll>=-60) :
            rg = abs(roll)/60.0
    movement = {'left' : lf, 'right' : rg, 'front' : fr, 'back' : b}
    return movement
    
    

def backend_thread_func(movement):
    device = init_device()
    print(device.name)

    while not exit_event.is_set():
        if start_event.is_set():
            #print("running...")
            movement.put(start_iio(device))
            #sleep(1)
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
        return False #daca se termina in False, se termina programul si nu se mai asculta urmatoarele comenzi
    
    return True

if __name__ == '__main__':
    start_event = Event()
    exit_event = Event() #vizibile in ambele threaduri

    init_movement = {'left' : 0, 'right' : 0, 'front' : 0, 'back' : 0}
    movement_queue = Queue()
    movement_queue.put(init_movement)

    frontend_thread = Thread(target=frontend_thread_func, args=(movement_queue,))
    frontend_thread.daemon = True #cand omoram programul, sa fie omorat si acest thread
    frontend_thread.start()

    backend_thread = Thread(target=backend_thread_func, args=(movement_queue,))
    backend_thread.daemon = True #cand omoram programul, sa fie omorat si acest thread
    backend_thread.start()

    listener = keyboard.Listener(on_press=on_keypress)
    listener.daemon = True
    listener.start()

    listener.join() #aici se blocheaza programul si daca e False se termina, iar daca e True asculta urmatoarele comenzi(ruleaza in continuare)
    #frontend_thread.join() #asteptam sa se termine threadul
    #backend_thread.join()
    
