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


#ifndef _XCPU_TAG_ASM_H_
#define _XCPU_TAG_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'xcpu_tag'."
#endif



//==============================================================================
// xcpu_tag
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_XCPU_TAG_BASE          0x01A1A000

#define REG_XCPU_TAG_BASE_HI       BASE_HI(REG_XCPU_TAG_BASE)
#define REG_XCPU_TAG_BASE_LO       BASE_LO(REG_XCPU_TAG_BASE)

#define REG_XCPU_TAG_LINE_0        REG_XCPU_TAG_BASE_LO + 0x00000000
#define REG_XCPU_TAG_LINE_1        REG_XCPU_TAG_BASE_LO + 0x00000004
#define REG_XCPU_TAG_LINE_2        REG_XCPU_TAG_BASE_LO + 0x00000008
#define REG_XCPU_TAG_LINE_3        REG_XCPU_TAG_BASE_LO + 0x0000000C
#define REG_XCPU_TAG_LINE_4        REG_XCPU_TAG_BASE_LO + 0x00000010
#define REG_XCPU_TAG_LINE_5        REG_XCPU_TAG_BASE_LO + 0x00000014
#define REG_XCPU_TAG_LINE_6        REG_XCPU_TAG_BASE_LO + 0x00000018
#define REG_XCPU_TAG_LINE_7        REG_XCPU_TAG_BASE_LO + 0x0000001C
#define REG_XCPU_TAG_LINE_8        REG_XCPU_TAG_BASE_LO + 0x00000020
#define REG_XCPU_TAG_LINE_9        REG_XCPU_TAG_BASE_LO + 0x00000024
#define REG_XCPU_TAG_LINE_10       REG_XCPU_TAG_BASE_LO + 0x00000028
#define REG_XCPU_TAG_LINE_11       REG_XCPU_TAG_BASE_LO + 0x0000002C
#define REG_XCPU_TAG_LINE_12       REG_XCPU_TAG_BASE_LO + 0x00000030
#define REG_XCPU_TAG_LINE_13       REG_XCPU_TAG_BASE_LO + 0x00000034
#define REG_XCPU_TAG_LINE_14       REG_XCPU_TAG_BASE_LO + 0x00000038
#define REG_XCPU_TAG_LINE_15       REG_XCPU_TAG_BASE_LO + 0x0000003C
#define REG_XCPU_TAG_LINE_16       REG_XCPU_TAG_BASE_LO + 0x00000040
#define REG_XCPU_TAG_LINE_17       REG_XCPU_TAG_BASE_LO + 0x00000044
#define REG_XCPU_TAG_LINE_18       REG_XCPU_TAG_BASE_LO + 0x00000048
#define REG_XCPU_TAG_LINE_19       REG_XCPU_TAG_BASE_LO + 0x0000004C
#define REG_XCPU_TAG_LINE_20       REG_XCPU_TAG_BASE_LO + 0x00000050
#define REG_XCPU_TAG_LINE_21       REG_XCPU_TAG_BASE_LO + 0x00000054
#define REG_XCPU_TAG_LINE_22       REG_XCPU_TAG_BASE_LO + 0x00000058
#define REG_XCPU_TAG_LINE_23       REG_XCPU_TAG_BASE_LO + 0x0000005C
#define REG_XCPU_TAG_LINE_24       REG_XCPU_TAG_BASE_LO + 0x00000060
#define REG_XCPU_TAG_LINE_25       REG_XCPU_TAG_BASE_LO + 0x00000064
#define REG_XCPU_TAG_LINE_26       REG_XCPU_TAG_BASE_LO + 0x00000068
#define REG_XCPU_TAG_LINE_27       REG_XCPU_TAG_BASE_LO + 0x0000006C
#define REG_XCPU_TAG_LINE_28       REG_XCPU_TAG_BASE_LO + 0x00000070
#define REG_XCPU_TAG_LINE_29       REG_XCPU_TAG_BASE_LO + 0x00000074
#define REG_XCPU_TAG_LINE_30       REG_XCPU_TAG_BASE_LO + 0x00000078
#define REG_XCPU_TAG_LINE_31       REG_XCPU_TAG_BASE_LO + 0x0000007C
#define REG_XCPU_TAG_LINE_32       REG_XCPU_TAG_BASE_LO + 0x00000080
#define REG_XCPU_TAG_LINE_33       REG_XCPU_TAG_BASE_LO + 0x00000084
#define REG_XCPU_TAG_LINE_34       REG_XCPU_TAG_BASE_LO + 0x00000088
#define REG_XCPU_TAG_LINE_35       REG_XCPU_TAG_BASE_LO + 0x0000008C
#define REG_XCPU_TAG_LINE_36       REG_XCPU_TAG_BASE_LO + 0x00000090
#define REG_XCPU_TAG_LINE_37       REG_XCPU_TAG_BASE_LO + 0x00000094
#define REG_XCPU_TAG_LINE_38       REG_XCPU_TAG_BASE_LO + 0x00000098
#define REG_XCPU_TAG_LINE_39       REG_XCPU_TAG_BASE_LO + 0x0000009C
#define REG_XCPU_TAG_LINE_40       REG_XCPU_TAG_BASE_LO + 0x000000A0
#define REG_XCPU_TAG_LINE_41       REG_XCPU_TAG_BASE_LO + 0x000000A4
#define REG_XCPU_TAG_LINE_42       REG_XCPU_TAG_BASE_LO + 0x000000A8
#define REG_XCPU_TAG_LINE_43       REG_XCPU_TAG_BASE_LO + 0x000000AC
#define REG_XCPU_TAG_LINE_44       REG_XCPU_TAG_BASE_LO + 0x000000B0
#define REG_XCPU_TAG_LINE_45       REG_XCPU_TAG_BASE_LO + 0x000000B4
#define REG_XCPU_TAG_LINE_46       REG_XCPU_TAG_BASE_LO + 0x000000B8
#define REG_XCPU_TAG_LINE_47       REG_XCPU_TAG_BASE_LO + 0x000000BC
#define REG_XCPU_TAG_LINE_48       REG_XCPU_TAG_BASE_LO + 0x000000C0
#define REG_XCPU_TAG_LINE_49       REG_XCPU_TAG_BASE_LO + 0x000000C4
#define REG_XCPU_TAG_LINE_50       REG_XCPU_TAG_BASE_LO + 0x000000C8
#define REG_XCPU_TAG_LINE_51       REG_XCPU_TAG_BASE_LO + 0x000000CC
#define REG_XCPU_TAG_LINE_52       REG_XCPU_TAG_BASE_LO + 0x000000D0
#define REG_XCPU_TAG_LINE_53       REG_XCPU_TAG_BASE_LO + 0x000000D4
#define REG_XCPU_TAG_LINE_54       REG_XCPU_TAG_BASE_LO + 0x000000D8
#define REG_XCPU_TAG_LINE_55       REG_XCPU_TAG_BASE_LO + 0x000000DC
#define REG_XCPU_TAG_LINE_56       REG_XCPU_TAG_BASE_LO + 0x000000E0
#define REG_XCPU_TAG_LINE_57       REG_XCPU_TAG_BASE_LO + 0x000000E4
#define REG_XCPU_TAG_LINE_58       REG_XCPU_TAG_BASE_LO + 0x000000E8
#define REG_XCPU_TAG_LINE_59       REG_XCPU_TAG_BASE_LO + 0x000000EC
#define REG_XCPU_TAG_LINE_60       REG_XCPU_TAG_BASE_LO + 0x000000F0
#define REG_XCPU_TAG_LINE_61       REG_XCPU_TAG_BASE_LO + 0x000000F4
#define REG_XCPU_TAG_LINE_62       REG_XCPU_TAG_BASE_LO + 0x000000F8
#define REG_XCPU_TAG_LINE_63       REG_XCPU_TAG_BASE_LO + 0x000000FC
#define REG_XCPU_TAG_LINE_64       REG_XCPU_TAG_BASE_LO + 0x00000100
#define REG_XCPU_TAG_LINE_65       REG_XCPU_TAG_BASE_LO + 0x00000104
#define REG_XCPU_TAG_LINE_66       REG_XCPU_TAG_BASE_LO + 0x00000108
#define REG_XCPU_TAG_LINE_67       REG_XCPU_TAG_BASE_LO + 0x0000010C
#define REG_XCPU_TAG_LINE_68       REG_XCPU_TAG_BASE_LO + 0x00000110
#define REG_XCPU_TAG_LINE_69       REG_XCPU_TAG_BASE_LO + 0x00000114
#define REG_XCPU_TAG_LINE_70       REG_XCPU_TAG_BASE_LO + 0x00000118
#define REG_XCPU_TAG_LINE_71       REG_XCPU_TAG_BASE_LO + 0x0000011C
#define REG_XCPU_TAG_LINE_72       REG_XCPU_TAG_BASE_LO + 0x00000120
#define REG_XCPU_TAG_LINE_73       REG_XCPU_TAG_BASE_LO + 0x00000124
#define REG_XCPU_TAG_LINE_74       REG_XCPU_TAG_BASE_LO + 0x00000128
#define REG_XCPU_TAG_LINE_75       REG_XCPU_TAG_BASE_LO + 0x0000012C
#define REG_XCPU_TAG_LINE_76       REG_XCPU_TAG_BASE_LO + 0x00000130
#define REG_XCPU_TAG_LINE_77       REG_XCPU_TAG_BASE_LO + 0x00000134
#define REG_XCPU_TAG_LINE_78       REG_XCPU_TAG_BASE_LO + 0x00000138
#define REG_XCPU_TAG_LINE_79       REG_XCPU_TAG_BASE_LO + 0x0000013C
#define REG_XCPU_TAG_LINE_80       REG_XCPU_TAG_BASE_LO + 0x00000140
#define REG_XCPU_TAG_LINE_81       REG_XCPU_TAG_BASE_LO + 0x00000144
#define REG_XCPU_TAG_LINE_82       REG_XCPU_TAG_BASE_LO + 0x00000148
#define REG_XCPU_TAG_LINE_83       REG_XCPU_TAG_BASE_LO + 0x0000014C
#define REG_XCPU_TAG_LINE_84       REG_XCPU_TAG_BASE_LO + 0x00000150
#define REG_XCPU_TAG_LINE_85       REG_XCPU_TAG_BASE_LO + 0x00000154
#define REG_XCPU_TAG_LINE_86       REG_XCPU_TAG_BASE_LO + 0x00000158
#define REG_XCPU_TAG_LINE_87       REG_XCPU_TAG_BASE_LO + 0x0000015C
#define REG_XCPU_TAG_LINE_88       REG_XCPU_TAG_BASE_LO + 0x00000160
#define REG_XCPU_TAG_LINE_89       REG_XCPU_TAG_BASE_LO + 0x00000164
#define REG_XCPU_TAG_LINE_90       REG_XCPU_TAG_BASE_LO + 0x00000168
#define REG_XCPU_TAG_LINE_91       REG_XCPU_TAG_BASE_LO + 0x0000016C
#define REG_XCPU_TAG_LINE_92       REG_XCPU_TAG_BASE_LO + 0x00000170
#define REG_XCPU_TAG_LINE_93       REG_XCPU_TAG_BASE_LO + 0x00000174
#define REG_XCPU_TAG_LINE_94       REG_XCPU_TAG_BASE_LO + 0x00000178
#define REG_XCPU_TAG_LINE_95       REG_XCPU_TAG_BASE_LO + 0x0000017C
#define REG_XCPU_TAG_LINE_96       REG_XCPU_TAG_BASE_LO + 0x00000180
#define REG_XCPU_TAG_LINE_97       REG_XCPU_TAG_BASE_LO + 0x00000184
#define REG_XCPU_TAG_LINE_98       REG_XCPU_TAG_BASE_LO + 0x00000188
#define REG_XCPU_TAG_LINE_99       REG_XCPU_TAG_BASE_LO + 0x0000018C
#define REG_XCPU_TAG_LINE_100      REG_XCPU_TAG_BASE_LO + 0x00000190
#define REG_XCPU_TAG_LINE_101      REG_XCPU_TAG_BASE_LO + 0x00000194
#define REG_XCPU_TAG_LINE_102      REG_XCPU_TAG_BASE_LO + 0x00000198
#define REG_XCPU_TAG_LINE_103      REG_XCPU_TAG_BASE_LO + 0x0000019C
#define REG_XCPU_TAG_LINE_104      REG_XCPU_TAG_BASE_LO + 0x000001A0
#define REG_XCPU_TAG_LINE_105      REG_XCPU_TAG_BASE_LO + 0x000001A4
#define REG_XCPU_TAG_LINE_106      REG_XCPU_TAG_BASE_LO + 0x000001A8
#define REG_XCPU_TAG_LINE_107      REG_XCPU_TAG_BASE_LO + 0x000001AC
#define REG_XCPU_TAG_LINE_108      REG_XCPU_TAG_BASE_LO + 0x000001B0
#define REG_XCPU_TAG_LINE_109      REG_XCPU_TAG_BASE_LO + 0x000001B4
#define REG_XCPU_TAG_LINE_110      REG_XCPU_TAG_BASE_LO + 0x000001B8
#define REG_XCPU_TAG_LINE_111      REG_XCPU_TAG_BASE_LO + 0x000001BC
#define REG_XCPU_TAG_LINE_112      REG_XCPU_TAG_BASE_LO + 0x000001C0
#define REG_XCPU_TAG_LINE_113      REG_XCPU_TAG_BASE_LO + 0x000001C4
#define REG_XCPU_TAG_LINE_114      REG_XCPU_TAG_BASE_LO + 0x000001C8
#define REG_XCPU_TAG_LINE_115      REG_XCPU_TAG_BASE_LO + 0x000001CC
#define REG_XCPU_TAG_LINE_116      REG_XCPU_TAG_BASE_LO + 0x000001D0
#define REG_XCPU_TAG_LINE_117      REG_XCPU_TAG_BASE_LO + 0x000001D4
#define REG_XCPU_TAG_LINE_118      REG_XCPU_TAG_BASE_LO + 0x000001D8
#define REG_XCPU_TAG_LINE_119      REG_XCPU_TAG_BASE_LO + 0x000001DC
#define REG_XCPU_TAG_LINE_120      REG_XCPU_TAG_BASE_LO + 0x000001E0
#define REG_XCPU_TAG_LINE_121      REG_XCPU_TAG_BASE_LO + 0x000001E4
#define REG_XCPU_TAG_LINE_122      REG_XCPU_TAG_BASE_LO + 0x000001E8
#define REG_XCPU_TAG_LINE_123      REG_XCPU_TAG_BASE_LO + 0x000001EC
#define REG_XCPU_TAG_LINE_124      REG_XCPU_TAG_BASE_LO + 0x000001F0
#define REG_XCPU_TAG_LINE_125      REG_XCPU_TAG_BASE_LO + 0x000001F4
#define REG_XCPU_TAG_LINE_126      REG_XCPU_TAG_BASE_LO + 0x000001F8
#define REG_XCPU_TAG_LINE_127      REG_XCPU_TAG_BASE_LO + 0x000001FC
#define REG_XCPU_TAG_LINE_128      REG_XCPU_TAG_BASE_LO + 0x00000200
#define REG_XCPU_TAG_LINE_129      REG_XCPU_TAG_BASE_LO + 0x00000204
#define REG_XCPU_TAG_LINE_130      REG_XCPU_TAG_BASE_LO + 0x00000208
#define REG_XCPU_TAG_LINE_131      REG_XCPU_TAG_BASE_LO + 0x0000020C
#define REG_XCPU_TAG_LINE_132      REG_XCPU_TAG_BASE_LO + 0x00000210
#define REG_XCPU_TAG_LINE_133      REG_XCPU_TAG_BASE_LO + 0x00000214
#define REG_XCPU_TAG_LINE_134      REG_XCPU_TAG_BASE_LO + 0x00000218
#define REG_XCPU_TAG_LINE_135      REG_XCPU_TAG_BASE_LO + 0x0000021C
#define REG_XCPU_TAG_LINE_136      REG_XCPU_TAG_BASE_LO + 0x00000220
#define REG_XCPU_TAG_LINE_137      REG_XCPU_TAG_BASE_LO + 0x00000224
#define REG_XCPU_TAG_LINE_138      REG_XCPU_TAG_BASE_LO + 0x00000228
#define REG_XCPU_TAG_LINE_139      REG_XCPU_TAG_BASE_LO + 0x0000022C
#define REG_XCPU_TAG_LINE_140      REG_XCPU_TAG_BASE_LO + 0x00000230
#define REG_XCPU_TAG_LINE_141      REG_XCPU_TAG_BASE_LO + 0x00000234
#define REG_XCPU_TAG_LINE_142      REG_XCPU_TAG_BASE_LO + 0x00000238
#define REG_XCPU_TAG_LINE_143      REG_XCPU_TAG_BASE_LO + 0x0000023C
#define REG_XCPU_TAG_LINE_144      REG_XCPU_TAG_BASE_LO + 0x00000240
#define REG_XCPU_TAG_LINE_145      REG_XCPU_TAG_BASE_LO + 0x00000244
#define REG_XCPU_TAG_LINE_146      REG_XCPU_TAG_BASE_LO + 0x00000248
#define REG_XCPU_TAG_LINE_147      REG_XCPU_TAG_BASE_LO + 0x0000024C
#define REG_XCPU_TAG_LINE_148      REG_XCPU_TAG_BASE_LO + 0x00000250
#define REG_XCPU_TAG_LINE_149      REG_XCPU_TAG_BASE_LO + 0x00000254
#define REG_XCPU_TAG_LINE_150      REG_XCPU_TAG_BASE_LO + 0x00000258
#define REG_XCPU_TAG_LINE_151      REG_XCPU_TAG_BASE_LO + 0x0000025C
#define REG_XCPU_TAG_LINE_152      REG_XCPU_TAG_BASE_LO + 0x00000260
#define REG_XCPU_TAG_LINE_153      REG_XCPU_TAG_BASE_LO + 0x00000264
#define REG_XCPU_TAG_LINE_154      REG_XCPU_TAG_BASE_LO + 0x00000268
#define REG_XCPU_TAG_LINE_155      REG_XCPU_TAG_BASE_LO + 0x0000026C
#define REG_XCPU_TAG_LINE_156      REG_XCPU_TAG_BASE_LO + 0x00000270
#define REG_XCPU_TAG_LINE_157      REG_XCPU_TAG_BASE_LO + 0x00000274
#define REG_XCPU_TAG_LINE_158      REG_XCPU_TAG_BASE_LO + 0x00000278
#define REG_XCPU_TAG_LINE_159      REG_XCPU_TAG_BASE_LO + 0x0000027C
#define REG_XCPU_TAG_LINE_160      REG_XCPU_TAG_BASE_LO + 0x00000280
#define REG_XCPU_TAG_LINE_161      REG_XCPU_TAG_BASE_LO + 0x00000284
#define REG_XCPU_TAG_LINE_162      REG_XCPU_TAG_BASE_LO + 0x00000288
#define REG_XCPU_TAG_LINE_163      REG_XCPU_TAG_BASE_LO + 0x0000028C
#define REG_XCPU_TAG_LINE_164      REG_XCPU_TAG_BASE_LO + 0x00000290
#define REG_XCPU_TAG_LINE_165      REG_XCPU_TAG_BASE_LO + 0x00000294
#define REG_XCPU_TAG_LINE_166      REG_XCPU_TAG_BASE_LO + 0x00000298
#define REG_XCPU_TAG_LINE_167      REG_XCPU_TAG_BASE_LO + 0x0000029C
#define REG_XCPU_TAG_LINE_168      REG_XCPU_TAG_BASE_LO + 0x000002A0
#define REG_XCPU_TAG_LINE_169      REG_XCPU_TAG_BASE_LO + 0x000002A4
#define REG_XCPU_TAG_LINE_170      REG_XCPU_TAG_BASE_LO + 0x000002A8
#define REG_XCPU_TAG_LINE_171      REG_XCPU_TAG_BASE_LO + 0x000002AC
#define REG_XCPU_TAG_LINE_172      REG_XCPU_TAG_BASE_LO + 0x000002B0
#define REG_XCPU_TAG_LINE_173      REG_XCPU_TAG_BASE_LO + 0x000002B4
#define REG_XCPU_TAG_LINE_174      REG_XCPU_TAG_BASE_LO + 0x000002B8
#define REG_XCPU_TAG_LINE_175      REG_XCPU_TAG_BASE_LO + 0x000002BC
#define REG_XCPU_TAG_LINE_176      REG_XCPU_TAG_BASE_LO + 0x000002C0
#define REG_XCPU_TAG_LINE_177      REG_XCPU_TAG_BASE_LO + 0x000002C4
#define REG_XCPU_TAG_LINE_178      REG_XCPU_TAG_BASE_LO + 0x000002C8
#define REG_XCPU_TAG_LINE_179      REG_XCPU_TAG_BASE_LO + 0x000002CC
#define REG_XCPU_TAG_LINE_180      REG_XCPU_TAG_BASE_LO + 0x000002D0
#define REG_XCPU_TAG_LINE_181      REG_XCPU_TAG_BASE_LO + 0x000002D4
#define REG_XCPU_TAG_LINE_182      REG_XCPU_TAG_BASE_LO + 0x000002D8
#define REG_XCPU_TAG_LINE_183      REG_XCPU_TAG_BASE_LO + 0x000002DC
#define REG_XCPU_TAG_LINE_184      REG_XCPU_TAG_BASE_LO + 0x000002E0
#define REG_XCPU_TAG_LINE_185      REG_XCPU_TAG_BASE_LO + 0x000002E4
#define REG_XCPU_TAG_LINE_186      REG_XCPU_TAG_BASE_LO + 0x000002E8
#define REG_XCPU_TAG_LINE_187      REG_XCPU_TAG_BASE_LO + 0x000002EC
#define REG_XCPU_TAG_LINE_188      REG_XCPU_TAG_BASE_LO + 0x000002F0
#define REG_XCPU_TAG_LINE_189      REG_XCPU_TAG_BASE_LO + 0x000002F4
#define REG_XCPU_TAG_LINE_190      REG_XCPU_TAG_BASE_LO + 0x000002F8
#define REG_XCPU_TAG_LINE_191      REG_XCPU_TAG_BASE_LO + 0x000002FC
#define REG_XCPU_TAG_LINE_192      REG_XCPU_TAG_BASE_LO + 0x00000300
#define REG_XCPU_TAG_LINE_193      REG_XCPU_TAG_BASE_LO + 0x00000304
#define REG_XCPU_TAG_LINE_194      REG_XCPU_TAG_BASE_LO + 0x00000308
#define REG_XCPU_TAG_LINE_195      REG_XCPU_TAG_BASE_LO + 0x0000030C
#define REG_XCPU_TAG_LINE_196      REG_XCPU_TAG_BASE_LO + 0x00000310
#define REG_XCPU_TAG_LINE_197      REG_XCPU_TAG_BASE_LO + 0x00000314
#define REG_XCPU_TAG_LINE_198      REG_XCPU_TAG_BASE_LO + 0x00000318
#define REG_XCPU_TAG_LINE_199      REG_XCPU_TAG_BASE_LO + 0x0000031C
#define REG_XCPU_TAG_LINE_200      REG_XCPU_TAG_BASE_LO + 0x00000320
#define REG_XCPU_TAG_LINE_201      REG_XCPU_TAG_BASE_LO + 0x00000324
#define REG_XCPU_TAG_LINE_202      REG_XCPU_TAG_BASE_LO + 0x00000328
#define REG_XCPU_TAG_LINE_203      REG_XCPU_TAG_BASE_LO + 0x0000032C
#define REG_XCPU_TAG_LINE_204      REG_XCPU_TAG_BASE_LO + 0x00000330
#define REG_XCPU_TAG_LINE_205      REG_XCPU_TAG_BASE_LO + 0x00000334
#define REG_XCPU_TAG_LINE_206      REG_XCPU_TAG_BASE_LO + 0x00000338
#define REG_XCPU_TAG_LINE_207      REG_XCPU_TAG_BASE_LO + 0x0000033C
#define REG_XCPU_TAG_LINE_208      REG_XCPU_TAG_BASE_LO + 0x00000340
#define REG_XCPU_TAG_LINE_209      REG_XCPU_TAG_BASE_LO + 0x00000344
#define REG_XCPU_TAG_LINE_210      REG_XCPU_TAG_BASE_LO + 0x00000348
#define REG_XCPU_TAG_LINE_211      REG_XCPU_TAG_BASE_LO + 0x0000034C
#define REG_XCPU_TAG_LINE_212      REG_XCPU_TAG_BASE_LO + 0x00000350
#define REG_XCPU_TAG_LINE_213      REG_XCPU_TAG_BASE_LO + 0x00000354
#define REG_XCPU_TAG_LINE_214      REG_XCPU_TAG_BASE_LO + 0x00000358
#define REG_XCPU_TAG_LINE_215      REG_XCPU_TAG_BASE_LO + 0x0000035C
#define REG_XCPU_TAG_LINE_216      REG_XCPU_TAG_BASE_LO + 0x00000360
#define REG_XCPU_TAG_LINE_217      REG_XCPU_TAG_BASE_LO + 0x00000364
#define REG_XCPU_TAG_LINE_218      REG_XCPU_TAG_BASE_LO + 0x00000368
#define REG_XCPU_TAG_LINE_219      REG_XCPU_TAG_BASE_LO + 0x0000036C
#define REG_XCPU_TAG_LINE_220      REG_XCPU_TAG_BASE_LO + 0x00000370
#define REG_XCPU_TAG_LINE_221      REG_XCPU_TAG_BASE_LO + 0x00000374
#define REG_XCPU_TAG_LINE_222      REG_XCPU_TAG_BASE_LO + 0x00000378
#define REG_XCPU_TAG_LINE_223      REG_XCPU_TAG_BASE_LO + 0x0000037C
#define REG_XCPU_TAG_LINE_224      REG_XCPU_TAG_BASE_LO + 0x00000380
#define REG_XCPU_TAG_LINE_225      REG_XCPU_TAG_BASE_LO + 0x00000384
#define REG_XCPU_TAG_LINE_226      REG_XCPU_TAG_BASE_LO + 0x00000388
#define REG_XCPU_TAG_LINE_227      REG_XCPU_TAG_BASE_LO + 0x0000038C
#define REG_XCPU_TAG_LINE_228      REG_XCPU_TAG_BASE_LO + 0x00000390
#define REG_XCPU_TAG_LINE_229      REG_XCPU_TAG_BASE_LO + 0x00000394
#define REG_XCPU_TAG_LINE_230      REG_XCPU_TAG_BASE_LO + 0x00000398
#define REG_XCPU_TAG_LINE_231      REG_XCPU_TAG_BASE_LO + 0x0000039C
#define REG_XCPU_TAG_LINE_232      REG_XCPU_TAG_BASE_LO + 0x000003A0
#define REG_XCPU_TAG_LINE_233      REG_XCPU_TAG_BASE_LO + 0x000003A4
#define REG_XCPU_TAG_LINE_234      REG_XCPU_TAG_BASE_LO + 0x000003A8
#define REG_XCPU_TAG_LINE_235      REG_XCPU_TAG_BASE_LO + 0x000003AC
#define REG_XCPU_TAG_LINE_236      REG_XCPU_TAG_BASE_LO + 0x000003B0
#define REG_XCPU_TAG_LINE_237      REG_XCPU_TAG_BASE_LO + 0x000003B4
#define REG_XCPU_TAG_LINE_238      REG_XCPU_TAG_BASE_LO + 0x000003B8
#define REG_XCPU_TAG_LINE_239      REG_XCPU_TAG_BASE_LO + 0x000003BC
#define REG_XCPU_TAG_LINE_240      REG_XCPU_TAG_BASE_LO + 0x000003C0
#define REG_XCPU_TAG_LINE_241      REG_XCPU_TAG_BASE_LO + 0x000003C4
#define REG_XCPU_TAG_LINE_242      REG_XCPU_TAG_BASE_LO + 0x000003C8
#define REG_XCPU_TAG_LINE_243      REG_XCPU_TAG_BASE_LO + 0x000003CC
#define REG_XCPU_TAG_LINE_244      REG_XCPU_TAG_BASE_LO + 0x000003D0
#define REG_XCPU_TAG_LINE_245      REG_XCPU_TAG_BASE_LO + 0x000003D4
#define REG_XCPU_TAG_LINE_246      REG_XCPU_TAG_BASE_LO + 0x000003D8
#define REG_XCPU_TAG_LINE_247      REG_XCPU_TAG_BASE_LO + 0x000003DC
#define REG_XCPU_TAG_LINE_248      REG_XCPU_TAG_BASE_LO + 0x000003E0
#define REG_XCPU_TAG_LINE_249      REG_XCPU_TAG_BASE_LO + 0x000003E4
#define REG_XCPU_TAG_LINE_250      REG_XCPU_TAG_BASE_LO + 0x000003E8
#define REG_XCPU_TAG_LINE_251      REG_XCPU_TAG_BASE_LO + 0x000003EC
#define REG_XCPU_TAG_LINE_252      REG_XCPU_TAG_BASE_LO + 0x000003F0
#define REG_XCPU_TAG_LINE_253      REG_XCPU_TAG_BASE_LO + 0x000003F4
#define REG_XCPU_TAG_LINE_254      REG_XCPU_TAG_BASE_LO + 0x000003F8
#define REG_XCPU_TAG_LINE_255      REG_XCPU_TAG_BASE_LO + 0x000003FC

//Line
#define XCPU_TAG_TAG(n)            (((n)&0xFFF)<<14)
#define XCPU_TAG_TAG_MASK          (0xFFF<<14)
#define XCPU_TAG_TAG_SHIFT         (14)
#define XCPU_TAG_VALID             (1<<29)
#define XCPU_TAG_VALID_MASK        (1<<29)
#define XCPU_TAG_VALID_SHIFT       (29)




#endif
