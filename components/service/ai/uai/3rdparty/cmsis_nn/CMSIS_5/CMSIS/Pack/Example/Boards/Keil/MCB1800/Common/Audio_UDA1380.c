/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Name:    Audio_USA1380.c
 * Purpose: Audio codec UDA1380 interface
 * Rev.:    1.01
 *
 *----------------------------------------------------------------------------*/

 #include "Driver_I2C.h"
#include "Driver_SAI.h"
#include "Board_Audio.h"

#include "SCU_LPC18xx.h"
#include "GPIO_LPC18xx.h"

// Auto gain control
#ifndef AUDIO_VOLUME_AUTO_GAIN
#define AUDIO_VOLUME_AUTO_GAIN    0x80
#endif

// UDA1380 input selection
#ifndef UDA1380_INPUT_LINE_IN
#define UDA1380_INPUT_MIC         1
#endif

// PGA gain (from 0 to 24 dB in steps of 3dB)
#ifndef UDA1380_PGA_GAIN
#define UDA1380_PGA_GAIN          6
#endif

// VGA gain (from 0 to 30 dB in steps of 2dB)
#ifndef UDA1380_VGA_GAIN
#define UDA1380_VGA_GAIN          0
#endif

// AGC time constant settings (see AGC register in UDA1380 datasheet)
#ifndef UDA1380_AGC_TIME
#define UDA1380_AGC_TIME          0
#endif


// UDA1380 Reset GPIO Definitions
const GPIO_ID UDA1380_RESET_GPIO = { 4, 0 };

// UDA1380 Reset pin definitions
const PIN_ID UDA1380_RESET_PIN   = { 8, 0, (SCU_PIN_CFG_PULLUP_DIS | SCU_PIN_CFG_MODE(0))};


#ifndef UDA1380_I2C_PORT
#define UDA1380_I2C_PORT          0         // I2C Port number
#endif

#define UDA1380_I2C_ADDR          0x1A      // UDA1380 I2C address

// I2C Driver
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(UDA1380_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(UDA1380_I2C_PORT))

#ifndef UDA1380_SAI_PORT
#define UDA1380_SAI_PORT          0         // SAI Port number
#endif

// SAI Driver
#define _SAI_Driver_(n)  Driver_SAI##n
#define  SAI_Driver_(n) _SAI_Driver_(n)
extern ARM_DRIVER_SAI    SAI_Driver_(UDA1380_SAI_PORT);
#define ptrSAI         (&SAI_Driver_(UDA1380_SAI_PORT))

// UDA1380 Register addresses
#define REG_CLOCK_SETTINGS        0x00U
#define REG_I2S_BUS_SETTINGS      0x01U
#define REG_POWER_CONTROL         0x02U
#define REG_ANALOG_MIXER          0x03U
#define REG_HEADPHONE_AMPLIFIER   0x04U
#define REG_MASTER_VOLUME         0x10U
#define REG_MIXER_VOLUME          0x11U
#define REG_MODE_SEL_BASS_TREBLE  0x12U
#define REG_MASTER_MUTE           0x13U
#define REG_MIXER                 0x14U
#define REG_DECIMATOR_VOLUME      0x20U
#define REG_PGA_SETTINGS          0x21U
#define REG_ADC_SETTINGS          0x22U
#define REG_AGC_SETTINGS          0x23U

Audio_SignalEvent_t Callback_event;

typedef struct _STREAM_FLAGS {
  uint8_t running  :1U;
  uint8_t paused   :1U;
  uint8_t mono     :1U;
  uint8_t agc      :1U;
} flags;

typedef struct _STREAM {
  uint32_t data_size;
  void    *data_buf;
  flags    flag;
  uint8_t  data_bits;
  uint32_t freq;
  uint8_t  vol_lch;
  uint8_t  vol_rch;
} STREAM;

