##############################################################################
# Run LVGL examples
# An example script should be provided as an argument.
#
# This script would:
# - Initialize LVGL and display
# - Read the example file
# - Chdir to the example file dir
# - Run the example
# - gc collect
# - Try to perform some actions such as sending click/value-changed events
# - Deinit the display
#
# It tries not to pollute the namespace of script under test.
# The only import visible for it is 'lv'.
#
# In case of an error, display the error and exit with code 255
# 255 was selected to stop xargs
#
##############################################################################

class __test():

    DELAY_MS=25
    MAX_CHILDREN=100

    import sys
    import gc
    import os
    import time
    import lvgl as lv

    events = [lv.EVENT.SCROLL, lv.EVENT.CLICKED, lv.EVENT.VALUE_CHANGED, lv.EVENT.READY]

    def __init__(self):
        self.sys.path.append('') # See: https://github.com/micropython/micropython/issues/6419
        import display_driver_utils
        self.lv.init()
        driver = display_driver_utils.driver()
        scr = self.lv.scr_act()
        self.objects = []

    def collect_objects(self, obj, user_data):
        if hasattr(obj, 'lv_obj'):
            obj = obj.lv_obj
        self.objects.append(obj)
        return self.lv.obj.TREE_WALK.NEXT

    def send_events(self):
        for obj in self.objects:
            if self.lv.obj.__cast__(obj): # skip deleted objects
                obj_info = ''
                if hasattr(obj, 'get_text'):
                    obj_info += ' text:"%s"' % obj.get_text()
                if hasattr(obj, 'get_value'):
                    obj_info += ' value:"%s"' % obj.get_value()
                print('%s %s' % (obj, obj_info))
                for event in self.events:
                    if not self.lv.obj.__cast__(obj): # skip deleted objects
                        continue
                    # print('\t%s' % get_member_name(lv.EVENT, event))
                    self.lv.event_send(obj, event, None)
                    self.time.sleep_ms(self.DELAY_MS)
                    self.gc.collect()

    def run(self):
        try:
            global __file__
            import lv_utils

            script = self.sys.argv[1]
            script_path = script[:script.rfind('/')] if script.find('/') >= 0 else '.'
            script_name = script[script.rfind('/')+1:] if script.find('/') >= 0 else script

            print('Running %s ...' % script)

            with open(script, 'r') as file:
                file_string = file.read()
                self.os.chdir(script_path)
                __file__ = script_name
                self.sys.argv[0] = __file__
                del self.sys.argv[1]
                exec(file_string)
                self.time.sleep_ms(self.DELAY_MS)
                self.gc.collect()
                self.lv.scr_act().tree_walk(self.collect_objects, None)
                self.send_events()
                self.time.sleep_ms(self.DELAY_MS)
                if lv_utils.event_loop.is_running():
                    lv_utils.event_loop.current_instance().deinit()
                    self.time.sleep_ms(self.DELAY_MS)

        except:
            exc = self.sys.exc_info()
            print('ERROR! %s: %s\n%s' % (
                exc[0].__name__,
                exc[1],
                exc[2] if exc[2] else ''))

            self.sys.exit(255) # 255 to exit xargs


import lvgl as lv
__test().run()


