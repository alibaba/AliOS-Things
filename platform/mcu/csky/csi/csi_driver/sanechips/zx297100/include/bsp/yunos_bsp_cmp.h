/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_CMP_H
#define YUNOS_BSP_CMP_H

typedef enum {
	CMP_VINN_0 = 0x0,
	CMP_VINN_1 = 0x1,
	CMP_VINN_2 = 0x2,
	CMP_VINN_3 = 0x3,
	CMP_VINN_4 = 0x4,
	CMP_VINN_5 = 0x5,
	CMP_VINN_6 = 0x5,
	CMP_VINN_7 = 0x7,
}t_cmp_vinn_sel;

typedef enum {
	CMP_VINP_0 = 0x0,
	CMP_VINP_1 = 0x1,
	CMP_VINP_2 = 0x2,
	CMP_VINP_3 = 0x3,
	CMP_VINP_4 = 0x4,
	CMP_VINP_5 = 0x5,
	CMP_VINP_6 = 0x5,
	CMP_VINP_7 = 0x7,
}t_cmp_vinp_sel;


typedef enum {
	CMP_LOWPOWER_300NU = 0x0,
	CMP_LOWPOWER_5UA = 0x1,
	CMP_LOWPOWER_40UA = 0x2,
	CMP_LOWPOWER_200UA = 0x3,
}t_cmp_lowpower;

typedef enum {
	CMP_VDIFF_0_0 = 0x0,
	CMP_VDIFF_1_4 = 0x1,
	CMP_VDIFF_1_2 = 0x2,
	CMP_VDIFF_3_4 = 0x3,
}t_cmp_vdiff;

/*if sel CMP_VOUTSEL_HIGH     
                          VINP[?]>VINN[?] CMP_OUT = 1
						  VINP[?]<VINN[?] CMP_OUT = 0
	if sel CMP_VOUTSEL_LOW     
                          VINP[?]>VINN[?] CMP_OUT = 0
						  VINP[?]<VINN[?] CMP_OUT = 1

*/
typedef enum {
	CMP_VOUTSEL_HIGH = 0x0,
	CMP_VOUTSEL_LOW = 0x1,
}t_cmp_voutsel;

typedef enum {
	CMP_DISEN = 0x0,
	CMP_EN = 0x1,
}t_cmp_en;

typedef enum {
	CMP_HYRSS_NO = 0x0,
	CMP_HYRSS_30MV = 0x1,
}t_cmp_hyrss;

typedef void (*cmp_si_cb)(int);

struct cmp_config_s {
	cmp_si_cb         cb;
	t_cmp_vinn_sel    vinn;	
	t_cmp_vinp_sel    vinp;	
	t_cmp_vdiff       vdiff;
	t_cmp_voutsel     polarity;
	t_cmp_hyrss       hyrss;
	t_cmp_lowpower    mode;
};

/**
 * set cmp configuration
 * @param[in]   id         bus id
 * @param[in]   i2c_config configuration
 * @return      0-success, other-error
 */
int yunos_bsp_cmp_setconfig(struct cmp_config_s *cmp_config);

void yunos_bsp_cmp_en(t_cmp_en flag);

#endif /* YUNOS_BSP_CMP_H */
