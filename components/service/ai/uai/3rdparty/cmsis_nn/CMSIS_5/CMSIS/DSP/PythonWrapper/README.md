# README

This Python wrapper for CMSIS-DSP is compatible with numpy.

It is a very experimental wrapper with lots of limitations as described in the corresponding section below.

But even with those limitations, it can be very useful to test a CMSIS-DSP implemention of an algorithm with all the power of numpy and scipy.

# How to build and install

## Tested configurations

The building of this package has been tested on Windows with the Python install from python.org and Microsoft Visual 2017. 

It has also been tested with cygwin. In that case, python-devel must be installed too. To run the examples, scipy and matplotlib must also be installed in cygwin.

On Linux, it worked with standard installation.

Other configurations should work but the setup.py file would have to be improved. It is a first version and the build process will have to be improved.

The package is working with Python 2 and 3.

## Building

The build is using a customized arm_math.h in folder cmsisdsp_pkg/src to be able to compile on windows.

As a consequence, if you build on an ARM computer, you won't get the optimizations of the CMSIS library. It is possible to get them by replacing the customized arm_math.h by the official one.

Since the CMSIS-DSP wrapper is using numpy, you must first install it if not already done. So, for instance to install it locally you could do:

    > pip install numpy --user

Once numpy is installed, you can build the CMSIS-DSP python wrapper. Go to folder CMSIS/DSP/PythonWrapper.

Following command will build in place if you have the right compiler and if Python can find it.

    > python setup.py build_ext --inplace

Then, if you launch Python from same directory you'll be able to play with the test scripts. You'll need to install a few additional Python packages to run the examples (scipy and matplotlib). See below.

If you want to install the cmsisdsp package, it is advised to install it into a virtualenv

With Python 3 you could:

Create a folder for this virtual environment. For instance : cmsisdsp_tests

Go to this folder.

Type:

    > python -m venv env

Activate the environment:

    > env\Scripts\activate

Install some packages to be able to run the examples

    > pip install numpy
    > pip install scipy
    > pip install matplotlib

Now, you can install the cmsisdsp package in editable mode:

    > pip install -e "Path To The Folder Containing setup.py"

Then you can copy the scripts testdsp.py and example.py and try to run them from this virtual environment. example.y is requiring a data file to be downloaded from the web. See below in this document for the link.

# Usage

You can look at testdsp.py and example.py for some examples.

The idea is to follow as closely as possible the CMSIS-DSP API to ease the migration to the final implementation on a board.

First you need to import the module

    > import cmsisdsp as dsp

If you use numpy:

    > import numpy as np

If you use scipy signal processing functions:

    > from scipy import signal

## Functions with no instance arguments

You can use a CMSIS-DSP function with numpy arrays:

    > r = dsp.arm_add_f32(np.array([1.,2,3]),np.array([4.,5,7]))

The function can also be called more simply with

    > r = dsp.arm_add_f32([1.,2,3],[4.,5,7])

The result of a CMSIS-DSP function will always be a numpy array whatever the arguments were (numpy array or list).

## Functions with instance arguments 

When the CMSIS-DSP function is requiring an instance data structure, it is just a bit more complex to use it:

First you need to create this instance:

    > firf32 = dsp.arm_fir_instance_f32()

Although the initialization function on Python side can also be used to initialize some of the fields of the corresponding instance using named arguments, it is not advised to do so. In CMSIS-DSP there are initialization functions for this and they may do some additional processing.

So, you need to call an init function:

    > dsp.arm_fir_init_f32(firf32,3,[1.,2,3],[0,0,0,0,0,0,0])

The third argument in this function is the state. Since all arguments (except the instance ones) are read-only in this Python API, this state will never be changed ! It is just used to communicate the length of the state array which must be allocated by the init function. This argument is required because it is present in the CMSIS-DSP API and in the final C implementation you'll need to allocate a state array with the right dimension.

Since the goal is to be as close as possible to the C API, the API is forcing the use of this argument.

The only change compared to the C API is that the size variables (like blockSize for filter) are computed automatically from the other arguments. This choice was made to make it a bit easier the use of numpy array with the API.

