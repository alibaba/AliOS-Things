## Contents

```sh
uagent/
├── aos.mk
├── Config.in
├── include
│   └── uagent_config.h
├── README.md
├── uagent_comm_mqtt.c
├── uagent_core.c
├── uagent_inner_api.h
├── uagent_send.c
└── uagent_utility.c
```

## Introduction

uagent is module of AliOS Things, supports other customer module(ulog, cli, ota, uND) register their service into it. An service recall from cloud(oriented from maintain platform) can call the device's service via uagent, uagent also helps the customer module send out the information help show its work status.

## Features

resource cost: ROM < 6K, RAM: 7.5K~9K, which allocate on below sites:
1. Asynchronized mechanism cost 4K task depth and 2K queue size
2. Normal static data and heap cost 1.5K.
3. If delay-send mechanism is invoked which happens on cli mount to uagent, then 1.5K extra size costed.

## Dependencies

mqtt: To subscribe /user/${ProductKey}/${DeviceName}/device/man and publish /user/${ProductKey}/${DeviceName}/device/info.
cjson: To parser payload.

## API

User service APIs:

```c
int uagent_init(void);

int uagent_ext_comm_start(const char *pk, const char *dn);

int uagent_register(const ua_mod_t mod, const char *mod_name, char *version,
                    const ua_func_t func, const char *func_name,
                    on_customer_service cb, void *arg);

int uagent_unregister(const ua_mod_t mod, const ua_func_t func);

int uagent_request_service(const ua_mod_t src, const ua_mod_t target,
                           const ua_func_t func, const unsigned short len,
                           const void *info_str);

int uagent_send(const ua_mod_t mod, const ua_func_t type,
                const unsigned short len, const void *info_str,
                const ua_send_policy_t policy);

```

## Simple use it
Step 1. uagent_init() will be called in kernel/init if uagent module is selected for using, user do not need care this process.
Step 2. Call uagent_ext_comm_start() and tell the uagent the ProductKey and DeviceName, which is ncessary for uagent to subscribe topic and make up publish topic before send any payload.
Step 3. User module shall call uagent_register() to register its service into uagent. Focus on parameter 'cb', this call-back function will be called once the uagent receive the related command from cloud, the key word of 'related comand' is that combined of parameter mod and parameter func.
Step 4. The customer module can post its information via uagent_send(), customer do not need care the topic, but only make up the payload.
Step 5. Any other modules, nevertheless it has register its own service into uagent, can call any other customer's service by uagent_request_service(), the later should complete registering service into uagent.
Step 6. If the customer module would like to remove its service from uagent, it shall call uagent_unregister() to unregister it.

By now, the module ulog, ota, uND has mount some of their service into uagent. You can refer this module on how to use the uagent.

## Test hardware
linuxhost, HF-LPX30(uno-91h), mk3080, esp32.
