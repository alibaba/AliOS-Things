
#include "Driver_MCI.h"


/* Usage example: ARM_MCI_Initialize ----------------------------------------*/

// ARM_MCI_SignalEvent callback function prototype
void MCI_SignalEvent_Callback (uint32_t event);

void init_driver (ARM_DRIVER_MCI *drv) {
  int32_t status;
  
  status = drv->Initialize (&MCI_SignalEvent_Callback);
  
  if (status != ARM_DRIVER_OK) {
    // Initialization and event callback registration failed
  }
}

/* Usage example: ARM_MCI_Uninitialize --------------------------------------*/

void uninit_driver (ARM_DRIVER_MCI *drv) {
  int32_t status;
  
  status = drv->Uninitialize ();
  
  if (status == ARM_DRIVER_OK) {
    // Driver successfully uninitialized
  }
}

/* Usage example: ARM_MCI_PowerControl --------------------------------------*/

void control_driver_power (ARM_DRIVER_MCI *drv, bool enable) {
  int32_t status;
  
  if (enable == true) {
    status = drv->PowerControl (ARM_POWER_FULL);
  }
  else {
    status = drv->PowerControl (ARM_POWER_OFF);
  }
  
  if (status == ARM_DRIVER_OK) {
    // Driver power enabled/disabled
  }
}

/* Usage example: ARM_MCI_CardPower -----------------------------------------*/

ARM_MCI_CAPABILITIES drv_capabilities;

void set_card_vdd_3v3 (ARM_DRIVER_MCI *drv) {
  int32_t status;
  
  if (drv_capabilities.vdd == 1U) {
    // Power switching to 3.3V supported
    status = drv->CardPower (ARM_MCI_POWER_VDD_3V3);
    
    if (status == ARM_DRIVER_OK) {
      // Card power set to 3.3V
    }
  }
}

/* Usage example: ARM_MCI_ReadCD --------------------------------------------*/

void read_card_detect_state (ARM_DRIVER_MCI *drv) {
  int32_t status;
  
  status = drv->ReadCD();
  
  if (status == 1) {
    // Memory card is detected
  }
  else {
    if (status == 0) {
      // Memory card is not detected
    }
    else {
      // Error reading card detect pin state
    }
  }
}

/* Usage example: ARM_MCI_ReadWP --------------------------------------------*/

void read_write_protect_state (ARM_DRIVER_MCI *drv) {
  int32_t status;
  
  status = drv->ReadWP();
  
  if (status == 1) {
    // Memory card write protection is enabled
  }
  else {
    if (status == 0) {
      // Memory card write protection is disabled
    }
    else {
      // Error reading write protect pin state
    }
  }
}

/* Usage example: ARM_MCI_SendCommand ---------------------------------------*/

volatile uint32_t MCI_Events;

void MCI_SignalEvent_Callback (uint32_t event) {
  // Save current event
  MCI_Events |= event;
}

void send_CMD0 (ARM_DRIVER_MCI *drv) {
  int32_t  status;
  uint32_t cmd;

  MCI_Events = 0U; //Clear MCI driver event flags
  cmd = 0U;        // Set GO_IDLE_STATE command code

  status = drv->SendCommand (cmd, 0U, ARM_MCI_CARD_INITIALIZE | ARM_MCI_RESPONSE_NONE, NULL);

  if (status == ARM_DRIVER_OK) {
    /* Wait for event */
    while ((MCI_Events & ARM_MCI_EVENT_COMMAND_COMPLETE) == 0U);
    // Command was successfully sent to memory card
    // ..
  }
  else {
    // Error
  }
}

/* Usage example: ARM_MCI_SetupTransfer -------------------------------------*/

volatile uint32_t MCI_Events;

void MCI_SignalEvent_Callback (uint32_t event) {
  MCI_Events |= event;  // Save current event
}

