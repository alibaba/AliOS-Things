# prov sample
  
It is a sample for id2 provisioning in alios things. 
## Contents

```sh
prov_app
└── app_entry.c        # id2 provisioning test demo code
```

## Introduction

This example is an local test code for id2 provisioning.

### Requirements

Need to replace id2 test data with real data (get from id2 datasource sdk) first.

### Features

* id2 provisioning
* id2 erasure (building with "ERASE=1")

### Dependencies

* cli
* prov

### Supported Boards

- mk3060
- mk3080
- linuxhost

### Result
```
1. "id2 provisioning":

  [   0.000]<V> AOS [application_start#70] : application started.
get prov res success prov_res is 0x0
prov test id2 success
 [   1.000]<V> AOS [app_delayed_action#61] : id2 prov test success


2. "id2 erasure":

 [   0.000]<V> AOS [application_start#70] : application started.
 [   1.000]<V> AOS [app_delayed_action#54] : key has been erased

```

