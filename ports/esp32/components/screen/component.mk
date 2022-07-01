#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)


SCREEN_DIR = controller_driver/ili9341 \
            controller_driver/st7789 \
            controller_driver/st7796 \
            controller_driver/nt35510 \
            controller_driver/ili9806 \
            controller_driver/ili9486 \
            controller_driver/ili9488 \
            controller_driver/ssd1351 \
            controller_driver/rm68120 \
            controller_driver/ssd1306 \
            controller_driver/ssd1307 \
            controller_driver/ssd1322 \
            controller_driver/ssd1963

COMPONENT_ADD_INCLUDEDIRS := . iface_driver $(SCREEN_DIR) screen_utility
COMPONENT_SRCDIRS := . iface_driver $(SCREEN_DIR) screen_utility
