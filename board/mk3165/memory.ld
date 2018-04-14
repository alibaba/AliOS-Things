
MEMORY
{
    BL_FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
    APP_FLASH (rx)  : ORIGIN = 0x0800C000, LENGTH = 900K
    RAM      (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
    ERAM      (rwx) : ORIGIN = 0, LENGTH = 0
}
