/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: ieee11073_nom_codes.h$
* $Version : 3.4.1.0$
* $Date    : Sep-15-2009$
*
* Comments:
*
* @brief This file contains the nomenclature codes used by IEEE_11073
*****************************************************************************/

#ifndef __ieee11073_nom_codes__
#define __ieee11073_nom_codes__

#define BYTESWAP16_L(x)	(intu16)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))

#define BYTESWAP32_L(val)	(intu32)((BYTESWAP16_L((uint_32)(val) & (uint_32)0xFFFF) << 0x10) |  \
					(BYTESWAP16_L((uint_32)((val) >>
 
 /******************************************************************************
 * Macro's
 *****************************************************************************/
 /* Partition codes */
#define MDC_PART_OBJ 								BYTESWAP16_L(1) /* Object Infrastr. */
#define MDC_PART_SCADA 								BYTESWAP16_L(2) /* SCADA (Physio IDs */
#define MDC_PART_DIM 								BYTESWAP16_L(4) /* Dimension */
#define MDC_PART_INFRA 								BYTESWAP16_L(8) /* Infrastructure */
#define MDC_PART_PHD_DM 							BYTESWAP16_L(128) /* Disease Mgmt */
#define MDC_PART_PHD_HF 							BYTESWAP16_L(129) /* Health and Fitness */
#define MDC_PART_PHD_AI 							BYTESWAP16_L(130) /* Aging Independently */
#define MDC_PART_RET_CODE 							BYTESWAP16_L(255) /* Return Codes */
#define MDC_PART_EXT_NOM 							BYTESWAP16_L(256) /* Ext. Nomenclature */
/**********************************************************************************
* From Object Infrastructure (MDC_PART_OBJ)
**********************************************************************************/
#define MDC_MOC_VMO_METRIC 							BYTESWAP16_L(4) /* */
#define MDC_MOC_VMO_METRIC_ENUM 					BYTESWAP16_L(5) /* */
#define MDC_MOC_VMO_METRIC_NU 						BYTESWAP16_L(6) /* */
#define MDC_MOC_VMO_METRIC_SA_RT 					BYTESWAP16_L(9) /* */
#define MDC_MOC_SCAN 								BYTESWAP16_L(16) /* */
#define MDC_MOC_SCAN_CFG 							BYTESWAP16_L(17) /* */
#define MDC_MOC_SCAN_CFG_EPI 						BYTESWAP16_L(18) /* */
#define MDC_MOC_SCAN_CFG_PERI 						BYTESWAP16_L(19) /* */
#define MDC_MOC_VMS_MDS_SIMP 						BYTESWAP16_L(37) /* */
#define MDC_MOC_VMO_PMSTORE 						BYTESWAP16_L(61) /* */
#define MDC_MOC_PM_SEGMENT 							BYTESWAP16_L(62) /* */
#define MDC_ATTR_CONFIRM_MODE 						BYTESWAP16_L(2323) /* */
#define MDC_ATTR_CONFIRM_TIMEOUT 					BYTESWAP16_L(2324) /* */
#define MDC_ATTR_ID_HANDLE 							BYTESWAP16_L(2337) /* */
#define MDC_ATTR_ID_INSTNO 							BYTESWAP16_L(2338) /* */
#define MDC_ATTR_ID_LABEL_STRING 					BYTESWAP16_L(2343) /* */
#define MDC_ATTR_ID_MODEL 							BYTESWAP16_L(2344) /* */
#define MDC_ATTR_ID_PHYSIO 							BYTESWAP16_L(2347) /* */
#define MDC_ATTR_ID_PROD_SPECN 						BYTESWAP16_L(2349) /* */
#define MDC_ATTR_ID_TYPE 							BYTESWAP16_L(2351) /* */
#define MDC_ATTR_METRIC_STORE_CAPAC_CNT 			BYTESWAP16_L(2369) /* */
#define MDC_ATTR_METRIC_STORE_SAMPLE_ALG 			BYTESWAP16_L(2371) /* */
#define MDC_ATTR_METRIC_STORE_USAGE_CNT 			BYTESWAP16_L(2372) /* */
#define MDC_ATTR_MSMT_STAT 							BYTESWAP16_L(2375) /* */
#define MDC_ATTR_NU_ACCUR_MSMT 						BYTESWAP16_L(2378) /* */
#define MDC_ATTR_NU_CMPD_VAL_OBS 					BYTESWAP16_L(2379) /* */
#define MDC_ATTR_NU_VAL_OBS 						BYTESWAP16_L(2384) /* */
#define MDC_ATTR_NUM_SEG 							BYTESWAP16_L(2385) /* */
#define MDC_ATTR_OP_STAT 							BYTESWAP16_L(2387) /* */
#define MDC_ATTR_POWER_STAT 						BYTESWAP16_L(2389) /* */
#define MDC_ATTR_SA_SPECN 							BYTESWAP16_L(2413) /* */
#define MDC_ATTR_SCALE_SPECN_I16 					BYTESWAP16_L(2415) /* */
#define MDC_ATTR_SCALE_SPECN_I32 					BYTESWAP16_L(2416) /* */
#define MDC_ATTR_SCALE_SPECN_I8 					BYTESWAP16_L(2417) /* */
#define MDC_ATTR_SCAN_REP_PD 						BYTESWAP16_L(2421) /* */
#define MDC_ATTR_SEG_USAGE_CNT 						BYTESWAP16_L(2427) /* */
#define MDC_ATTR_SYS_ID 							BYTESWAP16_L(2436) /* */
#define MDC_ATTR_SYS_TYPE 							BYTESWAP16_L(2438) /* */
#define MDC_ATTR_TIME_ABS 							BYTESWAP16_L(2439) /* */
#define MDC_ATTR_TIME_BATT_REMAIN 					BYTESWAP16_L(2440) /* */
#define MDC_ATTR_TIME_END_SEG 						BYTESWAP16_L(2442) /* */
#define MDC_ATTR_TIME_PD_SAMP 						BYTESWAP16_L(2445) /* */
#define MDC_ATTR_TIME_REL 							BYTESWAP16_L(2447) /* */
#define MDC_ATTR_TIME_STAMP_ABS 					BYTESWAP16_L(2448) /* */
#define MDC_ATTR_TIME_STAMP_REL 					BYTESWAP16_L(2449) /* */
#define MDC_ATTR_TIME_START_SEG 					BYTESWAP16_L(2450) /* */
#define MDC_ATTR_TX_WIND 							BYTESWAP16_L(2453) /* */
#define MDC_ATTR_UNIT_CODE 							BYTESWAP16_L(2454) /* */
#define MDC_ATTR_UNIT_LABEL_STRING 					BYTESWAP16_L(2457) /* */
#define MDC_ATTR_VAL_BATT_CHARGE 					BYTESWAP16_L(2460) /* */
#define MDC_ATTR_VAL_ENUM_OBS 						BYTESWAP16_L(2462) /* */
#define MDC_ATTR_TIME_REL_HI_RES 					BYTESWAP16_L(2536) /* */
#define MDC_ATTR_TIME_STAMP_REL_HI_RES 				BYTESWAP16_L(2537) /* */
#define MDC_ATTR_DEV_CONFIG_ID 						BYTESWAP16_L(2628) /* */
#define MDC_ATTR_MDS_TIME_INFO 						BYTESWAP16_L(2629) /* */
#define MDC_ATTR_METRIC_SPEC_SMALL 					BYTESWAP16_L(2630) /* */
#define MDC_ATTR_SOURCE_HANDLE_REF 					BYTESWAP16_L(2631) /* */
#define MDC_ATTR_SIMP_SA_OBS_VAL 					BYTESWAP16_L(2632) /* */
#define MDC_ATTR_ENUM_OBS_VAL_SIMP_OID 				BYTESWAP16_L(2633) /* */
#define MDC_ATTR_ENUM_OBS_VAL_SIMP_STR 				BYTESWAP16_L(2634) /* */
#define MDC_ATTR_REG_CERT_DATA_LIST 				BYTESWAP16_L(2635) /* */
#define MDC_ATTR_NU_VAL_OBS_BASIC 					BYTESWAP16_L(2636) /* */
#define MDC_ATTR_PM_STORE_CAPAB 					BYTESWAP16_L(2637) /* */
#define MDC_ATTR_PM_SEG_MAP 						BYTESWAP16_L(2638) /* */
#define MDC_ATTR_PM_SEG_PERSON_ID 					BYTESWAP16_L(2639) /* */
#define MDC_ATTR_SEG_STATS 							BYTESWAP16_L(2640) /* */
#define MDC_ATTR_SEG_FIXED_DATA 					BYTESWAP16_L(2641) /* */
#define MDC_ATTR_PM_SEG_ELEM_STAT_ATTR 				BYTESWAP16_L(2642) /* */
#define MDC_ATTR_SCAN_HANDLE_ATTR_VAL_MAP 			BYTESWAP16_L(2643) /* */
#define MDC_ATTR_SCAN_REP_PD_MIN 					BYTESWAP16_L(2644) /* */
#define MDC_ATTR_ATTRIBUTE_VAL_MAP 					BYTESWAP16_L(2645) /* */
#define MDC_ATTR_NU_VAL_OBS_SIMP 					BYTESWAP16_L(2646) /* */
#define MDC_ATTR_PM_STORE_LABEL_STRING 				BYTESWAP16_L(2647) /* */
#define MDC_ATTR_PM_SEG_LABEL_STRING 				BYTESWAP16_L(2648) /* */
#define MDC_ATTR_TIME_PD_MSMT_ACTIVE 				BYTESWAP16_L(2649) /* */
#define MDC_ATTR_SYS_TYPE_SPEC_LIST 				BYTESWAP16_L(2650) /* */
#define MDC_ATTR_METRIC_ID_PART 					BYTESWAP16_L(2655) /* */
#define MDC_ATTR_ENUM_OBS_VAL_PART			 		BYTESWAP16_L(2656) /* */
#define MDC_ATTR_SUPPLEMENTAL_TYPES 				BYTESWAP16_L(2657) /* */
#define MDC_ATTR_TIME_ABS_ADJUST 					BYTESWAP16_L(2658) /* */
#define MDC_ATTR_CLEAR_TIMEOUT 						BYTESWAP16_L(2659) /* */
#define MDC_ATTR_TRANSFER_TIMEOUT 					BYTESWAP16_L(2660) /* */
#define MDC_ATTR_ENUM_OBS_VAL_SIMP_BIT_STR 			BYTESWAP16_L(2661) /* */
#define MDC_ATTR_ENUM_OBS_VAL_BASIC_BIT_STR 		BYTESWAP16_L(2662) /* */
#define MDC_ATTR_METRIC_STRUCT_SMALL 				BYTESWAP16_L(2675) /* */

#define MDC_ATTR_NU_CMPD_VAL_OBS_SIMP 				BYTESWAP16_L(2676) /* */	
#define MDC_ATTR_NU_CMPD_VAL_OBS_BASIC 				BYTESWAP16_L(2677) /* */
#define MDC_ATTR_ID_PHYSIO_LIST 					BYTESWAP16_L(2678) /* */
#define MDC_ATTR_SCAN_HANDLE_LIST 					BYTESWAP16_L(2679) /* */
/* Partition: ACT */
#define MDC_ACT_SEG_CLR 							BYTESWAP16_L(3084) /* */
#define MDC_ACT_SEG_GET_INFO 						BYTESWAP16_L(3085) /* */
#define MDC_ACT_SET_TIME 							BYTESWAP16_L(3095) /* */
#define MDC_ACT_DATA_REQUEST 						BYTESWAP16_L(3099) /* */
#define MDC_ACT_SEG_TRIG_XFER 						BYTESWAP16_L(3100) /* */
#define MDC_NOTI_CONFIG	 							BYTESWAP16_L(3356) /* */
#define MDC_NOTI_SCAN_REPORT_FIXED 					BYTESWAP16_L(3357) /* */
#define MDC_NOTI_SCAN_REPORT_VAR 					BYTESWAP16_L(3358) /* */
#define MDC_NOTI_SCAN_REPORT_MP_FIXED 				BYTESWAP16_L(3359) /* */
#define MDC_NOTI_SCAN_REPORT_MP_VAR 				BYTESWAP16_L(3360) /* */
#define MDC_NOTI_SEGMENT_DATA 						BYTESWAP16_L(3361) /* */
#define MDC_NOTI_UNBUF_SCAN_REPORT_VAR 				BYTESWAP16_L(3362) /* */
#define MDC_NOTI_UNBUF_SCAN_REPORT_FIXED 			BYTESWAP16_L(3363) /* */
#define MDC_NOTI_UNBUF_SCAN_REPORT_GROUPED 			BYTESWAP16_L(3364) /* */
#define MDC_NOTI_UNBUF_SCAN_REPORT_MP_VAR 			BYTESWAP16_L(3365) /* */
#define MDC_NOTI_UNBUF_SCAN_REPORT_MP_FIXED 		BYTESWAP16_L(3366) /* */
#define MDC_NOTI_UNBUF_SCAN_REPORT_MP_GROUPED 		BYTESWAP16_L(3367) /* */
#define MDC_NOTI_BUF_SCAN_REPORT_VAR 				BYTESWAP16_L(3368) /* */
#define MDC_NOTI_BUF_SCAN_REPORT_FIXED 				BYTESWAP16_L(3369) /* */
#define MDC_NOTI_BUF_SCAN_REPORT_GROUPED 			BYTESWAP16_L(3370) /* */
#define MDC_NOTI_BUF_SCAN_REPORT_MP_VAR 			BYTESWAP16_L(3371) /* */
#define MDC_NOTI_BUF_SCAN_REPORT_MP_FIXED 			BYTESWAP16_L(3372) /* */
#define MDC_NOTI_BUF_SCAN_REPORT_MP_GROUPED 		BYTESWAP16_L(3373) /* */
/*********************************************************************************
* From Medical supervisory control and data acquisition (MDC_PART_SCADA)
**********************************************************************************/
#define MDC_TEMP_BODY 								BYTESWAP16_L(19292) /*TEMPbody */
#define MDC_PRESS_BLD_NONINV_SYS    				BYTESWAP16_L(18949)
#define MDC_PRESS_BLD_NONINV_DIA    				BYTESWAP16_L(18950)
#define MDC_PRESS_BLD_NONINV_MEAN   				BYTESWAP16_L(18951)
#define MDC_MASS_BODY_ACTUAL        				BYTESWAP16_L(57664) /* */
#define MDC_LEN_BODY_ACTUAL        					BYTESWAP16_L(57666)
/* Partition: SCADA/Other */
#define MDC_BODY_FAT 								BYTESWAP16_L(57676) /* */
#define  MDC_PRESS_BLD_NONINV   					BYTESWAP16_L(18948)
/*********************************************************************************
* From Dimensions (MDC_PART_DIM)
**********************************************************************************/
#define MDC_DIM_PERCENT 							BYTESWAP16_L(544) /* % */
#define MDC_DIM_KILO_G 								BYTESWAP16_L(1731) /* kg */
#define MDC_DIM_MIN 								BYTESWAP16_L(2208) /* min */
#define MDC_DIM_HR 									BYTESWAP16_L(2240) /* h */
#define MDC_DIM_DAY 								BYTESWAP16_L(2272) /* d */
#define MDC_DIM_DEGC 								BYTESWAP16_L(6048) /* oC */
#define MDC_DIM_MMHG 								BYTESWAP16_L(3872)
#define MDC_DIM_CENTI_M 							BYTESWAP16_L(1297) /* cm */
/*********************************************************************************
* From Communication Infrastructure (MDC_PART_INFRA)
**********************************************************************************/
#define MDC_DEV_SPEC_PROFILE_PULS_OXIM 				BYTESWAP16_L(4100) /* */
#define MDC_DEV_SPEC_PROFILE_BP 					BYTESWAP16_L(4103) /* */
#define MDC_DEV_SPEC_PROFILE_TEMP 					BYTESWAP16_L(4104) /* */
#define MDC_DEV_SPEC_PROFILE_SCALE 					BYTESWAP16_L(4111) /* */
#define MDC_DEV_SPEC_PROFILE_GLUCOSE 				BYTESWAP16_L(4113) /* */
#define MDC_DEV_SPEC_PROFILE_HF_CARDIO 				BYTESWAP16_L(4137) /* */

#define MDC_DEV_SPEC_PROFILE_HF_STRENGTH 			BYTESWAP16_L(4138) /* */
#define MDC_DEV_SPEC_PROFILE_AI_ACTIVITY_HUB 		BYTESWAP16_L(4167) /* */
#define MDC_DEV_SPEC_PROFILE_AI_MED_MINDER 			BYTESWAP16_L(4168) /* */
/* Placed 256 back from the start of the last Partition: OptionalPackageIdentifiers
(i.e. 8192-256). */
#define MDC_TIME_SYNC_NONE 							BYTESWAP16_L(7936) /* no time synchronization protocol supported */
#define MDC_TIME_SYNC_NTPV3 						BYTESWAP16_L(7937) /* RFC 1305 1992 Mar obs: 1119,1059,958 */
#define MDC_TIME_SYNC_NTPV4 						BYTESWAP16_L(7938_ /* <under development at ntp.org> */
#define MDC_TIME_SYNC_SNTPV4 						BYTESWAP16_L(7939) /* RFC 2030 1996 Oct obs: 1769 */
#define MDC_TIME_SYNC_SNTPV4330 					BYTESWAP16_L(7940) /* RFC 4330 2006 Jan obs: 2030,1769 */
#define MDC_TIME_SYNC_BTV1 							BYTESWAP16_L(7941) /* Bluetooth Medical Device Profile */
/*********************************************************************************
* From Return Codes (MDC_PART_RET_CODE)
**********************************************************************************/
#define MDC_RET_CODE_UNKNOWN 1 /* Generic error code */
/* Partition MDC_PART_RET_CODE/OBJ Object errors */
#define MDC_RET_CODE_OBJ_BUSY 1000 /* Object is busy so cannot handle the request */
/* Partition MDC_PART_RETURN_CODES/STORE Storage errors */
#define MDC_RET_CODE_STORE_EXH 2000 /* Storage such as disk is full */
#define MDC_RET_CODE_STORE_OFFLN 2001 /* Storage such as disk is offline */


#define MDC_MODALITY_FAST		BYTESWAP16_L(0x4C34)
#define MDC_TEMP_ORAL			BYTESWAP16_L(0xE008)
#define	MDC_TEMP_RECT			BYTESWAP16_L(0xE004)
#define MDC_HF_ACT_WALK			BYTESWAP16_L(1017)

#ifdef __CC_ARM
    #pragma pack(1)
#endif
/* Types */
typedef struct nomenclature_ascii_struct
{
 OID_Type type;
 int_8 *asciiString;
}NOMENCLATURE_ASCII_STRUCT;

typedef struct partition_ascii_struct
{
 NomPartition partition;
 int_8 *asciiString;
}PARTITION_ASCII_STRUCT;
                                       
extern const NOMENCLATURE_ASCII_STRUCT  NomenclatureAsciiTable[];
extern const PARTITION_ASCII_STRUCT  PartitionAsciiTable[];

extern const uint_16 NomAsciiCount;
extern const uint_16 PartitionAsciiCount;

#endif /* __ieee11073_nom_codes__ */
