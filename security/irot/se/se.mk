CHIPNAME = chip_template

NAME := se

LIBSE := .

$(NAME)_INCLUDES     += $(LIBSE)/../include
$(NAME)_INCLUDES     += $(LIBSE)/src
$(NAME)_INCLUDES     += $(LIBSE)/chipset/$(CHIPNAME)/include
$(NAME)_INCLUDES     += $(LIBSE)/chipset/$(CHIPNAME)

$(NAME)_SOURCES     += 			\
    $(LIBSE)/src/core/km_to_irot.c \
    $(LIBSE)/src/core/std_se_adapter.c \
    $(LIBSE)/src/core/mtk_se_adapter.c \
    $(LIBSE)/src/log/chiplog.c \
    $(LIBSE)/chipset/$(CHIPNAME)/irot_impl/irot_hal.c \
    $(LIBSE)/chipset/$(CHIPNAME)/se_driver_impl/se_driver.c \

