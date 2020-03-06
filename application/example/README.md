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
├── example_legacy   # apps run from API of "application_start"
├── others           # apps run from API of "aos_maintask" and then jump to 								           "application_start".

```
