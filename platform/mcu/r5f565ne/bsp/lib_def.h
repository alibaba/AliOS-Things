/*
*********************************************************************************************************
*                                                uC/LIB
*                                        CUSTOM LIBRARY MODULES
*
*                         (c) Copyright 2004-2014; Micrium, Inc.; Weston, FL
*
*                  All rights reserved.  Protected by international copyright laws.
*
*                  uC/LIB is provided in source form to registered licensees ONLY.  It is
*                  illegal to distribute this source code to any third party unless you receive
*                  written permission by an authorized Micrium representative.  Knowledge of
*                  the source code may NOT be used to develop a similar product.
*
*                  Please help us continue to provide the Embedded community with the finest
*                  software available.  Your honesty is greatly appreciated.
*
*                  You can find our product's user manual, API reference, release notes and
*                  more information at: https://doc.micrium.com
*
*                  You can contact us at: http://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     CORE CUSTOM LIBRARY MODULE
*
* Filename      : lib_def.h
* Version       : V1.38.01
* Programmer(s) : ITJ
*                 FBJ
*                 JFD
*********************************************************************************************************
* Note(s)       : (1) Assumes the following versions (or more recent) of software modules are included in
*                     the project build :
*
*                     (a) uC/CPU V1.29.00
*
*
*                 (2) NO compiler-supplied standard library functions are used in library or product software.
*
*                     (a) ALL standard library functions are implemented in the custom library modules :
*
*                         (1) \<Custom Library Directory>\lib_*.*
*
*                         (2) \<Custom Library Directory>\Ports\<cpu>\<compiler>\lib*_a.*
*
*                               where
*                                       <Custom Library Directory>      directory path for custom library software
*                                       <cpu>                           directory name for specific processor (CPU)
*                                       <compiler>                      directory name for specific compiler
*
*                     (b) Product-specific library functions are implemented in individual products.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This library definition header file is protected from multiple pre-processor inclusion
*               through use of the library definition module present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  LIB_DEF_MODULE_PRESENT
#define  LIB_DEF_MODULE_PRESENT

#include "k_types.h"
/*
*********************************************************************************************************
*                                CUSTOM LIBRARY MODULE VERSION NUMBER
*
* Note(s) : (1) (a) The custom library module software version is denoted as follows :
*
*                       Vx.yy.zz
*
*                           where
*                                   V               denotes 'Version' label
*                                   x               denotes     major software version revision number
*                                   yy              denotes     minor software version revision number
*                                   zz              denotes sub-minor software version revision number
*
*               (b) The software version label #define is formatted as follows :
*
*                       ver = x.yyzz * 100 * 100
*
*                           where
*                                   ver             denotes software version number scaled as an integer value
*                                   x.yyzz          denotes software version number, where the unscaled integer
*                                                       portion denotes the major version number & the unscaled
*                                                       fractional portion denotes the (concatenated) minor
*                                                       version numbers
*********************************************************************************************************
*/

#define  LIB_VERSION                                   13801u   /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*
* Note(s) : (1) The custom library software files are located in the following directories :
*
*               (a) \<Custom Library Directory>\lib_*.*
*
*                       where
*                               <Custom Library Directory>      directory path for custom library software
*
*           (2) CPU-configuration  software files are located in the following directories :
*
*               (a) \<CPU-Compiler Directory>\cpu_*.*
*               (b) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
*
*                       where
*                               <CPU-Compiler Directory>        directory path for common CPU-compiler software
*                               <cpu>                           directory name for specific processor (CPU)
*                               <compiler>                      directory name for specific compiler
*
*           (3) Compiler MUST be configured to include as additional include path directories :
*
*               (a) '\<Custom Library Directory>\' directory                            See Note #1a
*
*               (b) (1) '\<CPU-Compiler Directory>\'                  directory         See Note #2a
*                   (2) '\<CPU-Compiler Directory>\<cpu>\<compiler>\' directory         See Note #2b
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          STANDARD DEFINES
*********************************************************************************************************
*/

#define  DEF_NULL                                          0


                                                                /* ----------------- BOOLEAN DEFINES ------------------ */
#define  DEF_FALSE                                         0u
#define  DEF_TRUE                                          1u

#define  DEF_NO                                            0u
#define  DEF_YES                                           1u

#define  DEF_DISABLED                                      0u
#define  DEF_ENABLED                                       1u

#define  DEF_INACTIVE                                      0u
#define  DEF_ACTIVE                                        1u

#define  DEF_INVALID                                       0u
#define  DEF_VALID                                         1u

#define  DEF_OFF                                           0u
#define  DEF_ON                                            1u

#define  DEF_CLR                                           0u
#define  DEF_SET                                           1u

#define  DEF_FAIL                                          0u
#define  DEF_OK                                            1u


                                                                /* ------------------- BIT DEFINES -------------------- */
#define  DEF_BIT_NONE                                   0x00u

#define  DEF_BIT_00                                     0x01u
#define  DEF_BIT_01                                     0x02u
#define  DEF_BIT_02                                     0x04u
#define  DEF_BIT_03                                     0x08u
#define  DEF_BIT_04                                     0x10u
#define  DEF_BIT_05                                     0x20u
#define  DEF_BIT_06                                     0x40u
#define  DEF_BIT_07                                     0x80u

#define  DEF_BIT_08                                   0x0100u
#define  DEF_BIT_09                                   0x0200u
#define  DEF_BIT_10                                   0x0400u
#define  DEF_BIT_11                                   0x0800u
#define  DEF_BIT_12                                   0x1000u
#define  DEF_BIT_13                                   0x2000u
#define  DEF_BIT_14                                   0x4000u
#define  DEF_BIT_15                                   0x8000u

#define  DEF_BIT_16                               0x00010000u
#define  DEF_BIT_17                               0x00020000u
#define  DEF_BIT_18                               0x00040000u
#define  DEF_BIT_19                               0x00080000u
#define  DEF_BIT_20                               0x00100000u
#define  DEF_BIT_21                               0x00200000u
#define  DEF_BIT_22                               0x00400000u
#define  DEF_BIT_23                               0x00800000u

