#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_SPI_Flash_Library_$(PLATFORM)

$(NAME)_SOURCES := spi_flash.c spi_flash_platform.c

GLOBAL_INCLUDES := .

#(info $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS))
# $(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)