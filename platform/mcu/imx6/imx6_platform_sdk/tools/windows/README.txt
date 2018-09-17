This folder contains various tools needed for SDK on the Windows host.

Program sdk image to a SD card on Windows using USB card reader
----------------------------------------------------------------
    1. For the i.MX boards such as MX6DQ ARD, use:
           cfimager-imx.exe -o 0 -f ./output/mx6dq/bin/mx6dqard-ALL-sdk.bin -d xyz
       where xyz refer to the drive letter assigned by Windows.
