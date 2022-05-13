from ulab import numpy as np
import time

a = np.zeros(1024)
c = np.zeros(1024)
t = 0

Lx = 2 #2 seconds
omg = 2*np.pi/Lx
x = np.linspace(0,Lx,1024)

for b in [10 , 30 , 40]: #of times each wave is generated per two seconds
    for t in range(1024):
        a[t] = a[t] + np.sin(b * omg * x[t])

a = np.fft.fft(a)

for t in range(1024):
    c[t] = np.sqrt(((a[0][t]) * (a[0][t]) + (a[1][t]) * (a[1][t])))

for t in range(512 , 1024 , 1): #Ignoring the complex conjugates
    print("(" + str(c[t]) + ",)")  # print with plotting format
    time.sleep(0.01)


