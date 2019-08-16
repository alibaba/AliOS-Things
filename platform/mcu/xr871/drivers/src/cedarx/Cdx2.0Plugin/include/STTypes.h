/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef STTypes_H
#define STTypes_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef unsigned int    uint;
typedef unsigned long   ulong;

#ifdef __GNUC__
    // In GCC, include soundtouch_config.h made by config scritps
    //#include "soundtouch_config.h"
#endif

#ifndef _WINDEF_
    // if these aren't defined already by Windows headers, define now

    typedef int BOOL;

    #define FALSE   0
    #define TRUE    1

#endif  // _WINDEF_


//namespace soundtouch
//{
    /// Activate these undef's to overrule the possible sampletype 
    /// setting inherited from some other header file:
    //#undef SOUNDTOUCH_INTEGER_SAMPLES
    //#undef SOUNDTOUCH_FLOAT_SAMPLES

    #if !(SOUNDTOUCH_INTEGER_SAMPLES || SOUNDTOUCH_FLOAT_SAMPLES)
       
        /// Choose either 32bit floating point or 16bit integer sampletype
        /// by choosing one of the following defines, unless this selection 
        /// has already been done in some other file.
        ////
        /// Notes:
        /// - In Windows environment, choose the sample format with the
        ///   following defines.
        /// - In GNU environment, the floating point samples are used by 
        ///   default, but integer samples can be chosen by giving the 
        ///   following switch to the configure script:
        ///       ./configure --enable-integer-samples
        ///   However, if you still prefer to select the sample format here 
        ///   also in GNU environment, then please #undef the INTEGER_SAMPLE
        ///   and FLOAT_SAMPLE defines first as in comments above.
        #define SOUNDTOUCH_INTEGER_SAMPLES     1    //< 16bit integer samples
        //#define SOUNDTOUCH_FLOAT_SAMPLES       1    //< 32bit float samples
     
    #endif

    #if (WIN32 || __i386__ || __x86_64__)
        /// Define this to allow X86-specific assembler/intrinsic optimizations. 
        /// Notice that library contains also usual C++ versions of each of these
        /// these routines, so if you're having difficulties getting the optimized 
        /// routines compiled for whatever reason, you may disable these optimizations 
        /// to make the library compile.

        #define SOUNDTOUCH_ALLOW_X86_OPTIMIZATIONS     1

    #endif

    // If defined, allows the SIMD-optimized routines to take minor shortcuts 
    // for improved performance. Undefine to require faithfully similar SIMD 
    // calculations as in normal C implementation.
    #define SOUNDTOUCH_ALLOW_NONEXACT_SIMD_OPTIMIZATION    1


    #ifdef SOUNDTOUCH_INTEGER_SAMPLES
        // 16bit integer sample type
        typedef short SAMPLETYPE;
        // data type for sample accumulation: Use 32bit integer to prevent overflows
        typedef long  LONG_SAMPLETYPE;

        #ifdef SOUNDTOUCH_FLOAT_SAMPLES
            // check that only one sample type is defined
            #error "conflicting sample types defined"
        #endif // SOUNDTOUCH_FLOAT_SAMPLES

        #ifdef SOUNDTOUCH_ALLOW_X86_OPTIMIZATIONS
            // Allow MMX optimizations
            #define SOUNDTOUCH_ALLOW_MMX   1
        #endif

    #else

        // floating point samples
        typedef float  SAMPLETYPE;
        // data type for sample accumulation: Use double to utilize full precision.
        typedef double LONG_SAMPLETYPE;

        #ifdef SOUNDTOUCH_ALLOW_X86_OPTIMIZATIONS
            // Allow SSE optimizations
            #define SOUNDTOUCH_ALLOW_SSE       1
        #endif

    #endif  // SOUNDTOUCH_INTEGER_SAMPLES
//};


// When this #define is active, eliminates a clicking sound when the "rate" or "pitch" 
// parameter setting crosses from value <1 to >=1 or vice versa during processing. 
// Default is off as such crossover is untypical case and involves a slight sound 
// quality compromise.
//#define SOUNDTOUCH_PREVENT_CLICK_AT_RATE_CROSSOVER   1
#ifdef  __cplusplus
}

#endif

#endif