#define  DEF_BIT_24                               0x01000000u
#define  DEF_BIT_25                               0x02000000u
#define  DEF_BIT_26                               0x04000000u
#define  DEF_BIT_27                               0x08000000u
#define  DEF_BIT_28                               0x10000000u
#define  DEF_BIT_29                               0x20000000u
#define  DEF_BIT_30                               0x40000000u
#define  DEF_BIT_31                               0x80000000u
#define  DEF_BIT_32                       0x0000000100000000u
#define  DEF_BIT_33                       0x0000000200000000u
#define  DEF_BIT_34                       0x0000000400000000u
#define  DEF_BIT_35                       0x0000000800000000u
#define  DEF_BIT_36                       0x0000001000000000u
#define  DEF_BIT_37                       0x0000002000000000u
#define  DEF_BIT_38                       0x0000004000000000u
#define  DEF_BIT_39                       0x0000008000000000u

#define  DEF_BIT_40                       0x0000010000000000u
#define  DEF_BIT_41                       0x0000020000000000u
#define  DEF_BIT_42                       0x0000040000000000u
#define  DEF_BIT_43                       0x0000080000000000u
#define  DEF_BIT_44                       0x0000100000000000u
#define  DEF_BIT_45                       0x0000200000000000u
#define  DEF_BIT_46                       0x0000400000000000u
#define  DEF_BIT_47                       0x0000800000000000u

#define  DEF_BIT_48                       0x0001000000000000u
#define  DEF_BIT_49                       0x0002000000000000u
#define  DEF_BIT_50                       0x0004000000000000u
#define  DEF_BIT_51                       0x0008000000000000u
#define  DEF_BIT_52                       0x0010000000000000u
#define  DEF_BIT_53                       0x0020000000000000u
#define  DEF_BIT_54                       0x0040000000000000u
#define  DEF_BIT_55                       0x0080000000000000u

#define  DEF_BIT_56                       0x0100000000000000u
#define  DEF_BIT_57                       0x0200000000000000u
#define  DEF_BIT_58                       0x0400000000000000u
#define  DEF_BIT_59                       0x0800000000000000u
#define  DEF_BIT_60                       0x1000000000000000u
#define  DEF_BIT_61                       0x2000000000000000u
#define  DEF_BIT_62                       0x4000000000000000u
#define  DEF_BIT_63                       0x8000000000000000u


                                                                /* ------------------ ALIGN DEFINES ------------------- */
#define  DEF_ALIGN_MAX_NBR_OCTETS                       4096u


                                                                /* ------------------ OCTET DEFINES ------------------- */
#define  DEF_OCTET_NBR_BITS                                8u
#define  DEF_OCTET_MASK                                 0xFFu

#define  DEF_OCTET_TO_BIT_NBR_BITS                         3u
#define  DEF_OCTET_TO_BIT_SHIFT                          DEF_OCTET_TO_BIT_NBR_BITS
#define  DEF_OCTET_TO_BIT_MASK                          0x07u


#define  DEF_NIBBLE_NBR_BITS                               4u
#define  DEF_NIBBLE_MASK                                0x0Fu


                                                                /* --------------- NUMBER BASE DEFINES ---------------- */
#define  DEF_NBR_BASE_BIN                                  2u
#define  DEF_NBR_BASE_OCT                                  8u
#define  DEF_NBR_BASE_DEC                                 10u
#define  DEF_NBR_BASE_HEX                                 16u


                                                                /* ----------------- INTEGER DEFINES ------------------ */
#define  DEF_INT_08_NBR_BITS                               8u
#define  DEF_INT_08_MASK                                0xFFu

#define  DEF_INT_08U_MIN_VAL                               0u
#define  DEF_INT_08U_MAX_VAL                             255u

#define  DEF_INT_08S_MIN_VAL_ONES_CPL                  (-127)
#define  DEF_INT_08S_MAX_VAL_ONES_CPL                    127

#define  DEF_INT_08S_MIN_VAL                            (DEF_INT_08S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_08S_MAX_VAL                             DEF_INT_08S_MAX_VAL_ONES_CPL

#define  DEF_INT_08U_NBR_DIG_MIN                           1u
#define  DEF_INT_08U_NBR_DIG_MAX                           3u

#define  DEF_INT_08S_NBR_DIG_MIN                           3u
#define  DEF_INT_08S_NBR_DIG_MAX                           3u



#define  DEF_INT_16_NBR_BITS                              16u
#define  DEF_INT_16_MASK                              0xFFFFu

#define  DEF_INT_16U_MIN_VAL                               0u
#define  DEF_INT_16U_MAX_VAL                           65535u

#define  DEF_INT_16S_MIN_VAL_ONES_CPL                (-32767)
#define  DEF_INT_16S_MAX_VAL_ONES_CPL                  32767

#define  DEF_INT_16S_MIN_VAL                            (DEF_INT_16S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_16S_MAX_VAL                             DEF_INT_16S_MAX_VAL_ONES_CPL

#define  DEF_INT_16U_NBR_DIG_MIN                           1u
#define  DEF_INT_16U_NBR_DIG_MAX                           5u

#define  DEF_INT_16S_NBR_DIG_MIN                           5u
#define  DEF_INT_16S_NBR_DIG_MAX                           5u



#define  DEF_INT_32_NBR_BITS                              32u
#define  DEF_INT_32_MASK                          0xFFFFFFFFu

#define  DEF_INT_32U_MIN_VAL                               0u
#define  DEF_INT_32U_MAX_VAL                      4294967295u

#define  DEF_INT_32S_MIN_VAL_ONES_CPL           (-2147483647)
#define  DEF_INT_32S_MAX_VAL_ONES_CPL             2147483647

