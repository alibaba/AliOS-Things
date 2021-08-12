/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <math.h>
#include "drv_baro_qst_qmp6988.h"
#include "aos/hal/i2c.h"
#include "ulog/ulog.h"
#include <stdio.h>
#include <stdlib.h>

#define QMP6988_LOG
#define QMP6988_ERR printf

#if !defined(QMP6988_CALC_INT)
static float const Conv_A_S[10][2] = {
    {-6.30E-03, 4.30E-04},
    {-1.90E-11, 1.20E-10},
    {1.00E-01, 9.10E-02},
    {1.20E-08, 1.20E-06},
    {3.30E-02, 1.90E-02},
    {2.10E-07, 1.40E-07},
    {-6.30E-10, 3.50E-10},
    {2.90E-13, 7.60E-13},
    {2.10E-15, 1.20E-14},
    {1.30E-16, 7.90E-17},
};
#endif

static void qmp6988_delay(unsigned int delay)
{
    int i, j;
    for (i = 0; i < delay; i++) {
        for (j = 0; j < 1000; j++) {
            ;
        }
    }
}

uint8_t qmp6988_WriteReg(uint8_t slave, uint8_t reg_add, uint8_t reg_dat)
{
#if defined(QST_USE_SPI)
    return qmp6988_spi_write(reg_add, reg_dat);
#elif defined(QST_USE_SW_I2C)
    return qst_sw_writereg(slave, reg_add, reg_dat);
#else
    uint8_t write_buf[2];
    write_buf[0] = reg_add;
    write_buf[1] = reg_dat;
    sensor_i2c_master_send(1, slave, write_buf, 2, 1000);
#endif
}

uint8_t qmp6988_ReadData(uint8_t slave, uint8_t reg_add, unsigned char *Read, uint8_t num)
{
#if defined(QST_USE_SPI)
    return qmp6988_spi_read(reg_add, Read, num);
#elif defined(QST_USE_SW_I2C)
    return qst_sw_readreg(slave, reg_add, Read, num);
#else
    uint8_t data;
    sensor_i2c_master_send(1, slave, &reg_add, 1, 1000);
    aos_msleep(2);
    sensor_i2c_master_recv(1, slave, Read, num, 1000);
    return 1;
#endif
}

static uint8_t qmp6988_device_check(qmp6988_data *qmp6988)
{
    uint8_t slave_addr[1] = {QMP6988_SLAVE_ADDRESS_H};
    uint8_t ret = 0;
    uint8_t i;

    for (i = 0; i < 1; i++) {
        qmp6988->slave = slave_addr[i];
        ret = qmp6988_ReadData(qmp6988->slave, QMP6988_CHIP_ID_REG, &(qmp6988->chip_id), 1);
        if (ret == 0) {
            QMP6988_LOG("%s: read 0xD1 failed\n", __func__);
            continue;
        }
        QMP6988_LOG("qmp6988 read chip id = 0x%x\n", qmp6988->chip_id);
        if (qmp6988->chip_id == QMP6988_CHIP_ID) {
            return 1;
        }
    }

    return 0;
}

