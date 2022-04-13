NAME := uspace_parcel

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := serialize and deserialize parameters used by RPC or DService.

$(NAME)_SOURCES := parcel_decode.c parcel_encode.c parcel.c

$(NAME)_COMPONENTS += uspace

$(NAME)_INCLUDES += ./

GLOBAL_INCLUDES += ./include

