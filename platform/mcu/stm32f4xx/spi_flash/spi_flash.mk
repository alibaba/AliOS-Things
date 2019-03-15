
$(NAME)_SOURCES += spi_flash/spi_flash.c spi_flash/spi_flash_platform.c

GLOBAL_INCLUDES += ./spi_flash

#(info $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS))
# $(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)
