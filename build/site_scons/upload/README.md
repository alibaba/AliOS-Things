## Overview

Board upload configs for supporting upload process.

### Files

* registry_board.json: registry for board and its upload config file
* $boardname.json:     board specific upload configs

### registry_board.json

This file used for indicating that:
* What board is available for running upload
* Which configs should be used while uploading images to the board

Priority for searching upload configs:
* If exact board name found, use the configs;
* Otherwise, try to search with part of board name, such as 'stm32' for 'stm32*' boards;
* If nothing found, use the configs for 'ALLOTHERS'.

### $boardname.json

This file used for recording:
* What tools/options should be used
* Where the config file or binaries locate in
* ...

Note: Don't edit the *.json files directly, update them with `gen_upload_configs.py`.