// Local variables
static uint8_t  DeviceAddr;
static uint8_t  SignalSendComplete;
static uint8_t  SignalTxUnderflow;
static STREAM   Stream[2U];

#define STREAM_OUT ( 0U )
#define STREAM_IN  ( 1U )
#define STREAM_IDX(stream) ((stream & 0x80U) == 0U ? STREAM_OUT : STREAM_IN)

// Callback
/**
  \fn          void (uint8_t reg, uint16_t reg_val)
  \brief       UDA1380 callback function
  \param[in]   event
*/
void UDA1380_cb (uint32_t event) {

  if (event & ARM_SAI_EVENT_FRAME_ERROR) {
    // Audio interface does not support FRAME ERRORS
    event &= ~ARM_SAI_EVENT_FRAME_ERROR;
  }

  if (event & ARM_SAI_EVENT_TX_UNDERFLOW) {
    if ((Stream[STREAM_OUT].flag.running == 0U) ||
        (Stream[STREAM_OUT].flag.paused  == 1U)) {
        SignalTxUnderflow = 1;
        // TX running only because of codec clock
        event &= ~ARM_SAI_EVENT_TX_UNDERFLOW;
      }
  }

  if (event & ARM_SAI_EVENT_SEND_COMPLETE) {
    if (Stream[STREAM_OUT].flag.paused == 1U) {
        SignalSendComplete = 1;
        // TX running only because of codec clock
        event &= ~ARM_SAI_EVENT_SEND_COMPLETE;
      }
  }

  if ((Callback_event != NULL) && (event != 0U)) {
    Callback_event (event);
  }
}

