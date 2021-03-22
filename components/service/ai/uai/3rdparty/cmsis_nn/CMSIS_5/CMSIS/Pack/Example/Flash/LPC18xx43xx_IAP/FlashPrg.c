/* -----------------------------------------------------------------------------
 * Copyright (c) 2004 - 2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. Permission is granted to anyone to use this 
 * software for any purpose, including commercial applications, and to alter 
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be 
 *    appreciated but is not required. 
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be 
 *    misrepresented as being the original software. 
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 *   
 *
 * $Date:        17. June 2014
 * $Revision:    V1.01
 *  
 * Project:      Flash Device Description for NXP LPC18xx/43xx Flash using IAP
 * --------------------------------------------------------------------------- */
 
#include "..\FlashOS.H"        // FlashOS Structures

// Memory Mapping Control
#ifdef LPC18xx43xx
  #define BASE_Mx_CLK (*(volatile unsigned long *) (0x4005006C))
#endif


#define BANK_A    0x1A000000
#define BANK_B    0x1B000000

#ifdef LPC18xx43xx
  #if defined FLASH_512_BA || FLASH_512_BB
    #define END_SECTOR     14
  #endif
  #if defined FLASH_384_BA || FLASH_384_BB
    #define END_SECTOR     12
  #endif
  #if defined FLASH_256_BA || FLASH_256_BB
    #define END_SECTOR     10
  #endif
#endif  // LPC18xx43xx

unsigned long CCLK;            // CCLK in kHz

struct sIAP {                  // IAP Structure
  unsigned long cmd;           // Command
  unsigned long par[5];        // Parameters
  unsigned long stat;          // Status
  unsigned long res[4];        // Result
} IAP;


/* IAP Call */
typedef void (*IAP_Entry) (unsigned long *cmd, unsigned long *stat);
#define IAP_Call ((IAP_Entry) *((unsigned long *)0x10400100))

unsigned long base_adr;
unsigned long flash_bank;

/*
 * Get Sector Number
 *    Parameter:      adr:  Sector Address
 *    Return Value:   Sector Number
 */

