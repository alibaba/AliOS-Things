## Contents

## Attentions
1、 Apps in example/example_legacy dir run start from API of "application_start".
Other apps in example dir firstly run start from API of "aos_maintask" and than jump to "application_start".
2、 Boards in platform/board/board_legacy can only support apps in example/example_legacy.
While other boards in platform/board support all examples list in application dir.
3、 When compiling new board in platform/board and old app in example/example_legacy, component of "app_adapter" is added automatically. Please see codes in directory of application/app_adapter

The difference between new and legacy apps is in maintask.c.
For legacy app, "aos_maintask" is realized in each board/*/startup/startup.c.
For new format app, "aos_maintask" is realized in example/*/maintask.c.

### Standand directories layout

```
example
├── athostapp        # AT command based firmware for modules
├── blink            # basic io handling
├── bluetooth        # bluetooth demo applications
├── coapapp          # basic usage of CoAP
├── dk               # examples for developerkit
├── helloworld       # first AliOS Things program
├── http2app         # basic usage of http2
├── linkkit_gateway  # linkkit for gateway
├── linkkitapp       # linkkit demo for device
├── lorawan          # lorawan demo application
├── meshapp          # uMesh demo application
├── mqttapp          # basic usage of MQTT
├── otaapp           # OTA demo application
├── sk               # examples for starterkit
├── tinyengine_app   # TinyEngine builtin OS image
├── tls              # TLS sample
├── udataapp         # uData demo application
└── yts
```
