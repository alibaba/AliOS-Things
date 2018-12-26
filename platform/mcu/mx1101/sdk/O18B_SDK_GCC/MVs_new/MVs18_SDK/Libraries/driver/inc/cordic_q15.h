/**
 **************************************************************************************
 * @file    cordic_q15.h
 * @brief   CORDIC algorithm code (Q1.15 Fixed Point) to calculate sin & cos value
 *          the function is valid for arguments in range -pi/2 -- pi/2
 *          for values pi/2--pi: value = half_pi-(theta-half_pi) and similarly
 *          for values -pi---pi/2
 *
 * @author  Auto Generated
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __CORDIC_Q15_H__
#define __CORDIC_Q15_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @brief Q1.15 Fixed Point CORDIC algorithm to calculate sin & cos value
 * @param[in]  theta [-pi/2, pi/2], Q2.30
 * @param[out] s sin(theta), Q1.15
 * @param[out] c cos(theta), Q1.15
 * @return void
 */
void cordic_q15(int32_t theta, int16_t* s, int16_t* c);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__CORDIC_Q15_H__
