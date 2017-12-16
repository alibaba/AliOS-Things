How to download 2045B patch ram

There are two ways user can download 2045B patch ram, one is compiled into code, another one is download from a binary file.
In order to prepare the hex and binary download file. here is the step need to follow.
1) First use Bluetool to generate intel hex file from cgs file. Yon can import Burn_2045B_Runtime_RAM_Hex_Image.btp in 
   hcis/patchram directory as template to BuleTool.Setting SS location to 0x00000000 to generate the right bin file.
2) Run Hex2Prm.pl in hcis/patchram directory to generate prm file from the intel hex file.
   This perl script will strip all unnecessary hex from the intel hex file and just keep the data portion.
   The hex file ***.prm contained the hex code for patch ram, you can use this to 
   modify BCM2045_PatchRam array in bcm2045_prm.c to define internal build patch hex array.
3) Run HEX2BIN.EXE in hcis/patchram directory to generate bin file from the intel hex file
   The bin file ***.bin is the bin download patch which can be used by BTA to fetch and download.
4) the lower layer patch ram api bcm2045_prm_api.c provide lower layer download function. For example, BCM2045_PRM_Init can
   be used to download patch, you can define this as BTM_APP_DEV_INIT to download patch when BT device is starting up.
5) the high lever BTA api bta_prm_api.c provide high level support so that you can chose to download patch from a bin file.

NOTE: the latest Bluetool generate hcd download file from cgs file, so in order to convert hcd file into build-in c array and 
      binary file, following that step
1) First use Bluetool to generate hcd file from cgs file. Yon can import Burn_2045B_Runtime_RAM_Hex_Image.btp in 
   hcis/patchram directory as template to BuleTool.S etting SS location to 0x00000000 to generate the right hcd burn image file.
2) Run hcd2c.pl -s <hcd file> in hcis/patchram directory. The STDOUT is the build-in C array and it will also generate ***.bin
   file which can be used by BTA to fetch and download.

NOTE: To download hcd format, hcd file is converted c array with HCI header.
1) Generate hcd file from cgs file with proper SS location by using Bluetool
2) Run hcd2c.pl -n <hcd file> to generate C code without mini driver download.
3) The bcm2045_patchram_format must be BTE_PRM_FORMAT_HCD. (BTE_PRM_FORMAT_BIN must be used for bin format)