#define  DEF_INT_32S_MIN_VAL                            (DEF_INT_32S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_32S_MAX_VAL                             DEF_INT_32S_MAX_VAL_ONES_CPL

#define  DEF_INT_32U_NBR_DIG_MIN                           1u
#define  DEF_INT_32U_NBR_DIG_MAX                          10u

#define  DEF_INT_32S_NBR_DIG_MIN                          10u
#define  DEF_INT_32S_NBR_DIG_MAX                          10u



#define  DEF_INT_64_NBR_BITS                              64u
#define  DEF_INT_64_MASK                  0xFFFFFFFFFFFFFFFFu

#define  DEF_INT_64U_MIN_VAL                               0u
#define  DEF_INT_64U_MAX_VAL            18446744073709551615u

#define  DEF_INT_64S_MIN_VAL_ONES_CPL  (-9223372036854775807)
#define  DEF_INT_64S_MAX_VAL_ONES_CPL    9223372036854775807

#define  DEF_INT_64S_MIN_VAL                            (DEF_INT_64S_MIN_VAL_ONES_CPL - 1)
#define  DEF_INT_64S_MAX_VAL                             DEF_INT_64S_MAX_VAL_ONES_CPL

#define  DEF_INT_64U_NBR_DIG_MIN                           1u
#define  DEF_INT_64U_NBR_DIG_MAX                          20u

#define  DEF_INT_64S_NBR_DIG_MIN                          19u
#define  DEF_INT_64S_NBR_DIG_MAX                          19u


                                                                /* --------------- CPU INTEGER DEFINES ---------------- */
#define  DEF_INT_CPU_NBR_BITS                           (CPU_CFG_DATA_SIZE     * DEF_OCTET_NBR_BITS)
#define  DEF_INT_CPU_NBR_BITS_MAX                       (CPU_CFG_DATA_SIZE_MAX * DEF_OCTET_NBR_BITS)



#if     (DEF_INT_CPU_NBR_BITS == DEF_INT_08_NBR_BITS)


#define  DEF_INT_CPU_MASK                                DEF_INT_08_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_08U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_08U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_08S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_08S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_08S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_08S_MAX_VAL_ONES_CPL



#elif   (DEF_INT_CPU_NBR_BITS == DEF_INT_16_NBR_BITS)


#define  DEF_INT_CPU_MASK                                DEF_INT_16_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_16U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_16U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_16S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_16S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_16S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_16S_MAX_VAL_ONES_CPL



#elif   (DEF_INT_CPU_NBR_BITS == DEF_INT_32_NBR_BITS)


#define  DEF_INT_CPU_MASK                                DEF_INT_32_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_32U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_32U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_32S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_32S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_32S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_32S_MAX_VAL_ONES_CPL



#elif   (DEF_INT_CPU_NBR_BITS == DEF_INT_64_NBR_BITS)


#define  DEF_INT_CPU_MASK                                DEF_INT_64_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_64U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_64U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_64S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_64S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_64S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_64S_MAX_VAL_ONES_CPL



#else

#error  "CPU_CFG_DATA_SIZE  illegally #defined in 'cpu.h'      "
#error  "                   [See 'cpu.h  CONFIGURATION ERRORS']"

#endif


                                                                /* ------------------- TIME DEFINES ------------------- */
#define  DEF_TIME_NBR_DAY_PER_WK                           7u
#define  DEF_TIME_NBR_DAY_PER_YR                         365u
#define  DEF_TIME_NBR_DAY_PER_YR_LEAP                    366u

#define  DEF_TIME_NBR_HR_PER_DAY                          24u
#define  DEF_TIME_NBR_HR_PER_WK                         (DEF_TIME_NBR_HR_PER_DAY  * DEF_TIME_NBR_DAY_PER_WK     )
#define  DEF_TIME_NBR_HR_PER_YR                         (DEF_TIME_NBR_HR_PER_DAY  * DEF_TIME_NBR_DAY_PER_YR     )
#define  DEF_TIME_NBR_HR_PER_YR_LEAP                    (DEF_TIME_NBR_HR_PER_DAY  * DEF_TIME_NBR_DAY_PER_YR_LEAP)

#define  DEF_TIME_NBR_MIN_PER_HR                          60u
#define  DEF_TIME_NBR_MIN_PER_DAY                       (DEF_TIME_NBR_MIN_PER_HR  * DEF_TIME_NBR_HR_PER_DAY     )
#define  DEF_TIME_NBR_MIN_PER_WK                        (DEF_TIME_NBR_MIN_PER_DAY * DEF_TIME_NBR_DAY_PER_WK     )
#define  DEF_TIME_NBR_MIN_PER_YR                        (DEF_TIME_NBR_MIN_PER_DAY * DEF_TIME_NBR_DAY_PER_YR     )
#define  DEF_TIME_NBR_MIN_PER_YR_LEAP                   (DEF_TIME_NBR_MIN_PER_DAY * DEF_TIME_NBR_DAY_PER_YR_LEAP)

#define  DEF_TIME_NBR_SEC_PER_MIN                         60u
#define  DEF_TIME_NBR_SEC_PER_HR                        (DEF_TIME_NBR_SEC_PER_MIN * DEF_TIME_NBR_MIN_PER_HR     )
#define  DEF_TIME_NBR_SEC_PER_DAY                       (DEF_TIME_NBR_SEC_PER_HR  * DEF_TIME_NBR_HR_PER_DAY     )
#define  DEF_TIME_NBR_SEC_PER_WK                        (DEF_TIME_NBR_SEC_PER_DAY * DEF_TIME_NBR_DAY_PER_WK     )
#define  DEF_TIME_NBR_SEC_PER_YR                        (DEF_TIME_NBR_SEC_PER_DAY * DEF_TIME_NBR_DAY_PER_YR     )
#define  DEF_TIME_NBR_SEC_PER_YR_LEAP                   (DEF_TIME_NBR_SEC_PER_DAY * DEF_TIME_NBR_DAY_PER_YR_LEAP)

