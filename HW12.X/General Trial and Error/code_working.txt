import board
import sys
import time
import digitalio
import busio
from ulab import numpy as np
from adafruit_ov7670 import (
    OV7670,
    OV7670_SIZE_DIV16,
    OV7670_COLOR_YUV,
    OV7670_TEST_PATTERN_COLOR_BAR,
)

#Camera Initilization #######################################################

with digitalio.DigitalInOut(board.GP10) as reset:
    reset.switch_to_output(False)
    time.sleep(0.001)
    bus = busio.I2C(scl=board.GP5, sda=board.GP4)

cam = OV7670(
    bus,
    data_pins=[
        board.GP12,
        board.GP13,
        board.GP14,
        board.GP15,
        board.GP16,
        board.GP17,
        board.GP18,
        board.GP19,
    ],
    clock=board.GP11,
    vsync=board.GP7,
    href=board.GP21,
    mclk=board.GP20,
    shutdown=board.GP22,
    reset=board.GP10,
)

#pid = cam.product_id
#ver = cam.product_version
#print(f"Detected pid={pid:x} ver={ver:x}")

cam.size = OV7670_SIZE_DIV16
#print(cam.width)
##print(cam.height)

#cam.test_pattern = OV7670_TEST_PATTERN_COLOR_BAR

cam.colorspace = OV7670_COLOR_YUV
cam.flip_y = False

buf = bytearray(2 * cam.width * cam.height) # where all the raw data is stored

# Initialization of the needed arrays
red = np.linspace(1,1,3 * cam.height, dtype=np.float)
green = np.linspace(0,0,3 * cam.height, dtype=np.float)
blue = np.linspace(0,0,3 * cam.height, dtype=np.float)
color = np.linspace(0,0,3*cam.height, dtype=np.float)
row_1 = np.linspace(0,0,cam.height, dtype=np.float)
row_2 = np.linspace(0,0,cam.height, dtype=np.float)
row_3 = np.linspace(0,0,cam.height, dtype=np.float)
row_sum = np.linspace(0.01,0.01,3,dtype=np.float)
row_mass_sum = np.linspace(0,0,3,dtype=np.float)
row_center = np.linspace(0,0,3,dtype=np.float)
ind = 0


#HC6 Blutooth Module Initialization #############################################


led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

uart0 = busio.UART(tx=board.GP0, rx=board.GP1, baudrate=9600,  timeout= 0.01) # open the uart0


# UART1 Initialization for communication with the PIC######################################


uart1 = busio.UART(tx=board.GP8, rx=board.GP9, baudrate=115200,  timeout= 0.01) # open the uart


# Communication Initialization ############################################


movement = 0.0






