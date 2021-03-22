import cmsisdsp as dsp
import numpy as np
from scipy import signal
#import matplotlib.pyplot as plt
#from scipy.fftpack import dct 

#r = dsp.arm_add_f32(np.array([1.,2,3]),np.array([4.,5,7]))
#print(r)

#r = dsp.arm_add_q31([1,2,3],[4,5,7])
#print(r)
#
#r = dsp.arm_add_q15([1,2,3],[4,5,7])
#print(r)
#
#r = dsp.arm_add_q7([-1,2,3],[4,127,7])
#print(r)
#
#r = dsp.arm_scale_f32([1.,2,3],2)
#print(r)
#
#r = dsp.arm_scale_q31([0x7FFF,0x3FFF,0x1FFF],1 << 20,2)
#print(r)
#
#r = dsp.arm_scale_q15([0x7FFF,0x3FFF,0x1FFF],1 << 10,2)
#print(r)
#
#r = dsp.arm_scale_q7([0x7F,0x3F,0x1F],1 << 5,2)
#print(r)
#
#
#r = dsp.arm_negate_f32([1.,2,3])
#print(r)
#
#r = dsp.arm_negate_q31([1,2,3])
#print(r)
#
#r = dsp.arm_negate_q15([1,2,3])
#print(r)
#
#r = dsp.arm_negate_q7(np.array([0x80,0x81,0x82]))
#print(r)

#r = dsp.arm_cmplx_conj_f32([1.,2,3,4])
#print(r)

#r = dsp.arm_cmplx_conj_q31([1,2,3,4])
#print(r)

#r = dsp.arm_cmplx_conj_q15([1,2,3,4])
#print(r)

#r = dsp.arm_cmplx_dot_prod_f32([1.,2,3,4],[1.,2,3,4])
#print(r)

#r = dsp.arm_cmplx_dot_prod_q31([0x1FFF,0x3FFF,0x1FFF,0x3FFF],[0x1FFF,0x3FFF,0x1FFF,0x3FFF])
#print(r)

#r = dsp.arm_cmplx_mult_real_f32([1.0,2,3,4],[5.,5.,5.,5.])
#print(r)

#pidf32 = dsp.arm_pid_instance_f32(Kp=1.0,Ki=1.2,Kd=0.4)
#print(pidf32.Kp())
#print(pidf32.Ki())
#print(pidf32.Kd())
#print(pidf32.A0())
#
#dsp.arm_pid_init_f32(pidf32,0)
#print(pidf32.A0())

#print(dsp.arm_cos_f32(3.14/4.))

#print(dsp.arm_sqrt_q31(0x7FFF))

firf32 = dsp.arm_fir_instance_f32()
dsp.arm_fir_init_f32(firf32,3,[1.,2,3],[0,0,0,0,0,0,0])
print(firf32.numTaps())
filtered_x = signal.lfilter([3,2,1.], 1.0, [1,2,3,4,5,1,2,3,4,5])
print(filtered_x)
print(dsp.arm_fir_f32(firf32,[1,2,3,4,5]))
print(dsp.arm_fir_f32(firf32,[1,2,3,4,5]))

def q31sat(x):
     if x > 0x7FFFFFFF:
          return(np.int32(0x7FFFFFFF))
     elif x < -0x80000000:
          return(np.int32(0x80000000))
     else:
          return(np.int32(x))

q31satV=np.vectorize(q31sat)

def toQ31(x):
     return(q31satV(np.round(x * (1<<31))))

def q15sat(x):
     if x > 0x7FFF:
          return(np.int16(0x7FFF))
     elif x < -0x8000:
          return(np.int16(0x8000))
     else:
          return(np.int16(x))

q15satV=np.vectorize(q15sat)

def toQ15(x):
     return(q15satV(np.round(x * (1<<15))))

def q7sat(x):
     if x > 0x7F:
          return(np.int8(0x7F))
     elif x < -0x80:
          return(np.int8(0x80))
     else:
          return(np.int8(x))

q7satV=np.vectorize(q7sat)

def toQ7(x):
     return(q7satV(np.round(x * (1<<7))))

def Q31toF32(x):
     return(1.0*x / 2**31)

def Q15toF32(x):
     return(1.0*x / 2**15)

def Q7toF32(x):
     return(1.0*x / 2**7)

#firq31 = dsp.arm_fir_instance_q31()
#x=np.array([1,2,3,4,5])/10.0
#taps=np.array([1,2,3])/10.0
#xQ31=toQ31(x)
#tapsQ31=toQ31(taps)
#dsp.arm_fir_init_q31(firq31,3,tapsQ31,[0,0,0,0,0,0,0])
#print(firq31.numTaps())
#resultQ31=dsp.arm_fir_q31(firq31,xQ31)
#result=Q31toF32(resultQ31)
#print(result)

