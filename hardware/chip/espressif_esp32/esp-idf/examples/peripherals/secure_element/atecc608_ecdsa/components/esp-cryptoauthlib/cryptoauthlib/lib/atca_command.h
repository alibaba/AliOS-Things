/**
 * \file
 * \brief Microchip Crypto Auth device command object - this is a command builder only, it does
 * not send the command.  The result of a command method is a fully formed packet, ready to send
 * to the ATCAIFace object to dispatch.
 *
 * This command object supports the ATSHA and ATECC device family.
 * The command list is a superset of all device commands for this family.  The command object
 * differentiates the packet contents based on specific device type within the family.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef ATCA_COMMAND_H
#define ATCA_COMMAND_H

#include "atca_compiler.h"
#include "atca_status.h"
#include "atca_devtypes.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup command ATCACommand (atca_)
   \brief CryptoAuthLib command builder object, ATCACommand.  Member functions for the ATCACommand object.
   @{ */

/** \brief atca_command is the C object backing ATCACommand.
 */
struct atca_command
{
    ATCADeviceType dt;
    uint8_t        clock_divider;
    uint16_t       execution_time_msec;
};

/*--- ATCACommand ---------*/
typedef struct atca_command* ATCACommand;

ATCA_STATUS initATCACommand(ATCADeviceType device_type, ATCACommand ca_cmd);
ATCACommand newATCACommand(ATCADeviceType device_type);
void deleteATCACommand(ATCACommand *ca_cmd);

/* add ATCACommand declarations here
 *
 * since these are still C functions, not classes, naming is an important
 * consideration to keep the namespace from colliding with other 3rd party
 * libraries or even ourselves/ASF.
 *
 * Basic conventions:
 * all methods start with the prefix 'at'
 * all method names must be unique, obviously
 * all method implementations should be proceeded by their Doxygen comment header
 *
 **/


// this is the ATCACommand parameter structure.  The caller to the command method must
// initialize param1, param2 and data if appropriate.  The command method will fill in the rest
// and initialize the packet so it's ready to send via the ATCAIFace.
// this particular structure mimics the ATSHA and ATECC family device's command structures

// Note: pack @ 2 is required, @ 1 causes word alignment crash (though it should not), a known bug in GCC.
// @2, the wire still has the intended byte alignment with arm-eabi.  this is likely the least portable part of atca


#ifdef ATCA_NO_PRAGMA_PACK
typedef struct __attribute__ ((packed))
#else
#pragma pack( push, ATCAPacket, 2 )
typedef struct
#endif
/** \brief an ATCA packet structure.  This is a superset of the packet transmitted on the wire.  It's also
 * used as a buffer for receiving the response
 */
{

    // used for transmit/send
    uint8_t _reserved;  // used by HAL layer as needed (I/O tokens, Word address values)

    //--- start of packet i/o frame----
    uint8_t  txsize;
    uint8_t  opcode;
    uint8_t  param1;    // often same as mode
    uint16_t param2;
    uint8_t  data[192]; // includes 2-byte CRC.  data size is determined by largest possible data section of any
                        // command + crc (see: x08 verify data1 + data2 + data3 + data4)
                        // this is an explicit design trade-off (space) resulting in simplicity in use
                        // and implementation
    //--- end of packet i/o frame

    // used for receive
    uint8_t execTime;       // execution time of command by opcode

    // structure should be packed since it will be transmitted over the wire
    // this method varies by compiler.  As new compilers are supported, add their structure packing method here

} ATCAPacket;

#ifndef ATCA_NO_PRAGMA_PACK
#pragma pack( pop, ATCAPacket)
#endif