while True:


    


    #time1 = time.monotonic()
    #print(time1)
    #sys.stdin.readline() # wait for a newline before taking an image
    cam.capture(buf) # get the image

    # process the raw data into color pixels
    ind = 0
    for a in range(1,cam.height/2 * 3 + 1 , 1):

        if a<=15:
            d=2*40*a - 2*10 #getting the 30th column
        elif a<=30:
            d=2*40*(a-15) -  2*20 #getting the 20eth column
        elif a<=45:
            d=2*40*(a-30) -  2*30 #getting the 10th column



        u = buf[d+1] - 128
        v = buf[d+3] - 128
        red[ind] = buf[d] + 1.370705 * v
        if red[ind] > 255:
            red[ind] = 255
        if red[ind] < 0:
            red[ind] = 0
        green[ind] = buf[d] - 0.337633 * u - 0.698001 * v
        if green[ind] > 255:
            green[ind] = 255
        if green[ind] < 0:
            green[ind] = 0
        blue[ind] = buf[d] + 1.732446 * u
        if blue[ind] > 255:
            blue[ind] = 255
        if blue[ind] < 0:
            blue[ind] = 0

        ind = ind+1
        red[ind] = buf[d+2] + 1.370705 * v
        if red[ind] > 255:
            red[ind] = 255
        if red[ind] < 0:
            red[ind] = 0
        green[ind] = buf[d+2] - 0.337633 * u - 0.698001 * v
        if green[ind] > 255:
            green[ind] = 255
        if green[ind] < 0:
            green[ind] = 0
        blue[ind] = buf[d+2] + 1.732446 * u
        if blue[ind] > 255:
            blue[ind] = 255
        if blue[ind] < 0:
            blue[ind] = 0
        ind=ind+1

   
   # Determining the color purple or green #####################################################

    for a in range(red.size):

        total = red[a]+blue[a]+green[a]
        RB = red[a]+blue[a]
        if (RB/total)>0.67:
            color[a] = 1
        if (RB/total)<=0.67:
            color[a] = 0

    # Identfying the rows ################################################################

    for a in range(red.size):
        if a<30:
            row_1[a] = color[a]
        elif a<60:
            row_2[a-30] = color[a]
        else:
            row_3[a-60] = color[a]


    # Center of mass for the rows ##########################################################


   
    row_sum[0] = 0.01
    row_mass_sum[0] = 0
    for a in range(30):
        row_sum[0]  = row_sum[0] + row_1[a]
        row_mass_sum[0] = row_mass_sum[0] + a*row_1[a]
    row_center[0] = row_mass_sum[0]/row_sum[0]

    row_sum[1] = 0.01
    row_mass_sum[1] = 0
    for a in range(30):
        row_sum[1]  = row_sum[1] + row_2[a]
        row_mass_sum[1] = row_mass_sum[1] + a*row_2[a]
    row_center[1] = row_mass_sum[1]/row_sum[1]

    row_sum[2] = 0.01
    row_mass_sum[2] = 0
    for a in range(30):
        row_sum[2]  = row_sum[2] + row_3[a]
        row_mass_sum[2] = row_mass_sum[2] + a*row_3[a]
    row_center[2] = row_mass_sum[2]/row_sum[2]


    # Calculating the difference btw the centers of the row 3 and row 2 (the closest 2 lines) ###########

    row_delta = row_center[1] - row_center[2]

    #print(row_center)



    # Communication with the PIC and the Blutooth module ###############################################


    data_b = uart0.readline() #Check the hc6 for any instrucyions the the start of the while loop.

    if data_b is not None:
        #print(str(data_b,'utf-8')) # print the data as a string
        data =str(data_b,'utf-8').strip()
        if data == "a": #Bluttoth test LED on
            led.value = 1
        elif data == "p": #If the instruction is p (park) send it directly to the PIC 
            led.value = 1
            #uart1.write(bytearray(str(data)+"\n")) 
            time.sleep(0.2)
            movement = 0.0
            led.value = 0
        elif data == "g": #If the instruction is g (g) sen it directly to the PIC
            led.value = 1
            #uart1.write(bytearray(str(data)+"\n")) 
            time.sleep(0.2)
            led.value = 0
            movement = 1.0
        else:
            led.value = 0 #Blutooth test LED off


    uart1.write(bytearray(str(movement)+"\n"))
    uart1.write(bytearray(str(row_delta)+"\n"))
    uart1.write(bytearray(str(row_center[0])+"\n")) #Send the movement, row_delta
    # and the furthest center to the PIC for the PIC to control the motors

    


    #time2 = time.monotonic()

    #delta_time = time2-time1
    #print(row_delta)
    




    #uart0.write(bytearray(str(delta_time)+'\r\n'))

    # send the color data as index red green blue
    #string_write = ""
    #num = 0
    #for c in range(red.size):
    #    string_write = string_write + str(c)+" "+str(int(red[c]))+" "+str(int(green[c]))+" "+str(int(blue[c]))
    #    num = num+1
    #    if num < 20:
    #        string_write = string_write+"\n"
    #    if num == 20:
    #        print(string_write)
    #        string_write = ""
    #        num = 0
    #if len(string_write) != 0:
    #    print(string_write)
