///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: string_convert.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __STRING_CONVERT_H__
#define __STRING_CONVERT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define UNICODE_TO_GBK     0x0
#define UNICODE_BIG_TO_GBK 0x1
#define UTF8_TO_GBK        0x2
// 用户可在下面继续扩展其他类型的转换类型
#define GBK_TO_UNICODE 0x3

#define UNKNOWN_CONVERT 0xFF

/**********************************************
Convert string to special encode format (GBK, UNICODE, ...)
Param:
    DstText   - Output - converted string
    MaxDstLen - Input  - DstText's max Length(Byte)
    SrcChar   - Input  - String to be converted
    SrcLen    - Input  - SrcChar's length (Byte)
    Type      - Input  - Convert option: UNICODE_TO_GBK, GBK_TO_UNICODE, ...
Return:
    Converted string length, 0 means conversion error
**********************************************/
uint32_t StringConvert(uint8_t* DstText, uint32_t MaxDstLen,
                              uint8_t* SrcText, uint32_t SrcLen, uint32_t Type);

/**********************************************
Convert char to special encode format (GBK, UNICODE, ...)
Param:
    SrcChar - Input - Character code to be converted
    Type    - Input - Convert Option: UNICODE_TO_GBK, GBK_TO_UNICODE, ...
Return:
    Converted code, 0 means conversion error
**********************************************/
uint16_t CharConvert(uint16_t SrcChar, uint32_t Type);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__STRING_CONVERT_H_*/