ATCA_STATUS atCheckMAC(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atCounter(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atDeriveKey(ATCACommand ca_cmd, ATCAPacket *packet, bool has_mac);
ATCA_STATUS atECDH(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atGenDig(ATCACommand ca_cmd, ATCAPacket *packet, bool is_no_mac_key);
ATCA_STATUS atGenKey(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atHMAC(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atInfo(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atLock(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atMAC(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atNonce(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atPause(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atPrivWrite(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atRandom(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atRead(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atSecureBoot(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atSHA(ATCACommand ca_cmd, ATCAPacket *packet, uint16_t write_context_size);
ATCA_STATUS atSign(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atUpdateExtra(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atVerify(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atWrite(ATCACommand ca_cmd, ATCAPacket *packet, bool has_mac);
ATCA_STATUS atAES(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atSelfTest(ATCACommand ca_cmd, ATCAPacket *packet);
ATCA_STATUS atKDF(ATCACommand ca_cmd, ATCAPacket *packet);

bool atIsSHAFamily(ATCADeviceType device_type);
bool atIsECCFamily(ATCADeviceType device_type);
ATCA_STATUS isATCAError(uint8_t *data);


// command helpers
void atCRC(size_t length, const uint8_t *data, uint8_t *crc_le);
void atCalcCrc(ATCAPacket *pkt);
ATCA_STATUS atCheckCrc(const uint8_t *response);


/* command definitions */

//! minimum number of bytes in command (from count byte to second CRC byte)
#define ATCA_CMD_SIZE_MIN       ((uint8_t)7)
//! maximum size of command packet (Verify)
#define ATCA_CMD_SIZE_MAX       ((uint8_t)4 * 36 + 7)
//! status byte for success
#define CMD_STATUS_SUCCESS      ((uint8_t)0x00)
//! status byte after wake-up
#define CMD_STATUS_WAKEUP       ((uint8_t)0x11)
//! command parse error
#define CMD_STATUS_BYTE_PARSE   ((uint8_t)0x03)
//! command ECC error
#define CMD_STATUS_BYTE_ECC     ((uint8_t)0x05)
//! command execution error
#define CMD_STATUS_BYTE_EXEC    ((uint8_t)0x0F)
//! communication error
#define CMD_STATUS_BYTE_COMM    ((uint8_t)0xFF)

/** \name Opcodes for Crypto Authentication device commands
   @{ */
#define ATCA_CHECKMAC     ((uint8_t)0x28)  //!< CheckMac command op-code
#define ATCA_DERIVE_KEY   ((uint8_t)0x1C)  //!< DeriveKey command op-code
#define ATCA_INFO         ((uint8_t)0x30)  //!< Info command op-code
#define ATCA_GENDIG       ((uint8_t)0x15)  //!< GenDig command op-code
#define ATCA_GENKEY       ((uint8_t)0x40)  //!< GenKey command op-code
#define ATCA_HMAC         ((uint8_t)0x11)  //!< HMAC command op-code
#define ATCA_LOCK         ((uint8_t)0x17)  //!< Lock command op-code
#define ATCA_MAC          ((uint8_t)0x08)  //!< MAC command op-code
#define ATCA_NONCE        ((uint8_t)0x16)  //!< Nonce command op-code
#define ATCA_PAUSE        ((uint8_t)0x01)  //!< Pause command op-code
#define ATCA_PRIVWRITE    ((uint8_t)0x46)  //!< PrivWrite command op-code
#define ATCA_RANDOM       ((uint8_t)0x1B)  //!< Random command op-code
#define ATCA_READ         ((uint8_t)0x02)  //!< Read command op-code
#define ATCA_SIGN         ((uint8_t)0x41)  //!< Sign command op-code
#define ATCA_UPDATE_EXTRA ((uint8_t)0x20)  //!< UpdateExtra command op-code
#define ATCA_VERIFY       ((uint8_t)0x45)  //!< GenKey command op-code
#define ATCA_WRITE        ((uint8_t)0x12)  //!< Write command op-code
#define ATCA_ECDH         ((uint8_t)0x43)  //!< ECDH command op-code
#define ATCA_COUNTER      ((uint8_t)0x24)  //!< Counter command op-code
#define ATCA_SHA          ((uint8_t)0x47)  //!< SHA command op-code
#define ATCA_AES          ((uint8_t)0x51)  //!< AES command op-code
#define ATCA_KDF          ((uint8_t)0x56)  //!< KDF command op-code
#define ATCA_SECUREBOOT   ((uint8_t)0x80)  //!< Secure Boot command op-code
#define ATCA_SELFTEST     ((uint8_t)0x77)  //!< Self test command op-code




/** @} */


/** \name Definitions of Data and Packet Sizes
   @{ */
#define ATCA_BLOCK_SIZE             (32)                               //!< size of a block
#define ATCA_WORD_SIZE              (4)                                //!< size of a word
#define ATCA_PUB_KEY_PAD            (4)                                //!< size of the public key pad
#define ATCA_SERIAL_NUM_SIZE        (9)                                //!< number of bytes in the device serial number
#define ATCA_RSP_SIZE_VAL           ((uint8_t)7)                       //!< size of response packet containing four bytes of data
#define ATCA_KEY_COUNT              (16)                               //!< number of keys
#define ATCA_ECC_CONFIG_SIZE        (128)                              //!< size of configuration zone
#define ATCA_SHA_CONFIG_SIZE        (88)                               //!< size of configuration zone
#define ATCA_OTP_SIZE               (64)                               //!< size of OTP zone
#define ATCA_DATA_SIZE              (ATCA_KEY_COUNT * ATCA_KEY_SIZE)   //!< size of data zone
#define ATCA_AES_GFM_SIZE            ATCA_BLOCK_SIZE                   //!< size of GFM data

#define ATCA_CHIPMODE_OFFSET           (19)                            //!< ChipMode byte offset within the configuration zone
#define ATCA_CHIPMODE_I2C_ADDRESS_FLAG ((uint8_t)0x01)                 //!< ChipMode I2C Address in UserExtraAdd flag
#define ATCA_CHIPMODE_TTL_ENABLE_FLAG  ((uint8_t)0x02)                 //!< ChipMode TTLenable flag
#define ATCA_CHIPMODE_WATCHDOG_MASK    ((uint8_t)0x04)                 //!< ChipMode watchdog duration mask
#define ATCA_CHIPMODE_WATCHDOG_SHORT   ((uint8_t)0x00)                 //!< ChipMode short watchdog (~1.3s)
#define ATCA_CHIPMODE_WATCHDOG_LONG    ((uint8_t)0x04)                 //!< ChipMode long watchdog (~13s)
#define ATCA_CHIPMODE_CLOCK_DIV_MASK   ((uint8_t)0xF8)                 //!< ChipMode clock divider mask
#define ATCA_CHIPMODE_CLOCK_DIV_M0     ((uint8_t)0x00)                 //!< ChipMode clock divider M0
#define ATCA_CHIPMODE_CLOCK_DIV_M1     ((uint8_t)0x28)                 //!< ChipMode clock divider M1
#define ATCA_CHIPMODE_CLOCK_DIV_M2     ((uint8_t)0x68)                 //!< ChipMode clock divider M2

#define ATCA_COUNT_SIZE             ((uint8_t)1)                       //!< Number of bytes in the command packet Count
#define ATCA_CRC_SIZE               ((uint8_t)2)                       //!< Number of bytes in the command packet CRC
#define ATCA_PACKET_OVERHEAD        (ATCA_COUNT_SIZE + ATCA_CRC_SIZE)  //!< Number of bytes in the command packet

#define ATCA_PUB_KEY_SIZE           (64)                               //!< size of a p256 public key
#define ATCA_PRIV_KEY_SIZE          (32)                               //!< size of a p256 private key
#define ATCA_SIG_SIZE               (64)                               //!< size of a p256 signature
#define ATCA_KEY_SIZE               (32)                               //!< size of a symmetric SHA key
#define RSA2048_KEY_SIZE            (256)                              //!< size of a RSA private key

#define ATCA_RSP_SIZE_MIN           ((uint8_t)4)                       //!< minimum number of bytes in response
#define ATCA_RSP_SIZE_4             ((uint8_t)7)                       //!< size of response packet containing 4 bytes data
#define ATCA_RSP_SIZE_72            ((uint8_t)75)                      //!< size of response packet containing 64 bytes data
#define ATCA_RSP_SIZE_64            ((uint8_t)67)                      //!< size of response packet containing 64 bytes data
#define ATCA_RSP_SIZE_32            ((uint8_t)35)                      //!< size of response packet containing 32 bytes data
#define ATCA_RSP_SIZE_16            ((uint8_t)19)                      //!< size of response packet containing 16 bytes data
#define ATCA_RSP_SIZE_MAX           ((uint8_t)75)                      //!< maximum size of response packet (GenKey and Verify command)

#define OUTNONCE_SIZE               (32)                               //!< Size of the OutNonce response expected from several commands

/** \name Definitions for Command Parameter Ranges
   @{ */
#define ATCA_KEY_ID_MAX             ((uint8_t)15)       //!< maximum value for key id
#define ATCA_OTP_BLOCK_MAX          ((uint8_t)1)        //!< maximum value for OTP block
/** @} */

/** \name Definitions for Indexes Common to All Commands
   @{ */
#define ATCA_COUNT_IDX              (0)     //!< command packet index for count
#define ATCA_OPCODE_IDX             (1)     //!< command packet index for op-code
#define ATCA_PARAM1_IDX             (2)     //!< command packet index for first parameter
#define ATCA_PARAM2_IDX             (3)     //!< command packet index for second parameter
#define ATCA_DATA_IDX               (5)     //!< command packet index for data load
#define ATCA_RSP_DATA_IDX           (1)     //!< buffer index of data in response
/** @} */

/** \name Definitions for Zone and Address Parameters
   @{ */
#define ATCA_ZONE_CONFIG                ((uint8_t)0x00)         //!< Configuration zone
#define ATCA_ZONE_OTP                   ((uint8_t)0x01)         //!< OTP (One Time Programming) zone
#define ATCA_ZONE_DATA                  ((uint8_t)0x02)         //!< Data zone
#define ATCA_ZONE_MASK                  ((uint8_t)0x03)         //!< Zone mask
#define ATCA_ZONE_ENCRYPTED             ((uint8_t)0x40)         //!< Zone bit 6 set: Write is encrypted with an unlocked data zone.
#define ATCA_ZONE_READWRITE_32          ((uint8_t)0x80)         //!< Zone bit 7 set: Access 32 bytes, otherwise 4 bytes.
#define ATCA_ADDRESS_MASK_CONFIG        (0x001F)                //!< Address bits 5 to 7 are 0 for Configuration zone.
#define ATCA_ADDRESS_MASK_OTP           (0x000F)                //!< Address bits 4 to 7 are 0 for OTP zone.
#define ATCA_ADDRESS_MASK               (0x007F)                //!< Address bit 7 to 15 are always 0.
#define ATCA_TEMPKEY_KEYID              (0xFFFF)                //!< KeyID when referencing TempKey
/** @} */

/** \name Definitions for Key types
   @{ */
#define ATCA_B283_KEY_TYPE          0       //!< B283 NIST ECC key
#define ATCA_K283_KEY_TYPE          1       //!< K283 NIST ECC key
#define ATCA_P256_KEY_TYPE          4       //!< P256 NIST ECC key
#define ATCA_AES_KEY_TYPE           6       //!< AES-128 Key
#define ATCA_SHA_KEY_TYPE           7       //!< SHA key or other data
/** @} */

/** \name Definitions for the AES Command
   @{ */
#define AES_MODE_IDX                ATCA_PARAM1_IDX         //!< AES command index for mode
#define AES_KEYID_IDX               ATCA_PARAM2_IDX         //!< AES command index for key id
#define AES_INPUT_IDX               ATCA_DATA_IDX           //!< AES command index for input data
#define AES_COUNT                   (23)                    //!< AES command packet size
#define AES_MODE_MASK               ((uint8_t)0xC7)         //!< AES mode bits 3 to 5 are 0
#define AES_MODE_KEY_BLOCK_MASK     ((uint8_t)0xC0)         //!< AES mode mask for key block field
#define AES_MODE_OP_MASK            ((uint8_t)0x07)         //!< AES mode operation mask
#define AES_MODE_ENCRYPT            ((uint8_t)0x00)         //!< AES mode: Encrypt
#define AES_MODE_DECRYPT            ((uint8_t)0x01)         //!< AES mode: Decrypt
#define AES_MODE_GFM                ((uint8_t)0x03)         //!< AES mode: GFM calculation
#define AES_MODE_KEY_BLOCK_POS      (6)                     //!< Bit shift for key block in mode
#define AES_DATA_SIZE               (16)                    //!< size of AES encrypt/decrypt data
#define AES_RSP_SIZE                ATCA_RSP_SIZE_16        //!< AES command response packet size
/** @} */

/** \name Definitions for the CheckMac Command
   @{ */
#define CHECKMAC_MODE_IDX                   ATCA_PARAM1_IDX     //!< CheckMAC command index for mode
#define CHECKMAC_KEYID_IDX                  ATCA_PARAM2_IDX     //!< CheckMAC command index for key identifier
#define CHECKMAC_CLIENT_CHALLENGE_IDX       ATCA_DATA_IDX       //!< CheckMAC command index for client challenge
#define CHECKMAC_CLIENT_RESPONSE_IDX        (37)                //!< CheckMAC command index for client response
#define CHECKMAC_DATA_IDX                   (69)                //!< CheckMAC command index for other data
#define CHECKMAC_COUNT                      (84)                //!< CheckMAC command packet size
#define CHECKMAC_MODE_CHALLENGE             ((uint8_t)0x00)     //!< CheckMAC mode	   0: first SHA block from key id
#define CHECKMAC_MODE_BLOCK2_TEMPKEY        ((uint8_t)0x01)     //!< CheckMAC mode bit   0: second SHA block from TempKey
#define CHECKMAC_MODE_BLOCK1_TEMPKEY        ((uint8_t)0x02)     //!< CheckMAC mode bit   1: first SHA block from TempKey
#define CHECKMAC_MODE_SOURCE_FLAG_MATCH     ((uint8_t)0x04)     //!< CheckMAC mode bit   2: match TempKey.SourceFlag
#define CHECKMAC_MODE_INCLUDE_OTP_64        ((uint8_t)0x20)     //!< CheckMAC mode bit   5: include first 64 OTP bits
#define CHECKMAC_MODE_MASK                  ((uint8_t)0x27)     //!< CheckMAC mode bits 3, 4, 6, and 7 are 0.
#define CHECKMAC_CLIENT_CHALLENGE_SIZE      (32)                //!< CheckMAC size of client challenge
#define CHECKMAC_CLIENT_RESPONSE_SIZE       (32)                //!< CheckMAC size of client response
#define CHECKMAC_OTHER_DATA_SIZE            (13)                //!< CheckMAC size of "other data"
#define CHECKMAC_CLIENT_COMMAND_SIZE        (4)                 //!< CheckMAC size of client command header size inside "other data"
#define CHECKMAC_CMD_MATCH                  (0)                 //!< CheckMAC return value when there is a match
#define CHECKMAC_CMD_MISMATCH               (1)                 //!< CheckMAC return value when there is a mismatch
#define CHECKMAC_RSP_SIZE                   ATCA_RSP_SIZE_MIN   //!< CheckMAC response packet size
/** @} */

/** \name Definitions for the Counter command
   @{ */
#define COUNTER_COUNT                       ATCA_CMD_SIZE_MIN
#define COUNTER_MODE_IDX                    ATCA_PARAM1_IDX         //!< Counter command index for mode
#define COUNTER_KEYID_IDX                   ATCA_PARAM2_IDX         //!< Counter command index for key id
#define COUNTER_MODE_MASK                   ((uint8_t)0x01)         //!< Counter mode bits 1 to 7 are 0
#define COUNTER_MAX_VALUE                   ((uint32_t)2097151)     //!< Counter maximum value of the counter
#define COUNTER_MODE_READ                   ((uint8_t)0x00)         //!< Counter command mode for reading
#define COUNTER_MODE_INCREMENT              ((uint8_t)0x01)         //!< Counter command mode for incrementing
#define COUNTER_RSP_SIZE                    ATCA_RSP_SIZE_4         //!< Counter command response packet size
#define COUNTER_SIZE                        ATCA_RSP_SIZE_MIN       //!< Counter size in binary
/** @} */

/** \name Definitions for the DeriveKey Command
   @{ */
#define DERIVE_KEY_RANDOM_IDX           ATCA_PARAM1_IDX     //!< DeriveKey command index for random bit
#define DERIVE_KEY_TARGETKEY_IDX        ATCA_PARAM2_IDX     //!< DeriveKey command index for target slot
#define DERIVE_KEY_MAC_IDX              ATCA_DATA_IDX       //!< DeriveKey command index for optional MAC
#define DERIVE_KEY_COUNT_SMALL          ATCA_CMD_SIZE_MIN   //!< DeriveKey command packet size without MAC
#define DERIVE_KEY_MODE                 ((uint8_t)0x04)     //!< DeriveKey command mode set to 4 as in datasheet
#define DERIVE_KEY_COUNT_LARGE          (39)                //!< DeriveKey command packet size with MAC
#define DERIVE_KEY_RANDOM_FLAG          ((uint8_t)4)        //!< DeriveKey 1. parameter; has to match TempKey.SourceFlag
#define DERIVE_KEY_MAC_SIZE             (32)                //!< DeriveKey MAC size
#define DERIVE_KEY_RSP_SIZE             ATCA_RSP_SIZE_MIN   //!< DeriveKey response packet size
/** @} */

/** \name Definitions for the ECDH Command
   @{ */
#define ECDH_PREFIX_MODE                    ((uint8_t)0x00)
#define ECDH_COUNT                          (ATCA_CMD_SIZE_MIN + ATCA_PUB_KEY_SIZE)
#define ECDH_MODE_SOURCE_MASK               ((uint8_t)0x01)
#define ECDH_MODE_SOURCE_EEPROM_SLOT        ((uint8_t)0x00)
#define ECDH_MODE_SOURCE_TEMPKEY            ((uint8_t)0x01)
#define ECDH_MODE_OUTPUT_MASK               ((uint8_t)0x02)
#define ECDH_MODE_OUTPUT_CLEAR              ((uint8_t)0x00)
#define ECDH_MODE_OUTPUT_ENC                ((uint8_t)0x02)
#define ECDH_MODE_COPY_MASK                 ((uint8_t)0x0C)
#define ECDH_MODE_COPY_COMPATIBLE           ((uint8_t)0x00)
#define ECDH_MODE_COPY_EEPROM_SLOT          ((uint8_t)0x04)
#define ECDH_MODE_COPY_TEMP_KEY             ((uint8_t)0x08)
#define ECDH_MODE_COPY_OUTPUT_BUFFER        ((uint8_t)0x0C)
#define ECDH_KEY_SIZE                       ATCA_BLOCK_SIZE     //!< ECDH output data size
#define ECDH_RSP_SIZE                       ATCA_RSP_SIZE_64    //!< ECDH command packet size
/** @} */

/** \name Definitions for the GenDig Command
   @{ */
#define GENDIG_ZONE_IDX             ATCA_PARAM1_IDX     //!< GenDig command index for zone
#define GENDIG_KEYID_IDX            ATCA_PARAM2_IDX     //!< GenDig command index for key id
#define GENDIG_DATA_IDX             ATCA_DATA_IDX       //!< GenDig command index for optional data
#define GENDIG_COUNT                ATCA_CMD_SIZE_MIN   //!< GenDig command packet size without "other data"
#define GENDIG_ZONE_CONFIG          ((uint8_t)0)        //!< GenDig zone id config. Use KeyID to specify any of the four 256-bit blocks of the Configuration zone.
#define GENDIG_ZONE_OTP             ((uint8_t)1)        //!< GenDig zone id OTP. Use KeyID to specify either the first or second 256-bit block of the OTP zone.
#define GENDIG_ZONE_DATA            ((uint8_t)2)        //!< GenDig zone id data. Use KeyID to specify a slot in the Data zone or a transport key in the hardware array.
#define GENDIG_ZONE_SHARED_NONCE    ((uint8_t)3)        //!< GenDig zone id shared nonce. KeyID specifies the location of the input value in the message generation.
#define GENDIG_ZONE_COUNTER         ((uint8_t)4)        //!< GenDig zone id counter. KeyID specifies the monotonic counter ID to be included in the message generation.
#define GENDIG_ZONE_KEY_CONFIG      ((uint8_t)5)        //!< GenDig zone id key config. KeyID specifies the slot for which the configuration information is to be included in the message generation.
#define GENDIG_RSP_SIZE             ATCA_RSP_SIZE_MIN   //!< GenDig command response packet size
/** @} */

/** \name Definitions for the GenKey Command
   @{ */
#define GENKEY_MODE_IDX             ATCA_PARAM1_IDX         //!< GenKey command index for mode
#define GENKEY_KEYID_IDX            ATCA_PARAM2_IDX         //!< GenKey command index for key id
#define GENKEY_DATA_IDX             (5)                     //!< GenKey command index for other data
#define GENKEY_COUNT                ATCA_CMD_SIZE_MIN       //!< GenKey command packet size without "other data"
#define GENKEY_COUNT_DATA           (10)                    //!< GenKey command packet size with "other data"
#define GENKEY_OTHER_DATA_SIZE      (3)                     //!< GenKey size of "other data"
#define GENKEY_MODE_MASK            ((uint8_t)0x1C)         //!< GenKey mode bits 0 to 1 and 5 to 7 are 0
#define GENKEY_MODE_PRIVATE         ((uint8_t)0x04)         //!< GenKey mode: private key generation
#define GENKEY_MODE_PUBLIC          ((uint8_t)0x00)         //!< GenKey mode: public key calculation
#define GENKEY_MODE_DIGEST          ((uint8_t)0x08)         //!< GenKey mode: PubKey digest will be created after the public key is calculated
#define GENKEY_MODE_PUBKEY_DIGEST   ((uint8_t)0x10)         //!< GenKey mode: Calculate PubKey digest on the public key in KeyId
#define GENKEY_PRIVATE_TO_TEMPKEY   ((uint16_t)0xFFFF)      //!< GenKey Create private key and store to tempkey (608 only)
#define GENKEY_RSP_SIZE_SHORT       ATCA_RSP_SIZE_MIN       //!< GenKey response packet size in Digest mode
#define GENKEY_RSP_SIZE_LONG        ATCA_RSP_SIZE_64        //!< GenKey response packet size when returning a public key
/** @} */

/** \name Definitions for the HMAC Command
   @{ */
#define HMAC_MODE_IDX               ATCA_PARAM1_IDX     //!< HMAC command index for mode
#define HMAC_KEYID_IDX              ATCA_PARAM2_IDX     //!< HMAC command index for key id
#define HMAC_COUNT                  ATCA_CMD_SIZE_MIN   //!< HMAC command packet size
#define HMAC_MODE_FLAG_TK_RAND      ((uint8_t)0x00)     //!< HMAC mode bit 2: The value of this bit must match the value in TempKey.SourceFlag or the command will return an error.
#define HMAC_MODE_FLAG_TK_NORAND    ((uint8_t)0x04)     //!< HMAC mode bit 2: The value of this bit must match the value in TempKey.SourceFlag or the command will return an error.
#define HMAC_MODE_FLAG_OTP88        ((uint8_t)0x10)     //!< HMAC mode bit 4: Include the first 88 OTP bits (OTP[0] through OTP[10]) in the message.; otherwise, the corresponding message bits are set to zero. Not applicable for ATECC508A.
#define HMAC_MODE_FLAG_OTP64        ((uint8_t)0x20)     //!< HMAC mode bit 5: Include the first 64 OTP bits (OTP[0] through OTP[7]) in the message.; otherwise, the corresponding message bits are set to zero. If Mode[4] is set, the value of this mode bit is ignored. Not applicable for ATECC508A.
#define HMAC_MODE_FLAG_FULLSN       ((uint8_t)0x40)     //!< HMAC mode bit 6: If set, include the 48 bits SN[2:3] and SN[4:7] in the message.; otherwise, the corresponding message bits are set to zero.
#define HMAC_MODE_MASK              ((uint8_t)0x74)     //!< HMAC mode bits 0, 1, 3, and 7 are 0.
#define HMAC_DIGEST_SIZE            (32)                //!< HMAC size of digest response
#define HMAC_RSP_SIZE               ATCA_RSP_SIZE_32    //!< HMAC command response packet size
/** @} */

/** \name Definitions for the Info Command
   @{ */
#define INFO_PARAM1_IDX             ATCA_PARAM1_IDX     //!< Info command index for 1. parameter
#define INFO_PARAM2_IDX             ATCA_PARAM2_IDX     //!< Info command index for 2. parameter
#define INFO_COUNT                  ATCA_CMD_SIZE_MIN   //!< Info command packet size
#define INFO_MODE_REVISION          ((uint8_t)0x00)     //!< Info mode Revision
#define INFO_MODE_KEY_VALID         ((uint8_t)0x01)     //!< Info mode KeyValid
#define INFO_MODE_STATE             ((uint8_t)0x02)     //!< Info mode State
#define INFO_MODE_GPIO              ((uint8_t)0x03)     //!< Info mode GPIO
#define INFO_MODE_VOL_KEY_PERMIT    ((uint8_t)0x04)     //!< Info mode GPIO
#define INFO_MODE_MAX               ((uint8_t)0x03)     //!< Info mode maximum value
#define INFO_NO_STATE               ((uint8_t)0x00)     //!< Info mode is not the state mode.
#define INFO_OUTPUT_STATE_MASK      ((uint8_t)0x01)     //!< Info output state mask
#define INFO_DRIVER_STATE_MASK      ((uint8_t)0x02)     //!< Info driver state mask
#define INFO_PARAM2_SET_LATCH_STATE ((uint16_t)0x0002)  //!< Info param2 to set the persistent latch state.
#define INFO_PARAM2_LATCH_SET       ((uint16_t)0x0001)  //!< Info param2 to set the persistent latch
#define INFO_PARAM2_LATCH_CLEAR     ((uint16_t)0x0000)  //!< Info param2 to clear the persistent latch
#define INFO_SIZE                   ((uint8_t)0x04)     //!< Info return size
#define INFO_RSP_SIZE               ATCA_RSP_SIZE_VAL   //!< Info command response packet size
/** @} */

/** \name Definitions for the KDF Command
   @{ */
#define KDF_MODE_IDX                     ATCA_PARAM1_IDX           //!< KDF command index for mode
#define KDF_KEYID_IDX                    ATCA_PARAM2_IDX           //!< KDF command index for key id
#define KDF_DETAILS_IDX                  ATCA_DATA_IDX             //!< KDF command index for details
#define KDF_DETAILS_SIZE                 4                         //!< KDF details (param3) size
#define KDF_MESSAGE_IDX                  (ATCA_DATA_IDX + KDF_DETAILS_SIZE)

#define KDF_MODE_SOURCE_MASK             ((uint8_t)0x03)           //!< KDF mode source key mask
#define KDF_MODE_SOURCE_TEMPKEY          ((uint8_t)0x00)           //!< KDF mode source key in TempKey
#define KDF_MODE_SOURCE_TEMPKEY_UP       ((uint8_t)0x01)           //!< KDF mode source key in upper TempKey
#define KDF_MODE_SOURCE_SLOT             ((uint8_t)0x02)           //!< KDF mode source key in a slot
#define KDF_MODE_SOURCE_ALTKEYBUF        ((uint8_t)0x03)           //!< KDF mode source key in alternate key buffer

#define KDF_MODE_TARGET_MASK             ((uint8_t)0x1C)           //!< KDF mode target key mask
#define KDF_MODE_TARGET_TEMPKEY          ((uint8_t)0x00)           //!< KDF mode target key in TempKey
#define KDF_MODE_TARGET_TEMPKEY_UP       ((uint8_t)0x04)           //!< KDF mode target key in upper TempKey
#define KDF_MODE_TARGET_SLOT             ((uint8_t)0x08)           //!< KDF mode target key in slot
#define KDF_MODE_TARGET_ALTKEYBUF        ((uint8_t)0x0C)           //!< KDF mode target key in alternate key buffer
#define KDF_MODE_TARGET_OUTPUT           ((uint8_t)0x10)           //!< KDF mode target key in output buffer
#define KDF_MODE_TARGET_OUTPUT_ENC       ((uint8_t)0x14)           //!< KDF mode target key encrypted in output buffer

#define KDF_MODE_ALG_MASK                ((uint8_t)0x60)           //!< KDF mode algorithm mask
#define KDF_MODE_ALG_PRF                 ((uint8_t)0x00)           //!< KDF mode PRF algorithm
#define KDF_MODE_ALG_AES                 ((uint8_t)0x20)           //!< KDF mode AES algorithm
#define KDF_MODE_ALG_HKDF                ((uint8_t)0x40)           //!< KDF mode HKDF algorithm

#define KDF_DETAILS_PRF_KEY_LEN_MASK     ((uint32_t)0x00000003)    //!< KDF details for PRF, source key length mask
#define KDF_DETAILS_PRF_KEY_LEN_16       ((uint32_t)0x00000000)    //!< KDF details for PRF, source key length is 16 bytes
#define KDF_DETAILS_PRF_KEY_LEN_32       ((uint32_t)0x00000001)    //!< KDF details for PRF, source key length is 32 bytes
#define KDF_DETAILS_PRF_KEY_LEN_48       ((uint32_t)0x00000002)    //!< KDF details for PRF, source key length is 48 bytes
#define KDF_DETAILS_PRF_KEY_LEN_64       ((uint32_t)0x00000003)    //!< KDF details for PRF, source key length is 64 bytes

#define KDF_DETAILS_PRF_TARGET_LEN_MASK  ((uint32_t)0x00000100)    //!< KDF details for PRF, target length mask
#define KDF_DETAILS_PRF_TARGET_LEN_32    ((uint32_t)0x00000000)    //!< KDF details for PRF, target length is 32 bytes
#define KDF_DETAILS_PRF_TARGET_LEN_64    ((uint32_t)0x00000100)    //!< KDF details for PRF, target length is 64 bytes

#define KDF_DETAILS_PRF_AEAD_MASK        ((uint32_t)0x00000600)    //!< KDF details for PRF, AEAD processing mask
#define KDF_DETAILS_PRF_AEAD_MODE0       ((uint32_t)0x00000000)    //!< KDF details for PRF, AEAD no processing
#define KDF_DETAILS_PRF_AEAD_MODE1       ((uint32_t)0x00000200)    //!< KDF details for PRF, AEAD First 32 go to target, second 32 go to output buffer

#define KDF_DETAILS_AES_KEY_LOC_MASK     ((uint32_t)0x00000003)    //!< KDF details for AES, key location mask

#define KDF_DETAILS_HKDF_MSG_LOC_MASK    ((uint32_t)0x00000003)    //!< KDF details for HKDF, message location mask
#define KDF_DETAILS_HKDF_MSG_LOC_SLOT    ((uint32_t)0x00000000)    //!< KDF details for HKDF, message location in slot
#define KDF_DETAILS_HKDF_MSG_LOC_TEMPKEY ((uint32_t)0x00000001)    //!< KDF details for HKDF, message location in TempKey
#define KDF_DETAILS_HKDF_MSG_LOC_INPUT   ((uint32_t)0x00000002)    //!< KDF details for HKDF, message location in input parameter
#define KDF_DETAILS_HKDF_MSG_LOC_IV      ((uint32_t)0x00000003)    //!< KDF details for HKDF, message location is a special IV function
#define KDF_DETAILS_HKDF_ZERO_KEY        ((uint32_t)0x00000004)    //!< KDF details for HKDF, key is 32 bytes of zero
/** @} */

/** \name Definitions for the Lock Command
   @{ */
#define LOCK_ZONE_IDX               ATCA_PARAM1_IDX     //!< Lock command index for zone
#define LOCK_SUMMARY_IDX            ATCA_PARAM2_IDX     //!< Lock command index for summary
#define LOCK_COUNT                  ATCA_CMD_SIZE_MIN   //!< Lock command packet size
#define LOCK_ZONE_CONFIG            ((uint8_t)0x00)     //!< Lock zone is Config
#define LOCK_ZONE_DATA              ((uint8_t)0x01)     //!< Lock zone is OTP or Data
#define LOCK_ZONE_DATA_SLOT         ((uint8_t)0x02)     //!< Lock slot of Data
#define LOCK_ZONE_NO_CRC            ((uint8_t)0x80)     //!< Lock command: Ignore summary.
#define LOCK_ZONE_MASK              (0xBF)              //!< Lock parameter 1 bits 6 are 0.
#define ATCA_UNLOCKED               (0x55)              //!< Value indicating an unlocked zone
#define ATCA_LOCKED                 (0x00)              //!< Value indicating a locked zone
#define LOCK_RSP_SIZE               ATCA_RSP_SIZE_MIN   //!< Lock command response packet size
/** @} */

/** \name Definitions for the MAC Command
   @{ */
#define MAC_MODE_IDX                    ATCA_PARAM1_IDX         //!< MAC command index for mode
#define MAC_KEYID_IDX                   ATCA_PARAM2_IDX         //!< MAC command index for key id
#define MAC_CHALLENGE_IDX               ATCA_DATA_IDX           //!< MAC command index for optional challenge
#define MAC_COUNT_SHORT                 ATCA_CMD_SIZE_MIN       //!< MAC command packet size without challenge
#define MAC_COUNT_LONG                  (39)                    //!< MAC command packet size with challenge
#define MAC_MODE_CHALLENGE              ((uint8_t)0x00)         //!< MAC mode       0: first SHA block from data slot
#define MAC_MODE_BLOCK2_TEMPKEY         ((uint8_t)0x01)         //!< MAC mode bit   0: second SHA block from TempKey
#define MAC_MODE_BLOCK1_TEMPKEY         ((uint8_t)0x02)         //!< MAC mode bit   1: first SHA block from TempKey
#define MAC_MODE_SOURCE_FLAG_MATCH      ((uint8_t)0x04)         //!< MAC mode bit   2: match TempKey.SourceFlag
#define MAC_MODE_PTNONCE_TEMPKEY        ((uint8_t)0x06)         //!< MAC mode bit   0: second SHA block from TempKey
#define MAC_MODE_PASSTHROUGH            ((uint8_t)0x07)         //!< MAC mode bit 0-2: pass-through mode
#define MAC_MODE_INCLUDE_OTP_88         ((uint8_t)0x10)         //!< MAC mode bit   4: include first 88 OTP bits
#define MAC_MODE_INCLUDE_OTP_64         ((uint8_t)0x20)         //!< MAC mode bit   5: include first 64 OTP bits
#define MAC_MODE_INCLUDE_SN             ((uint8_t)0x40)         //!< MAC mode bit   6: include serial number
#define MAC_CHALLENGE_SIZE              (32)                    //!< MAC size of challenge
#define MAC_SIZE                        (32)                    //!< MAC size of response
#define MAC_MODE_MASK                   ((uint8_t)0x77)         //!< MAC mode bits 3 and 7 are 0.
#define MAC_RSP_SIZE                    ATCA_RSP_SIZE_32        //!< MAC command response packet size
/** @} */

/** \name Definitions for the Nonce Command
   @{ */
#define NONCE_MODE_IDX                  ATCA_PARAM1_IDX          //!< Nonce command index for mode
#define NONCE_PARAM2_IDX                ATCA_PARAM2_IDX          //!< Nonce command index for 2. parameter
#define NONCE_INPUT_IDX                 ATCA_DATA_IDX            //!< Nonce command index for input data
#define NONCE_COUNT_SHORT               (ATCA_CMD_SIZE_MIN + 20) //!< Nonce command packet size for 20 bytes of NumIn
#define NONCE_COUNT_LONG                (ATCA_CMD_SIZE_MIN + 32) //!< Nonce command packet size for 32 bytes of NumIn
#define NONCE_COUNT_LONG_64             (ATCA_CMD_SIZE_MIN + 64) //!< Nonce command packet size for 64 bytes of NumIn
#define NONCE_MODE_MASK                 ((uint8_t)0x03)          //!< Nonce mode bits 2 to 7 are 0.
#define NONCE_MODE_SEED_UPDATE          ((uint8_t)0x00)          //!< Nonce mode: update seed
#define NONCE_MODE_NO_SEED_UPDATE       ((uint8_t)0x01)          //!< Nonce mode: do not update seed
#define NONCE_MODE_INVALID              ((uint8_t)0x02)          //!< Nonce mode 2 is invalid.
#define NONCE_MODE_PASSTHROUGH          ((uint8_t)0x03)          //!< Nonce mode: pass-through

#define NONCE_MODE_INPUT_LEN_MASK       ((uint8_t)0x20)          //!< Nonce mode: input size mask
#define NONCE_MODE_INPUT_LEN_32         ((uint8_t)0x00)          //!< Nonce mode: input size is 32 bytes
#define NONCE_MODE_INPUT_LEN_64         ((uint8_t)0x20)          //!< Nonce mode: input size is 64 bytes

#define NONCE_MODE_TARGET_MASK          ((uint8_t)0xC0)          //!< Nonce mode: target mask
#define NONCE_MODE_TARGET_TEMPKEY       ((uint8_t)0x00)          //!< Nonce mode: target is TempKey
#define NONCE_MODE_TARGET_MSGDIGBUF     ((uint8_t)0x40)          //!< Nonce mode: target is Message Digest Buffer
#define NONCE_MODE_TARGET_ALTKEYBUF     ((uint8_t)0x80)          //!< Nonce mode: target is Alternate Key Buffer

#define NONCE_ZERO_CALC_MASK            ((uint16_t)0x8000)       //!< Nonce zero (param2): calculation mode mask
#define NONCE_ZERO_CALC_RANDOM          ((uint16_t)0x0000)       //!< Nonce zero (param2): calculation mode random, use RNG in calculation and return RNG output
#define NONCE_ZERO_CALC_TEMPKEY         ((uint16_t)0x8000)       //!< Nonce zero (param2): calculation mode TempKey, use TempKey in calculation and return new TempKey value

#define NONCE_NUMIN_SIZE                (20)                     //!< Nonce NumIn size for random modes
#define NONCE_NUMIN_SIZE_PASSTHROUGH    (32)                     //!< Nonce NumIn size for 32-byte pass-through mode

#define NONCE_RSP_SIZE_SHORT            ATCA_RSP_SIZE_MIN        //!< Nonce command response packet size with no output
#define NONCE_RSP_SIZE_LONG             ATCA_RSP_SIZE_32         //!< Nonce command response packet size with output
/** @} */

/** \name Definitions for the Pause Command
   @{ */
#define PAUSE_SELECT_IDX                ATCA_PARAM1_IDX     //!< Pause command index for Selector
#define PAUSE_PARAM2_IDX                ATCA_PARAM2_IDX     //!< Pause command index for 2. parameter
#define PAUSE_COUNT                     ATCA_CMD_SIZE_MIN   //!< Pause command packet size
#define PAUSE_RSP_SIZE                  ATCA_RSP_SIZE_MIN   //!< Pause command response packet size
/** @} */

/** \name Definitions for the PrivWrite Command
   @{ */
#define PRIVWRITE_ZONE_IDX          ATCA_PARAM1_IDX     //!< PrivWrite command index for zone
#define PRIVWRITE_KEYID_IDX         ATCA_PARAM2_IDX     //!< PrivWrite command index for KeyID
#define PRIVWRITE_VALUE_IDX         ( 5)                //!< PrivWrite command index for value
#define PRIVWRITE_MAC_IDX           (41)                //!< PrivWrite command index for MAC
#define PRIVWRITE_COUNT             (75)                //!< PrivWrite command packet size
#define PRIVWRITE_ZONE_MASK         ((uint8_t)0x40)     //!< PrivWrite zone bits 0 to 5 and 7 are 0.
#define PRIVWRITE_MODE_ENCRYPT      ((uint8_t)0x40)     //!< PrivWrite mode: encrypted
#define PRIVWRITE_RSP_SIZE          ATCA_RSP_SIZE_MIN   //!< PrivWrite command response packet size
/** @} */

/** \name Definitions for the Random Command
   @{ */
#define RANDOM_MODE_IDX                 ATCA_PARAM1_IDX     //!< Random command index for mode
#define RANDOM_PARAM2_IDX               ATCA_PARAM2_IDX     //!< Random command index for 2. parameter
#define RANDOM_COUNT                    ATCA_CMD_SIZE_MIN   //!< Random command packet size
#define RANDOM_SEED_UPDATE              ((uint8_t)0x00)     //!< Random mode for automatic seed update
#define RANDOM_NO_SEED_UPDATE           ((uint8_t)0x01)     //!< Random mode for no seed update
#define RANDOM_NUM_SIZE                 ((uint8_t)32)       //!< Number of bytes in the data packet of a random command
#define RANDOM_RSP_SIZE                 ATCA_RSP_SIZE_32    //!< Random command response packet size
/** @} */

/** \name Definitions for the Read Command
   @{ */
#define READ_ZONE_IDX               ATCA_PARAM1_IDX         //!< Read command index for zone
#define READ_ADDR_IDX               ATCA_PARAM2_IDX         //!< Read command index for address
#define READ_COUNT                  ATCA_CMD_SIZE_MIN       //!< Read command packet size
#define READ_ZONE_MASK              ((uint8_t)0x83)         //!< Read zone bits 2 to 6 are 0.
#define READ_4_RSP_SIZE             ATCA_RSP_SIZE_VAL       //!< Read command response packet size when reading 4 bytes
#define READ_32_RSP_SIZE            ATCA_RSP_SIZE_32        //!< Read command response packet size when reading 32 bytes
/** @} */

/** \name Definitions for the SecureBoot Command
   @{ */
#define SECUREBOOT_MODE_IDX           ATCA_PARAM1_IDX                                                          //!< SecureBoot command index for mode
#define SECUREBOOT_DIGEST_SIZE        (32)                                                                     //!< SecureBoot digest input size
#define SECUREBOOT_SIGNATURE_SIZE     (64)                                                                     //!< SecureBoot signature input size
#define SECUREBOOT_COUNT_DIG          (ATCA_CMD_SIZE_MIN + SECUREBOOT_DIGEST_SIZE)                             //!< SecureBoot command packet size for just a digest
#define SECUREBOOT_COUNT_DIG_SIG      (ATCA_CMD_SIZE_MIN + SECUREBOOT_DIGEST_SIZE + SECUREBOOT_SIGNATURE_SIZE) //!< SecureBoot command packet size for a digest and signature
#define SECUREBOOT_MAC_SIZE           (32)                                                                     //!< SecureBoot MAC output size
#define SECUREBOOT_RSP_SIZE_NO_MAC    ATCA_RSP_SIZE_MIN                                                        //!< SecureBoot response packet size for no MAC
#define SECUREBOOT_RSP_SIZE_MAC       (ATCA_PACKET_OVERHEAD + SECUREBOOT_MAC_SIZE)                             //!< SecureBoot response packet size with MAC

#define SECUREBOOT_MODE_MASK          ((uint8_t)0x07)                                                          //!< SecureBoot mode mask
#define SECUREBOOT_MODE_FULL          ((uint8_t)0x05)                                                          //!< SecureBoot mode Full
#define SECUREBOOT_MODE_FULL_STORE    ((uint8_t)0x06)                                                          //!< SecureBoot mode FullStore
#define SECUREBOOT_MODE_FULL_COPY     ((uint8_t)0x07)                                                          //!< SecureBoot mode FullCopy
#define SECUREBOOT_MODE_PROHIBIT_FLAG ((uint8_t)0x40)                                                          //!< SecureBoot mode flag to prohibit SecureBoot until next power cycle
#define SECUREBOOT_MODE_ENC_MAC_FLAG  ((uint8_t)0x80)                                                          //!< SecureBoot mode flag for encrypted digest and returning validating MAC

#define SECUREBOOTCONFIG_OFFSET         (70)                                                                   //!< SecureBootConfig byte offset into the configuration zone
#define SECUREBOOTCONFIG_MODE_MASK      ((uint16_t)0x0003)                                                     //!< Mask for SecureBootMode field in SecureBootConfig value
#define SECUREBOOTCONFIG_MODE_DISABLED  ((uint16_t)0x0000)                                                     //!< Disabled SecureBootMode in SecureBootConfig value
#define SECUREBOOTCONFIG_MODE_FULL_BOTH ((uint16_t)0x0001)                                                     //!< Both digest and signature always required SecureBootMode in SecureBootConfig value
#define SECUREBOOTCONFIG_MODE_FULL_SIG  ((uint16_t)0x0002)                                                     //!< Signature stored SecureBootMode in SecureBootConfig value
#define SECUREBOOTCONFIG_MODE_FULL_DIG  ((uint16_t)0x0003)                                                     //!< Digest stored SecureBootMode in SecureBootConfig value
/** @} */

/** \name Definitions for the SelfTest Command
   @{ */
#define SELFTEST_MODE_IDX                   ATCA_PARAM1_IDX          //!< SelfTest command index for mode
#define SELFTEST_COUNT                      ATCA_CMD_SIZE_MIN        //!< SelfTest command packet size
#define SELFTEST_MODE_RNG                   ((uint8_t)0x01)          //!< SelfTest mode RNG DRBG function
#define SELFTEST_MODE_ECDSA_SIGN_VERIFY     ((uint8_t)0x02)          //!< SelfTest mode ECDSA verify function
#define SELFTEST_MODE_ECDH                  ((uint8_t)0x08)          //!< SelfTest mode ECDH function
#define SELFTEST_MODE_AES                   ((uint8_t)0x10)          //!< SelfTest mode AES encrypt function
#define SELFTEST_MODE_SHA                   ((uint8_t)0x20)          //!< SelfTest mode SHA function
#define SELFTEST_MODE_ALL                   ((uint8_t)0x3B)          //!< SelfTest mode all algorithms
#define SELFTEST_RSP_SIZE                   ATCA_RSP_SIZE_MIN        //!< SelfTest command response packet size
/** @} */

/** \name Definitions for the SHA Command
   @{ */
#define SHA_COUNT_SHORT                     ATCA_CMD_SIZE_MIN
#define SHA_COUNT_LONG                      ATCA_CMD_SIZE_MIN  //!< Just a starting size
#define ATCA_SHA_DIGEST_SIZE                (32)
#define SHA_DATA_MAX                        (64)
#define ATCA_SHA256_BLOCK_SIZE              (64)
#define SHA_CONTEXT_MAX_SIZE                (99)

#define SHA_MODE_MASK                       ((uint8_t)0x07)   //!< Mask the bit 0-2
#define SHA_MODE_SHA256_START               ((uint8_t)0x00)   //!< Initialization, does not accept a message
#define SHA_MODE_SHA256_UPDATE              ((uint8_t)0x01)   //!< Add 64 bytes in the meesage to the SHA context
#define SHA_MODE_SHA256_END                 ((uint8_t)0x02)   //!< Complete the calculation and return the digest
#define SHA_MODE_SHA256_PUBLIC              ((uint8_t)0x03)   //!< Add 64 byte ECC public key in the slot to the SHA context
#define SHA_MODE_HMAC_START                 ((uint8_t)0x04)   //!< Initialization, HMAC calculation
#define SHA_MODE_HMAC_UPDATE                ((uint8_t)0x01)   //!< Add 64 bytes in the meesage to the SHA context
#define SHA_MODE_HMAC_END                   ((uint8_t)0x05)   //!< Complete the HMAC computation and return digest
#define SHA_MODE_608_HMAC_END               ((uint8_t)0x02)   //!< Complete the HMAC computation and return digest... Different command on 608
#define SHA_MODE_READ_CONTEXT               ((uint8_t)0x06)   //!< Read current SHA-256 context out of the device
#define SHA_MODE_WRITE_CONTEXT              ((uint8_t)0x07)   //!< Restore a SHA-256 context into the device
#define SHA_MODE_TARGET_MASK                ((uint8_t)0xC0)   //!< Resulting digest target location mask
#define SHA_MODE_TARGET_TEMPKEY             ((uint8_t)0x00)   //!< Place resulting digest both in Output buffer and TempKey
#define SHA_MODE_TARGET_MSGDIGBUF           ((uint8_t)0x40)   //!< Place resulting digest both in Output buffer and Message Digest Buffer
#define SHA_MODE_TARGET_OUT_ONLY            ((uint8_t)0xC0)   //!< Place resulting digest both in Output buffer ONLY

#define SHA_RSP_SIZE                        ATCA_RSP_SIZE_32  //!< SHA command response packet size
#define SHA_RSP_SIZE_SHORT                  ATCA_RSP_SIZE_MIN //!< SHA command response packet size only status code
#define SHA_RSP_SIZE_LONG                   ATCA_RSP_SIZE_32  //!< SHA command response packet size
/** @} */

/** @} *//** \name Definitions for the Sign Command
   @{ */
#define SIGN_MODE_IDX               ATCA_PARAM1_IDX     //!< Sign command index for mode
#define SIGN_KEYID_IDX              ATCA_PARAM2_IDX     //!< Sign command index for key id
#define SIGN_COUNT                  ATCA_CMD_SIZE_MIN   //!< Sign command packet size
#define SIGN_MODE_MASK              ((uint8_t)0xE1)     //!< Sign mode bits 1 to 4 are 0
#define SIGN_MODE_INTERNAL          ((uint8_t)0x00)     //!< Sign mode	 0: internal
#define SIGN_MODE_INVALIDATE        ((uint8_t)0x01)     //!< Sign mode bit 1: Signature will be used for Verify(Invalidate)
#define SIGN_MODE_INCLUDE_SN        ((uint8_t)0x40)     //!< Sign mode bit 6: include serial number
#define SIGN_MODE_EXTERNAL          ((uint8_t)0x80)     //!< Sign mode bit 7: external
#define SIGN_MODE_SOURCE_MASK       ((uint8_t)0x20)     //!< Sign mode message source mask
#define SIGN_MODE_SOURCE_TEMPKEY    ((uint8_t)0x00)     //!< Sign mode message source is TempKey
#define SIGN_MODE_SOURCE_MSGDIGBUF  ((uint8_t)0x20)     //!< Sign mode message source is the Message Digest Buffer
#define SIGN_RSP_SIZE               ATCA_RSP_SIZE_MAX   //!< Sign command response packet size
/** @} */

/** \name Definitions for the UpdateExtra Command
   @{ */
#define UPDATE_MODE_IDX             ATCA_PARAM1_IDX       //!< UpdateExtra command index for mode
#define UPDATE_VALUE_IDX            ATCA_PARAM2_IDX       //!< UpdateExtra command index for new value
#define UPDATE_COUNT                ATCA_CMD_SIZE_MIN     //!< UpdateExtra command packet size
#define UPDATE_MODE_USER_EXTRA      ((uint8_t)0x00)       //!< UpdateExtra mode update UserExtra (config byte 84)
#define UPDATE_MODE_SELECTOR        ((uint8_t)0x01)       //!< UpdateExtra mode update Selector (config byte 85)
#define UPDATE_MODE_USER_EXTRA_ADD  UPDATE_MODE_SELECTOR  //!< UpdateExtra mode update UserExtraAdd (config byte 85)
#define UPDATE_MODE_DEC_COUNTER     ((uint8_t)0x02)       //!< UpdateExtra mode: decrement counter
#define UPDATE_RSP_SIZE             ATCA_RSP_SIZE_MIN     //!< UpdateExtra command response packet size
/** @} */

/** \name Definitions for the Verify Command
   @{ */
#define VERIFY_MODE_IDX                ATCA_PARAM1_IDX      //!< Verify command index for mode
#define VERIFY_KEYID_IDX               ATCA_PARAM2_IDX      //!< Verify command index for key id
#define VERIFY_DATA_IDX                (  5)                //!< Verify command index for data
#define VERIFY_256_STORED_COUNT        ( 71)                //!< Verify command packet size for 256-bit key in stored mode
#define VERIFY_283_STORED_COUNT        ( 79)                //!< Verify command packet size for 283-bit key in stored mode
#define VERIFY_256_VALIDATE_COUNT      ( 90)                //!< Verify command packet size for 256-bit key in validate mode
#define VERIFY_283_VALIDATE_COUNT      ( 98)                //!< Verify command packet size for 283-bit key in validate mode
#define VERIFY_256_EXTERNAL_COUNT      (135)                //!< Verify command packet size for 256-bit key in external mode
#define VERIFY_283_EXTERNAL_COUNT      (151)                //!< Verify command packet size for 283-bit key in external mode
#define VERIFY_256_KEY_SIZE            ( 64)                //!< Verify key size for 256-bit key
#define VERIFY_283_KEY_SIZE            ( 72)                //!< Verify key size for 283-bit key
#define VERIFY_256_SIGNATURE_SIZE      ( 64)                //!< Verify signature size for 256-bit key
#define VERIFY_283_SIGNATURE_SIZE      ( 72)                //!< Verify signature size for 283-bit key
#define VERIFY_OTHER_DATA_SIZE         ( 19)                //!< Verify size of "other data"
#define VERIFY_MODE_MASK               ((uint8_t)0x03)      //!< Verify mode bits 2 to 7 are 0
#define VERIFY_MODE_STORED             ((uint8_t)0x00)      //!< Verify mode: stored
#define VERIFY_MODE_VALIDATE_EXTERNAL  ((uint8_t)0x01)      //!< Verify mode: validate external
#define VERIFY_MODE_EXTERNAL           ((uint8_t)0x02)      //!< Verify mode: external
#define VERIFY_MODE_VALIDATE           ((uint8_t)0x03)      //!< Verify mode: validate
#define VERIFY_MODE_INVALIDATE         ((uint8_t)0x07)      //!< Verify mode: invalidate
#define VERIFY_MODE_SOURCE_MASK        ((uint8_t)0x20)      //!< Verify mode message source mask
#define VERIFY_MODE_SOURCE_TEMPKEY     ((uint8_t)0x00)      //!< Verify mode message source is TempKey
#define VERIFY_MODE_SOURCE_MSGDIGBUF   ((uint8_t)0x20)      //!< Verify mode message source is the Message Digest Buffer
#define VERIFY_MODE_MAC_FLAG           ((uint8_t)0x80)      //!< Verify mode: MAC
#define VERIFY_KEY_B283                ((uint16_t)0x0000)   //!< Verify key type: B283
#define VERIFY_KEY_K283                ((uint16_t)0x0001)   //!< Verify key type: K283
#define VERIFY_KEY_P256                ((uint16_t)0x0004)   //!< Verify key type: P256
#define VERIFY_RSP_SIZE                ATCA_RSP_SIZE_MIN    //!< Verify command response packet size
#define VERIFY_RSP_SIZE_MAC            ATCA_RSP_SIZE_32     //!< Verify command response packet size with validating MAC
/** @} */

/** \name Definitions for the Write Command
   @{ */
#define WRITE_ZONE_IDX              ATCA_PARAM1_IDX     //!< Write command index for zone
#define WRITE_ADDR_IDX              ATCA_PARAM2_IDX     //!< Write command index for address
#define WRITE_VALUE_IDX             ATCA_DATA_IDX       //!< Write command index for data
#define WRITE_MAC_VS_IDX            ( 9)                //!< Write command index for MAC following short data
#define WRITE_MAC_VL_IDX            (37)                //!< Write command index for MAC following long data
#define WRITE_MAC_SIZE              (32)                //!< Write MAC size
#define WRITE_ZONE_MASK             ((uint8_t)0xC3)     //!< Write zone bits 2 to 5 are 0.
#define WRITE_ZONE_WITH_MAC         ((uint8_t)0x40)     //!< Write zone bit 6: write encrypted with MAC
#define WRITE_ZONE_OTP              ((uint8_t)1)        //!< Write zone id OTP
#define WRITE_ZONE_DATA             ((uint8_t)2)        //!< Write zone id data
#define WRITE_RSP_SIZE              ATCA_RSP_SIZE_MIN   //!< Write command response packet size
/** @} */

#ifdef __cplusplus
}
#endif
#endif