#a=np.array([[1.,2,3,4],[5,6,7,8],[9,10,11,12]])
#b=np.array([[1.,2,3,4],[5.1,6,7,8],[9.1,10,11,12]])
#print(a+b)
#v=dsp.arm_mat_add_f32(a,b)
#print(v)

#a=np.array([[1.,2,3,4],[5,6,7,8],[9,10,11,12]])
#b=np.array([[1.,2,3],[5.1,6,7],[9.1,10,11],[5,8,4]])
#print(np.dot(a , b))
#v=dsp.arm_mat_mult_f32(a,b)
#print(v)

def imToReal2D(a):
    ar=np.zeros(np.array(a.shape) * [1,2])
    ar[::,0::2]=a.real
    ar[::,1::2]=a.imag
    return(ar)

def realToIm2D(ar):
    return(ar[::,0::2] + 1j * ar[::,1::2])

#a=np.array([[1. + 2j,3 + 4j],[5 + 6j,7 + 8j],[9 + 10j,11 + 12j]])
#b=np.array([[1. + 2j, 3 + 5.1j ,6 + 7j],[9.1 + 10j,11 + 5j,8 +4j]])
#print(np.dot(a , b))
#
# Convert complex array to real array for use in CMSIS DSP
#ar = imToReal2D(a)
#br = imToReal2D(b)
#
#v=dsp.arm_mat_cmplx_mult_f32(ar,br)
#print(v)

#a=np.array([[1.,2,3,4],[5,6,7,8],[9,10,11,12]]) / 30.0
#b=np.array([[1.,2,3,4],[5.1,6,7,8],[9.1,10,11,12]]) / 30.0
#print(a+b)
#
#aQ31=toQ31(a)
#bQ31=toQ31(b)
#v=dsp.arm_mat_add_q31(aQ31,bQ31)
#rQ31=v[1]
#r=Q31toF32(rQ31)
#print(r)#

#a=np.array([[1.,2,3,4],[5,6,7,8],[9,10,11,12]])
#print(np.transpose(a))
#print(dsp.arm_mat_trans_f32(a))

#a = np.array([[1., 2.], [3., 4.]])
#print(np.linalg.inv(a))
#print(dsp.arm_mat_inverse_f32(a))

#a = np.array([[1., 2.], [3., 4.]])
#print(np.linalg.inv(a))
#print(dsp.arm_mat_inverse_f64(a))

#a=np.array([[1.,2,3,4],[5,6,7,8],[9,10,11,12]])
#print(2.5*a)
#print(dsp.arm_mat_scale_f32(a,2.5))

#a=np.array([1.,2,3,4,5,6,7,8,9,10,11,12])
#print(np.max(a))
#print(np.argmax(a))
#print(dsp.arm_max_f32(a))
#
#print(np.mean(a))
#print(dsp.arm_mean_f32(a))
#
#print(np.dot(a,a))
#print(dsp.arm_power_f32(a))
#

def imToReal1D(a):
    ar=np.zeros(np.array(a.shape) * 2)
    ar[0::2]=a.real
    ar[1::2]=a.imag
    return(ar)

def realToIm1D(ar):
    return(ar[0::2] + 1j * ar[1::2])

#nb = 16
#signal = np.cos(2 * np.pi * np.arange(nb) / nb)

#result=np.fft.fft(signal)
#print(result)
#signalR = imToReal1D(signal)
#cfftf32=dsp.arm_cfft_instance_f32()
#status=dsp.arm_cfft_init_f32(cfftf32,nb)
#print(status)
#resultR = dsp.arm_cfft_f32(cfftf32,signalR,0,1)
#resultI = realToIm1D(resultR)
#print(resultI)

#signal = signal / 10.0
#result=np.fft.fft(signal)
#print(result)
#
#signalR = imToReal1D(signal)
#signalRQ31=toQ31(signalR)
#cfftq31=dsp.arm_cfft_instance_q31()
#status=dsp.arm_cfft_init_q31(cfftq31,nb)
#print(status)
#resultR = dsp.arm_cfft_q31(cfftq31,signalRQ31,0,1)
#resultI = realToIm1D(Q31toF32(resultR))*16
#print(resultI)

#signal = signal / 10.0
#result=np.fft.fft(signal)
#print(result)
##
#signalR = imToReal1D(signal)
#signalRQ15=toQ15(signalR)
#cfftq15=dsp.arm_cfft_instance_q15()
#status=dsp.arm_cfft_init_q15(cfftq15,nb)
#print(status)
#resultR = dsp.arm_cfft_q15(cfftq15,signalRQ15,0,1)
#resultR=Q15toF32(resultR)
#resultI = realToIm1D(resultR)*16
#print(resultI)