unsigned long GetSecNum (unsigned long adr) {
  unsigned long n;

  n = (adr & 0x000FF000) >> 13;                //  8kB Sector
  if (n >= 0x07) {
    n = 0x07 + (n >> 3);                       // 64kB Sector
  }

  return (n);                                  // Sector Number
}


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

  base_adr   = adr;                            // store Flash Base Address
  flash_bank = (adr == BANK_A) ? 0 : 1;

  BASE_Mx_CLK = (0x01 << 11) |                 // Autoblock En
                (0x01 << 24) ;                 // Set clock source to IRC
  CCLK  = 12000;                               // 12MHz Internal RC Oscillator

  IAP.cmd  = 49;                               // IAP initialization
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command

  IAP.stat =  0;                               // Note: Some Bootloader versions don't set the status if this command is executed
  IAP.cmd  = 54;                               // Read Part ID
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  return (0);
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {
  return (0);
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {

  IAP.cmd    = 50;                             // Prepare Sector for Erase
  IAP.par[0] = 0;                              // Start Sector
  IAP.par[1] = END_SECTOR;                     // End Sector
  IAP.par[2] = flash_bank;                     // Flashbank
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 52;                             // Erase Sector
  IAP.par[0] = 0;                              // Start Sector
  IAP.par[1] = END_SECTOR;                     // End Sector
  IAP.par[2] = CCLK;                           // CCLK in kHz
  IAP.par[3] = flash_bank;                     // Flashbank
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  return (0);                                  // Finished without Errors
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {
  unsigned long n;

  n = GetSecNum(adr);                          // Get Sector Number

  IAP.cmd    = 50;                             // Prepare Sector for Erase
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP.par[2] = flash_bank;                     // Flashbank
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 52;                             // Erase Sector
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP.par[2] = CCLK;                           // CCLK in kHz
  IAP.par[3] = flash_bank;                     // Flashbank
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  return (0);                                  // Finished without Errors
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {
  unsigned long n;
  unsigned long sig;

  n = GetSecNum(adr);                          // Get Sector Number

  IAP.cmd    = 50;                             // Prepare Sector for Write
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP.par[2] = flash_bank;                     // Flashbank
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 51;                             // Copy RAM to Flash
  IAP.par[0] = adr;                            // Destination Flash Address
  IAP.par[1] = (unsigned long)buf;             // Source RAM Address
  IAP.par[2] = 1024;                           // Fixed Page Size
  IAP.par[3] = CCLK;                           // CCLK in kHz
  IAP.par[4] = flash_bank;                     // Flashbank
  IAP_Call (&IAP.cmd, &IAP.stat);              // Call IAP Command
  if (IAP.stat) return (1);                    // Command Failed

/* calculates vectore checksum and more
   It is not used because it erases the signature in th eother bank!
   This could cause trouble if we want to have valid code or data in the other bank.
   Utility ElfDwt.exe must be used to calculate Signatute and stote it in AXF file.
 */
//  if (adr == base_adr) {                       // Set active partition to FLASH_BANK if vectors are written
//    IAP.cmd    = 60;                           // Set active partition
//    IAP.par[0] = FLASH_BANK;                   // Flashbank
//    IAP.par[1] = CCLK;                         // CCLK in kHz
//    IAP_Call (&IAP.cmd, &IAP.stat);            // Call IAP Command
//    if (IAP.stat) return (1);                  // Command Failed
//  }
#if 0
  we also removed all special handling about the vector checksum because
	it causes more trouble than solving problems. The user must now take 
	care that only on valid checksum is in the application.

  if (adr == BANK_B) {
    sig = *((unsigned long *)(BANK_B + 0x00)) +
          *((unsigned long *)(BANK_B + 0x04)) +
          *((unsigned long *)(BANK_B + 0x08)) +
          *((unsigned long *)(BANK_B + 0x0C)) +
          *((unsigned long *)(BANK_B + 0x10)) +
          *((unsigned long *)(BANK_B + 0x14)) +
          *((unsigned long *)(BANK_B + 0x18));

    if (*((unsigned long *)(BANK_B + 0x1C)) == (0 - sig)) {
    /* we have just written a valid signature to BankB. We assume that we must boot from BankB
       check if a valid signature is in BankA. If so delete it. */

      sig = *((unsigned long *)(BANK_A + 0x00)) +
            *((unsigned long *)(BANK_A + 0x04)) +
            *((unsigned long *)(BANK_A + 0x08)) +
            *((unsigned long *)(BANK_A + 0x0C)) +
            *((unsigned long *)(BANK_A + 0x10)) +
            *((unsigned long *)(BANK_A + 0x14)) +
            *((unsigned long *)(BANK_A + 0x18));

      /* delete signature if a valid signature is found */
      if (*((unsigned long *)(BANK_A + 0x1C)) == (0 - sig)) {

        for (n = 0; n < (512); n++) {
          *(buf + n) = *((unsigned char *)(BANK_A + n)); // copy page to RAM
        }

        *((unsigned long *)(buf + 0x1C)) = 0;          // invalid Signature at Reserved Vector

        IAP.cmd    = 50;                               // Prepare Sector for Write
        IAP.par[0] = 0;                                // Start Sector
        IAP.par[1] = 0;                                // End Sector
        IAP.par[2] = 0;                                // Flashbank
        IAP_Call (&IAP.cmd, &IAP.stat);                // Call IAP Command
        if (IAP.stat) return (1);                      // Command Failed

        IAP.cmd    = 59;                               // Erase Page
        IAP.par[0] = BANK_A;                           // Start Page Address
        IAP.par[1] = BANK_A;                           // End Page Address
        IAP.par[2] = CCLK;                             // CCLK in kHz
        IAP_Call (&IAP.cmd, &IAP.stat);                // Call IAP Command
        if (IAP.stat) return (1);                      // Command Failed

        IAP.cmd    = 50;                               // Prepare Sector for Write
        IAP.par[0] = 0;                                // Start Sector
        IAP.par[1] = 0;                                // End Sector
        IAP.par[2] = 0;                                // Flashbank
        IAP_Call (&IAP.cmd, &IAP.stat);                // Call IAP Command
        if (IAP.stat) return (1);                      // Command Failed

        IAP.cmd    = 51;                               // Copy RAM to Flash
        IAP.par[0] = BANK_A;                           // Destination Flash Address
        IAP.par[1] = (unsigned long)buf;               // Source RAM Address
        IAP.par[2] = 512;                              // Fixed Page Size
        IAP.par[3] = CCLK;                             // CCLK in kHz
        IAP.par[4] = 0;                                // Flashbank
        IAP_Call (&IAP.cmd, &IAP.stat);                // Call IAP Command
        if (IAP.stat) return (1);                      // Command Failed
      }
    }
  }
#endif
  return (0);                                          // Finished without Errors
}
