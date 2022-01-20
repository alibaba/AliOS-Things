![Build lv_micropython unix port](https://github.com/lvgl/lv_binding_micropython/workflows/Build%20lv_micropython%20unix%20port/badge.svg)

# Bindings for LVGL

---

This repo is a submodule of [lv_micropython](https://github.com/lvgl/lv_micropython).  
Please fork [lv_micropython](https://github.com/lvgl/lv_micropython) for a quick start with LVGL Micropython Bindings.

---

See also [Micropython + LittlevGL](https://blog.lvgl.io/2019-02-20/micropython-bindings) blog post. (LittlevGL is the previous name of LVGL.)  
For advanced features, see [Pure Micropython Display Driver](https://blog.lvgl.io/2019-08-05/micropython-pure-display-driver) blog post.   
For questions and discussions - please use the forum: https://forum.lvgl.io/c/micropython

## Micropython

Micropython Binding for LVGL provides an automatically generated Micropython module with classes and functions that allow the user access much of the LVGL library.  
The module is generated automatically by the script [`gen_mpy.py`](https://github.com/lvgl/lv_binding_micropython/blob/master/gen/gen_mpy.py).  
This script reads, preprocesses and parses LVGL header files, and generates a C file `lv_mpy.c` which defines the Micropython module (API) for accessing LVGL from Micropython.  
Micopython's build script (Makefile or CMake) should run `gen_mpy.py` automatically to generate and compile `lv_mpy.c`.

- If you would like to see an example of how a generated `lv_mpy.c` looks like, have a look at [`lv_mpy_example.c`](https://raw.githubusercontent.com/lvgl/lv_binding_micropython/master/gen/lv_mpy_example.c). Note that its only exported (non static) symbol is `mp_module_lvgl` which should be registered in Micropython as a module.  
- lv_binding_micropython is usually used as a git submodule of [lv_micropython](https://github.com/lvgl/lv_micropython) which builds Micropython + LVGL + lvgl-bindings, but can also be used on other forks of Micropython.

It's worth noting that the Mircopython Bindings module (`lv_mpy.c`) is dependant on LVGL configuration.  
LVGL is configured by `lv_conf.h` where different objects and features could be enabled or disabled. LVGL bindings are generated only for the enabled objects and features. Changing `lv_conf.h` requires re running `gen_mpy.py`, therfore it's useful to run it automatically in the build script, as done by lv_micropython.

### Memory Management

When LVGL is built as a Micropython library, it is configured to allocate memory using Micropython memory allocation functions and take advantage of Micropython *Garbage Collection* ("gc").  
This means that structs allocated for LVGL use don't need to be deallocated explicitly, gc takes care of that.  
For this to work correctly, LVGL is configured to use gc and to use Micropython's memory allocation functions, and also register all LVGL "root" global variables to Micropython's gc.  

From the user's perspective, structs can be created and will be collected by gc when they are no longer referenced.  
However, LVGL screen objects (`lv.obj` with no parent) are automatically assigned to default display, therefor not collected by gc even when no longer explicitly referenced.  
When you want to free a screen and all its decendants so gc could collect their memory, make sure you call `screen.delete()` when you no longer need it.  

Make sure you keep a reference to your display driver and input driver to prevent them from being collected.

### Concurrency

This implementation of Micropython Bindings to LVGL assumes that Micropython and LVGL are running **on a single thread** and **on the same thread** (or alternatively, running without multithreading at all).  
No synchronization means (locks, mutexes) are taken.  
However, asynchronous calls to LVGL still take place periodically for screen refresh and other LVGL tasks such as animation.

This is achieved by using the internal Micropython scheduler (that must be enabled), by calling `mp_sched_schedule`.  
`mp_sched_schedule` is called when screen needs to be refreshed. LVGL expects the function `lv_task_handler` to be called periodically (see [lvgl/README.md#porting](https://github.com/lvgl/lvgl/blob/6718decbb7b561b68e450203b83dff60ce3d802c/README.md#porting)). This is ususally handled in the display device driver.  
Here is [an example](https://github.com/lvgl/lv_binding_micropython/blob/77b0c9f2678b6fbd0950fbf27380052246841082/driver/SDL/modSDL.c#L23) of calling `lv_task_handler` with `mp_sched_schedule` for refreshing LVGL. [`mp_lv_task_handler`](https://github.com/lvgl/lv_binding_micropython/blob/77b0c9f2678b6fbd0950fbf27380052246841082/driver/SDL/modSDL.c#L7) is scheduled to run on the same thread Micropython is running, and it calls both `lv_task_handler` for LVGL task handling and `monitor_sdl_refr_core` for refreshing the display and handling mouse events.  

With REPL (interactive console), when waiting for the user input, asynchronous events can also happen. In [this example](https://github.com/lvgl/lv_mpy/blob/bc635700e4186f39763e5edee73660fbe1a27cd4/ports/unix/unix_mphal.c#L176) we just call `mp_handle_pending` periodically when waiting for a keypress. `mp_handle_pending` takes care of dispatching asynchronous events registered with `mp_sched_schedule`.

### Structs Classes and globals

The LVGL binding script parses LVGL headers and provides API to access LVGL **classes** (such as `btn`) and **structs** (such as `color_t`). All structs and classes are available under lvgl micropython module.  

lvgl Class contains:
- functions (such as `set_x`)
- enums related to that class (such as `STATE` of a `btn`)

lvgl struct contains only attributes that can be read or written. For example:
```python
c = lvgl.color_t()
c.ch.red = 0xff
```
structs can also be initialized from dict. For example, the example above can be written like this:
```python
c = lvgl.color_t({'ch': {'red' : 0xff}})
```

All lvgl globals (functions, enums, types) are avaiable under lvgl module. For example, `lvgl.SYMBOL` is an "enum" of symbol strings, `lvgl.anim_create` will create animation etc.

### Callbacks

In C a callback is a function pointer.  
In Micropython we would also need to register a *Micropython callable object* for each callback.  
Therefore in the Micropython binding we need to register both a function pointer and a Micropython object for every callback.  

Therefore we defined a **callback convention** that expects lvgl headers to be defined in a certain way. Callbacks that are declared according to the convention would allow the binding to register a Micropython object next to the function pointer when registering a callback, and access that object when the callback is called.  
The Micropython callable object is automatically saved in a `user_data` variable which is provided when registering or calling the callback.

The callback convetion assumes the following:
- There's a struct that contains a field called `void * user_data`.
- A pointer to that struct is provided as the first argument of a callback registration function.
- A pointer to that struct is provided as the first argument of the callback itself.

Another option is that the callback function pointer is just a field of a struct, in that case we expect the same struct to contain `user_data` field as well.

Another option is:
- A parameter called `void * user_data` is provided to the registration function as the last argument.
- The callback itself recieves `void *` as the last argument

In this case, the user should provide either `None` or a dict as the `user_data` argument of the registration function.
The callback will recieve a Blob which can be casted to the dict in the last argument.
(See `async_call` example below)

As long as the convention above is followed, the lvgl Micropython binding script would automatically set and use `user_data` when callbacks are set and used.  

From the user perspective, any python callable object (such as python regular function, class function, lambda etc.) can be user as an lvgl callbacks. For example:
```python
lvgl.anim_set_custom_exec_cb(anim, lambda anim, val, obj=obj: obj.set_y(val))
```
In this example an exec callback is registered for an animation `anim`, which would animate the y coordinate of `obj`.  
An lvgl API function can also be used as a callback directly, so the example above could also be written like this:
```python
lv.anim_set_exec_cb(anim, obj, obj.set_y)
```

lvgl callbacks that do not follow the Callback Convention cannot be used with micropython callable objects. A discussion related to adjusting lvgl callbacks to the convention: https://github.com/lvgl/lvgl/issues/1036  

The `user_data` field **must not be used directly by the user**, since it is used internally to hold pointers to Micropython objects.

### Display and Input Drivers

LVGL can be configured to use different displays and different input devices. More information is available on [LVGL documentation](https://docs.lvgl.io/master/porting/display.html).  
Registering a driver is essentially calling a registeration function (for example `disp_drv_register`) and passing a function pointer as a parameter (actually a struct that contains function pointers). The function pointer is used to access the actual display / input device.  

When implementing a display or input LVGL driver with Micropython, there are 3 option:
- Implement a Pure Python driver. It the easiest way to implement a driver, but may perform poorly
- Implement a Pure C driver.
- Implemnent a Hybrid driver where the critical parts (such as the `flush` function) are in C, and the non-critical part (such as initializing the display) are implemented in Python.

An example of Pure/Hybrid driver is the [ili9XXX.py](https://github.com/lvgl/lv_binding_micropython/blob/master/driver/esp32/ili9XXX.py).

The driver registration should eventually be perfomed in the Micropython script, either in the driver code itself in case of the pure/hybrid driver or in user code in case of C driver (for example, in the case of the SDL driver). Registering the driver on Python and not in C is important to make it easy for the user to select and replace drivers without building the project and changing C files.

When creating a display or input LVGL driver, make sure you let the user **configure all parameters on runtime**, such as SPI pins, frequency, etc.  
Eventually the user would want to build the firmware once and use the same driver in different configuration without re-building the C project.  
This is different from standard LVGL C drivers where you usually use macros to configure parameters and require the user to re-build when any configurations changes.

Example:

```python

# Initialize ILI9341 display

from ili9XXX import ili9341
self.disp = ili9341(dc=32, cs=33, power=-1, backlight=-1)
        
# Register xpt2046 touch driver

from xpt2046 import xpt2046
self.touch = xpt2046()
```

Example:

```python
# init

import lvgl as lv
lv.init()

import SDL
SDL.init()

# Register SDL display driver.

disp_buf1 = lv.disp_buf_t()
buf1_1 = bytes(480*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.buffer = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 320
disp_drv.register()

# Regsiter SDL mouse driver

indev_drv = lv.indev_drv_t()
indev_drv.init() 
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()
```

In this example we import SDL. SDL module gives access to display and input device on a unix/linux machine. It contains several objects such as `SDL.monitor_flush`, which are wrappers around function pointers and can be registerd as LVGL display and input driver.  
Behind the scences these objects implement the buffer protocol to give access to the function pointer bytes.

Starting from version 6.0, lvgl supports setting the display settings (width, length) on runtime. In this example they are set to 480x320. Color depth is set on compile time.  

Currently supported drivers for Micropyton are 

- SDL unix drivers (display and mouse)
- Linux Frame Buffer (`/dev/fb0`)
- ILI9341 driver for ESP32
- XPT2046 driver for ESP32
- FT6X36 (capacitive touch IC) for ESP32
- Raw Resistive Touch for ESP32 (ADC connected to screen directly, no touch IC)

Driver code is under `/driver` directory.

Drivers can also be implemented in pure Micropython, by providing callbacks (`disp_drv.flush_cb`, `indev_drv.read_cb` etc.)
Currently the supported ILI9341, FT6X36 and XPT2046 are pure micropython drivers.

### Where are the drivers?

LVGL C drivers and Micropython drivers (either C or Python) are **separate and independent** from each other.  
The main reason is configuration:
- The C driver is usually configured with C macros (which pins it uses, frequency, etc.)  
Any configuration change requires rebuilding the firmware but that's understandable since any change in the application requires rebuilding the firmware anyway.
- In Micropython the driver is built once with Micropython firmware (if it's a C driver) or not built at all (if it's pure Python driver). On runtime the user initializes the driver and configures it. If the user switches SPI pins or some other configuration, there is no need to rebuild the firmware, just change the Python script and initialize the driver differently **on runtime**.

So the location for Micropython drivers is https://github.com/lvgl/lv_binding_micropython/tree/master/driver and is unrelated to https://github.com/lvgl/lv_drivers.

### The Event Loop

LVGL requires an Event Loop to re-draw the screen, handle user input etc.
The default Event Loop is implement in [lv_utils.py](https://github.com/lvgl/lv_binding_micropython/blob/master/lib/lv_utils.py) which uses Micropython Timer to schedule calls to LVGL.  
It also supports running the Event Loop in [uasyncio](https://docs.micropython.org/en/latest/library/uasyncio.html) if needed.

### Adding Micropython Bindings to a project

An example project of "Micropython + lvgl + Bindings" is [`lv_mpy`](https://github.com/lvgl/lv_mpy).  
Here is a procedure for adding lvgl to an existing Micropython project. (The examples in this list are taken from [`lv_mpy`](https://github.com/lvgl/lv_mpy)):

- Add [`lv_bindings`](https://github.com/lvgl/lv_binding_micropython) as a sub-module under `lib`.
- Add `lv_conf.h` in `lib`
- Edit the Makefile to run `gen_mpy.py` and build its product automatically. Here is [an example](https://github.com/lvgl/lv_micropython/blob/2940838bf6d4999050efecb29a4152ab5796d5b3/py/py.mk#L22-L38).
- Register lvgl module and display/input drivers in Micropython as a builtin module. [An example](https://github.com/lvgl/lv_micropython/blob/2940838bf6d4999050efecb29a4152ab5796d5b3/ports/unix/mpconfigport.h#L230).
- Add lvgl roots to gc roots. [An example](https://github.com/lvgl/lv_micropython/blob/2940838bf6d4999050efecb29a4152ab5796d5b3/ports/unix/mpconfigport.h#L317-L318). 
- ~Configure lvgl to use *Garbage Collection* by setting several `LV_MEM_CUSTOM_*` and `LV_GC_*` macros [example](https://github.com/lvgl/lv_mpy/blob/bc635700e4186f39763e5edee73660fbe1a27cd4/lib/lv_conf.h#L28)~ lv_conf.h was moved to lv_binding_micropython git module.
- Make sure you configure partitions correctly in `partitions.csv` and leave enough room for the LVGL module.
- Something I forgot? Please let me know.

### gen_mpy.py syntax
```
usage: gen_mpy.py [-h] [-I <Include Path>] [-D <Macro Name>]
                  [-E <Preprocessed File>] [-M <Module name string>]
                  [-MP <Prefix string>] [-MD <MetaData File Name>]
                  input [input ...]

positional arguments:
  input

optional arguments:
  -h, --help            show this help message and exit
  -I <Include Path>, --include <Include Path>
                        Preprocesor include path
  -D <Macro Name>, --define <Macro Name>
                        Define preprocessor macro
  -E <Preprocessed File>, --external-preprocessing <Preprocessed File>
                        Prevent preprocessing. Assume input file is already
                        preprocessed
  -M <Module name string>, --module_name <Module name string>
                        Module name
  -MP <Prefix string>, --module_prefix <Prefix string>
                        Module prefix that starts every function name
  -MD <MetaData File Name>, --metadata <MetaData File Name>
                        Optional file to emit metadata (introspection)
```

Example: 

```
python gen_mpy.py -MD lv_mpy_example.json -M lvgl -MP lv -I../../berkeley-db-1.xx/PORT/include -I../../lv_binding_micropython -I. -I../.. -Ibuild -I../../mp-readline -I ../../lv_binding_micropython/pycparser/utils/fake_libc_include ../../lv_binding_micropython/lvgl/lvgl.h
```

### Binding other C libraries

The lvgl binding script can be used to bind other C libraries to Micropython.  
I used it with [lodepng](https://github.com/lvandeve/lodepng) and with parts of ESP-IDF.  
For more details please read [this blog post](https://blog.lvgl.io/2019-08-05/micropython-pure-display-driver).

## Micropython Bindings Usage

A simple example: [`advanced_demo.py`](https://github.com/lvgl/lv_binding_micropython/blob/master/gen/examples/advanced_demo.py).
More examples can be found under `/examples` folder.

#### Importing and Initializing LVGL
```python
import lvgl as lv
lv.init()
```
#### Registering Display and Input drivers
```python
import SDL
SDL.init()

# Register SDL display driver.

disp_buf1 = lv.disp_buf_t()
buf1_1 = bytes(480*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.buffer = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 320
disp_drv.register()

# Register SDL mouse driver

indev_drv = lv.indev_drv_t()
indev_drv.init() 
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()
```
In this example, SDL display and input drivers are registered on a unix port of Micropython.

Here is an alternative example for ESP32 ILI9341 + XPT2046 drivers:

```python
import lvgl as lv

# Import ILI9341 driver and initialized it

from ili9XXX import ili9341
disp = ili9341()

# Import XPT2046 driver and initalize it

from xpt2046 import xpt2046
touch = xpt2046()
```

By default, both ILI9341 and XPT2046 are initialized on the same SPI bus with the following parameters:

- ILI9341: `miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, spihost=esp.HSPI_HOST, mhz=40, factor=4, hybrid=True`
- XPT2046: `cs=25, spihost=esp.HSPI_HOST, mhz=5, max_cmds=16, cal_x0 = 3783, cal_y0 = 3948, cal_x1 = 242, cal_y1 = 423, transpose = True, samples = 3`

You can change any of these parameters on ili9341/xpt2046 constructor.
You can also initalize them on different SPI buses if you want, by providing miso/mosi/clk parameters. Set them to -1 to use existing (initialized) spihost bus.

Here's another example, this time importing and initialising display and touch drivers for the M5Stack Core2 device, which uses an FT6336 chip on the I2C bus to read from its capacitive touch screen and uses an ili9342 display controller, which has some inverted signals compared to the ili9341:

```python
from ili9XXX import ili9341
disp = ili9341(mosi=23, miso=38, clk=18, dc=15, cs=5, invert=True, rot=0x10)

from ft6x36 import ft6x36
touch = ft6x36(sda=21, scl=22, width=320, height=280)
```


### Creating a screen with a button and a label
```python
scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Button")

# Load the screen

lv.scr_load(scr)

```

#### Creating an instance of a struct
```python
symbolstyle = lv.style_t(lv.style_plain)
```
symbolstyle would be an instance of `lv_style_t` initialized to the same value of `lv_style_plain`

#### Setting a field in a struct
```python
symbolstyle.text.color = lv.color_hex(0xffffff)
```
symbolstyle.text.color would be initialized to the color struct returned by `lv_color_hex`

#### Setting a nested struct using dict
```python
symbolstyle.text.color = {"red":0xff, "green":0xff, "blue":0xff}
```

#### Creating an instance of an object
```python
self.tabview = lv.tabview(lv.scr_act())
```
The first argument to an object constructor is the parent object, the second is which element to copy this element from.  
Both arguments are optional.

#### Calling an object method
```python
self.symbol.align(self, lv.ALIGN.CENTER,0,0)
```
In this example `lv.ALIGN` is an enum and `lv.ALIGN.CENTER` is an enum member (an integer value).

#### Using callbacks
```python
for btn, name in [(self.btn1, 'Play'), (self.btn2, 'Pause')]:
    btn.set_event_cb(lambda obj=None, event=-1, name=name: self.label.set_text('%s %s' % (name, get_member_name(lv.EVENT, event))))
```

Using callback with `user_data` argument:

```python
def cb(user_data):
    print(user_data.cast()['value'])

lv.async_call(cb, {'value':42})
```

#### Listing available functions/memebers/constants etc.
```python
print('\n'.join(dir(lvgl)))
print('\n'.join(dir(lvgl.btn)))
...
```

