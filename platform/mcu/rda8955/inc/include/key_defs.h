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


#ifndef __KEY_DEFS_H__
#define __KEY_DEFS_H__

#define KP_PWR       'P' // On-off button
#define KP_VD        'D'
#define KP_VU        'U'
#define KP_UP        '^'
#define KP_DW        'V'
#define KP_LT        '<'
#define KP_RT        '>'
#define KP_OK        'M'
#define KP_SND       'S' // if __SENDKEY2_SUPPORT__ was defined, do NOT use KP_SND, use KP_BACK and KP_DEL instead.
#define KP_END       'E'
#define KP_1         '1'
#define KP_2         '2'
#define KP_3         '3'
#define KP_4         '4'
#define KP_5         '5'
#define KP_6         '6'
#define KP_7         '7'
#define KP_8         '8'
#define KP_9         '9'
#define KP_0         '0'
#define KP_STAR      '*'
#define KP_POUND     '#'
#define KP_SR        ']'
#define KP_SL        '['
#define KP_SC        '-'
#define KP_BACK      'C' // if __SENDKEY2_SUPPORT__ was defined, KP_BACK is used as KEY_SEND1, otherwise, KEY_INVALID.
#define KP_DEL       'Y' // if __SENDKEY2_SUPPORT__ was defined, KP_DEL is used as KEY_SEND2, otherwise, KEY_CLEAR.
// Extra Function Keys
#define KP_F0        'F'
#define KP_F1        'G'
#define KP_F2        'H'
#define KP_F3        'I'
#define KP_F4        'J'
#define KP_F5        'K'
#define KP_F6        'L'
#define KP_F7        'N'
#define KP_F8        'O'

#define KP_NB        'Z' // no button
#define KP_UNMAPPED  'X'

#define KP_A         'a'
#define KP_B         'b'
#define KP_C         'c'
#define KP_D         'd'
#define KP_E         'e'
#define KP_F         'f'
#define KP_G         'g'

#define KP_H         'h'
#define KP_I         'i'
#define KP_J         'j'
#define KP_K         'k'
#define KP_L         'l'
#define KP_M         'm'
#define KP_N         'n'

#define KP_O         'o'
#define KP_P         'p'
#define KP_Q         'q'
#define KP_R         'r'
#define KP_S         's'
#define KP_T         't'

#define KP_U         'u'
#define KP_V         'v'
#define KP_W         'w'
#define KP_X         'x'
#define KP_Y         'y'
#define KP_Z         'z'

#define KP_SMS       '$'
#define KP_PBOOK     '%'
#define KP_BKSPACE   '~'
#define KP_COMMA     ','
#define KP_STOP      '.'
#define KP_NUMLOCK   '|'
#define KP_CTRL      '{'
#define KP_SHIFT     '}'
#define KP_SPACE     ' '
#define KP_AND       '&'
#define KP_QUES      '?'
#define KP_ENTER     '\n'
#define KP_FM         '\\'
#define KP_CALENDER  '!'
#define KP_SLASH     '/'
#define KP_MUSIC     '+'
#define KP_MUSIC_PLAY  ':'
#define KP_MUSIC_STOP  ';'
#define KP_MUSIC_PREV '('
#define KP_MUSIC_NEXT ')'
#define KP_QQ        'Q'

// Available printable characters:
// " ' = @ A B R T W _ `

#endif //__KEY_DEFS_H__



