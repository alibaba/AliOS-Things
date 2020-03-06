## Contents

## Attentions
When compiling new board in platform/board and old app in example/example_legacy, component of "app_adapter" is added automatically. 

The difference between new and legacy apps is in maintask.c.
For legacy app, "aos_maintask" is realized in each board/*/startup/startup.c.
For new format app, "aos_maintask" is realized in example/*/maintask.c.

### Standand directories layout

```
app_adapter
├── maintask.c   # realize of "aos_maintask"


```
