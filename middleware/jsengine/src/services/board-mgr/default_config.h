/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_DEFAULT_CONFIG_H
#define BE_DEFAULT_CONFIG_H

static const char* default_config =
    "{\"UART\":[{\"id\":\"uart2\",\"port\":2,\"data_width\":3,\"baud_rate\":"
    "115200,\"stop_bits\":0,\"flow_control\":0,\"parity_config\":0}],\"GPIO\":["
    "{\"id\":\"gled\",\"port\":60,\"dir\":0,\"pull\":1},{\"id\":\"key\","
    "\"port\":74,\"dir\":2,\"pull\":1}],\"I2C\":[{\"id\":\"shtc1\",\"port\":2,"
    "\"address_width\":7,\"freq\":100000,\"mode\":1,\"dev_addr\":224},{\"id\":"
    "\"bmp280\",\"port\":2,\"address_width\":7,\"freq\":400000,\"mode\":1,"
    "\"dev_addr\":238},{\"id\":\"ltr553\",\"port\":2,\"address_width\":7,"
    "\"freq\":200000,\"mode\":1,\"dev_addr\":70},{\"id\":\"mmc3680kj\","
    "\"port\":1,\"address_width\":7,\"freq\":200000,\"mode\":1,\"dev_addr\":96}"
    ",{\"id\":\"lsm6dsl\",\"port\":1,\"address_width\":7,\"freq\":200000,"
    "\"mode\":1,\"dev_addr\":214}]}";

#endif /* BE_DEFAULT_CONFIG_H */
