#ifndef __NDS32_CONTROLLER_MATH_H__
#define __NDS32_CONTROLLER_MATH_H__
/***************************************************************************
 * nds32_controller_math.h                                                 *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup controller Controller Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"


//void nds32_sin_cos_f32(float32_t theta, float32_t *sin, float32_t *cos);
//void nds32_sin_cos_q31(q31_t theta, q31_t *sin, q31_t *cos);

// Clarke Transform
/**
 * @brief Clarke transform of floating-point input.
 * @param[in]       a       input three-phase coordinate a.
 * @param[in]       b       input three-phase coordinate b.
 * @param[out]      *alpha  output two-phase orthogonal vector axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vector axis beta.
 * @return none.
 */
void nds32_clarke_f32(float32_t a, float32_t b, float32_t *alpha,
                      float32_t *beta);

/**
 * @brief Clarke transform of q31 input.
 * @param[in]       a       input three-phase coordinate a.
 * @param[in]       b       input three-phase coordinate b.
 * @param[out]      *alpha  output two-phase orthogonal vector axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vector axis beta.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
void nds32_clarke_q31(q31_t a, q31_t b, q31_t *alpha, q31_t *beta);

// Inverse Clarke Transform
/**
 * @brief Inverse Clarke transform of floating-point input.
 * @param[in]       alpha   input two-phase orthogonal vector axis alpha.
 * @param[in]       beta    input two-phase orthogonal vector axis beta.
 * @param[out]      *a      output three-phase coordinate a.
 * @param[in]       *b      output three-phase coordinate b.
 * @return none.
 */
void nds32_inv_clarke_f32(float32_t alpha, float32_t beta, float32_t *a,
                          float32_t *b);

/**
 * @brief Inverse Clarke transform of q31 input.
 * @param[in]       alpha   input two-phase orthogonal vector axis alpha.
 * @param[in]       beta    input two-phase orthogonal vector axis beta.
 * @param[out]      *a      output three-phase coordinate a.
 * @param[in]       *b      output three-phase coordinate b.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
void nds32_inv_clarke_q31(q31_t alpha, q31_t beta, q31_t *a, q31_t *b);

// Park Transform
/**
 * @brief Park transform of floating-point input.
 * @param[in]       alpha input two-phase coordinate alpha.
 * @param[in]       beta  input two-phase coordinate beta.
 * @param[out]      *a    output rotor frame a.
 * @param[out]      *b    output rotor frame b.
 * @param[in]       sin   sine value of rotation angle θ.
 * @param[in]       cos   cosine value of rotation angle θ.
 * @return none.
 */
void nds32_park_f32(float32_t alpha, float32_t beta, float32_t *a,
                    float32_t *b, float32_t sin, float32_t cos);

/**
 * @brief Park transform of q31 input.
 * @param[in]       alpha input two-phase coordinate alpha.
 * @param[in]       beta  input two-phase coordinate beta.
 * @param[out]      *a    output rotor frame a.
 * @param[out]      *b    output rotor frame b.
 * @param[in]       sin   sine value of rotation angle θ.
 * @param[in]       cos   cosine value of rotation angle θ.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
void nds32_park_q31(q31_t alpha, q31_t beta, q31_t *a, q31_t *b, q31_t sin,
                    q31_t cos);

// Inverse Park Transform
/**
 * @brief Inverse Park transform of floating-point input.
 * @param[in]       a       input coordinate of rotor frame a.
 * @param[in]       b       input coordinate of rotor frame b.
 * @param[out]      *alpha  output two-phase orthogonal vec axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vec axis beta.
 * @param[in]       sin     sine value of rotation angle θ.
 * @param[in]       cos     cosine value of rotation angle θ.
 * @return none.
 */
void nds32_inv_park_f32(float32_t a, float32_t b, float32_t *alpha,
                        float32_t *beta, float32_t sin, float32_t cos);

/**
 * @brief Inverse Park transform of q31 input.
 * @param[in]       a       input coordinate of rotor frame a.
 * @param[in]       b       input coordinate of rotor frame b.
 * @param[out]      *alpha  output two-phase orthogonal vec axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vec axis beta.
 * @param[in]       sin     sine value of rotation angle θ.
 * @param[in]       cos     cosine value of rotation angle θ.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
void nds32_inv_park_q31(q31_t a, q31_t b, q31_t *alpha, q31_t *beta,
                        q31_t sin, q31_t cos);

/**
 * @brief Structure for the floating-point PID Control.
 */
typedef struct
{
    float32_t gain1;       // gain1 = Kp + Ki + Kd.
    float32_t gain2;       // gain2 = -Kp - 2Kd.
    float32_t gain3;       // gain3 = Kd.
    float32_t state[3];    // State buffer of length 3.
    float32_t Kp;          // Proportional gain, a tuning parameter.
    float32_t Ki;          // Integral gain, a tuning parameter.
    float32_t Kd;          // Derivative gain, a tuning parameter.
} nds32_pid_f32_t;

/**
 * @brief Structure for the Q31 PID Control.
 */
typedef struct
{
    q31_t gain1;       // gain1 = Kp + Ki + Kd.
    q31_t gain2;       // gain2 = -Kp - 2Kd.
    q31_t gain3;       // gain3 = Kd.
    q31_t state[3];    // State buffer of length 3.
    q31_t Kp;          // Proportional gain, a tuning parameter.
    q31_t Ki;          // Integral gain, a tuning parameter.
    q31_t Kd;          // Derivative gain, a tuning parameter.
} nds32_pid_q31_t;

/**
 * @brief Structure for the Q15 PID Control.
 */
typedef struct
{
    q15_t gain1;       // gain1 = Kp + Ki + Kd.
    q15_t gain2;       // gain2 = -Kp - 2Kd.
    q15_t gain3;       // gain3 = Kd.
    q15_t state[3];    // State buffer of length 3.
    q15_t Kp;          // Proportional gain, a tuning parameter.
    q15_t Ki;          // Integral gain, a tuning parameter.
    q15_t Kd;          // Derivative gain, a tuning parameter.
} nds32_pid_q15_t;

// PID Control
/**
 * @brief PID control of floating-point input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
float32_t nds32_pid_f32(nds32_pid_f32_t *instance, float32_t src);

/**
 * @brief PID control of Q15 input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
q15_t nds32_pid_q15(nds32_pid_q15_t *instance, q15_t src);

/**
 * @brief PID control of Q31 input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
q31_t nds32_pid_q31(nds32_pid_q31_t *instance, q31_t src);

// Init PID function
/**
 * @brief PID initializatopn control function of floating-point formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
void nds32_init_pid_f32(nds32_pid_f32_t *instance, int32_t set);

/**
 * @brief PID initializatopn control function of Q15 formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
void nds32_init_pid_q15(nds32_pid_q15_t *instance, int32_t set);

/**
 * @brief PID initializatopn control function of Q31 formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
void nds32_init_pid_q31(nds32_pid_q31_t *instance, int32_t set);
#ifdef  __cplusplus
}
#endif
#endif // __NDS32_TRANSFORM_MATH_H__