#nb = 128
#signal = np.cos(2 * np.pi * np.arange(nb) / nb)
#
#result=np.fft.fft(signal)
##print(result)
#cfftradix4f32=dsp.arm_cfft_radix4_instance_f32()
#rfftf32=dsp.arm_rfft_instance_f32()
#status=dsp.arm_rfft_init_f32(rfftf32,cfftradix4f32,nb,0,1)
#print(status)
#resultI = dsp.arm_rfft_f32(rfftf32,signal)
#print(result)

#nb = 128
#signal = np.cos(2 * np.pi * np.arange(nb) / nb)
#signalRQ31=toQ31(signal)
#
#result=np.fft.fft(signal)
##print(result)
#rfftq31=dsp.arm_rfft_instance_q31()
#status=dsp.arm_rfft_init_q31(rfftq31,nb,0,1)
#print(status)
#resultI = dsp.arm_rfft_q31(rfftq31,signalRQ31)
#resultI=Q31toF32(resultI)*(1 << 7)
##print(result)

#nb = 128
#signal = np.cos(2 * np.pi * np.arange(nb) / nb)
#signalRQ15=toQ15(signal)
#
#result=np.fft.fft(signal)
##print(result)
#rfftq15=dsp.arm_rfft_instance_q15()
#status=dsp.arm_rfft_init_q15(rfftq15,nb,0,1)
#print(status)
#resultI = dsp.arm_rfft_q15(rfftq15,signalRQ15)
#resultI=Q15toF32(resultI)*(1 << 7)
#print(result)


#nb = 128
#nb2=64
#signal = np.cos(2 * np.pi * np.arange(nb) / nb)
#result=dct(signal,4,norm='ortho')
##print(result)

#cfftradix4f32=dsp.arm_cfft_radix4_instance_f32()
#rfftf32=dsp.arm_rfft_instance_f32()
#dct4f32=dsp.arm_dct4_instance_f32()
#status=dsp.arm_dct4_init_f32(dct4f32,rfftf32,cfftradix4f32,nb,nb2,0.125)
#print(status)
#state=np.zeros(2*nb)
#resultI = dsp.arm_dct4_f32(dct4f32,state,signal)
##print(resultI)


#signal = signal / 10.0
#result=dct(signal,4,norm='ortho')
#signalQ31=toQ31(signal)
#cfftradix4q31=dsp.arm_cfft_radix4_instance_q31()
#rfftq31=dsp.arm_rfft_instance_q31()
#dct4q31=dsp.arm_dct4_instance_q31()
#status=dsp.arm_dct4_init_q31(dct4q31,rfftq31,cfftradix4q31,nb,nb2,0x10000000)
#print(status)
#state=np.zeros(2*nb)
#resultI = dsp.arm_dct4_q31(dct4q31,state,signalQ31)
#resultI=Q31toF32(resultI)*(1 << 7)

#nb = 128
#nb2=64
#signal = np.cos(2 * np.pi * np.arange(nb) / nb)
#signal = signal / 10.0
#result=dct(signal,4,norm='ortho')
#signalQ15=toQ15(signal)
#cfftradix4q15=dsp.arm_cfft_radix4_instance_q15()
#rfftq15=dsp.arm_rfft_instance_q15()
#dct4q15=dsp.arm_dct4_instance_q15()
#status=dsp.arm_dct4_init_q15(dct4q15,rfftq15,cfftradix4q15,nb,nb2,0x1000)
#print(status)
#state=np.zeros(2*nb)
#resultI = dsp.arm_dct4_q15(dct4q15,state,signalQ15)
#resultI=Q15toF32(resultI)*(1 << 7)
#
#
#from pylab import figure, clf, plot, xlabel, ylabel, xlim, ylim, title, grid, axes, show
#figure(1)
#plot(np.absolute(signal))
#t = np.arange(nb)
#freq = np.fft.fftfreq(t.shape[-1])
#resultmag=np.absolute(result)
#figure(2)
#plot(resultmag)
#figure(3)
#cmsigmag=np.absolute(resultI)
#plot(cmsigmag)
#show()##

#biquadf32 = dsp.arm_biquad_casd_df1_inst_f32()
#numStages=1
#state=np.zeros(numStages*4)
#coefs=[1.,2,3,4,5]
#dsp.arm_biquad_cascade_df1_init_f32(biquadf32,1,coefs,state)
#print(dsp.arm_biquad_cascade_df1_f32(biquadf32,[1,2,3,4,5]))#