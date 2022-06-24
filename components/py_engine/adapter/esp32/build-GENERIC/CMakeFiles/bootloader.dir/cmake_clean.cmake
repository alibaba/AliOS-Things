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
  "CMakeFiles/bootloader"
  "CMakeFiles/bootloader-complete"
  "bootloader-prefix/src/bootloader-stamp/bootloader-build"
  "bootloader-prefix/src/bootloader-stamp/bootloader-configure"
  "bootloader-prefix/src/bootloader-stamp/bootloader-download"
  "bootloader-prefix/src/bootloader-stamp/bootloader-install"
  "bootloader-prefix/src/bootloader-stamp/bootloader-mkdir"
  "bootloader-prefix/src/bootloader-stamp/bootloader-patch"
  "bootloader-prefix/src/bootloader-stamp/bootloader-update"
  "bootloader/bootloader.bin"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.map"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/bootloader.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
