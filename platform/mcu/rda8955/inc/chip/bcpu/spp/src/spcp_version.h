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

#ifndef _SPC_VERSION_H_
#define _SPC_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define SPC_VERSION_REVISION                     (0xd4bb65dc)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef SPC_VERSION_NUMBER
#define SPC_VERSION_NUMBER                       (0)
#endif

#ifndef SPC_VERSION_DATE
#define SPC_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef SPC_VERSION_STRING
#define SPC_VERSION_STRING                       "SPC version string not defined"
#endif

#ifndef SPC_VERSION_STRING_WITH_BRANCH
#define SPC_VERSION_STRING_WITH_BRANCH           SPC_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define SPC_VERSION_STRUCT                       {SPC_VERSION_REVISION, \
                                                  SPC_VERSION_NUMBER, \
                                                  SPC_VERSION_DATE, \
                                                  SPC_VERSION_STRING_WITH_BRANCH}

#endif // _SPC_VERSION_H_
