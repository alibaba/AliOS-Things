NAME := uspace_pm

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES += pm_req/process_req.c
GLOBAL_INCLUDES += ./pm_req