#define  DEF_TIME_NBR_mS_PER_SEC                        1000u
#define  DEF_TIME_NBR_uS_PER_SEC                     1000000u
#define  DEF_TIME_NBR_nS_PER_SEC                  1000000000u


/*
*********************************************************************************************************
*                                             ERROR CODES
*
* Note(s) : (1) All library error codes are #define'd in 'lib_def.h';
*********************************************************************************************************
*/

typedef enum lib_err {

    LIB_ERR_NONE                            =         0u,

    LIB_MEM_ERR_NONE                        =     10000u,
    LIB_MEM_ERR_NULL_PTR                    =     10001u,       /* Ptr arg(s) passed NULL ptr(s).                       */

    LIB_MEM_ERR_INVALID_MEM_SIZE            =     10100u,       /* Invalid mem     size.                                */
    LIB_MEM_ERR_INVALID_MEM_ALIGN           =     10101u,       /* Invalid mem     align.                               */
    LIB_MEM_ERR_INVALID_SEG_SIZE            =     10110u,       /* Invalid mem seg size.                                */
    LIB_MEM_ERR_INVALID_SEG_OVERLAP         =     10111u,       /* Invalid mem seg overlaps other mem seg(s).           */
    LIB_MEM_ERR_INVALID_SEG_EXISTS          =     10112u,       /* Invalid mem seg already exists.                      */
    LIB_MEM_ERR_INVALID_POOL                =     10120u,       /* Invalid mem pool.                                    */
    LIB_MEM_ERR_INVALID_BLK_NBR             =     10130u,       /* Invalid mem pool blk nbr.                            */
    LIB_MEM_ERR_INVALID_BLK_SIZE            =     10131u,       /* Invalid mem pool blk size.                           */
    LIB_MEM_ERR_INVALID_BLK_ALIGN           =     10132u,       /* Invalid mem pool blk align.                          */
    LIB_MEM_ERR_INVALID_BLK_IX              =     10133u,       /* Invalid mem pool ix.                                 */
    LIB_MEM_ERR_INVALID_BLK_ADDR            =     10135u,       /* Invalid mem pool blk addr.                           */
    LIB_MEM_ERR_INVALID_BLK_ADDR_IN_POOL    =     10136u,       /* Mem pool blk addr already in mem pool.               */

    LIB_MEM_ERR_SEG_EMPTY                   =     10200u,       /* Mem seg  empty; i.e. NO avail mem in seg.            */
    LIB_MEM_ERR_SEG_OVF                     =     10201u,       /* Mem seg  ovf;   i.e. req'd mem ovfs rem mem in seg.  */
    LIB_MEM_ERR_POOL_FULL                   =     10205u,       /* Mem pool full;  i.e. all mem blks avail in mem pool. */
    LIB_MEM_ERR_POOL_EMPTY                  =     10206u,       /* Mem pool empty; i.e. NO  mem blks avail in mem pool. */
    LIB_MEM_ERR_POOL_UNLIMITED              =     10207u,       /* Mem pool is unlimited.                               */

    LIB_MEM_ERR_HEAP_EMPTY                  =     10210u,       /* Heap seg empty; i.e. NO avail mem in heap.           */
    LIB_MEM_ERR_HEAP_OVF                    =     10211u,       /* Heap seg ovf;   i.e. req'd mem ovfs rem mem in heap. */
    LIB_MEM_ERR_HEAP_NOT_FOUND              =     10215u        /* Heap seg NOT found.                                  */

} LIB_ERR;


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               TRACING
*********************************************************************************************************
*/

                                                                /* Trace level, default to TRACE_LEVEL_OFF.             */
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                                   0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                                  1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                                   2u
#endif

#ifndef  TRACE_LEVEL_LOG
#define  TRACE_LEVEL_LOG                                   3u
#endif


/*
*********************************************************************************************************
*                                             BIT MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              DEF_BIT()
*
* Description : Create bit mask with single, specified bit set.
*
* Argument(s) : bit         Bit number of bit to set.
*
* Return(s)   : Bit mask with single, specified bit set.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'bit' SHOULD be a non-negative integer.
*
*               (2) (a) 'bit' values that overflow the target CPU &/or compiler environment (e.g. negative
*                       or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
*********************************************************************************************************
*/

#define  DEF_BIT(bit)                                                   (1u << (bit))


/*
*********************************************************************************************************
*                                             DEF_BITxx()
*
* Description : Create bit mask of specified bit size with single, specified bit set.
*
* Argument(s) : bit         Bit number of bit to set.
*
* Return(s)   : Bit mask with single, specified bit set.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'bit' SHOULD be a non-negative integer.
*
*               (2) (a) 'bit' values that overflow the target CPU &/or compiler environment (e.g. negative
*                       or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
*
*                   (b) To avoid overflowing any target CPU &/or compiler's integer data type, unsigned
*                       bit constant '1' is cast to specified integer data type size.
*
*               (3) Ideally, DEF_BITxx() macro's should be named DEF_BIT_xx(); however, these names already
*                   previously-released for bit constant #define's (see 'STANDARD DEFINES  BIT DEFINES').
*********************************************************************************************************
*/

#define  DEF_BIT08(bit)                        ((CPU_INT08U)((CPU_INT08U)1u  << (bit)))

#define  DEF_BIT16(bit)                        ((CPU_INT16U)((CPU_INT16U)1u  << (bit)))

#define  DEF_BIT32(bit)                        ((CPU_INT32U)((CPU_INT32U)1u  << (bit)))

#define  DEF_BIT64(bit)                        ((CPU_INT64U)((CPU_INT64U)1u  << (bit)))


