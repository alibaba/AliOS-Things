file(REMOVE_RECURSE
  "bootloader/bootloader.bin"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.map"
  "config/sdkconfig.cmake"
  "config/sdkconfig.h"
  "flash_project_args"
  "micropython.bin"
  "micropython.map"
  "project_elf_src.c"
  "CMakeFiles/erase_flash"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/erase_flash.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
