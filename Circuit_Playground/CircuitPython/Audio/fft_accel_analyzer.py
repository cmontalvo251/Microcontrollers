import math as MM
import time
from adafruit_circuitplayground.express import cpx

def FFTReimmannSIN(w,x):
    global dt
    out = 0
    for i in range(0,len(x)):
        ti = dt*i
        out += x[i]*(dt)*MM.sin(w*ti)
    return out

def FFTReimmannCOS(w,x):
    global dt
    out = 0
    for i in range(0,len(x)):
        ti = dt*i
        out += x[i]*(dt)*MM.cos(w*ti)
    return out

dt = 0.01
N = 100
nmax = 10
f = [0]*N
L = dt*N
while True:
    ##TAKE 100 DATA POINT AND PLACE INTO ARRAY
    print('Taking Data...')
    for i in range(0,N):
        x, y, z = cpx.acceleration
        f[i] = z
        time.sleep(dt)
        print((z,))
    print('Done')
    ###PERFORM FFT ON FREQUENCIES FROM 0 TO NMAX
    awn_fund = 0
    bwn_fund = 0
    anMAX = 0.0
    bnMAX = 0.0
    bf_fund = 0
    af_fund = 0
    print('Computing FFT....')
    for i in range(1,nmax):
        #print(i,' out of ',nmax)
        #Frequency
        w = 2.0*i*(MM.pi)/L
        #Reimann Sum
        ani = (2.0/L)*FFTReimmannSIN(w,f)
        bni = (2.0/L)*FFTReimmannCOS(w,f)
        if abs(ani) > anMAX:
            anMAX = abs(ani)
            awn_fund = w
            af_fund = (i-1)/L
        if abs(bni) > bnMAX:
            bnMAX = abs(bni)
            bwn_fund = w
            bf_fund = (i-1)/L
    if af_fund < bf_fund:
        f_fund = af_fund
    else:
        f_fund = bf_fund
    print('Fundamental Frequency (Hz) = ',f_fund)
    print('Waiting 1 second')
    time.sleep(1)
