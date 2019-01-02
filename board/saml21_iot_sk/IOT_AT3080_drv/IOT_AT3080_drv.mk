NAME := IOT_AT3080_drv


$(NAME)_SOURCES := AliCloud_SDS/alicloud_sds.c
GLOBAL_INCLUDES += AliCloud_SDS
                   
$(NAME)_SOURCES += drv_button/io_button.c
GLOBAL_INCLUDES += drv_button

$(NAME)_SOURCES += drv_color_led/color_led.c \
                   drv_color_led/hsb2rgb.c
GLOBAL_INCLUDES += drv_color_led

$(NAME)_SOURCES += drv_sht20/sht2x.c
GLOBAL_INCLUDES += drv_sht20

$(NAME)_SOURCES += drv_VGM128064/oled.c
GLOBAL_INCLUDES += drv_VGM128064

$(NAME)_SOURCES += helper/emhost/ATCmdParser/ATCmdParser.c \
                   helper/emhost/emh_alisds.c \
                   helper/emhost/emh_arg.c \
                   helper/emhost/emh_module.c \
                   helper/emhost/emh_wlan.c
               
GLOBAL_INCLUDES += helper/emhost/ATCmdParser \
                   helper/emhost
                  
$(NAME)_SOURCES += helper/hal/mx_i2c.c \
                   helper/hal/mx_serial.c \
                   helper/hal/mx_stdio.c \
                   helper/hal/mx_tick.c
               
GLOBAL_INCLUDES += helper/hal


$(NAME)_SOURCES += helper/jsmn/jsmn.c \
                   helper/jsmn/json_escape_str.c \
                   helper/jsmn/json_generator.c \
                   helper/jsmn/json_utils.c \
                   helper/jsmn/json_wrappers.c
$(NAME)_INCLUDES += helper/jsmn


$(NAME)_SOURCES += drv_board.c drv_board_test.c 
GLOBAL_INCLUDES += . helper