/*
*********************************************************************************************************
*                                           DEF_BIT_MASK()
*
* Description : Shift a bit mask.
*
* Argument(s) : bit_mask    Bit mask to shift.
*
*               bit_shift   Number of bit positions to left-shift bit mask.
*
* Return(s)   : Shifted bit mask.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) (a) 'bit_mask'  SHOULD be an unsigned    integer.
*
*                   (b) 'bit_shift' SHOULD be a non-negative integer.
*
*               (2) 'bit_shift' values that overflow the target CPU &/or compiler environment (e.g. negative
*                   or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
*********************************************************************************************************
*/

#define  DEF_BIT_MASK(bit_mask, bit_shift)                                     ((bit_mask) << (bit_shift))


/*
*********************************************************************************************************
*                                          DEF_BIT_MASK_xx()
*
* Description : Shift a bit mask of specified bit size.
*
* Argument(s) : bit_mask    Bit mask to shift.
*
*               bit_shift   Number of bit positions to left-shift bit mask.
*
* Return(s)   : Shifted bit mask.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) (a) 'bit_mask'  SHOULD be an unsigned    integer.
*
*                   (b) 'bit_shift' SHOULD be a non-negative integer.
*
*               (2) 'bit_shift' values that overflow the target CPU &/or compiler environment (e.g. negative
*                   or greater-than-CPU-data-size values) MAY generate compiler warnings &/or errors.
*********************************************************************************************************
*/

#define  DEF_BIT_MASK_08(bit_mask, bit_shift)         ((CPU_INT08U)((CPU_INT08U)(bit_mask) << (bit_shift)))

#define  DEF_BIT_MASK_16(bit_mask, bit_shift)         ((CPU_INT16U)((CPU_INT16U)(bit_mask) << (bit_shift)))

#define  DEF_BIT_MASK_32(bit_mask, bit_shift)         ((CPU_INT32U)((CPU_INT32U)(bit_mask) << (bit_shift)))

#define  DEF_BIT_MASK_64(bit_mask, bit_shift)         ((CPU_INT64U)((CPU_INT64U)(bit_mask) << (bit_shift)))


/*
*********************************************************************************************************
*                                           DEF_BIT_FIELD()
*
* Description : Create & shift a contiguous bit field.
*
* Argument(s) : bit_field   Number of contiguous bits to set in the bit field.
*
*               bit_shift   Number of bit positions   to left-shift bit field.
*
* Return(s)   : Shifted bit field.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'bit_field' & 'bit_shift' SHOULD be non-negative integers.
*
*               (2) (a) 'bit_field'/'bit_shift' values that overflow the target CPU &/or compiler
*                       environment (e.g. negative or greater-than-CPU-data-size values) MAY generate
*                       compiler warnings &/or errors.
*
*                   (b) To avoid overflowing any target CPU &/or compiler's integer data type, unsigned
*                       bit constant '1' is suffixed with 'L'ong integer modifier.
*
*                       This may still be insufficient for CPUs &/or compilers that support 'long long'
*                       integer data types, in which case 'LL' integer modifier should be suffixed.
*                       However, since almost all 16- & 32-bit CPUs & compilers support 'long' integer
*                       data types but many may NOT support 'long long' integer data types, only 'long'
*                       integer data types & modifiers are supported.
*
*                       See also 'DEF_BIT_FIELD_xx()  Note #1b'.
*********************************************************************************************************
*/

#define  DEF_BIT_FIELD(bit_field, bit_shift)                                 ((((bit_field) >= DEF_INT_CPU_NBR_BITS) ? (DEF_INT_CPU_U_MAX_VAL)     \
                                                                                                                     : (DEF_BIT(bit_field) - 1uL)) \
                                                                                                                            << (bit_shift))

/*
*********************************************************************************************************
*                                         DEF_BIT_FIELD_xx()
*
* Description : Create & shift a contiguous bit field of specified bit size.
*
* Argument(s) : bit_field   Number of contiguous bits to set in the bit field.
*
*               bit_shift   Number of bit positions   to left-shift bit field.
*
* Return(s)   : Shifted bit field.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'bit_field' & 'bit_shift' SHOULD be non-negative integers.
*
*               (2) (a) 'bit_field'/'bit_shift' values that overflow the target CPU &/or compiler
*                       environment (e.g. negative or greater-than-CPU-data-size values) MAY generate
*                       compiler warnings &/or errors.
*
*                   (b) To avoid overflowing any target CPU &/or compiler's integer data type, unsigned
*                       bit constant '1' is cast to specified integer data type size.
*********************************************************************************************************
*/

#define  DEF_BIT_FIELD_08(bit_field, bit_shift)     ((CPU_INT08U)((((CPU_INT08U)(bit_field) >= (CPU_INT08U)DEF_INT_08_NBR_BITS) ? (CPU_INT08U)(DEF_INT_08U_MAX_VAL)                    \
                                                                                                                                : (CPU_INT08U)(DEF_BIT08(bit_field) - (CPU_INT08U)1u)) \
                                                                                                                                                     << (bit_shift)))

#define  DEF_BIT_FIELD_16(bit_field, bit_shift)     ((CPU_INT16U)((((CPU_INT16U)(bit_field) >= (CPU_INT16U)DEF_INT_16_NBR_BITS) ? (CPU_INT16U)(DEF_INT_16U_MAX_VAL)                    \
                                                                                                                                : (CPU_INT16U)(DEF_BIT16(bit_field) - (CPU_INT16U)1u)) \
                                                                                                                                                     << (bit_shift)))

#define  DEF_BIT_FIELD_32(bit_field, bit_shift)     ((CPU_INT32U)((((CPU_INT32U)(bit_field) >= (CPU_INT32U)DEF_INT_32_NBR_BITS) ? (CPU_INT32U)(DEF_INT_32U_MAX_VAL)                    \
                                                                                                                                : (CPU_INT32U)(DEF_BIT32(bit_field) - (CPU_INT32U)1u)) \
                                                                                                                                                     << (bit_shift)))

