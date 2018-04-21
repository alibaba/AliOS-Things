src = Split('''
        spi_flash.c
        spi_flash_platform.c 
''')

component = aos_component('Lib_SPI_Flash_Library', src)