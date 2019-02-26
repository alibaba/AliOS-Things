#ifndef GW_DEVICE_H
#define GW_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef  PT_SCANNER
#define PRODUCT_KEY             "a1nd0MZYM0i"
#define DEVICE_NAME             "oJIVGf4fEQg8A3kuQskO"
#define DEVICE_SECRET           "QNbKukzArV2fMKd6iQAQO2bpj4IvP4YH"
#define PRODUCT_SECRET          "QNbKukzArV2fMKd6iQAQO2bpj4IvP4YH"

#elif PT_SENSOR
#define PRODUCT_KEY             "a1dvDr5NE2D"
#define PRODUCT_SECRET          "T1ZksZAbClinjkj8lCxWu2xq2Z9hNY6Y"
#define DEVICE_NAME             "Nctj2cMVtiRdaOWeABtd"
#define DEVICE_SECRET           "OGGDpyMK5hgJ17HSjUHNmfxnCmNNLy7y"

#define PROP_POST_FORMAT_HUMI         "{\"CurrentHumidity\":%.1f}"
#define PROP_POST_FORMAT_TEMP         "{\"CurrentTemperature\":%.1f}"
#define PROP_POST_FORMAT_ACC          "{\"Accelerometer\":{\"X\":%.2f, \"Y\":%.2f, \"Z\":%.2f}}"
#define PROP_POST_FORMAT_GYRO         "{\"Gyroscope\":{\"x_dps\":%d, \"y_dps\":%d, \"z_dps\":%d}}"
#define PROP_POST_FORMAT_RTC          "{\"RTC\":{\"year\":%d, \"month\":%d, \"date\":%d, \"day\":%d, \"hours\":%d, \"minutes\":%d, \"seconds\":%d}}"

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GW_DEVICE_H */
