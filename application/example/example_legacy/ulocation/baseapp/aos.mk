NAME := baseapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := ulocation base app

$(NAME)_SOURCES :=   ./linkkit/linkkit_example_solo.c ./ulocation_demo.c
$(NAME)_COMPONENTS += ulocation linkkit_sdk_c netmgr cjson

$(NAME)_INCLUDES += ./ ./linkkit