#define  DEF_BIT_FIELD_64(bit_field, bit_shift)     ((CPU_INT64U)((((CPU_INT64U)(bit_field) >= (CPU_INT64U)DEF_INT_64_NBR_BITS) ? (CPU_INT64U)(DEF_INT_64U_MAX_VAL)                    \
                                                                                                                                : (CPU_INT64U)(DEF_BIT64(bit_field) - (CPU_INT64U)1u)) \
                                                                                                                                                     << (bit_shift)))


/*
*********************************************************************************************************
*                                            DEF_BIT_SET()
*
* Description : Set specified bit(s) in a value.
*
* Argument(s) : val         Value to modify by setting specified bit(s).
*
*               mask        Mask of bits to set.
*
* Return(s)   : Modified value with specified bit(s) set.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*********************************************************************************************************
*/

#define  DEF_BIT_SET(val, mask)                        ((val) = ((val) | (mask)))


/*
*********************************************************************************************************
*                                          DEF_BIT_SET_xx()
*
* Description : Set specified bit(s) in a value of specified bit size.
*
* Argument(s) : val         Value to modify by setting specified bit(s).
*
*               mask        Mask of bits to set.
*
* Return(s)   : Modified value with specified bit(s) set.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*
*               (2) These macros are deprecated and should be replaced by the DEF_BIT_SET macro.
*********************************************************************************************************
*/

#define  DEF_BIT_SET_08(val, mask)                     DEF_BIT_SET((val), (mask))

#define  DEF_BIT_SET_16(val, mask)                     DEF_BIT_SET((val), (mask))

#define  DEF_BIT_SET_32(val, mask)                     DEF_BIT_SET((val), (mask))

#define  DEF_BIT_SET_64(val, mask)                     DEF_BIT_SET((val), (mask))


/*
*********************************************************************************************************
*                                            DEF_BIT_CLR()
*
* Description : Clear specified bit(s) in a value.
*
* Argument(s) : val         Value to modify by clearing specified bit(s).
*
*               mask        Mask of bits to clear.
*
* Return(s)   : Modified value with specified bit(s) clear.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*********************************************************************************************************
*/

#define  DEF_BIT_CLR(val, mask)                        ((val) = ((val) & ~(mask)))


/*
*********************************************************************************************************
*                                          DEF_BIT_CLR_xx()
*
* Description : Clear specified bit(s) in a value of specified bit size.
*
* Argument(s) : val         Value to modify by clearing specified bit(s).
*
*               mask        Mask of bits to clear.
*
* Return(s)   : Modified value with specified bit(s) clear.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*
*               (2) These macros are deprecated and should be replaced by the DEF_BIT_CLR macro.
*********************************************************************************************************
*/

#define  DEF_BIT_CLR_08(val, mask)                     DEF_BIT_CLR((val), (mask))

#define  DEF_BIT_CLR_16(val, mask)                     DEF_BIT_CLR((val), (mask))

#define  DEF_BIT_CLR_32(val, mask)                     DEF_BIT_CLR((val), (mask))

#define  DEF_BIT_CLR_64(val, mask)                     DEF_BIT_CLR((val), (mask))


/*
*********************************************************************************************************
*                                            DEF_BIT_TOGGLE()
*
* Description : Toggles specified bit(s) in a value.
*
* Argument(s) : val         Value to modify by toggling specified bit(s).
*
*               mask        Mask of bits to toggle.
*
* Return(s)   : Modified value with specified bit(s) toggled.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*********************************************************************************************************
*/

#define  DEF_BIT_TOGGLE(val, mask)                      ((val) ^= (mask))


/*
*********************************************************************************************************
*                                           DEF_BIT_FIELD_RD()
*
* Description : Reads a 'val' field, masked and shifted, given by mask 'field_mask'.
*
* Argument(s) : val         Value to read from.
*
*               field_mask  Mask of field to read. See note #1, #2 and #3.
*
* Return(s)   : Field value, masked and right-shifted to bit position 0.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'field_mask' argument must NOT be 0.
*
*               (2) 'field_mask' argument must contain a mask with contiguous set bits.
*
*               (3) 'val' & 'field_mask' SHOULD be unsigned integers.
*********************************************************************************************************
*/

#define  DEF_BIT_FIELD_RD(val, field_mask)              (((val) & (field_mask)) / ((field_mask) & ~((field_mask) << 1u)))


/*
*********************************************************************************************************
*                                          DEF_BIT_FIELD_ENC()
*
* Description : Encodes given 'field_val' at position given by mask 'field_mask'.
*
* Argument(s) : field_val   Value to encode.
*
*               field_mask  Mask of field to read. See note #1 and #2.
*
* Return(s)   : Field value, masked and left-shifted to field position.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'field_mask' argument must contain a mask with contiguous set bits.
*
*               (2) 'field_val' & 'field_mask' SHOULD be unsigned integers.
*********************************************************************************************************
*/

#define  DEF_BIT_FIELD_ENC(field_val, field_mask)       (((field_val) * ((field_mask) & ~((field_mask) << 1u))) & (field_mask))


/*
*********************************************************************************************************
*                                           DEF_BIT_FIELD_WR()
*
* Description : Writes 'field_val' field at position given by mask 'field_mask' in variable 'var'.
*
* Argument(s) : var         Variable to write field to. See note #2.
*
*               field_val   Desired value for field. See note #2.
*
*               field_mask  Mask of field to write to. See note #1 and #2.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'field_mask' argument must contain a mask with contiguous set bits.
*
*               (2) 'var', 'field_val' & 'field_mask' SHOULD be unsigned integers.
*********************************************************************************************************
*/

#define  DEF_BIT_FIELD_WR(var, field_val, field_mask)   (var) = (((var) & ~(field_mask)) | DEF_BIT_FIELD_ENC((field_val), (field_mask)))


