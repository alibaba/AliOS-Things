#!/bin/bash

# Minimal script pushing and running a file on device!
# Contemporary versions of ADB properly propagate exit codes so nothing more
# is needed to let CTest report test success/failure.

# TODO: consider clearing temporary files after testing, although that will
# get in the way of debugging and will make code more complex... also,
# Ruy's test files aren't huge and people running these probably have
# bigger clutter issues in their /data/local/tmp anyway. Anyway, if we want
# to do this, we could copy IREE's code.

device_tmpdir=/data/local/tmp

adb push "$1" "${device_tmpdir}"
adb shell "${device_tmpdir}/$(basename "$1")"
