NAME := libiot_sdk_impl

$(NAME)_SOURCES := ./sdk-impl.c \
		   ./impl_dynreg.c \
		   ./impl_linkkit.c \
		   ./impl_logpost.c
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_INCLUDES := ../../src/infra \
../../src/infra/log \
../../src/infra/utils \
../../src/infra/utils/misc \
../../src/infra/utils/digest \
../../src/infra/system \