// Local functions
/**
  \fn          int32_t UDA1380_RegRead (uint8_t reg, uint16_t * reg_val)
  \brief       Read value from UDA1380 register
  \param[in]   reg      UDA1380 register address
  \param[Out]  reg_val  Pointer to value read from UDA1380 register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t UDA1380_RegRead (uint8_t reg, uint16_t *reg_val) {
  uint8_t  val[2];

  if (ptrI2C->MasterTransmit (DeviceAddr, &reg, 1U, false) != ARM_DRIVER_OK) {
    return -1;
  }
  while (ptrI2C->GetStatus().busy);

  if (ptrI2C->MasterReceive  (DeviceAddr, val, 2U, false) != ARM_DRIVER_OK) {
    return -1;
  }
  while (ptrI2C->GetStatus().busy);

  *reg_val  = val[0] << 8;
  *reg_val |= val[1];

  return 0;
}

/**
  \fn          int32_t UDA1380_RegWrite (uint8_t reg, uint16_t reg_val)
  \brief       Write value to UDA1380 register
  \param[in]   reg      UDA1380 register address
  \param[in]   reg_val  Value to be written to UDA1380 register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t UDA1380_RegWrite (uint8_t reg, uint16_t reg_val) {
  uint8_t   val[3];
  uint16_t  rd_val;

  val[0] =  reg;
  val[1] = (reg_val >> 8) & 0xFFU;
  val[2] =  reg_val & 0xFFU;

  if (ptrI2C->MasterTransmit (DeviceAddr, val, 3U, false) != ARM_DRIVER_OK) {
    return -1;
  }
  while (ptrI2C->GetStatus().busy);

  UDA1380_RegRead (reg, &rd_val);
  while (ptrI2C->GetStatus().busy);

  if (reg_val != rd_val) {
    return -1;
  }

  return 0;
}

/**
  \fn          int32_t SAI_Configure (uint8_t stream)
  \brief       Configure SAI for selected stream
  \param[in]   stream
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t SAI_Configure (uint8_t stream) {
  uint32_t cfg, freq_set;
  STREAM *ptrStream;

  ptrStream = &Stream[STREAM_IDX(stream)];

  cfg = ARM_SAI_MODE_MASTER                      |
        ARM_SAI_PROTOCOL_I2S                     |
        ARM_SAI_DATA_SIZE(ptrStream->data_bits)  |
        ARM_SAI_COMPANDING_NONE                  |
        ARM_SAI_MSB_FIRST                        |
        ARM_SAI_MCLK_PIN_INACTIVE;

  if (ptrStream->flag.mono == 1U) {
    cfg |= ARM_SAI_MONO_MODE;
  }

  freq_set = ARM_SAI_MCLK_PRESCALER(256U);

  switch (stream) {
    case AUDIO_STREAM_OUT:
      cfg |= ARM_SAI_CONFIGURE_TX |
             ARM_SAI_ASYNCHRONOUS;
      freq_set |= (ptrStream->freq & ARM_SAI_AUDIO_FREQ_Msk);
      break;
    case AUDIO_STREAM_IN:
      cfg |= ARM_SAI_CONFIGURE_RX |
             ARM_SAI_ASYNCHRONOUS;
      freq_set |= (ptrStream->freq & ARM_SAI_AUDIO_FREQ_Msk);
      break;
    default: return (-1);
  }

  if (ptrSAI->Control (cfg, ARM_SAI_FRAME_LENGTH(ptrStream->data_bits * 2U), freq_set) != ARM_DRIVER_OK) {
    return -1;
  }

  return 0;
}


// Driver functions

/**
  \fn          int32_t Audio_Initialize (Audio_SignalEvent_t cb_event)
  \brief       Initialize Audio Interface
  \param[in]   cb_event  pointer to event notification function
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_Initialize (Audio_SignalEvent_t cb_event) {
  uint16_t reg_val, val;

  Callback_event = cb_event;

  // Clear Stream flags
  Stream[STREAM_IN].flag.running  = 0U;
  Stream[STREAM_IN].flag.paused   = 0U;
  Stream[STREAM_IN].flag.mono     = 0U;
  Stream[STREAM_IN].flag.agc      = 0U;
  Stream[STREAM_OUT].flag.running = 0U;
  Stream[STREAM_OUT].flag.paused  = 0U;
  Stream[STREAM_OUT].flag.mono    = 0U;
  Stream[STREAM_OUT].data_size    = 0U;
  Stream[STREAM_OUT].data_buf     = NULL;

  // Set Default values
  Stream[STREAM_IN].data_bits     = 8U;
  Stream[STREAM_IN].freq          = 8000U;
  Stream[STREAM_OUT].data_bits    = 8U;
  Stream[STREAM_OUT].freq         = 8000U;

  SignalSendComplete = 0U;
  SignalTxUnderflow  = 0U;

  // Reset pin configuration
  SCU_PinConfigure(UDA1380_RESET_PIN.port,
                   UDA1380_RESET_PIN.num,
                   UDA1380_RESET_PIN.config_val);

  GPIO_PortClock(1);
  GPIO_PinWrite(UDA1380_RESET_GPIO.port, UDA1380_RESET_GPIO.num, 0U);
  GPIO_SetDir(UDA1380_RESET_GPIO.port, UDA1380_RESET_GPIO.num, GPIO_DIR_OUTPUT);

  // I2C Initialization and configuration
  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  ptrI2C->Control      (ARM_I2C_BUS_CLEAR, 0U);

  // SAI Initialization
  ptrSAI->Initialize   (UDA1380_cb);
  ptrSAI->PowerControl (ARM_POWER_FULL);

  // Set default TX SAI configuration
  SAI_Configure (AUDIO_STREAM_OUT);

  // Reset codec procedure
  // Start transmitter to run Codec clock
  ptrSAI->Control      (ARM_SAI_CONTROL_TX, 1U, 0U);
  //Wait
  for (val = 0xFFFFU; val; val--) { __nop(); }
  // reset
  GPIO_PinWrite(UDA1380_RESET_GPIO.port, UDA1380_RESET_GPIO.num, 1U);
  // Wait
  for (val = 0xFFFFU; val; val--) { __nop(); }
  GPIO_PinWrite(UDA1380_RESET_GPIO.port, UDA1380_RESET_GPIO.num, 0U);
  // Wait
  for (val = 0xFFFFU; val; val--) { __nop(); }

  // Disable SAI receiver
  ptrSAI->Control      (ARM_SAI_CONTROL_RX, 0, 0);

  // Init UDA1380 device
  DeviceAddr = UDA1380_I2C_ADDR;

  // UDA1370 Power control settings
  // Power on: WSPLL, headphone drive, DAC, BIAS, LNA, PGAL, ADCL, PGAR, ADCR;
  reg_val = (1U << 15) | (1U << 13) | (1U << 10) | (1U << 8) |
            (1U <<  4) | (1U <<  3) | (1U <<  2) | (1U << 1) | 1U;
  if (UDA1380_RegWrite (REG_POWER_CONTROL, reg_val) == -1)    { return -1; }

  // Enable clocks: ADC, DEC, DAC,INT
  if (UDA1380_RegWrite (REG_CLOCK_SETTINGS,   0x0F30U) == -1) { return -1; }

  // Mute Analog mixer
  if (UDA1380_RegWrite (REG_ANALOG_MIXER,     0x3F3FU) == -1) { return -1; }

  if (UDA1380_RegWrite (REG_MASTER_MUTE,           0U) == -1) { return -1; }

  // Configure Audio input
#ifdef UDA1380_INPUT_MIC
  // Select Microphone as input and set VGA gain
  val = 0x0DU |((UDA1380_VGA_GAIN / 2U) << 8);
  if (UDA1380_RegWrite (REG_ADC_SETTINGS,         val) == -1) { return -1; }
  // Set PGA Gain to 0dB
  if (UDA1380_RegWrite (REG_PGA_SETTINGS,     0x0000U) == -1) { return -1; }
#endif

#ifdef UDA1380_INPUT_LINE_IN
  // Select Line in as input and set VGA gain to 0
  if (UDA1380_RegWrite (REG_ADC_SETTINGS,     0x0001U) == -1) { return -1; }

  // Set PGA Gain
  val = UDA1380_PGA_GAIN / 3;
  val |= val << 8;
  if (UDA1380_RegWrite (REG_PGA_SETTINGS,         val) == -1) { return -1; }
#endif

  if (UDA1380_RegWrite (REG_AGC_SETTINGS,           0) == -1) { return -1; }

  return 0;
}

/**
  \fn          int32_t Audio_Uninitialize (void)
  \brief       De-initialize Audio Interface
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_Uninitialize (void) {

  // Abort SAI Send and receive
  ptrSAI->Control (ARM_SAI_ABORT_SEND,    0U, 0U);
  ptrSAI->Control (ARM_SAI_ABORT_RECEIVE, 0U, 0U);

  // Disable clocks: ADC, DEC, DAC,INT
  if (UDA1380_RegWrite (REG_CLOCK_SETTINGS, 0U) == -1) { return -1; }

  // UDA1370 Power Power Off
  if (UDA1380_RegWrite (REG_POWER_CONTROL,  0U) == -1) { return -1; }

  // Disable SAI transmitter and receiver
  ptrSAI->Control (ARM_SAI_CONTROL_TX, 0U, 0U);
  ptrSAI->Control (ARM_SAI_CONTROL_RX, 0U, 0U);

  return 0;
}

/**
  \fn          int32_t Audio_SendData (const void *data, uint32_t num)
  \brief       Start sending data to Audio output stream
  \param[in]   data  pointer to buffer with data to send
  \param[in]   num   number of data items to send
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_SendData (const void *data, uint32_t num) {
  if ((Stream[STREAM_OUT].flag.running == 0U) ||
      (Stream[STREAM_OUT].flag.paused  == 1U)) {
    // Save data info
    Stream[STREAM_OUT].data_buf = (void *)data;
    Stream[STREAM_OUT].data_size = num;
  } else {
    Stream[STREAM_OUT].data_buf = NULL;
    Stream[STREAM_OUT].data_size = 0;
    if (ptrSAI->Send (data, num) != ARM_DRIVER_OK) { return -1; }
  }
  return 0;
}

/**
  \fn          int32_t Audio_ReceiveData (void *data, uint32_t num)
  \brief       Start receiving data from Audio input stream
  \param[out]  data  pointer to buffer for data to receive
  \param[in]   num   number of data items to send
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_ReceiveData (void *data, uint32_t num) {
  if (ptrSAI->Receive (data, num) != ARM_DRIVER_OK) { return -1; }
  return 0;
}

/**
  \fn          int32_t Audio_Start (uint8_t stream)
  \brief       Start Audio stream
  \param[in]   stream  stream identifier
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_Start (uint8_t stream) {
  void * data;
  uint32_t num, evt = 0;

  switch (stream) {
    case AUDIO_STREAM_OUT:
      Stream[STREAM_OUT].flag.running = 1U;
      if (Stream[STREAM_OUT].flag.paused == 0U) {
        if (Stream[STREAM_OUT].data_size != 0U) {
          SignalSendComplete = 0U;
          SignalTxUnderflow  = 0U;
          num  = Stream[STREAM_OUT].data_size;
          data = Stream[STREAM_OUT].data_buf;
          Stream[STREAM_OUT].data_size = 0U;
          Stream[STREAM_OUT].data_buf  = NULL;;
          if (ptrSAI->Send (data, num) != ARM_DRIVER_OK) { return -1; }
        } else {
          if (SignalSendComplete == 1U) { evt |= AUDIO_EVENT_SEND_COMPLETE; }
          if (SignalTxUnderflow  == 1U) { evt |= AUDIO_EVENT_TX_UNDERFLOW ; }

          SignalSendComplete = 0U;
          SignalTxUnderflow  = 0U;
          if ((Callback_event != NULL) && (evt != 0U)) { Callback_event (evt); }
        }
      }
      break;
    case AUDIO_STREAM_IN:
      Stream[STREAM_IN].flag.running = 1U;
      ptrSAI->Control (ARM_SAI_CONTROL_RX, 1U, 0U);
      break;
    default: return (-1);
  }

  return 0;
}

/**
  \fn          int32_t Audio_Stop (uint8_t stream)
  \brief       Stop Audio stream
  \param[in]   stream  stream identifier
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_Stop (uint8_t stream) {
 
  switch (stream) {
    case AUDIO_STREAM_OUT:
      Stream[STREAM_OUT].flag.running = 0U;
      // Abort transfer
      ptrSAI->Control (ARM_SAI_ABORT_SEND, 1U, 0U);
      break;
    case AUDIO_STREAM_IN:
      Stream[STREAM_IN].flag.running = 0U;
      ptrSAI->Control (ARM_SAI_CONTROL_RX, 0U, 0U);
      // Abort transfer
      ptrSAI->Control (ARM_SAI_ABORT_RECEIVE, 1U, 0U);
      break;
    default: return (-1);
  }

  return 0;
}

/**
  \fn          int32_t Audio_Pause (uint8_t stream)
  \brief       Pause Audio stream
  \param[in]   stream  stream identifier
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_Pause (uint8_t stream) {

  switch (stream) {
    case AUDIO_STREAM_OUT:
      if (Stream[STREAM_OUT].flag.running == 0U) { return 0; }
      SignalSendComplete = 0U;
      SignalTxUnderflow  = 0U;
      Stream[STREAM_OUT].flag.paused  = 1U;
      break;
    case AUDIO_STREAM_IN:
      if (Stream[STREAM_IN].flag.running == 0U) { return 0; }
      Stream[STREAM_IN].flag.paused  = 1U;
      ptrSAI->Control (ARM_SAI_CONTROL_RX, 0U, 0U);
      break;
    default: return (-1);
  }

  return 0;
}

/**
  \fn          int32_t Audio_Resume (uint8_t stream)
  \brief       Resume Audio stream
  \param[in]   stream  stream identifier
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_Resume (uint8_t stream) {
  uint32_t num, evt = 0;
  void * data;

  switch (stream) {
    case AUDIO_STREAM_OUT:
      if ((Stream[STREAM_OUT].flag.paused  == 1U) &&
          (Stream[STREAM_OUT].flag.running == 1U)) {
        Stream[STREAM_OUT].flag.paused      = 0U;
        if (Stream[STREAM_OUT].data_size != 0) {
          SignalSendComplete = 0U;
          SignalTxUnderflow  = 0U;
          num  = Stream[STREAM_OUT].data_size;
          data = Stream[STREAM_OUT].data_buf;
          Stream[STREAM_OUT].data_size = 0U;
          Stream[STREAM_OUT].data_buf  = NULL;
          if (ptrSAI->Send (data, num) != ARM_DRIVER_OK) { return -1; }
        } else {
          if (SignalSendComplete == 1U) { evt |= AUDIO_EVENT_SEND_COMPLETE; }
          if (SignalTxUnderflow  == 1U) { evt |= AUDIO_EVENT_TX_UNDERFLOW ; }

          SignalSendComplete = 0U;
          SignalTxUnderflow  = 0U;

          if ((Callback_event != NULL) && (evt != 0U)) { Callback_event (evt); }
        }
      }
      break;
    case AUDIO_STREAM_IN:
      if (Stream[STREAM_IN].flag.paused != 0U) {
        Stream[STREAM_IN].flag.paused  = 0U ;
        ptrSAI->Control (ARM_SAI_CONTROL_RX, 1U, 0U);
      }
      break;
    default: return (-1);
  }

  return 0;
}

/**
  \fn          int32_t Audio_SetVolume (uint8_t stream, uint8_t channel, uint8_t volume)
  \brief       Set volume level for Audio stream
  \param[in]   stream   stream identifier
  \param[in]   channel  channel identifier
  \param[in]   volume   volume level (0..100)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_SetVolume (uint8_t stream, uint8_t channel, uint8_t volume) {
  uint32_t out_vol;
  int32_t   in_vol;
  uint16_t reg_val;
  STREAM *ptrStream;

  ptrStream = &Stream[STREAM_IDX(stream)];

  switch (stream) {
    case AUDIO_STREAM_OUT:
      // Check if Volume is in range
      if (volume > 100U) { return -1; }

      // Scale master volume value
      out_vol  = (252U * volume) / 100U;
      out_vol  = 252U - out_vol;
      out_vol &= 0xFFU;

      reg_val = 0U;

      // Set master volume
      switch (channel) {
        case AUDIO_CHANNEL_LEFT:
          ptrStream->vol_lch = out_vol;
          reg_val           |= out_vol | (ptrStream->vol_rch << 8);
          break;
        case AUDIO_CHANNEL_RIGHT:
          ptrStream->vol_rch = out_vol;
          reg_val           |= ((out_vol & 0xFFU) << 8) |  ptrStream->vol_lch;
          break;
        case AUDIO_CHANNEL_MASTER:
          ptrStream->vol_lch = out_vol;
          ptrStream->vol_rch = out_vol;
          reg_val           |= ((out_vol & 0xFFU) << 8) | (out_vol);
          break;
        default: return -1;
      }
      if (UDA1380_RegWrite (REG_MASTER_VOLUME,  reg_val) == -1) { return -1; }
      break;
    case AUDIO_STREAM_IN:

      if (volume & AUDIO_VOLUME_AUTO_GAIN) {
#ifdef UDA1380_INPUT_MIC
        if (channel != AUDIO_CHANNEL_MASTER) { return -1; }

        // Volume level
        volume &= ~AUDIO_VOLUME_AUTO_GAIN;

        // Check if Volume is in range
        if (volume > 100U) { return -1; }

        // Set AGC flag;
        ptrStream->flag.agc = 1U;

        // Scale volume
        volume /= 25;
        if (volume > 3) { volume = 3 ; }
        reg_val = 1 | (volume << 2) | ((UDA1380_AGC_TIME & 7) << 8);

        // Enable and configure AGC
        if (UDA1380_RegWrite (REG_AGC_SETTINGS,  reg_val) == -1) { return -1; }
#else
        return -1;
#endif
      } else {

        // Clear AGC flag
        ptrStream->flag.agc = 0U;

        // Check if Volume is in range
        if (volume > 100U) { return -1; }

        // Decimator volume
        in_vol  = (176 * volume) / 100;
        in_vol -= 128;

        reg_val = 0;

        // Set master volume
        switch (channel) {
          case AUDIO_CHANNEL_LEFT:
            ptrStream->vol_lch = in_vol;
            reg_val           |= ((in_vol & 0xFFU) << 8) | ptrStream->vol_rch;
            break;
          case AUDIO_CHANNEL_RIGHT:
            ptrStream->vol_rch = in_vol;
            reg_val           |= in_vol | ((ptrStream->vol_lch & 0xFFU) << 8);
            break;
          case AUDIO_CHANNEL_MASTER:
            ptrStream->vol_lch = in_vol;
            ptrStream->vol_rch = in_vol;
            reg_val           |= ((in_vol & 0xFFU) << 8) | (in_vol);
            break;
          default: return -1;
        }
        if (UDA1380_RegWrite (REG_DECIMATOR_VOLUME,  reg_val) == -1) { return -1; }

#ifdef UDA1380_INPUT_MIC
        // Disable AGC
        if (UDA1380_RegWrite (REG_AGC_SETTINGS,            0) == -1) { return -1; }
#endif
      }

      break;
    default: return (-1);
  }

  return 0;
}

/**
  \fn          int32_t Audio_SetMute (uint8_t stream, uint8_t channel, bool mute)
  \brief       Set mute state for Audio stream
  \param[in]   stream   stream identifier
  \param[in]   channel  channel identifier
  \param[in]   mute     mute state
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_SetMute (uint8_t stream, uint8_t channel, bool mute) {
  uint16_t reg_val;
  STREAM *ptrStream;

  ptrStream = &Stream[STREAM_IDX(stream)];

  reg_val = 0U;
  switch (stream) {
    case AUDIO_STREAM_OUT:
      switch (channel) {
        case AUDIO_CHANNEL_LEFT:
          // Set or Reset mute on left channel
          if (mute == true) { reg_val = 0xFCU; }
          else              { reg_val = ptrStream->vol_lch; }

          // Right channel volume is unchanged
          reg_val |= (ptrStream->vol_rch << 8);
          break;
        case AUDIO_CHANNEL_RIGHT:
          // Set or Reset mute on Right channel
          if (mute == true) { reg_val = (0xFCU << 8); }
          else              { reg_val = (ptrStream->vol_rch << 8); }

          // Left channel volume is unchanged
          reg_val |= ptrStream->vol_lch;
          break;
        case AUDIO_CHANNEL_MASTER:
          // Set or reset mute on both channels (left and right)
          if (mute == true) { reg_val = (0xFCU << 8) | 0xFCU; }
          else              { reg_val = (ptrStream->vol_rch << 8) | ptrStream->vol_lch; }
          break;
        default: return -1;
      }
      if (UDA1380_RegWrite (REG_MASTER_VOLUME,  reg_val) == -1) { return -1; }
      break;
    case AUDIO_STREAM_IN:
#ifdef UDA1380_INPUT_MIC
      if (ptrStream->flag.agc != 0U) { return -1; }
#endif
      switch (channel) {
        case AUDIO_CHANNEL_LEFT:
          // Set or Reset mute on left channel
          if (mute == true) { reg_val = (0x80U << 8); }
          else              { reg_val = (ptrStream->vol_lch << 8); }

          // Right channel volume is unchanged
          reg_val |= ptrStream->vol_rch;
          break;
        case AUDIO_CHANNEL_RIGHT:
          // Set or Reset mute on Right channel
          if (mute == true) { reg_val = 0x80U; }
          else              { reg_val = ptrStream->vol_rch; }

          // Left channel volume is unchanged
          reg_val |= ptrStream->vol_lch << 8;
          break;
        case AUDIO_CHANNEL_MASTER:
          // Set or reset mute on both channels (left and right)
          if (mute == true) { reg_val = (0x80U << 8) | 0x80U; }
          else              { reg_val = (ptrStream->vol_lch << 8) | ptrStream->vol_rch; }
          break;
        default: return -1;
      }
      if (UDA1380_RegWrite (REG_DECIMATOR_VOLUME,  reg_val) == -1) { return -1; }
      break;
    default: return (-1);
  }

  return 0;
}

/**
  \fn          int32_t Audio_SetDataFormat (uint8_t stream, uint8_t format)
  \brief       Set Audio data format
  \param[in]   stream  stream identifier
  \param[in]   format  data format
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_SetDataFormat (uint8_t stream, uint8_t format) {
  STREAM *ptrStream;

  ptrStream = &Stream[STREAM_IDX(stream)];

  switch (format) {
    case AUDIO_DATA_8_MONO:
      ptrStream->data_bits = 8U;
      ptrStream->flag.mono = 1U;
      break;
    case AUDIO_DATA_16_MONO:
      ptrStream->data_bits = 16U;
      ptrStream->flag.mono = 1U;
      break;
    case AUDIO_DATA_32_MONO:
      ptrStream->data_bits = 32U;
      ptrStream->flag.mono = 1U;
      break;
    case AUDIO_DATA_8_STEREO:
      ptrStream->data_bits = 8U;
      ptrStream->flag.mono = 0U;
      break;
    case AUDIO_DATA_16_STEREO:
      ptrStream->data_bits = 16U;
      ptrStream->flag.mono = 0U;
      break;
    case AUDIO_DATA_32_STEREO:
      ptrStream->data_bits = 32U;
      ptrStream->flag.mono = 0U;
      break;
    default: return -1;
  }

  return (SAI_Configure (stream));
}

/**
  \fn          int32_t Audio_SetFrequency (uint8_t stream, uint32_t frequency)
  \brief       Set Audio stream frequency
  \param[in]   stream     stream identifier
  \param[in]   frequency  Audio frequency in Hz
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Audio_SetFrequency (uint8_t stream, uint32_t frequency) {
  uint16_t  reg_val;
  uint16_t  pll;
  STREAM   *ptrStream;

  ptrStream = &Stream[STREAM_IDX(stream)];

  // Configure Clock register
  if (UDA1380_RegRead (REG_CLOCK_SETTINGS, &reg_val) == -1) { return -1; }

  // Set proper PLL (in REG_CLOCK_SETTINGS), regarding input freq range
  if      (frequency <    6250U) { return -1; }
  if      (frequency <=  12500U) { pll = 0U;  }
  else if (frequency <=  25000U) { pll = 1U;  }
  else if (frequency <=  50000U) { pll = 2U;  }
  else if (frequency <= 100000U) { pll = 3U;  }
  else                           { return -1; }

  reg_val &= ~3U;
  reg_val |=  pll;

  // ADC_CLK and DAC_CLK are derived from the WSPLL
  reg_val |= (1U << 4) | (1U << 5);

  if (UDA1380_RegWrite (REG_CLOCK_SETTINGS, reg_val) == -1) { return -1; }

  ptrStream->freq = frequency;

  return (SAI_Configure (stream));
}
