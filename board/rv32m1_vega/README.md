## Environment setup

1. Download the pre-built pulp-riscv-gcc toolchain from [here](TBD). Extract and
   add to the system variable PATH.
2. Download the pre-built openocd from [here](TBD). Extract and add to the system
   variable PATH.
3. Refer the section _Install OpenOCD_ in [vega_sdk_riscv user guide](https://github.com/open-isa-vega/vega_sdk_riscv)
   to setup the OpenOCD in Windows.

## Download the binary to board

When the binary built successfully, follow the next steps to download and run.

1. Connect Jlink to J55
2. Connect J12 and PC.
3. Open the terminal and set like this:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
4. In the binary output directory, for example _out/helloworld@rv32m1\_vega/binary_,
   run the command
   `openocd -f ../../../board/rv32m1_vega/vega_ri5cy.cfg -c "program helloworld@rv32m1_vega.elf;reset;exit"`
   then the board resets and run, the log is output to the terminal.
