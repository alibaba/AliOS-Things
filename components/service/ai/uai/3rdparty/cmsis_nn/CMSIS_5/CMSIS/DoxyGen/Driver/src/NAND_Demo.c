#include "Driver_NAND.h"
 
/* ONFI commands */
#define ONFI_CMD_READ_1ST               0x00   ///< Read 1st Cycle
#define ONFI_CMD_PROGRAM_2ND            0x10   ///< Page Program 2nd Cycle
#define ONFI_CMD_READ_2ND               0x30   ///< Read 2nd Cycle
#define ONFI_CMD_PROGRAM_1ST            0x80   ///< Page Program 1st Cycle
#define ONFI_CMD_RESET                  0xFF   ///< Reset Command
 
/* NAND Signal Event callback function */
volatile uint32_t NAND_Events;
void NAND_SignalEventCallback (uint32_t dev_num, uint32_t event) {
  if (dev_num == 0) {
    NAND_Events |= event;
  }
  else {
    // ..
  }
}
 
/* NAND device Power ON */
void PowerOn (ARM_DRIVER_NAND *drv, uint32_t dev_num) {
  ARM_NAND_CAPABILITIES capabilities;
 
  // Query drivers capabilities
  capabilities = drv->GetCapabilities();
 
  // Initialize NAND device
  drv->Initialize (NAND_SignalEventCallback);
 
  // Power-on NAND driver
  drv->PowerControl (ARM_POWER_FULL);
 
  // Turn ON device power
  uint32_t volt = 0U;
 
  if (capabilities.vcc)      { volt |= ARM_NAND_POWER_VCC_3V3;  }
  if (capabilities.vcc_1v8)  { volt |= ARM_NAND_POWER_VCC_1V8;  }
  if (capabilities.vccq)     { volt |= ARM_NAND_POWER_VCCQ_3V3; }
  if (capabilities.vccq_1v8) { volt |= ARM_NAND_POWER_VCCQ_1V8; }
 
  if (volt != 0U) {  
    drv->DevicePower (volt);
  }
 
  // Setting bus mode
  drv->Control (0U, ARM_NAND_BUS_MODE, ARM_NAND_BUS_SDR);
 
  // Setting bus data width
  drv->Control (0U, ARM_NAND_BUS_DATA_WIDTH, ARM_NAND_BUS_DATA_WIDTH_8);
 
  // Enable chip manually if needed
  if (capabilities.ce_manual) {
    drv->ChipEnable (dev_num, true);
  }
 
  // Send ONFI Reset command */
  drv->SendCommand (dev_num, ONFI_CMD_RESET);
}
 
/* NAND device Power OFF */
void PowerOff (ARM_DRIVER_NAND *drv, uint32_t dev_num) {
  ARM_NAND_CAPABILITIES capabilities;
 
  // Query drivers capabilities
  capabilities = drv->GetCapabilities();
 
  // Disable chip manually if needed
  if (capabilities.ce_manual) {
    drv->ChipEnable (0U, false);
  }
 
  // Switch OFF gracefully
  uint32_t volt = 0U;
 
  if (capabilities.vcc)  { volt |= ARM_NAND_POWER_VCC_OFF;  }
  if (capabilities.vccq) { volt |= ARM_NAND_POWER_VCCQ_OFF; }
  if (volt) {
    drv->DevicePower (volt);
  }
  drv->PowerControl (ARM_POWER_OFF);
  drv->Uninitialize ();
}
 
/* Read NAND page. */
void ReadPage (ARM_DRIVER_NAND *drv, uint32_t row, uint8_t *data, uint32_t cnt) {
  uint32_t dev_num = 0;   // Device number
  uint32_t mode;
 
  // Send Read 1st command
  drv->SendCommand (dev_num, ONFI_CMD_READ_1ST);
 
  // Send address (column: 2 cycles, row: 3 cycles)
  drv->SendAddress (dev_num, 0x00);
  drv->SendAddress (dev_num, 0x00);
  drv->SendAddress (dev_num, (uint8_t)(row));
  drv->SendAddress (dev_num, (uint8_t)(row >>  8));
  drv->SendAddress (dev_num, (uint8_t)(row >> 16));
 
  // Send Read 2nd command
  drv->SendCommand (dev_num, ONFI_CMD_READ_2ND);
 
  // Wait until device ready
  while (drv->GetDeviceBusy(dev_num) == 1) { ; }
 
  // Use ECC algorithm number 2, ECC0 (ECC over main+spare)
  mode = ARM_NAND_ECC(2) | ARM_NAND_ECC0;
 
  // Transfer data from the NAND chip
  if (drv->ReadData (dev_num, data, cnt, mode | ARM_NAND_DRIVER_DONE_EVENT) != cnt) {
    // Wait until driver done event received
    while ((NAND_Events & ARM_NAND_DRIVER_DONE_EVENT) == 0) { ; }
    // Read page completed
 
    if ((NAND_Events & ARM_NAND_EVENT_ECC_ERROR) != 0) {
      // ECC correction failed
    }
  }
}
 
/* Write NAND page (ExecuteSequence interface). */
void WritePage_Seq (ARM_DRIVER_NAND *drv, uint32_t row, const uint8_t *data, uint32_t cnt) {
  uint32_t dev_num = 0;   // Device number
  uint32_t cmd;
  uint32_t code;
  uint32_t seq;
 
  // Prepare commands to send
  cmd = ONFI_CMD_PROGRAM_1ST | (ONFI_CMD_PROGRAM_2ND << 8);
 
  // Construct sequence code:
  // - Send command 1
  // - Send 2 cycles of column address and 3 cycles of row address
  // - Write data from memory to device
  // - Send command 2
  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_SEND_ADDR_COL2 |
         ARM_NAND_CODE_SEND_ADDR_ROW1 |
         ARM_NAND_CODE_SEND_ADDR_ROW2 |
         ARM_NAND_CODE_SEND_ADDR_ROW3 |
         ARM_NAND_CODE_WRITE_DATA     |
         ARM_NAND_CODE_SEND_CMD2      ;
 
  // - Use ECC algorithm number 2, ECC0 (ECC over main+spare)
  code |= ARM_NAND_ECC(2) | ARM_NAND_ECC0;
 
  // Number of iterations in a sequence
  seq = 1;
 
  drv->ExecuteSequence (dev_num,        // Device number
                        code,           // Sequence code
                        cmd,            // Command(s)
                        0,              // Column address
                        row,            // Row address
                        (void *)data,   // Data buffer
                        cnt,            // Number of data items (per iteration)
                        NULL,           // Device status will not be read
                        &seq);          // Number of iterations
 
  // Wait until done
  while (drv->GetStatus(dev_num).busy != 0) { ; }
 
  // Page write completed
}
