import cmsisdsp as dsp
import numpy as np
from scipy import signal
from pylab import figure, clf, plot, xlabel, ylabel, xlim, ylim, title, grid, axes, show,semilogx, semilogy
# Data file from https://www.physionet.org/pn3/ecgiddb/Person_87/rec_2.dat

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

def Q31toF32(x):
     return(1.0*x / 2**31)

filename = 'rec_2.dat'

f = open(filename,"r")
sig = np.fromfile(f, dtype=np.int16)
f.closed

sig = 1.0*sig / (1 << 12)


p0 = np.exp(1j*0.05) * 0.98 
p1 = np.exp(1j*0.25) * 0.9 
p2 = np.exp(1j*0.45) * 0.97

z0 = np.exp(1j*0.02)
z1 = np.exp(1j*0.65)
z2 = np.exp(1j*1.0)

g = 0.02

nb = 300

sos = signal.zpk2sos(
      [z0,np.conj(z0),z1,np.conj(z1),z2,np.conj(z2)]
     ,[p0, np.conj(p0),p1, np.conj(p1),p2, np.conj(p2)]
     ,g)

res=signal.sosfilt(sos,sig)
figure()
plot(sig[1:nb])
figure()
plot(res[1:nb])




biquadQ31 = dsp.arm_biquad_casd_df1_inst_q31()
numStages=3
state=np.zeros(numStages*4)
# For use in CMSIS, denominator coefs must be negated
# and first a0 coef wihich is always 1 must be removed
coefs=np.reshape(np.hstack((sos[:,:3],-sos[:,4:])),15)
coefs = coefs / 4.0 
coefsQ31 = toQ31(coefs)
postshift = 2
dsp.arm_biquad_cascade_df1_init_q31(biquadQ31,numStages,coefsQ31,state,postshift)
sigQ31=toQ31(sig)
nbSamples=sigQ31.shape[0]
# Here we demonstrate how we can process a long sequence of samples per block
# and thus check that the state of the biquad is well updated and preserved
# between the calls.
half = int(round(nbSamples / 2))
res2a=dsp.arm_biquad_cascade_df1_q31(biquadQ31,sigQ31[1:half])
res2b=dsp.arm_biquad_cascade_df1_q31(biquadQ31,sigQ31[half+1:nbSamples])
res2=Q31toF32(np.hstack((res2a,res2b)))
figure()
plot(res2[1:nb])
show()#