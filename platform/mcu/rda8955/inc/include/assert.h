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

#ifndef _ASSERT_H_
#define _ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#undef assert
#endif

#define STRINGIFY_VALUE(s) STRINGIFY(s)
#define STRINGIFY(s) #s

void __assert(const char *fmt);

#define assert(boolcondition)             \
    do                                    \
    {                                     \
        if (!(boolcondition))             \
            __assert("ASSERTION FAILED"); \
    } while (0)

#ifdef __cplusplus
} // extern C
#endif

#endif /* _ASSERT_H_ */