void read_sector (ARM_DRIVER_MCI *drv, uint8_t *buf, uint32_t sz) {
  int32_t status;
  uint32_t cmd, arg;
  uint32_t resp;

  if (sz < 512U) {
    // Invalid buffer size, sector consists of 512 bytes
    //...
  }

  status = drv->SetupTransfer (buf, 1U, 512U, ARM_MCI_TRANSFER_READ | ARM_MCI_TRANSFER_BLOCK);

  if (status == ARM_DRIVER_OK) {
    MCI_Events = 0U; //Clear MCI driver event flags

    cmd = 17U;       // Set READ_SINGLE_BLOCK command
    arg = 0U;        // Set sector number

    status  = drv->SendCommand (cmd, arg, ARM_MCI_RESPONSE_SHORT | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA, &resp);

    if (status == ARM_DRIVER_OK) {
      /* Wait for event */
      while ((MCI_Events & ARM_MCI_EVENT_COMMAND_COMPLETE) == 0U);
      // Command was successfully sent to memory card
      if ((resp & 0x03U) == 0U) {
        // Sector number is valid, wait until data transfer completes
        while ((MCI_Events & ARM_MCI_EVENT_TRANSFER_COMPLETE) == 0U);
        // Data was successfully read from memory card
        // ...
      }
    }
  }
}

/* Usage example: ARM_MCI_AbortTransfer -------------------------------------*/

void abort_data_transfer (ARM_DRIVER_MCI *drv) {
  ARM_MCI_STATUS drv_status;

  drv_status = drv->GetStatus();
  
  if (drv_status.transfer_active == 1U) {
    // Data transfer is active, abort the transfer
    if (drv->AbortTransfer() == ARM_DRIVER_OK) {
      // Transfer aborted
      // ...
    }
  }
}

/* Usage example: ARM_MCI_GetStatus -----------------------------------------*/

void check_transfer_status (ARM_DRIVER_MCI *drv) {
  ARM_MCI_STATUS drv_status;

  drv_status = drv->GetStatus();

  if (drv_status.transfer_active == 1U) {
    // Data transfer is active
  }
  
  if (drv_status.transfer_timeout == 1U) {
    // Data not received, timeout expired
  }
  
  if (drv_status.transfer_error == 1U) {
    // Data transfer ended with error
  }
}

/* Usage example: ARM_MCI_SignalEvent ---------------------------------------*/

void MCI_SignalEvent_Callback (uint32_t event) {
  if ((event & ARM_MCI_EVENT_CARD_INSERTED) != 0U) {
    // Memory card was inserted into socket
  }
  if ((event & ARM_MCI_EVENT_CARD_REMOVED) != 0U) {
    // Memory card was removed from socket
  }

  if ((event & ARM_MCI_EVENT_COMMAND_COMPLETE) != 0U) {
    // Command was successfully sent to memory card
  }
  if ((event & ARM_MCI_EVENT_COMMAND_TIMEOUT) != 0U) {
    // Command response was not received in time
  }
  if ((event & ARM_MCI_EVENT_COMMAND_ERROR) != 0U) {
    // Command response was invalid
  }

  if ((event & ARM_MCI_EVENT_TRANSFER_COMPLETE) != 0U) {
    // Data successfully transferred from/to memory card
  }
  if ((event & ARM_MCI_EVENT_TRANSFER_TIMEOUT) != 0U) {
    // Data not transferred from/to memory card, timeout expired
  }
  if ((event & ARM_MCI_EVENT_TRANSFER_ERROR) != 0U) {
    // Data transfer ended with errors
  }
  
  if ((event & ARM_MCI_EVENT_SDIO_INTERRUPT) != 0U) {
    // SD I/O card sent interrupt request
  }
  
  if ((event & ARM_MCI_EVENT_CCS) != 0U) {
    // CE-ATA command completion signal received
  }
  if ((event & ARM_MCI_EVENT_CCS_TIMEOUT) != 0U) {
    // CE-ATA command completion signal wait timeout expired
  }
}
