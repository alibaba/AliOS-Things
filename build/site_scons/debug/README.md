## Overview

Board debug configs for supporting debug process.

### Files

* registry_board.json: registry for board and its debug config file
* $boardname.json:     board specific debug configs

### registry_board.json

This file used for indicating that:
* What board is available for running debug
* Which configs should be used while running debug process

### $boardname.json

This file used for recording:
* Tools and commands for starting GDB server;
* Tools and commands for starting GDB client;

Note: Don't edit the *.json files directly, update them with `gen_debug_configs.py`.
