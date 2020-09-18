# component name
NAME := amp
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := amp component
# source files and the folder of internal include files
$(NAME)_INCLUDES += . include ./hal

$(NAME)_PREBUILT_LIBRARY := ./libamp.a