/*
*********************************************************************************************************
*                                          DEF_BIT_IS_SET()
*
* Description : Determine if specified bit(s) in a value are set.
*
* Argument(s) : val         Value to check for specified bit(s) set.
*
*               mask        Mask of bits to check if set (see Note #2).
*
* Return(s)   : DEF_YES, if ALL specified bit(s) are     set in value.
*
*               DEF_NO,  if ALL specified bit(s) are NOT set in value.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*
*               (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
*********************************************************************************************************
*/

#define  DEF_BIT_IS_SET(val, mask)                    (((((val) & (mask)) == (mask)) && \
                                                         ((mask)          !=  0u))    ? (DEF_YES) : (DEF_NO))


/*
*********************************************************************************************************
*                                          DEF_BIT_IS_CLR()
*
* Description : Determine if specified bit(s) in a value are clear.
*
* Argument(s) : val         Value to check for specified bit(s) clear.
*
*               mask        Mask of bits to check if clear (see Note #2).
*
* Return(s)   : DEF_YES, if ALL specified bit(s) are     clear in value.
*
*               DEF_NO,  if ALL specified bit(s) are NOT clear in value.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*
*               (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
*********************************************************************************************************
*/

#define  DEF_BIT_IS_CLR(val, mask)                    (((((val) & (mask)) ==  0u)  && \
                                                         ((mask)          !=  0u))  ? (DEF_YES) : (DEF_NO))


/*
*********************************************************************************************************
*                                        DEF_BIT_IS_SET_ANY()
*
* Description : Determine if any specified bit(s) in a value are set.
*
* Argument(s) : val         Value to check for specified bit(s) set.
*
*               mask        Mask of bits to check if set (see Note #2).
*
* Return(s)   : DEF_YES, if ANY specified bit(s) are     set in value.
*
*               DEF_NO,  if ALL specified bit(s) are NOT set in value.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*
*               (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
*********************************************************************************************************
*/

#define  DEF_BIT_IS_SET_ANY(val, mask)               ((((val) & (mask)) ==  0u)     ? (DEF_NO ) : (DEF_YES))


/*
*********************************************************************************************************
*                                        DEF_BIT_IS_CLR_ANY()
*
* Description : Determine if any specified bit(s) in a value are clear.
*
* Argument(s) : val         Value to check for specified bit(s) clear.
*
*               mask        Mask of bits to check if clear (see Note #2).
*
* Return(s)   : DEF_YES, if ANY specified bit(s) are     clear in value.
*
*               DEF_NO,  if ALL specified bit(s) are NOT clear in value.
*
* Note(s)     : (1) 'val' & 'mask' SHOULD be unsigned integers.
*
*               (2) NULL 'mask' allowed; returns 'DEF_NO' since NO mask bits specified.
*********************************************************************************************************
*/

#define  DEF_BIT_IS_CLR_ANY(val, mask)               ((((val) & (mask)) == (mask))  ? (DEF_NO ) : (DEF_YES))


/*
*********************************************************************************************************
*                                            VALUE MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          DEF_CHK_VAL_MIN()
*
* Description : Validate a value as greater than or equal to a specified minimum value.
*
* Argument(s) : val        Value to validate.
*
*               val_min    Minimum value to test.
*
* Return(s)   : DEF_OK,    Value is greater than or equal to minimum value.
*
*               DEF_FAIL,  otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) DEF_CHK_VAL_MIN() avoids directly comparing any two values if only one of the values
*                   is negative since the negative value might be incorrectly promoted to an arbitrary
*                   unsigned value if the other value to compare is unsigned.
*
*               (2) Validation of values is limited to the range supported by the compiler &/or target
*                   environment.  All other values that underflow/overflow the supported range will
*                   modulo/wrap into the supported range as arbitrary signed or unsigned values.
*
*                   Therefore, any values that underflow the most negative signed value or overflow
*                   the most positive unsigned value supported by the compiler &/or target environment
*                   cannot be validated :
*
*                           (    N-1       N     ]
*                           ( -(2   )  ,  2  - 1 ]
*                           (                    ]
*
*                               where
*                                       N       Number of data word bits supported by the compiler
*                                                   &/or target environment
*
*                   (a) Note that the most negative value, -2^(N-1), is NOT included in the supported
*                       range since many compilers do NOT always correctly handle this value.
*
*               (3) 'val' and 'val_min' are compared to 1 instead of 0 to avoid warning generated for
*                   unsigned numbers.
*********************************************************************************************************
*/

#define  DEF_CHK_VAL_MIN(val, val_min)            (((!(((val)     >= 1) && ((val_min) < 1))) && \
                                                     ((((val_min) >= 1) && ((val)     < 1))  || \
                                                       ((val) < (val_min)))) ? DEF_FAIL : DEF_OK)


/*
*********************************************************************************************************
*                                          DEF_CHK_VAL_MAX()
*
* Description : Validate a value as less than or equal to a specified maximum value.
*
* Argument(s) : val        Value to validate.
*
*               val_max    Maximum value to test.
*
* Return(s)   : DEF_OK,    Value is less than or equal to maximum value.
*
*               DEF_FAIL,  otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) DEF_CHK_VAL_MAX() avoids directly comparing any two values if only one of the values
*                   is negative since the negative value might be incorrectly promoted to an arbitrary
*                   unsigned value if the other value to compare is unsigned.
*
*               (2) Validation of values is limited to the range supported by the compiler &/or target
*                   environment.  All other values that underflow/overflow the supported range will
*                   modulo/wrap into the supported range as arbitrary signed or unsigned values.
*
*                   Therefore, any values that underflow the most negative signed value or overflow
*                   the most positive unsigned value supported by the compiler &/or target environment
*                   cannot be validated :
*
*                           (    N-1       N     ]
*                           ( -(2   )  ,  2  - 1 ]
*                           (                    ]
*
*                               where
*                                       N       Number of data word bits supported by the compiler
*                                                   &/or target environment
*
*                   (a) Note that the most negative value, -2^(N-1), is NOT included in the supported
*                       range since many compilers do NOT always correctly handle this value.
*
*               (3) 'val' and 'val_max' are compared to 1 instead of 0 to avoid warning generated for
*                   unsigned numbers.
*********************************************************************************************************
*/

