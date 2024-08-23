from threading import Event, Thread
from queue import Queue
from pynput import keyboard

from time import sleep,time
import math
import iio



URI="ip:10.76.84.240"
DEVICE="ad5592r_s"

PWM_TIMEOUT=0.4

def frontend_thread_func(movement):
    pass

def init_device():
    ctx=iio.Context(URI)

    if not ctx:
        print("No context")
   
    dev=ctx.find_device(DEVICE)

    if not dev:
        print("No device")
    return dev

def get_data(device):
    axes=['x','y','z']
    data={
        'x':{'-': int, '+': int},
        'y':{'-': int, '+': int},
        'z':{'-': int, '+': int},

    }
    channels = []
    values = []
    for i in range(6):
        chan = device.find_channel(f"voltage{i}")
        values.append(chan.attrs['raw'].value)
       # print("{0} {1}".format(chan.id,values[i]))

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
    movement={'left':0, 'right':0, 'front':0, 'back':0}
    if roll>0:
        movement['right']=abs(roll/60)
        if abs(roll)>45:
            movement['right']=1
        elif abs(roll)<5:
            movement['right']=0
    
    elif roll<0:
        movement['left']=abs(roll/60)
        if abs(roll)>45:
            movement['left']=1
        elif abs(roll)<5:
            movement['right']=0
    
    if pitch>0:
        movement['front']=abs(pitch/60)
        if abs(pitch)>45:
            movement['front']=1

        elif abs(pitch)<5:
            movement['right']=0


    elif pitch<0:
        movement['back']=abs(pitch/60)
        if abs(pitch)>45:
            movement['back']=1

        elif abs(pitch)<5:
            movement['right']=0




    return movement

def key_press(key, time):
    kb_controller=keyboard.Controller()

    kb_controller.press(key)
    sleep(time)
    kb_controller.release(key)


def start_iio(device):
    start=time()
    data =get_data(device)
    roll, pitch=get_roll_pitch(data)
   # print(roll,pitch)
    movement=get_movement(roll,pitch)
    iio_timeout=time()-start

    on_time=-1
    for move, keyt in [("left",'a'),('right','d'),('front', 'w'), ('back', 's')]:
        value=movement[move]
        if value>0:
            on_time=PWM_TIMEOUT*value

            thread=Thread(target=key_press, args=(keyt,on_time,))
            thread.daemon=True
            thread.start()
    if on_time != -1:
        sleep(PWM_TIMEOUT)
    return movement


#simulation loop


def backend_thread_func(movement):
    device=init_device()

    while not exit_event.is_set():
        if start_event.is_set():
           
           movement.put(start_iio(device))


        start_event.wait()


def on_keypress(key):

    if key==keyboard.Key.ctrl:
        if start_event.is_set():
            start_event.clear()
            print("startd")
        else:
            start_event.set()
            print("started")

    elif key==keyboard.Key.esc:
        start_event.set()
        exit_event.set()
        print("exit")
        return False
    return True

    print(key)


    

if __name__=='__main__':
    start_event=Event()
    exit_event=Event()
    init_movement={'left':0, 'right':0, 'front':0, 'back':0}
    movement_queue=Queue()
    movement_queue.put(init_movement)

    frontend_thread=Thread(target=frontend_thread_func, args=(movement_queue,))
    frontend_thread.daemon=True
    frontend_thread.start()

    backend_thread=Thread(target=backend_thread_func, args=(movement_queue,))
    backend_thread.daemon=True
    backend_thread.start()

    listener=keyboard.Listener(on_press=on_keypress)
    listener.daemon=True
    listener.start()


    frontend_thread.join()
    backend_thread.join()
    listener.join()