Now, you can check that the instance was initialized correctly.

    > print(firf32.numTaps())

Then, you can filter with CMSIS-DSP.:

    > print(dsp.arm_fir_f32(firf32,[1,2,3,4,5]))

The size of this signal should be blockSize. blockSize was inferred from the size of the state array : numTaps + blockSize - 1 according to CMSIS-DSP. So here the signal must have 5 samples.

If you want to filter more than 5 samples, then you can just call the function again. The state variable inside firf32 will ensure that it works like in the CMSIS-DSP C code. 

    > print(dsp.arm_fir_f32(firf32,[6,7,8,9,10]))

If you want to compare with scipy it is easy but warning : coefficients for the filter are in opposite order in scipy :

    > filtered_x = signal.lfilter([3,2,1.], 1.0, [1,2,3,4,5,6,7,8,9,10])
    > print(filtered_x)

The principles are the same for all other APIs.

## FFT 

For Fourier transforms there are no init functions in the CMSIS-DSP for the instance variables. They must be initialized from a C struct. To make it simpler to use them from Python, the wrapper is introducing its own init functions.

Here is an example for using FFT from the Python interface:

Let's define a signal you will use for the FFT.

    > nb = 16
    > signal = np.cos(2 * np.pi * np.arange(nb) / nb)

The CMSIS-DSP cfft is requiring complex signals with a specific layout in memory.

To remain as close as possible to the C API, we are not using complex numbers in the wrapper. So a complex signal must be converted into a real one. The function imToReal1D is defined in testdsp.py 

    > signalR = imToReal1D(signal)

Then, you create the FFT instance with:

    > cfftf32=dsp.arm_cfft_instance_f32()

You initialize the instance with the init function provided by the wrapper:

    > status=dsp.arm_cfft_init_f32(cfftf32, nb)
    > print(status)

You compute the FFT of the signal with:

    > resultR = dsp.arm_cfft_f32(cfftf32,signalR,0,1)

You convert back to a complex format to compare with scipy:

    > resultI = realToIm1D(resultR)
    > print(resultI)

## Matrix 

For matrix, the instance variables are masked by the Python API. We decided that for matrix only there was no use for having the CMSIS-DSP instance visibles since they contain the same information as the numpy array (samples and dimension).

So to use a CMSIS-DSP matrix function, it is very simple:

    > a=np.array([[1.,2,3,4],[5,6,7,8],[9,10,11,12]])
    > b=np.array([[1.,2,3],[5.1,6,7],[9.1,10,11],[5,8,4]])

Numpy result as reference:

    > print(np.dot(a , b))

CMSIS-DSP result:

    > v=dsp.arm_mat_mult_f32(a,b)
    > print(v)

In a real C code, a pointer to a data structure for the result v would have to be passed as argument of the function.

## example.py

This example depends on a data file which can be downloaded here:

https://www.physionet.org/pn3/ecgiddb/Person_87/rec_2.dat

This signal was created for a master thesis:

Lugovaya T.S. Biometric human identification based on electrocardiogram. [Master's thesis] Faculty of Computing Technologies and Informatics, Electrotechnical University "LETI", Saint-Petersburg, Russian Federation; June 2005. 

and it is part of the PhysioNet database

Goldberger AL, Amaral LAN, Glass L, Hausdorff JM, Ivanov PCh, Mark RG, Mietus JE, Moody GB, Peng C-K, Stanley HE. PhysioBank, PhysioToolkit, and PhysioNet: Components of a New Research Resource for Complex Physiologic Signals. Circulation 101(23):e215-e220 [Circulation Electronic Pages; http://circ.ahajournals.org/cgi/content/full/101/23/e215]; 2000 (June 13). 


# LIMITATIONS

Due to the high number of functions in the CMSIS-DSP, the first version of the wrapper was generated automatically from a custom script.

Only a subset of the functions has been tested.

It is likely that some problems are present. The API is quite regular in CMSIS-DSP but there are a few exceptions and the generation script is not managing all of them.

So, the API may crash due to unallocated variables or wrong data conversions.

The generated C code is a first version for bootstrapping the process. Now that this C file exists, the improvements will be done on the C code rather than on the generation script.
