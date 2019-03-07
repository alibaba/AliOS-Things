## Contents

```sh
lorawan
├──linkwan
  ├── linkwan.c       # linkwan source code
  ├── Config.in       # kconfig file
  ├── linkwan.mk      # aos build system file(for make)
  ├── k_app_config.h  # aos app config file
```

## Introduction

The **Lorawan.linkwan** example will run lorawan app, the example is the entrance for lorawan protocal stack.

### Dependencies

* NA

### Supported Boards

- eml3047
- m100c
- m400
- asr6501

### Build

```sh
# generate lorawan.linkwan@m400 default config
aos make lorawan.linkwan@m400 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).


### Result

```sh
[network/lorawan/lorawan_4_4_2/linkwan/region/RegionCN470A.c: 676] Tx, Band 1, Freq: 473300000,DR: 2, len: 23, duration 371, at 54
[network/lorawan/lorawan_4_4_2/linkwan/linkwan.c: 706] Start to Join, method 1, nb_trials:8
[network/lorawan/lorawan_4_4_2/lora/mac/LoRaMac.c:1609] Rx, Freq 473300000, DR 2, window 1
[network/lorawan/lorawan_4_4_2/linkwan/linkwan.c: 735] Joined

```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
