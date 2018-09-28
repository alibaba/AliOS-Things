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
#define PRODUCT_KEY             "a1FWvP18TBY"
#define PRODUCT_SECRET          "T1ZksZAbClinjkj8lCxWu2xq2Z9hNY6Y"
#define DEVICE_NAME             "vmZxkG4I1lZTfN23aDwD"
#define DEVICE_SECRET           "T1ZksZAbClinjkj8lCxWu2xq2Z9hNY6Y"
#else
#define PRODUCT_KEY             "a1nsU3FPZxG"
#define PRODUCT_SECRET          "RjqK1iXdGQjA54g0msef09Y0qxXdfbGp"
#define DEVICE_NAME             "NgxBBivo7dgsK0Yk4R3B"
#define DEVICE_SECRET           "RjqK1iXdGQjA54g0msef09Y0qxXdfbGp"
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GW_DEVICE_H */