#define  DEF_CHK_VAL_MAX(val, val_max)            (((!(((val_max) >= 1) && ((val)     < 1))) && \
                                                     ((((val)     >= 1) && ((val_max) < 1))  || \
                                                       ((val) > (val_max)))) ? DEF_FAIL : DEF_OK)


/*
*********************************************************************************************************
*                                            DEF_CHK_VAL()
*
* Description : Validate a value as greater than or equal to a specified minimum value & less than or
*                   equal to a specified maximum value.
*
* Argument(s) : val        Value to validate.
*
*               val_min    Minimum value to test.
*
*               val_max    Maximum value to test.
*
* Return(s)   : DEF_OK,    Value is greater than or equal to minimum value AND
*                                   less    than or equal to maximum value.
*
*               DEF_FAIL,  otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) DEF_CHK_VAL() avoids directly comparing any two values if only one of the values
*                   is negative since the negative value might be incorrectly promoted to an arbitrary
*                   unsigned value if the other value to compare is unsigned.
*
*               (2) Validation of values is limited to the range supported by the compiler &/or target
*                   environment.  All other values that underflow/overflow the supported range will
*                   modulo/wrap into the supported range as arbitrary signed or unsigned values.
*
*                   Therefore, any values that underflow the most negative signed value or overflow
*                   the most positive unsigned value supported by the compiler &/or target environment
*                   cannot be validated :
*
*                           (    N-1       N     ]
*                           ( -(2   )  ,  2  - 1 ]
*                           (                    ]
*
*                               where
*                                       N       Number of data word bits supported by the compiler
*                                                   &/or target environment
*
*                   (a) Note that the most negative value, -2^(N-1), is NOT included in the supported
*                       range since many compilers do NOT always correctly handle this value.
*
*               (3) DEF_CHK_VAL() does NOT validate that the maximum value ('val_max') is greater than
*                   or equal to the minimum value ('val_min').
*********************************************************************************************************
*/

#define  DEF_CHK_VAL(val, val_min, val_max)          (((DEF_CHK_VAL_MIN((val), (val_min)) == DEF_FAIL) ||                  \
                                                       (DEF_CHK_VAL_MAX((val), (val_max)) == DEF_FAIL)) ? DEF_FAIL : DEF_OK)


/*
*********************************************************************************************************
*                                         DEF_GET_U_MAX_VAL()
*
* Description : Get the maximum unsigned value that can be represented in an unsigned integer variable
*                   of the same data type size as an object.
*
* Argument(s) : obj         Object or data type to return maximum unsigned value (see Note #1).
*
* Return(s)   : Maximum unsigned integer value that can be represented by the object, if NO error(s).
*
*               0,                                                                    otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) 'obj' SHOULD be an integer object or data type but COULD also be a character or
*                   pointer object or data type.
*********************************************************************************************************
*/

#if     (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_08)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL : 0)


#elif   (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_16)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL :   \
                                                ((sizeof(obj) == CPU_WORD_SIZE_16) ? DEF_INT_16U_MAX_VAL : 0))


#elif   (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_32)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL :    \
                                                ((sizeof(obj) == CPU_WORD_SIZE_16) ? DEF_INT_16U_MAX_VAL :    \
                                                ((sizeof(obj) == CPU_WORD_SIZE_32) ? DEF_INT_32U_MAX_VAL : 0)))


#elif   (CPU_CFG_DATA_SIZE_MAX == CPU_WORD_SIZE_64)

#define  DEF_GET_U_MAX_VAL(obj)                 ((sizeof(obj) == CPU_WORD_SIZE_08) ? DEF_INT_08U_MAX_VAL :     \
                                                ((sizeof(obj) == CPU_WORD_SIZE_16) ? DEF_INT_16U_MAX_VAL :     \
                                                ((sizeof(obj) == CPU_WORD_SIZE_32) ? DEF_INT_32U_MAX_VAL :     \
                                                ((sizeof(obj) == CPU_WORD_SIZE_64) ? DEF_INT_64U_MAX_VAL : 0))))

#else

#error  "CPU_CFG_DATA_SIZE_MAX  illegally #defined in 'cpu.h'      "
#error  "                       [See 'cpu.h  CONFIGURATION ERRORS']"

#endif


/*
*********************************************************************************************************
*                                            MATH MACRO'S
*
* Note(s) : (1) Ideally, ALL mathematical macro's & functions SHOULD be defined in the custom mathematics
*               library ('lib_math.*').  #### However, to maintain backwards compatibility with previously-
*               released modules, mathematical macro & function definitions should only be moved to the
*               custom mathematics library once all previously-released modules are updated to include the
*               custom mathematics library.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              DEF_MIN()
*
* Description : Determine the minimum of two values.
*
* Argument(s) : a           First  value.
*
*               b           Second value.
*
* Return(s)   : Minimum of the two values.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

#define  DEF_MIN(a, b)                                  (((a) < (b)) ? (a) : (b))


/*
*********************************************************************************************************
*                                              DEF_MAX()
*
* Description : Determine the maximum of two values.
*
* Argument(s) : a           First  value.
*
*               b           Second value.
*
* Return(s)   : Maximum of the two values.
*
* Note(s)     : none.
*********************************************************************************************************
*/

#define  DEF_MAX(a, b)                                  (((a) > (b)) ? (a) : (b))


/*
*********************************************************************************************************
*                                              DEF_ABS()
*
* Description : Determine the absolute value of a value.
*
* Argument(s) : a           Value to calculate absolute value.
*
* Return(s)   : Absolute value of the value.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

#define  DEF_ABS(a)                                     (((a) < 0) ? (-(a)) : (a))


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                    LIBRARY CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* See 'lib_def.h  Note #1a'.                           */

/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'lib_def.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of lib def module include.                       */

