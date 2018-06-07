/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/










#include "cs_types.h"

// *************************************
// coefficients of C/I measuring filter for TCH AFS in Q16
// *************************************
const INT16 AFSfilterCI[101] =
{
    1794,1764,1732,1686,1656,1638,1604,1568,1530,1498,1468,1444,1414,1380,1358,1332,
    1306,1282,1260,1230,1196,1168,1142,1124,1102,1076,1058,1026,996,976,960,932,906,
    878,854,834,810,790,772,748,722,700,676,654,628,608,596,578,560,542,524,514,496,
    478,454,432,412,394,384,370,354,334,322,310,304,290,278,260,252,236,230,204,190,
    176,152,134,118,108,90,78,64,48,34,20,4,-12,-18,-22,-28,-42,-52,-68,-70,-80,-96,
    -106,-120,-128,-136,-152,-166
};

// *************************************
// coefficients of C/I measuring filter for TCH AHS in Q16
// *************************************
const INT16 AHSfilterCI[51] =
{
    2620,2558,2478,2392,2302,2232,2160,2112,2042,1990,1908,1856,1806,1758,1694,1654,
    1600,1552,1492,1438,1388,1332,1260,1204,1172,1122,1072,1012,966,938,898,850,828,
    800,774,744,708,638,602,548,518,490,446,424,402,366,350,324,286,264,230
};

// muted compressed frames to send before the first valid frame
const UINT32 MutedFrame_AMR[2]= {0x16c1e364, 0x00000000};
const UINT32 MutedFrame_HR[4]= {0x97712c80, 0xffffffff, 0xffffffff, 0x2d41ffff};
const UINT32 MutedFrame_EFR[8]=
{
    0x9c4bfcdb, 0xffff600d, 0xc000001f, 0x007ffffd, 0xfff00000, 0x00000fff, 0x3ff3fff0, 0x2fa00000
};
const UINT32 MutedFrame_FR[9]=
{
    0xd6174820, 0x0000284, 0x80000000, 0x00000002, 0x2800000, 0x00000000, 0x0028000, 0x00000000, 0x1d3f0000
};

// EGPRS MCS data block size in bytes
const UINT8 spcSizeFromMCS[13]= {23,34,40,54,22,28,37,44,56,74,56,68,74};