static int qmp6988_get_calibration_data(qmp6988_data *qmp6988)
{
    int status = 0;
    // BITFIELDS temp_COE;
    uint8_t a_data_u8r[QMP6988_CALIBRATION_DATA_LENGTH] = {0};
    int len;

    for (len = 0; len < QMP6988_CALIBRATION_DATA_LENGTH; len += 1) {
        // if((qmp6988_cali_data_LENGTH-len) >= 8 )
        //  status = qmp6988_ReadData(qmp6988_cali_data_START+len,&a_data_u8r[len],8);
        // else
        //  status = qmp6988_ReadData(qmp6988_cali_data_START+len,&a_data_u8r[len],(qmp6988_cali_data_LENGTH-len));
        status = qmp6988_ReadData(qmp6988->slave, QMP6988_CALIBRATION_DATA_START + len, &a_data_u8r[len], 1);
        if (status == 0) {
            QMP6988_LOG("qmp6988 read 0xA0 error!");
            return status;
        }
    }

    qmp6988->qmp6988_cali.COE_a0 = (QMP6988_S32_t)(((a_data_u8r[18] << SHIFT_LEFT_12_POSITION) \
                                   | (a_data_u8r[19] << SHIFT_LEFT_4_POSITION) \
                                   | (a_data_u8r[24] & 0x0f)) << 12);
    qmp6988->qmp6988_cali.COE_a0 = qmp6988->qmp6988_cali.COE_a0 >> 12;

    qmp6988->qmp6988_cali.COE_a1 = (QMP6988_S16_t)(((a_data_u8r[20]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[21]);
    qmp6988->qmp6988_cali.COE_a2 = (QMP6988_S16_t)(((a_data_u8r[22]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[23]);

    qmp6988->qmp6988_cali.COE_b00 = (QMP6988_S32_t)(((a_data_u8r[0] << SHIFT_LEFT_12_POSITION) \
                                    | (a_data_u8r[1] << SHIFT_LEFT_4_POSITION) \
                                    | ((a_data_u8r[24] & 0xf0) >> SHIFT_RIGHT_4_POSITION)) << 12);
    qmp6988->qmp6988_cali.COE_b00 = qmp6988->qmp6988_cali.COE_b00 >> 12;

    qmp6988->qmp6988_cali.COE_bt1 = (QMP6988_S16_t)(((a_data_u8r[2]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[3]);
    qmp6988->qmp6988_cali.COE_bt2 = (QMP6988_S16_t)(((a_data_u8r[4]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[5]);
    qmp6988->qmp6988_cali.COE_bp1 = (QMP6988_S16_t)(((a_data_u8r[6]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[7]);
    qmp6988->qmp6988_cali.COE_b11 = (QMP6988_S16_t)(((a_data_u8r[8]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[9]);
    qmp6988->qmp6988_cali.COE_bp2 = (QMP6988_S16_t)(((a_data_u8r[10]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[11]);
    qmp6988->qmp6988_cali.COE_b12 = (QMP6988_S16_t)(((a_data_u8r[12]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[13]);
    qmp6988->qmp6988_cali.COE_b21 = (QMP6988_S16_t)(((a_data_u8r[14]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[15]);
    qmp6988->qmp6988_cali.COE_bp3 = (QMP6988_S16_t)(((a_data_u8r[16]) << SHIFT_LEFT_8_POSITION) | a_data_u8r[17]);

    QMP6988_LOG("<-----------calibration data-------------->\n");
    QMP6988_LOG("COE_a0[%d] COE_a1[%d] COE_a2[%d] COE_b00[%d]\n", \
        qmp6988->qmp6988_cali.COE_a0, qmp6988->qmp6988_cali.COE_a1, qmp6988->qmp6988_cali.COE_a2, qmp6988->qmp6988_cali.COE_b00);
    QMP6988_LOG("COE_bt1[%d] COE_bt2[%d] COE_bp1[%d] COE_b11[%d]\n", \
        qmp6988->qmp6988_cali.COE_bt1, qmp6988->qmp6988_cali.COE_bt2, qmp6988->qmp6988_cali.COE_bp1, qmp6988->qmp6988_cali.COE_b11);
    QMP6988_LOG("COE_bp2[%d] COE_b12[%d] COE_b21[%d] COE_bp3[%d]\n", \
        qmp6988->qmp6988_cali.COE_bp2, qmp6988->qmp6988_cali.COE_b12, qmp6988->qmp6988_cali.COE_b21, qmp6988->qmp6988_cali.COE_bp3);
    QMP6988_LOG("<-----------calibration data-------------->\n");

#if defined(QMP6988_CALC_INT)
    qmp6988->ik.a0 = qmp6988->qmp6988_cali.COE_a0; // 20Q4
    qmp6988->ik.b00 = qmp6988->qmp6988_cali.COE_b00; // 20Q4

    qmp6988->ik.a1 = 3608L * (QMP6988_S32_t)qmp6988->qmp6988_cali.COE_a1 - 1731677965L; // 31Q23
    qmp6988->ik.a2 = 16889L * (QMP6988_S32_t) qmp6988->qmp6988_cali.COE_a2 - 87619360L; // 30Q47

    qmp6988->ik.bt1 = 2982L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_bt1 + 107370906L; // 28Q15
    qmp6988->ik.bt2 = 329854L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_bt2 + 108083093L; // 34Q38
    qmp6988->ik.bp1 = 19923L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_bp1 + 1133836764L; // 31Q20
    qmp6988->ik.b11 = 2406L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_b11 + 118215883L; // 28Q34
    qmp6988->ik.bp2 = 3079L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_bp2 - 181579595L; // 29Q43
    qmp6988->ik.b12 = 6846L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_b12 + 85590281L; // 29Q53
    qmp6988->ik.b21 = 13836L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_b21 + 79333336L; // 29Q60
    qmp6988->ik.bp3 = 2915L * (QMP6988_S64_t)qmp6988->qmp6988_cali.COE_bp3 + 157155561L; // 28Q65
/*
    QMP6988_LOG("<----------- int calibration data -------------->\n");
    QMP6988_LOG("a0[%d] a1[%d] a2[%d] b00[%d]\n", qmp6988->ik.a0, qmp6988->ik.a1, qmp6988->ik.a2, qmp6988->ik.b00);
    QMP6988_LOG("bt1[%lld] bt2[%lld] bp1[%lld] b11[%lld]\n", qmp6988->ik.bt1, qmp6988->ik.bt2, qmp6988->ik.bp1, qmp6988->ik.b11);
    QMP6988_LOG("bp2[%lld] b12[%lld] b21[%lld] bp3[%lld]\n", qmp6988->ik.bp2, qmp6988->ik.b12, qmp6988->ik.b21, qmp6988->ik.bp3);
    QMP6988_LOG("<----------- int calibration data -------------->\n");
*/
#else
    qmp6988->fk.a0 = qmp6988->qmp6988_cali.COE_a0 / 16.0f;
    qmp6988->fk.b00 = qmp6988->qmp6988_cali.COE_b00 / 16.0f;

    qmp6988->fk.a1 = Conv_A_S[0][0] + Conv_A_S[0][1] * qmp6988->qmp6988_cali.COE_a1 / 32767.0f;
    qmp6988->fk.a2 = Conv_A_S[1][0] + Conv_A_S[1][1] * qmp6988->qmp6988_cali.COE_a2 / 32767.0f;
    qmp6988->fk.bt1 = Conv_A_S[2][0] + Conv_A_S[2][1] * qmp6988->qmp6988_cali.COE_bt1 / 32767.0f;
    qmp6988->fk.bt2 = Conv_A_S[3][0] + Conv_A_S[3][1] * qmp6988->qmp6988_cali.COE_bt2 / 32767.0f;
    qmp6988->fk.bp1 = Conv_A_S[4][0] + Conv_A_S[4][1] * qmp6988->qmp6988_cali.COE_bp1 / 32767.0f;
    qmp6988->fk.b11 = Conv_A_S[5][0] + Conv_A_S[5][1] * qmp6988->qmp6988_cali.COE_b11 / 32767.0f;
    qmp6988->fk.bp2 = Conv_A_S[6][0] + Conv_A_S[6][1] * qmp6988->qmp6988_cali.COE_bp2 / 32767.0f;
    qmp6988->fk.b12 = Conv_A_S[7][0] + Conv_A_S[7][1] * qmp6988->qmp6988_cali.COE_b12 / 32767.0f;
    qmp6988->fk.b21 = Conv_A_S[8][0] + Conv_A_S[8][1] * qmp6988->qmp6988_cali.COE_b21 / 32767.0f;
    qmp6988->fk.bp3 = Conv_A_S[9][0] + Conv_A_S[9][1] * qmp6988->qmp6988_cali.COE_bp3 / 32767.0f;
/*
    QMP6988_LOG("<----------- float calibration data -------------->\n");
    QMP6988_LOG("a0[%lle] a1[%lle] a2[%lle] b00[%lle]\n", qmp6988->fk.a0, qmp6988->fk.a1, qmp6988->fk.a2, qmp6988->fk.b00);
    QMP6988_LOG("bt1[%lle] bt2[%lle] bp1[%lle] b11[%lle]\n", qmp6988->fk.bt1, qmp6988->fk.bt2, qmp6988->fk.bp1, qmp6988->fk.b11);
    QMP6988_LOG("bp2[%lle] b12[%lle] b21[%lle] bp3[%lle]\n", qmp6988->fk.bp2, qmp6988->fk.b12, qmp6988->fk.b21, qmp6988->fk.bp3);
    QMP6988_LOG("<----------- float calibration data -------------->\n");
*/
#endif

    return 1;
}

#if defined(QMP6988_CALC_INT)
static QMP6988_S16_t qmp6988_convTx_02e(qmp6988_ik_data *ik, QMP6988_S32_t dt)
{
    QMP6988_S16_t ret;
    QMP6988_S64_t wk1, wk2;

    // wk1: 60Q4 // bit size
    wk1 = ((QMP6988_S64_t)ik->a1 * (QMP6988_S64_t)dt); // 31Q23+24-1=54 (54Q23)
    wk2 = ((QMP6988_S64_t)ik->a2 * (QMP6988_S64_t)dt) >> 14; // 30Q47+24-1=53 (39Q33)
    wk2 = (wk2 * (QMP6988_S64_t)dt) >> 10; // 39Q33+24-1=62 (52Q23)
    wk2 = ((wk1 + wk2) / 32767) >> 19; // 54,52->55Q23 (20Q04)
    ret = (QMP6988_S16_t)((ik->a0 + wk2) >> 4); // 21Q4 -> 17Q0
    return ret;
}

static QMP6988_S32_t qmp6988_get_pressure_02e(qmp6988_ik_data *ik, QMP6988_S32_t dp, QMP6988_S16_t tx)
{
    QMP6988_S32_t ret;
    QMP6988_S64_t wk1, wk2, wk3;

    // wk1 = 48Q16 // bit size
    wk1 = ((QMP6988_S64_t)ik->bt1 * (QMP6988_S64_t)tx); // 28Q15+16-1=43 (43Q15)
    wk2 = ((QMP6988_S64_t)ik->bp1 * (QMP6988_S64_t)dp) >> 5; // 31Q20+24-1=54 (49Q15)
    wk1 += wk2; // 43,49->50Q15
    wk2 = ((QMP6988_S64_t)ik->bt2 * (QMP6988_S64_t)tx) >> 1; // 34Q38+16-1=49 (48Q37)
    wk2 = (wk2 * (QMP6988_S64_t)tx) >> 8; // 48Q37+16-1=63 (55Q29)
    wk3 = wk2; // 55Q29
    wk2 = ((QMP6988_S64_t)ik->b11 * (QMP6988_S64_t)tx) >> 4; // 28Q34+16-1=43 (39Q30)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1; // 39Q30+24-1=62 (61Q29)
    wk3 += wk2; // 55,61->62Q29
    wk2 = ((QMP6988_S64_t)ik->bp2 * (QMP6988_S64_t)dp) >> 13; // 29Q43+24-1=52 (39Q30)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1; // 39Q30+24-1=62 (61Q29)
    wk3 += wk2; // 62,61->63Q29
    wk1 += wk3 >> 14; // Q29 >> 14 -> Q15
    wk2 = ((QMP6988_S64_t)ik->b12 * (QMP6988_S64_t)tx); // 29Q53+16-1=45 (45Q53)
    wk2 = (wk2 * (QMP6988_S64_t)tx) >> 22; // 45Q53+16-1=61 (39Q31)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1; // 39Q31+24-1=62 (61Q30)
    wk3 = wk2; // 61Q30
    wk2 = ((QMP6988_S64_t)ik->b21 * (QMP6988_S64_t)tx) >> 6; // 29Q60+16-1=45 (39Q54)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 23; // 39Q54+24-1=62 (39Q31)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 1; // 39Q31+24-1=62 (61Q20)
    wk3 += wk2; // 61,61->62Q30
    wk2 = ((QMP6988_S64_t)ik->bp3 * (QMP6988_S64_t)dp) >> 12; // 28Q65+24-1=51 (39Q53)
    wk2 = (wk2 * (QMP6988_S64_t)dp) >> 23; // 39Q53+24-1=62 (39Q30)
    wk2 = (wk2 * (QMP6988_S64_t)dp); // 39Q30+24-1=62 (62Q30)
    wk3 += wk2; // 62,62->63Q30
    wk1 += wk3 >> 15; // Q30 >> 15 = Q15
    wk1 /= 32767L;
    wk1 >>= 11; // Q15 >> 7 = Q4
    wk1 += ik->b00; // Q4 + 20Q4
    // wk1 >>= 4; // 28Q4 -> 24Q0
    ret = (QMP6988_S32_t)wk1;
    return ret;
}
#endif

static void qmp6988_software_reset(qmp6988_data *qmp6988)
{
/*
    uint8_t ret = 0;

    ret = qmp6988_WriteReg(qmp6988->slave, QMP6988_RESET_REG, 0xe6);
    if (ret == 0) {
        QMP6988_LOG("qmp6988_software_reset fail!!!\n");
    }
    qmp6988_delay(20);
    ret = qmp6988_WriteReg(qmp6988->slave, QMP6988_RESET_REG, 0x00);
*/
}

static void qmp6988_set_powermode(qmp6988_data *qmp6988, int power_mode)
{
    uint8_t data;

    QMP6988_LOG("qmp_set_powermode %d\n", power_mode);
    // if(power_mode == qmp6988->power_mode)
    //  return;

    qmp6988->power_mode = power_mode;
    qmp6988_ReadData(qmp6988->slave, QMP6988_CTRLMEAS_REG, &data, 1);
    data = data & 0xfc;
    if (power_mode == QMP6988_SLEEP_MODE) {
        data |= 0x00;
    } else if (power_mode == QMP6988_FORCED_MODE) {
        data |= 0x01;
    } else if (power_mode == QMP6988_NORMAL_MODE) {
        data |= 0x03;
    }
    qmp6988_WriteReg(qmp6988->slave, QMP6988_CTRLMEAS_REG, data);

    QMP6988_LOG("qmp_set_powermode 0xf4=0x%x\n", data);

    qmp6988_delay(20);
}


static void qmp6988_set_filter(qmp6988_data *qmp6988, unsigned char filter)
{
    uint8_t data;

    data = (filter & 0x03);
    qmp6988_WriteReg(qmp6988->slave, QMP6988_CONFIG_REG, data);

    qmp6988_delay(20);
}

static void qmp6988_set_oversampling_p(qmp6988_data *qmp6988, unsigned char oversampling_p)
{
    uint8_t data;

    qmp6988_ReadData(qmp6988->slave, QMP6988_CTRLMEAS_REG, &data, 1);
    data &= 0xe3;
    data |= (oversampling_p << 2);
    qmp6988_WriteReg(qmp6988->slave, QMP6988_CTRLMEAS_REG, data);
    qmp6988_delay(20);
}

static void qmp6988_set_oversampling_t(qmp6988_data *qmp6988, unsigned char oversampling_t)
{
    uint8_t data;

    qmp6988_ReadData(qmp6988->slave, QMP6988_CTRLMEAS_REG, &data, 1);
    data &= 0x1f;
    data |= (oversampling_t << 5);
    qmp6988_WriteReg(qmp6988->slave, QMP6988_CTRLMEAS_REG, data);
    qmp6988_delay(20);
}

float qmp6988_calc_altitude(float pressure, float temp)
{
    float altitude;

    altitude = (pow((101325 / pressure), 1 / 5.257) - 1) * (temp + 273.15) / 0.0065;
    // QMP6988_LOG("altitude = %f\n" ,altitude);
    return altitude;
}

float qmp6988_calc_pressure(qmp6988_data *qmp6988)
{
    uint8_t err = 0;
    //  uint8_t retry_count = 0;
    QMP6988_U32_t P_read, T_read;
    QMP6988_S32_t P_raw, T_raw;
    uint8_t a_data_u8r[6] = {0};
#if defined(QMP6988_CALC_INT)
    QMP6988_S32_t T_int, P_int;
#else
    float a0, b00;
    float a1, a2, bt1, bt2, bp1, b11, bp2, b12, b21, bp3;
    double Tr;
#endif

/*
    a_data_u8r[0] = 0x08;
    retry_count = 0;

    while (a_data_u8r[0] & 0x08) {
        err = qmp6988_ReadData(QMP6988_DEVICE_STAT_REG, a_data_u8r, 1);
        if (err == 0) {
            QMP6988_LOG("qmp6988 read status reg error!\n");
            return;
        }
        QMP6988_LOG("qmp6988 read status 0xf3 = 0x%02x\n", a_data_u8r[0]);
        qmp6988_delay(10);
        retry_count++;
        if (retry_count > 5)
            return;
    }
*/
    // press
    err = qmp6988_ReadData(qmp6988->slave, QMP6988_PRESSURE_MSB_REG, a_data_u8r, 6);
    if (err == 0) {
        QMP6988_LOG("qmp6988 read press raw error!\n");
        return 0.0f;
    }
    P_read = (QMP6988_U32_t)(
                 (((QMP6988_U32_t)(a_data_u8r[0])) << SHIFT_LEFT_16_POSITION) |
                 (((QMP6988_U16_t)(a_data_u8r[1])) << SHIFT_LEFT_8_POSITION) | (a_data_u8r[2]));
    P_raw = (QMP6988_S32_t)(P_read - SUBTRACTOR);

    /*
    // temp
    err = qmp6988_ReadData(QMP6988_TEMPERATURE_MSB_REG, a_data_u8r, 3);
    if (err == 0) {
        QMP6988_LOG("qmp6988 read temp raw error!\n");
    }
    */

    T_read = (QMP6988_U32_t)(
                 (((QMP6988_U32_t)(a_data_u8r[3])) << SHIFT_LEFT_16_POSITION) |
                 (((QMP6988_U16_t)(a_data_u8r[4])) << SHIFT_LEFT_8_POSITION) | (a_data_u8r[5]));
    T_raw = (QMP6988_S32_t)(T_read - SUBTRACTOR);

#if defined(QMP6988_CALC_INT)
    T_int = qmp6988_convTx_02e(&(qmp6988->ik), T_raw);
    P_int = qmp6988_get_pressure_02e(&(qmp6988->ik), P_raw, T_int);
    qmp6988->temperature = (float)T_int / 256.0f;
    qmp6988->pressure = (float)P_int / 16.0f;
    //  QMP6988_LOG("int temp=%f   Pressure = %f\n",qmp6988->temperature, qmp6988->pressure);
#else
    a0 = qmp6988->fk.a0;
    b00 = qmp6988->fk.b00;
    a1 = qmp6988->fk.a1;
    a2 = qmp6988->fk.a2;
    bt1 = qmp6988->fk.bt1;
    bt2 = qmp6988->fk.bt2;
    bp1 = qmp6988->fk.bp1;
    b11 = qmp6988->fk.b11;
    bp2 = qmp6988->fk.bp2;
    b12 = qmp6988->fk.b12;
    b21 = qmp6988->fk.b21;
    bp3 = qmp6988->fk.bp3;

    Tr = a0 + (a1 * T_raw) + (a2 * T_raw) *T_raw;
    // Unit centigrade
    qmp6988->temperature = Tr / 256.0f;
    // compensation pressure, Unit Pa
    qmp6988->pressure = b00 + (bt1 * Tr) + (bp1 * P_raw) + (b11 * Tr) *P_raw + (bt2 * Tr) *Tr + (bp2 * P_raw) *P_raw + (b12 * P_raw) *(Tr *Tr) + (b21 * P_raw) *(P_raw *Tr) + (bp3 * P_raw) *(P_raw *P_raw);
    QMP6988_LOG("float temp=%f Pressure = %f\n", qmp6988->temperature, qmp6988->pressure);
#endif
    qmp6988->altitude = qmp6988_calc_altitude(qmp6988->pressure, qmp6988->temperature);
    // QMP6988_LOG("altitude = %f\n",qmp6988->altitude);
    return qmp6988->pressure;
}

uint8_t qmp6988_init(qmp6988_data *qmp6988)
{
    int32_t ret = sensor_i2c_open(1, QMP6988_SLAVE_ADDRESS_H, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        printf("sensor i2c open failed, ret:%d\n", ret);
        return;
    }

    ret = qmp6988_device_check(qmp6988);
    if (ret == 0) {
        return 0;
    }

    qmp6988_software_reset(qmp6988);
    qmp6988_get_calibration_data(qmp6988);
    qmp6988_set_powermode(qmp6988, QMP6988_NORMAL_MODE);
    qmp6988_set_filter(qmp6988, QMP6988_FILTERCOEFF_OFF);
    qmp6988_set_oversampling_p(qmp6988, QMP6988_OVERSAMPLING_2X);
    qmp6988_set_oversampling_t(qmp6988, QMP6988_OVERSAMPLING_1X);

    return 1;
}

void qmp6988_deinit(void)
{
    int32_t ret = sensor_i2c_close(1);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }
    return;
}
