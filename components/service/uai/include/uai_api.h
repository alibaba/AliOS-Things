/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_API_H
#define UAI_API_H

#include "uai_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_DNN
/**
 * init dnn configuration.
 *
 * @param[in]  model_config     configuration information.
 * @param[in]  model_data_src   model data source description.
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_dnn_init(uai_dnn_config_t *model_config, char *model_data_src);

/*
 * deinit dnn, free allocated memory
 *
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_dnn_deinit(void);

/*
 * run dnn model
 *
 *
 * @param  in_data    input data
 * @param  out_data   output data
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_dnn_run(int8_t *in_data, int8_t *out_data);
#endif

#ifdef UAI_CNN
/**
 * init cnn configuration.
 *
 * @param[in]  model_config     configuration information.
 * @param[in]  model_data_src   model data source description.
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_cnn_init(uai_cnn_config_t *model_config, char *model_data_src);

/*
 * deinit cnn, free allocated memory
 *
 *
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_cnn_deinit(void);

/*
 * run dnn model
 *
 *
 * @param  in_data    input data
 * @param  out_data   output data
 * @return  UAI_SUCCESS: success; other: fail.
 */
int32_t uai_cnn_run(int8_t *in_data, int8_t *out_data);
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of UAI_API_H */
