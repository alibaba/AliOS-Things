/*
*********************************************************************************************************
*
*                                         BOARD SUPPORT PACKAGE
*
*                                            Renesas RX64M
*                                                on the
*                                      RSKRX64M Evaluation Board
*
*                                 (c) Copyright 2014, Micrium, Weston, FL
*                                          All Rights Reserved
*
* Filename      : bsp_int_vect_tbl.h
* Version       : V1.00
* Programmer(s) : JFD
*                 AL
*                 CM
*                 DC
*********************************************************************************************************
*/

#ifndef  BSP_INT_VECT_TBL_H_
#define  BSP_INT_VECT_TBL_H_


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  BSP_IntVectSet     (uint16_t     int_id,
                          CPU_FNCT_VOID  isr);
void  BSP_IntVectTblInit (void);


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                          (STUB ISR HANDLERS)
*********************************************************************************************************
*/

void  BSP_IntHandler_000 (void);
void  BSP_IntHandler_001 (void);
void  BSP_IntHandler_002 (void);
void  BSP_IntHandler_003 (void);
void  BSP_IntHandler_004 (void);
void  BSP_IntHandler_005 (void);
void  BSP_IntHandler_006 (void);
void  BSP_IntHandler_007 (void);
void  BSP_IntHandler_008 (void);
void  BSP_IntHandler_009 (void);
void  BSP_IntHandler_010 (void);
void  BSP_IntHandler_011 (void);
void  BSP_IntHandler_012 (void);
void  BSP_IntHandler_013 (void);
void  BSP_IntHandler_014 (void);
void  BSP_IntHandler_015 (void);
void  BSP_IntHandler_016 (void);
void  BSP_IntHandler_017 (void);
void  BSP_IntHandler_018 (void);
void  BSP_IntHandler_019 (void);
void  BSP_IntHandler_020 (void);
void  BSP_IntHandler_021 (void);
void  BSP_IntHandler_022 (void);
void  BSP_IntHandler_023 (void);
void  BSP_IntHandler_024 (void);
void  BSP_IntHandler_025 (void);
void  BSP_IntHandler_026 (void);
void  BSP_IntHandler_027 (void);
void  BSP_IntHandler_028 (void);
void  BSP_IntHandler_029 (void);
void  BSP_IntHandler_030 (void);
void  BSP_IntHandler_031 (void);
void  BSP_IntHandler_032 (void);
void  BSP_IntHandler_033 (void);
void  BSP_IntHandler_034 (void);
void  BSP_IntHandler_035 (void);
void  BSP_IntHandler_036 (void);
void  BSP_IntHandler_037 (void);
void  BSP_IntHandler_038 (void);
void  BSP_IntHandler_039 (void);
void  BSP_IntHandler_040 (void);
void  BSP_IntHandler_041 (void);
void  BSP_IntHandler_042 (void);
void  BSP_IntHandler_043 (void);
void  BSP_IntHandler_044 (void);
void  BSP_IntHandler_045 (void);
void  BSP_IntHandler_046 (void);
void  BSP_IntHandler_047 (void);
void  BSP_IntHandler_048 (void);
void  BSP_IntHandler_049 (void);
void  BSP_IntHandler_050 (void);
void  BSP_IntHandler_051 (void);
void  BSP_IntHandler_052 (void);
void  BSP_IntHandler_053 (void);
void  BSP_IntHandler_054 (void);
void  BSP_IntHandler_055 (void);
void  BSP_IntHandler_056 (void);
void  BSP_IntHandler_057 (void);
void  BSP_IntHandler_058 (void);
void  BSP_IntHandler_059 (void);
void  BSP_IntHandler_060 (void);
void  BSP_IntHandler_061 (void);
void  BSP_IntHandler_062 (void);
void  BSP_IntHandler_063 (void);
void  BSP_IntHandler_064 (void);
void  BSP_IntHandler_065 (void);
void  BSP_IntHandler_066 (void);
void  BSP_IntHandler_067 (void);
void  BSP_IntHandler_068 (void);
void  BSP_IntHandler_069 (void);
void  BSP_IntHandler_070 (void);
void  BSP_IntHandler_071 (void);
void  BSP_IntHandler_072 (void);
void  BSP_IntHandler_073 (void);
void  BSP_IntHandler_074 (void);
void  BSP_IntHandler_075 (void);
void  BSP_IntHandler_076 (void);
void  BSP_IntHandler_077 (void);
void  BSP_IntHandler_078 (void);
void  BSP_IntHandler_079 (void);
void  BSP_IntHandler_080 (void);
void  BSP_IntHandler_081 (void);
void  BSP_IntHandler_082 (void);
void  BSP_IntHandler_083 (void);
void  BSP_IntHandler_084 (void);
void  BSP_IntHandler_085 (void);
void  BSP_IntHandler_086 (void);
void  BSP_IntHandler_087 (void);
void  BSP_IntHandler_088 (void);
void  BSP_IntHandler_089 (void);
void  BSP_IntHandler_090 (void);
void  BSP_IntHandler_091 (void);
void  BSP_IntHandler_092 (void);
void  BSP_IntHandler_093 (void);
void  BSP_IntHandler_094 (void);
void  BSP_IntHandler_095 (void);
void  BSP_IntHandler_096 (void);
void  BSP_IntHandler_097 (void);
void  BSP_IntHandler_098 (void);
void  BSP_IntHandler_099 (void);
void  BSP_IntHandler_100 (void);
void  BSP_IntHandler_101 (void);
void  BSP_IntHandler_102 (void);
void  BSP_IntHandler_103 (void);
void  BSP_IntHandler_104 (void);
void  BSP_IntHandler_105 (void);
void  BSP_IntHandler_106 (void);
void  BSP_IntHandler_107 (void);
void  BSP_IntHandler_108 (void);
void  BSP_IntHandler_109 (void);
void  BSP_IntHandler_110 (void);
void  BSP_IntHandler_111 (void);
void  BSP_IntHandler_112 (void);
void  BSP_IntHandler_113 (void);
void  BSP_IntHandler_114 (void);
void  BSP_IntHandler_115 (void);
void  BSP_IntHandler_116 (void);
void  BSP_IntHandler_117 (void);
void  BSP_IntHandler_118 (void);
void  BSP_IntHandler_119 (void);
void  BSP_IntHandler_120 (void);
void  BSP_IntHandler_121 (void);
void  BSP_IntHandler_122 (void);
void  BSP_IntHandler_123 (void);
void  BSP_IntHandler_124 (void);
void  BSP_IntHandler_125 (void);
void  BSP_IntHandler_126 (void);
void  BSP_IntHandler_127 (void);
void  BSP_IntHandler_128 (void);
void  BSP_IntHandler_129 (void);
void  BSP_IntHandler_130 (void);
void  BSP_IntHandler_131 (void);
void  BSP_IntHandler_132 (void);
void  BSP_IntHandler_133 (void);
void  BSP_IntHandler_134 (void);
void  BSP_IntHandler_135 (void);
void  BSP_IntHandler_136 (void);
void  BSP_IntHandler_137 (void);
void  BSP_IntHandler_138 (void);
void  BSP_IntHandler_139 (void);
void  BSP_IntHandler_140 (void);
void  BSP_IntHandler_141 (void);
void  BSP_IntHandler_142 (void);
void  BSP_IntHandler_143 (void);
void  BSP_IntHandler_144 (void);
void  BSP_IntHandler_145 (void);
void  BSP_IntHandler_146 (void);
void  BSP_IntHandler_147 (void);
void  BSP_IntHandler_148 (void);
void  BSP_IntHandler_149 (void);
void  BSP_IntHandler_150 (void);
void  BSP_IntHandler_151 (void);
void  BSP_IntHandler_152 (void);
void  BSP_IntHandler_153 (void);
void  BSP_IntHandler_154 (void);
void  BSP_IntHandler_155 (void);
void  BSP_IntHandler_156 (void);
void  BSP_IntHandler_157 (void);
void  BSP_IntHandler_158 (void);
void  BSP_IntHandler_159 (void);
void  BSP_IntHandler_160 (void);
void  BSP_IntHandler_161 (void);
void  BSP_IntHandler_162 (void);
void  BSP_IntHandler_163 (void);
void  BSP_IntHandler_164 (void);
void  BSP_IntHandler_165 (void);
void  BSP_IntHandler_166 (void);
void  BSP_IntHandler_167 (void);
void  BSP_IntHandler_168 (void);
void  BSP_IntHandler_169 (void);
void  BSP_IntHandler_170 (void);
void  BSP_IntHandler_171 (void);
void  BSP_IntHandler_172 (void);
void  BSP_IntHandler_173 (void);
void  BSP_IntHandler_174 (void);
void  BSP_IntHandler_175 (void);
void  BSP_IntHandler_176 (void);
void  BSP_IntHandler_177 (void);
void  BSP_IntHandler_178 (void);
void  BSP_IntHandler_179 (void);
void  BSP_IntHandler_180 (void);
void  BSP_IntHandler_181 (void);
void  BSP_IntHandler_182 (void);
void  BSP_IntHandler_183 (void);
void  BSP_IntHandler_184 (void);
void  BSP_IntHandler_185 (void);
void  BSP_IntHandler_186 (void);
void  BSP_IntHandler_187 (void);
void  BSP_IntHandler_188 (void);
void  BSP_IntHandler_189 (void);
void  BSP_IntHandler_190 (void);
void  BSP_IntHandler_191 (void);
void  BSP_IntHandler_192 (void);
void  BSP_IntHandler_193 (void);
void  BSP_IntHandler_194 (void);
void  BSP_IntHandler_195 (void);
void  BSP_IntHandler_196 (void);
void  BSP_IntHandler_197 (void);
void  BSP_IntHandler_198 (void);
void  BSP_IntHandler_199 (void);
void  BSP_IntHandler_200 (void);
void  BSP_IntHandler_201 (void);
void  BSP_IntHandler_202 (void);
void  BSP_IntHandler_203 (void);
void  BSP_IntHandler_204 (void);
void  BSP_IntHandler_205 (void);
void  BSP_IntHandler_206 (void);
void  BSP_IntHandler_207 (void);
void  BSP_IntHandler_208 (void);
void  BSP_IntHandler_209 (void);
void  BSP_IntHandler_210 (void);
void  BSP_IntHandler_211 (void);
void  BSP_IntHandler_212 (void);
void  BSP_IntHandler_213 (void);
void  BSP_IntHandler_214 (void);
void  BSP_IntHandler_215 (void);
void  BSP_IntHandler_216 (void);
void  BSP_IntHandler_217 (void);
void  BSP_IntHandler_218 (void);
void  BSP_IntHandler_219 (void);
void  BSP_IntHandler_220 (void);
void  BSP_IntHandler_221 (void);
void  BSP_IntHandler_222 (void);
void  BSP_IntHandler_223 (void);
void  BSP_IntHandler_224 (void);
void  BSP_IntHandler_225 (void);
void  BSP_IntHandler_226 (void);
void  BSP_IntHandler_227 (void);
void  BSP_IntHandler_228 (void);
void  BSP_IntHandler_229 (void);
void  BSP_IntHandler_230 (void);
void  BSP_IntHandler_231 (void);
void  BSP_IntHandler_232 (void);
void  BSP_IntHandler_233 (void);
void  BSP_IntHandler_234 (void);
void  BSP_IntHandler_235 (void);
void  BSP_IntHandler_236 (void);
void  BSP_IntHandler_237 (void);
void  BSP_IntHandler_238 (void);
void  BSP_IntHandler_239 (void);
void  BSP_IntHandler_240 (void);
void  BSP_IntHandler_241 (void);
void  BSP_IntHandler_242 (void);
void  BSP_IntHandler_243 (void);
void  BSP_IntHandler_244 (void);
void  BSP_IntHandler_245 (void);
void  BSP_IntHandler_246 (void);
void  BSP_IntHandler_247 (void);
void  BSP_IntHandler_248 (void);
void  BSP_IntHandler_249 (void);
void  BSP_IntHandler_250 (void);
void  BSP_IntHandler_251 (void);
void  BSP_IntHandler_252 (void);
void  BSP_IntHandler_253 (void);
void  BSP_IntHandler_254 (void);
void  BSP_IntHandler_255 (void);

#endif
