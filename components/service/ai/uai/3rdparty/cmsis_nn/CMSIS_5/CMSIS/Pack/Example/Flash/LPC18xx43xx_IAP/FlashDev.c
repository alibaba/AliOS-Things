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
 * $Date:        30. May 2014
 * $Revision:    V1.00
 *  
 * Project:      Flash Device Description for NXP LPC18xx/43xx Flash using IAP
 * --------------------------------------------------------------------------- */

#include "..\FlashOS.H"        // FlashOS Structures


#ifdef LPC18xx43xx

#ifdef FLASH_512_BA
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "LPC18xx/43xx IAP 512kB Flash Bank A",  // Device Name
   ONCHIP,                     // Device Type
   0x1A000000,                 // Device Start Address
   0x00080000,                 // Device Size (512kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   5000,                       // Program Page Timeout 5000 mSec
   5000,                       // Erase Sector Timeout 5000 mSec

// Specify Size and Address of Sectors
   0x002000, 0x000000,         // Sector Size  8kB (8 Sectors)
   0x010000, 0x010000,         // Sector Size 64kB (7 Sectors)
   SECTOR_END
};
#endif

#ifdef FLASH_512_BB
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "LPC18xx/43xx IAP 512kB Flash Bank B",  // Device Name
   ONCHIP,                     // Device Type
   0x1B000000,                 // Device Start Address
   0x00080000,                 // Device Size (512kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   5000,                       // Program Page Timeout 5000 mSec
   5000,                       // Erase Sector Timeout 5000 mSec

// Specify Size and Address of Sectors
   0x002000, 0x000000,         // Sector Size  8kB (8 Sectors)
   0x010000, 0x010000,         // Sector Size 64kB (7 Sectors)
   SECTOR_END
};
#endif

#ifdef FLASH_384_BA
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "LPC18xx/43xx IAP 384kB Flash Bank A",  // Device Name
   ONCHIP,                     // Device Type
   0x1A000000,                 // Device Start Address
   0x00060000,                 // Device Size (384kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   5000,                       // Program Page Timeout 5000 mSec
   5000,                       // Erase Sector Timeout 5000 mSec

// Specify Size and Address of Sectors
   0x002000, 0x000000,         // Sector Size  8kB (8 Sectors)
   0x010000, 0x010000,         // Sector Size 64kB (5 Sectors)
   SECTOR_END
};
#endif

#ifdef FLASH_384_BB
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "LPC18xx/43xx IAP 384kB Flash Bank B",  // Device Name
   ONCHIP,                     // Device Type
   0x1B000000,                 // Device Start Address
   0x00060000,                 // Device Size (384kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   5000,                       // Program Page Timeout 5000 mSec
   5000,                       // Erase Sector Timeout 5000 mSec

// Specify Size and Address of Sectors
   0x002000, 0x000000,         // Sector Size  8kB (8 Sectors)
   0x010000, 0x010000,         // Sector Size 64kB (5 Sectors)
   SECTOR_END
};
#endif

#ifdef FLASH_256_BA
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "LPC18xx/43xx IAP 256kB Flash Bank A",  // Device Name
   ONCHIP,                     // Device Type
   0x1A000000,                 // Device Start Address
   0x00040000,                 // Device Size (256kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   5000,                       // Program Page Timeout 5000 mSec
   5000,                       // Erase Sector Timeout 5000 mSec

// Specify Size and Address of Sectors
   0x002000, 0x000000,         // Sector Size  8kB (8 Sectors)
   0x010000, 0x010000,         // Sector Size 64kB (3 Sectors)
   SECTOR_END
};
#endif

#ifdef FLASH_256_BB
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "LPC18xx/43xx IAP 256kB Flash Bank B",  // Device Name
   ONCHIP,                     // Device Type
   0x1B000000,                 // Device Start Address
   0x00040000,                 // Device Size (256kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   5000,                       // Program Page Timeout 5000 mSec
   5000,                       // Erase Sector Timeout 5000 mSec

// Specify Size and Address of Sectors
   0x002000, 0x000000,         // Sector Size  8kB (8 Sectors)
   0x010000, 0x010000,         // Sector Size 64kB (3 Sectors)
   SECTOR_END
};
#endif

#endif // LPC18xx43xx
