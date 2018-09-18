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
* $FileName: ieee11073_nom_strings.c$
* $Version : 1.0.0..0$
* $Date    : Jan-29-2010$
*
* Comments:
*
* This file contains the nomenclature strings equivalents of the IEEE 11073 nomenclature
* codes. It is used by the manager to print the OID-Type attributes and partition codes.
*
*END************************************************************************/


#include "types.h"
#ifdef LITTLE_ENDIAN
#include "ieee11073_phd_types_kinetis.h"
#include "ieee11073_nom_codes_kinetis.h"
#else
#include "ieee11073_phd_types.h"
#include "ieee11073_nom_codes.h"
#endif

/* Table definition */
const NOMENCLATURE_ASCII_STRUCT  NomenclatureAsciiTable[] =
{
 /* From Object Infrastructure (MDC_PART_OBJ) */
 {MDC_ATTR_ID_TYPE,          (int_8*)"MDC_ATTR_ID_TYPE"},
 {MDC_ATTR_METRIC_SPEC_SMALL,(int_8*)"MDC_ATTR_METRIC_SPEC_SMALL"},
 {MDC_ATTR_UNIT_CODE,        (int_8*)"MDC_ATTR_UNIT_CODE"},
 {MDC_ATTR_ATTRIBUTE_VAL_MAP,(int_8*)"MDC_ATTR_ATTRIBUTE_VAL_MAP"},
 /* From Medical supervisory control and data acquisition (MDC_PART_SCADA) */
 {MDC_TEMP_BODY,             (int_8*)"MDC_TEMP_BODY"},
 {MDC_PRESS_BLD_NONINV_SYS,  (int_8*)"MDC_PRESS_BLD_NONINV_SYS"},
 {MDC_PRESS_BLD_NONINV_DIA,  (int_8*)"MDC_PRESS_BLD_NONINV_DIA"},
 {MDC_PRESS_BLD_NONINV_MEAN, (int_8*)"MDC_PRESS_BLD_NONINV_MEAN"},
 {MDC_MASS_BODY_ACTUAL,      (int_8*)"MDC_MASS_BODY_ACTUAL"},
 {MDC_LEN_BODY_ACTUAL,       (int_8*)"MDC_LEN_BODY_ACTUAL"},
 /* From Dimensions (MDC_PART_DIM) */
 {MDC_DIM_PERCENT,           (int_8*)"%"},
 {MDC_DIM_KILO_G,            (int_8*)"kg"},
 {MDC_DIM_MIN,               (int_8*)"min"},
 {MDC_DIM_HR,                (int_8*)"h"},
 {MDC_DIM_DAY,               (int_8*)"d"},
 {MDC_DIM_DEGC,              (int_8*)"degr C"},
 {MDC_DIM_MMHG,              (int_8*)"mm Hg"},
 {MDC_DIM_CENTI_M,           (int_8*)"cm"},
 /* From Communication Infrastructure (MDC_PART_INFRA) */
 {MDC_DEV_SPEC_PROFILE_PULS_OXIM,  (int_8*)"MDC_DEV_SPEC_PROFILE_PULS_OXIM"},
 {MDC_DEV_SPEC_PROFILE_BP,         (int_8*)"MDC_DEV_SPEC_PROFILE_BP"},
 {MDC_DEV_SPEC_PROFILE_TEMP,       (int_8*)"MDC_DEV_SPEC_PROFILE_TEMP"},
 {MDC_DEV_SPEC_PROFILE_SCALE,      (int_8*)"MDC_DEV_SPEC_PROFILE_SCALE"},
 {MDC_DEV_SPEC_PROFILE_GLUCOSE,    (int_8*)"MDC_DEV_SPEC_PROFILE_GLUCOSE"},
 {MDC_DEV_SPEC_PROFILE_HF_CARDIO,  (int_8*)"MDC_DEV_SPEC_PROFILE_HF_CARDIO"},
 {MDC_DEV_SPEC_PROFILE_HF_STRENGTH,(int_8*)"MDC_DEV_SPEC_PROFILE_HF_STRENGTH"}  
};

/* Table definition */
const PARTITION_ASCII_STRUCT  PartitionAsciiTable[] =
{
 {MDC_PART_OBJ,              (int_8*)"MDC_PART_OBJ"},
 {MDC_PART_SCADA,            (int_8*)"MDC_PART_SCADA"},
 {MDC_PART_DIM,              (int_8*)"MDC_PART_DIM"},
 {MDC_PART_INFRA,            (int_8*)"MDC_PART_INFRA"},
 {MDC_PART_PHD_DM,           (int_8*)"MDC_PART_PHD_DM"},
 {MDC_PART_PHD_HF,           (int_8*)"MDC_PART_PHD_HF"},
 {MDC_PART_PHD_AI,           (int_8*)"MDC_PART_PHD_AI"},
 {MDC_PART_RET_CODE,         (int_8*)"MDC_PART_RET_CODE"},
 {MDC_PART_EXT_NOM,          (int_8*)"MDC_PART_EXT_NOM"}
};

const uint_16 NomAsciiCount = sizeof(NomenclatureAsciiTable)/sizeof(NomenclatureAsciiTable[0]);
const uint_16 PartitionAsciiCount = sizeof(PartitionAsciiTable)/sizeof(PartitionAsciiTable[0]);
