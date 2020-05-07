import math as MM
import time
import audiobusio
import audioio
import array
import board

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

## 5 cycles for an 82 cycle signal
dt = (1/5125.)
N = 1000
nmax = 80
f = array.array("H", [0] * (N))
mic = audiobusio.PDMIn(board.MICROPHONE_CLOCK, board.MICROPHONE_DATA, sample_rate=16000, bit_depth=16,mono=True)
L = dt*N
print(dt)
print(L)
#time.sleep(10)
while True:
    ##TAKE 100 DATA POINT AND PLACE INTO ARRAY
    print('Taking Data in 3 second...')
    time.sleep(3)
    mic.record(f, len(f)) ##Interpolates
    print(f)
    print(q)
    ctr = 0
    #for fi in f:
    #    ctr+=1
    #    if ctr > 1:
    #        if fi > 0:
    #            print((fi-32768-500,))
    #            ctr = 0
    #    time.sleep(0.01)
    print('Done')
    ###PERFORM FFT ON FREQUENCIES FROM 0 TO NMAX
    awn_fund = 0
    bwn_fund = 0
    anMAX = 0.0
    bnMAX = 0.0
    bf_fund = 0
    af_fund = 0
    print('Computing SFT....')
    for i in range(1,0):
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
    print('Fundamental Frequency (Hz) = ',af_fund,bf_fund)
    #print('Waiting  second')
    #time.sleep(10)