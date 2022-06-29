##############################################################################
# Event Loop module: advancing tick count and scheduling lvgl task handler.
# Import after lvgl module.
# This should be imported and used by display driver.
# Display driver should first check if an event loop is already running.
#
# Usage example with SDL:
#
#        SDL.init(auto_refresh=False)
#        # Register SDL display driver.
#        # Regsiter SDL mouse driver
#        event_loop = lv_utils.event_loop(refresh_cb = SDL.refresh)
#
#
# uasyncio example with SDL:
#
#        SDL.init(auto_refresh=False)
#        # Register SDL display driver.
#        # Regsiter SDL mouse driver
#        event_loop = lv_utils.event_loop(refresh_cb = SDL.refresh, asynchronous=True)
#        uasyncio.Loop.run_forever()
#
# uasyncio example with ili9341:
#
#        event_loop = lv_utils.event_loop(asynchronous=True) # Optional!
#        self.disp = ili9341(asynchronous=True)
#        uasyncio.Loop.run_forever()
#
# MIT license; Copyright (c) 2021 Amir Gonnen
#
##############################################################################

import lvgl as lv
import micropython
import usys

# Try standard machine.Timer, or custom timer from lv_timer, if available

try:
    from machine import Timer
except:
    try:
        from lv_timer import Timer
    except:
        raise RuntimeError("Missing machine.Timer implementation!")

# Try to determine default timer id

default_timer_id = 0
if usys.platform == 'pyboard':
    # stm32 only supports SW timer -1
    default_timer_id = -1

# Try importing uasyncio, if available

try:
    import uasyncio
    uasyncio_available = True
except:
    uasyncio_available = False

##############################################################################

class event_loop():

    _current_instance = None

    def __init__(self, freq=25, timer_id=default_timer_id, max_scheduled=2, refresh_cb=None, asynchronous=False):
        if self.is_running():
            raise RuntimeError("Event loop is already running!")

        if not lv.is_initialized():
            lv.init()

        event_loop._current_instance = self

        self.delay = 1000 // freq
        self.refresh_cb = refresh_cb

        self.asynchronous = asynchronous
        if self.asynchronous:
            if not uasyncio_available:
                raise RuntimeError("Cannot run asynchronous event loop. uasyncio is not available!")
            self.refresh_event = uasyncio.Event()
            self.refresh_task = uasyncio.create_task(self.async_refresh())
            self.timer_task = uasyncio.create_task(self.async_timer())
        else:
            self.timer = Timer(timer_id)
            self.task_handler_ref = self.task_handler  # Allocation occurs here
            self.timer.init(mode=Timer.PERIODIC, period=self.delay, callback=self.timer_cb)
            self.max_scheduled = max_scheduled
            self.scheduled = 0

    def deinit(self):
        if self.asynchronous:
            self.refresh_task.cancel()
            self.timer_task.cancel()
        else:
            self.timer.deinit()
        event_loop._current_instance = None

    @staticmethod
    def is_running():
        return event_loop._current_instance is not None

    @staticmethod
    def current_instance():
        return event_loop._current_instance

    def task_handler(self, _):
        lv.task_handler()
        if self.refresh_cb: self.refresh_cb()
        self.scheduled -= 1

    def timer_cb(self, t):
        # Can be called in Interrupt context
        # Use task_handler_ref since passing self.task_handler would cause allocation.
        lv.tick_inc(self.delay)
        if self.scheduled < self.max_scheduled:
            try:
                micropython.schedule(self.task_handler_ref, 0)
                self.scheduled += 1
            except:
                pass

    async def async_refresh(self):
        while True:
            await self.refresh_event.wait()
            self.refresh_event.clear()
            lv.task_handler()
            if self.refresh_cb: self.refresh_cb()

    async def async_timer(self):
        while True:
            await uasyncio.sleep_ms(self.delay)
            lv.tick_inc(self.delay)
            self.refresh_event.set()

