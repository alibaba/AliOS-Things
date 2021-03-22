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
 * $Date:        02. March 2016
 * $Revision:    V2.1
 *
 * Project:      USB Driver Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __USB_LPC18XX_H
#define __USB_LPC18XX_H

#include <stdint.h>

#ifndef USB_ENDPT_MSK
#define USB_ENDPT_MSK                          (0x3FU)
#endif

// USB Device Command Register
#define USB_USBCMD_D_RS                        (1U           )
#define USB_USBCMD_D_RST                       (1U     <<  1U)
#define USB_USBCMD_D_SUTW                      (1U     << 13U)
#define USB_USBCMD_D_ATDTW                     (1U     << 14U)
#define USB_USBCMD_D_ITC_POS                   (          16U)
#define USB_USBCMD_D_ITC_MSK                   (0xFFU  << USB_USBCMD_D_ITC_POS)
#define USB_USBCMD_D_ITC(n)                    (((n)   << USB_USBCMD_D_ITC_POS) & USB_USBCMD_D_ITC_MSK)

// USB Host Command Register
#define USB_USBCMD_H_RS                        (1U           )
#define USB_USBCMD_H_RST                       (1U     <<  1U)
#define USB_USBCMD_H_FS0                       (1U     <<  2U)
#define USB_USBCMD_H_FS1                       (1U     <<  3U)
#define USB_USBCMD_H_PSE                       (1U     <<  4U)
#define USB_USBCMD_H_ASE                       (1U     <<  5U)
#define USB_USBCMD_H_IAA                       (1U     <<  6U)
#define USB_USBCMD_H_ASP1_0_POS                (           8U)
#define USB_USBCMD_H_ASP1_0_MSK                (3U     <<  USB_USBCMD_H_ASP1_0_POS)
#define USB_USBCMD_H_ASPE                      (1U     << 11U)
#define USB_USBCMD_H_FS2                       (1U     << 15U)
#define USB_USBCMD_H_ITC_POS                   (          16U)
#define USB_USBCMD_H_ITC_MSK                   (0xFFU  << USB_USBCMD_H_ITC_POS)
#define USB_USBCMD_H_ITC(n)                    (((n)   << USB_USBCMD_H_ITC_POS) & USB_USBCMD_H_ITC_MSK)

// USB Device Status Register
#define USB_USBDSTS_D_UI                       (1U           )
#define USB_USBDSTS_D_UEI                      (1U     <<  1U)
#define USB_USBDSTS_D_PCI                      (1U     <<  2U)
#define USB_USBDSTS_D_URI                      (1U     <<  6U)
#define USB_USBDSTS_D_SRI                      (1U     <<  7U)
#define USB_USBDSTS_D_SLI                      (1U     <<  8U)
#define USB_USBDSTS_D_NAKI                     (1U     << 16U)

// USB Host Status Register
#define USB_USBDSTS_H_UI                       (1U           )
#define USB_USBDSTS_H_UEI                      (1U     <<  1U)
#define USB_USBDSTS_H_PCI                      (1U     <<  2U)
#define USB_USBDSTS_H_FRI                      (1U     <<  3U)
#define USB_USBDSTS_H_AAI                      (1U     <<  5U)
#define USB_USBDSTS_H_SRI                      (1U     <<  7U)
#define USB_USBDSTS_H_HCH                      (1U     << 12U)
#define USB_USBDSTS_H_RCL                      (1U     << 13U)
#define USB_USBDSTS_H_PS                       (1U     << 14U)
#define USB_USBDSTS_H_AS                       (1U     << 15U)
#define USB_USBDSTS_H_UAI                      (1U     << 18U)
#define USB_USBDSTS_H_UPI                      (1U     << 19U)

// USB Device Interrupt Register
#define USB_USBINTR_D_UE                       (1U           )
#define USB_USBINTR_D_UEE                      (1U     <<  1U)
#define USB_USBINTR_D_PCE                      (1U     <<  2U)
#define USB_USBINTR_D_URE                      (1U     <<  6U)
#define USB_USBINTR_D_SRE                      (1U     <<  7U)
#define USB_USBINTR_D_SLE                      (1U     <<  8U)
#define USB_USBINTR_D_NAKE                     (1U     << 16U)

// USB Host Interrupt Register
#define USB_USBINTR_H_UE                       (1U           )
#define USB_USBINTR_H_UEE                      (1U     <<  1U)
#define USB_USBINTR_H_PCE                      (1U     <<  2U)
#define USB_USBINTR_H_FRE                      (1U     <<  3U)
#define USB_USBINTR_H_AAE                      (1U     <<  5U)
#define USB_USBINTR_H_SRE                      (1U     <<  7U)
#define USB_USBINTR_H_UAIE                     (1U     << 18U)
#define USB_USBINTR_H_UPIA                     (1U     << 19U)

// USB Device Frame Index Register
#define USB_FRINDEX_D_FRINDEX2_0_POS           (          0U)
#define USB_FRINDEX_D_FRINDEX2_0_MSK           (7U          )
#define USB_FRINDEX_D_FRINDEX13_3_POS          (          3U)
#define USB_FRINDEX_D_FRINDEX13_3_MSK          (0x7FFU << USB_FRINDEX_D_FRINDEX13_3_POS)

// USB Host Frame Index Register
#define USB_FRINDEX_H_FRINDEX2_0_POS           (          0U)
#define USB_FRINDEX_H_FRINDEX2_0_MSK           (7U          )
#define USB_FRINDEX_H_FRINDEX12_3_POS          (          3U)
#define USB_FRINDEX_H_FRINDEX12_3_MSK          (0x3FFU << USB_FRINDEX_H_FRINDEX12_3_POS)

// USB Device Address Register
#define USB_DEVICEADDR_USBADRA                 (1U     << 24U)
#define USB_DEVICEADDR_USBADR_POS              (          25U)
#define USB_DEVICEADDR_USBADR_MSK              (0x7FUL << USB_DEVICEADDR_USBADR_POS)

// USB Endpoint List Address Register
#define USB_ENDPOINTLISTADDR_EPBASE31_11_POS   (          11U)
#define USB_ENDPOINTLISTADDR_EPBASE31_11_MSK   (0x1FFFFFUL << USB_ENDPOINTLISTADDR_EPBASE31_11_POS)

// USB Burst Size Register
#define USB_BURSTSIZE_RXPBURST_POS             (           0U)
#define USB_BURSTSIZE_RXPBURST_MSK             (0xFFU        )
#define USB_BURSTSIZE_TXPBURST_POS             (           8U)
#define USB_BURSTSIZE_TXPBURST_MSK             (0xFFU  <<  USB_BURSTSIZE_TXPBURST_POS)

// USB ULPI Viewport register (USB1 only)
#define USB_ULPIVIEWPORT_ULPIDATWR_POS         (           0U)
#define USB_ULPIVIEWPORT_ULPIDATRW_MSK         (0xFFU        )
#define USB_ULPIVIEWPORT_ULPIDATRD_POS         (           8U)
#define USB_ULPIVIEWPORT_ULPIDATRD_MSK         (0xFFU  <<  USB_ULPIVIEWPORT_ULPIDATRD_POS)
#define USB_ULPIVIEWPORT_ULPIADDR_POS          (          16U)
#define USB_ULPIVIEWPORT_ULPIADDR_MSK          (0xFFU  << USB_ULPIVIEWPORT_ULPIADDR_POS)
#define USB_ULPIVIEWPORT_ULPIPORT_POS          (          24U)
#define USB_ULPIVIEWPORT_ULPIPORT_MSK          (7U     << USB_ULPIVIEWPORT_ULPIPORT_POS)
#define USB_ULPIVIEWPORT_ULPISS                (1U     << 27U)
#define USB_ULPIVIEWPORT_ULPIRW                (1U     << 29U)
#define USB_ULPIVIEWPORT_ULPIRUN               (1U     << 30U)
#define USB_ULPIVIEWPORT_ULPIWU                (1UL    << 31U)

// USB BInterval Register
#define USB_BINTERVAL_BINT_POS                 (           0U)
#define USB_BINTERVAL_BINT_MSK                 (0x0FU  <<  USB_BINTERVAL_BINT_POS)

// USB Endpoint NAK Register
#define USB_ENDPTNAK_EPRN_POS                  (           0U)
#define USB_ENDPTNAK_EPRN_MSK                  (USB_ENDPT_MSK)
#define USB_ENDPTNAK_EPTN_POS                  (          16U)
#define USB_ENDPTNAK_EPTN_MSK                  (USB_ENDPT_MSK << USB_ENDPTNAK_EPTN_POS)

// USB Endpoint NAK Enable Register
#define USB_ENDPTNAKEN_EPRNE_POS               (           0U)
#define USB_ENDPTNAKEN_EPRNE_MSK               (USB_ENDPT_MSK)
#define USB_ENDPTNAKEN_EPTNE_POS               (          16U)
#define USB_ENDPTNAKEN_EPTNE_MSK               (USB_ENDPT_MSK << USB_ENDPTNAKEN_EPTNE_POS)

// USB Device Port Status and Control Register
#define USB_PORTSC1_D_CCS                      (1U           )
#define USB_PORTSC1_D_PE                       (1U     <<  2U)
#define USB_PORTSC1_D_PEC                      (1U     <<  3U)
#define USB_PORTSC1_D_FPR                      (1U     <<  6U)
#define USB_PORTSC1_D_SUSP                     (1U     <<  7U)
#define USB_PORTSC1_D_PR                       (1U     <<  8U)
#define USB_PORTSC1_D_HSP                      (1U     <<  9U)
#define USB_PORTSC1_D_PIC1_0_POS               (          14U)
#define USB_PORTSC1_D_PIC1_0_MSK               (3U     << USB_PORTSC1_D_PIC1_0_POS)
#define USB_PORTSC1_D_PIC1_0(n)                (((n)   << USB_PORTSC1_D_PIC1_0_POS) & USB_PORTSC1_D_PIC1_0_MSK)
#define USB_PORTSC1_D_PTC3_0_POS               (          16U)
#define USB_PORTSC1_D_PTC3_0_MSK               (0x0FU  << USB_PORTSC1_D_PTC3_0_POS)
#define USB_PORTSC1_D_PHCD                     (1U     << 23U)
#define USB_PORTSC1_D_PFSC                     (1U     << 24U)
#define USB_PORTSC1_D_PSPD_POS                 (          26U)
#define USB_PORTSC1_D_PSPD_MSK                 (3U     << USB_PORTSC1_D_PSPD_POS)
#define USB_PORTSC1_D_PTS_POS                  (          30U)
#define USB_PORTSC1_D_PTS_MSK                  (3UL    << USB_PORTSC1_D_PTS_POS)
#define USB_PORTSC1_D_PTS(n)                   (((n)   << USB_PORTSC1_D_PTS_POS) & USB_PORTSC1_D_PTS_MSK)

// USB Host Port Status and Control Register
#define USB_PORTSC1_H_CCS                      (1U           )
#define USB_PORTSC1_H_CSC                      (1U     <<  1U)
#define USB_PORTSC1_H_PE                       (1U     <<  2U)
#define USB_PORTSC1_H_PEC                      (1U     <<  3U)
#define USB_PORTSC1_H_OCA                      (1U     <<  4U)
#define USB_PORTSC1_H_OCC                      (1U     <<  5U)
#define USB_PORTSC1_H_FPR                      (1U     <<  6U)
#define USB_PORTSC1_H_SUSP                     (1U     <<  7U)
#define USB_PORTSC1_H_PR                       (1U     <<  8U)
#define USB_PORTSC1_H_HSP                      (1U     <<  9U)
#define USB_PORTSC1_H_LS_POS                   (          10U)
#define USB_PORTSC1_H_LS_MSK                   (3U     << USB_PORTSC1_H_LS_POS)
#define USB_PORTSC1_H_PP                       (1U     << 12U)
#define USB_PORTSC1_H_PIC1_0_POS               (          14U)
#define USB_PORTSC1_H_PIC1_0_MSK               (3U     << USB_PORTSC1_H_PIC1_0_POS)
#define USB_PORTSC1_H_PIC1_0(n)                (((n)   << USB_PORTSC1_H_PIC1_0_POS) & USB_PORTSC1_H_PIC1_0_MSK)
#define USB_PORTSC1_H_PTC3_0_POS               (          16U)
#define USB_PORTSC1_H_PTC3_0_MSK               (0x0FU  << USB_PORTSC1_H_PTC3_0_POS)
#define USB_PORTSC1_H_WKCN                     (1U     << 20U)
#define USB_PORTSC1_H_WKDC                     (1U     << 21U)
#define USB_PORTSC1_H_WKOC                     (1U     << 22U)
#define USB_PORTSC1_H_PHCD                     (1U     << 23U)
#define USB_PORTSC1_H_PFSC                     (1U     << 24U)
#define USB_PORTSC1_H_PSPD_POS                 (          26U)
#define USB_PORTSC1_H_PSPD_MSK                 (3U     << USB_PORTSC1_H_PSPD_POS)
#define USB_PORTSC1_H_PTS_POS                  (          30U)
#define USB_PORTSC1_H_PTS_MSK                  (3UL    << USB_PORTSC1_H_PTS_POS)
#define USB_PORTSC1_H_PTS(n)                   (((n)   << USB_PORTSC1_H_PTS_POS) & USB_PORTSC1_H_PTS_MSK)

// OTG Status and Control Register (USB0 only)
#define USB_OTGSC_VD                           (1U           )
#define USB_OTGSC_VC                           (1U     <<  1U)
#define USB_OTGSC_HAAR                         (1U     <<  2U)
#define USB_OTGSC_OT                           (1U     <<  3U)
#define USB_OTGSC_DP                           (1U     <<  4U)
#define USB_OTGSC_IDPU                         (1U     <<  5U)
#define USB_OTGSC_HADP                         (1U     <<  6U)
#define USB_OTGSC_HABA                         (1U     <<  7U)
#define USB_OTGSC_ID                           (1U     <<  8U)
#define USB_OTGSC_AVV                          (1U     <<  9U)
#define USB_OTGSC_ASV                          (1U     << 10U)
#define USB_OTGSC_BSV                          (1U     << 11U)
#define USB_OTGSC_BSE                          (1U     << 12U)
#define USB_OTGSC_MS1T                         (1U     << 13U)
#define USB_OTGSC_DPS                          (1U     << 14U)
#define USB_OTGSC_IDIS                         (1U     << 16U)
#define USB_OTGSC_AVVIS                        (1U     << 17U)
#define USB_OTGSC_ASVIS                        (1U     << 18U)
#define USB_OTGSC_BSVIS                        (1U     << 19U)
#define USB_OTGSC_BSEIS                        (1U     << 20U)
#define USB_OTGSC_MS1S                         (1U     << 21U)
#define USB_OTGSC_DPIS                         (1U     << 22U)
#define USB_OTGSC_IDIE                         (1U     << 24U)
#define USB_OTGSC_AVVIE                        (1U     << 25U)
#define USB_OTGSC_ASVIE                        (1U     << 26U)
#define USB_OTGSC_BSVIE                        (1U     << 27U)
#define USB_OTGSC_BSEIE                        (1U     << 28U)
#define USB_OTGSC_MS1E                         (1U     << 29U)
#define USB_OTGSC_DPIE                         (1U     << 30U)

// USB Device Mode Register
#define USB_USBMODE_D_CM1_0_POS                (           0U)
#define USB_USBMODE_D_CM1_0_MSK                (3U           )
#define USB_USBMODE_D_CM1_0(n)                 ((n)    &   USB_USBMODE_D_CM1_0_MSK)
#define USB_USBMODE_D_ES                       (1U     <<  2U)
#define USB_USBMODE_D_SLOM                     (1U     <<  3U)
#define USB_USBMODE_D_SDIS                     (1U     <<  4U)

// USB Device Mode Register
#define USB_USBMODE_H_CM1_0_POS                (           0U)
#define USB_USBMODE_H_CM1_0_MSK                (3U           )
#define USB_USBMODE_H_CM1_0(n)                 ((n)    &   USB_USBMODE_H_CM1_0_MSK)
#define USB_USBMODE_H_ES                       (1U     <<  2U)
#define USB_USBMODE_H_SDIS                     (1U     <<  4U)
#define USB_USBMODE_H_VBPS                     (1U     <<  5U)

// USB Endpoint Setup Status Register
#define USB_ENDPTSETUPSTAT_POS                 (           0U)
#define USB_ENDPTSETUPSTAT_MSK                 (USB_ENDPT_MSK << USB_ENDPTSETUPSTAT_POS)

// USB Endpoint Prime Register
#define USB_ENDPTRPRIME_PERB_POS               (           0U)
#define USB_ENDPTRPRIME_PERB_MSK               (USB_ENDPT_MSK)
#define USB_ENDPTRPRIME_PETB_POS               (          16U)
#define USB_ENDPTRPRIME_PETB_MSK               (USB_ENDPT_MSK << USB_ENDPTRPRIME_PETB_POS)

// USB Endpoint Flush Register
#define USB_ENDPTFLUSH_FERB_POS                (           0U)
#define USB_ENDPTFLUSH_FERB_MSK                (USB_ENDPT_MSK)
#define USB_ENDPTFLUSH_FETB_POS                (          16U)
#define USB_ENDPTFLUSH_FETB_MSK                (USB_ENDPT_MSK << USB_ENDPTFLUSH_FETB_POS)

// USB Endpoint Status Register
#define USB_ENDPTSTAT_ERBR_POS                 (           0U)
#define USB_ENDPTSTAT_ERBR_MSK                 (USB_ENDPT_MSK)
#define USB_ENDPTSTAT_ETBR_POS                 (          16U)
#define USB_ENDPTSTAT_ETBR_MSK                 (USB_ENDPT_MSK << USB_ENDPTSTAT_ETBR_POS)

// USB Endpoint Complete Register
#define USB_ENDPTCOMPLETE_ERCE_POS             (           0U)
#define USB_ENDPTCOMPLETE_ERCE_MSK             (USB_ENDPT_MSK)
#define USB_ENDPTCOMPLETE_ETCE_POS             (          16U)
#define USB_ENDPTCOMPLETE_ETCE_MSK             (USB_ENDPT_MSK << USB_ENDPTCOMPLETE_ETCE_POS)

// USB Endpoint Control Register
#define USB_ENDPTCTRL_RXS                      (1U           )
#define USB_ENDPTCTRL_RXT_POS                  (           2U)
#define USB_ENDPTCTRL_RXT_MSK                  (3U     <<  USB_ENDPTCTRL_RXT_POS)
#define USB_ENDPTCTRL_RXT(n)                   (((n)   <<  USB_ENDPTCTRL_RXT_POS) & USB_ENDPTCTRL_RXT_MSK)
#define USB_ENDPTCTRL_RXI                      (1U     <<  5U)
#define USB_ENDPTCTRL_RXR                      (1U     <<  6U)
#define USB_ENDPTCTRL_RXE                      (1U     <<  7U)
#define USB_ENDPTCTRL_TXS                      (1U     << 16U)
#define USB_ENDPTCTRL_TXT_POS                  (          18U)
#define USB_ENDPTCTRL_TXT_MSK                  (3U     << USB_ENDPTCTRL_TXT_POS)
#define USB_ENDPTCTRL_TXT(n)                   (((n)   << USB_ENDPTCTRL_TXT_POS) & USB_ENDPTCTRL_TXT_MSK)
#define USB_ENDPTCTRL_TXI                      (1U     << 21U)
#define USB_ENDPTCTRL_TXR                      (1U     << 22U)
#define USB_ENDPTCTRL_TXE                      (1U     << 23U)

// Endpoint Queue Head Capabilities and Characteristics
#define USB_EPQH_CAP_IOS                       (1U     << 15U)
#define USB_EPQH_CAP_MAX_PACKET_LEN_POS        (          16U)
#define USB_EPQH_CAP_MAX_PACKET_LEN_MSK        (0x7FFU << USB_EPQH_CAP_MAX_PACKET_LEN_POS)
#define USB_EPQH_CAP_MAX_PACKET_LEN(n)         (((n)   << USB_EPQH_CAP_MAX_PACKET_LEN_POS) & USB_EPQH_CAP_MAX_PACKET_LEN_MSK)
#define USB_EPQH_CAP_ZLT                       (1U     << 29U)
#define USB_EPQH_CAP_MULT_POS                  (          30U)
#define USB_EPQH_CAP_MULT_MSK                  (3UL    << USB_EPQH_CAP_MULT_POS)

// Transfer Descriptor Token
#define USB_bTD_TOKEN_STATUS_POS               (           0U)
#define USB_bTD_TOKEN_STATUS_MSK               (0xFFU        )
#define USB_bTD_TOKEN_STATUS(n)                (n      &  USB_bTD_TOKEN_STATUS_MSK)
#define USB_bTD_TOKEN_STATUS_TRAN_ERROR        (0x08U  &  USB_bTD_TOKEN_STATUS_MSK)
#define USB_bTD_TOKEN_STATUS_BUFFER_ERROR      (0x20U  &  USB_bTD_TOKEN_STATUS_MSK)
#define USB_bTD_TOKEN_STATUS_HALTED            (0x40U  &  USB_bTD_TOKEN_STATUS_MSK)
#define USB_bTD_TOKEN_STATUS_ACTIVE            (0x80U  &  USB_bTD_TOKEN_STATUS_MSK)
#define USB_bTD_TOKEN_MULTO_POS                (          10U)
#define USB_bTD_TOKEN_MULTO_MSK                (3U     << USB_bTD_TOKEN_MULTO_POS)
#define USB_bTD_TOKEN_MULTO(n)                 (((n)   << USB_bTD_TOKEN_MULTO_POS) & USB_bTD_TOKEN_MULTO_MSK)
#define USB_bTD_TOKEN_IOC                      (1U     << 15U)
#define USB_bTD_TOKEN_TOTAL_BYTES_POS          (          16U)
#define USB_bTD_TOKEN_TOTAL_BYTES_MSK          (0x7FFFU<< USB_bTD_TOKEN_TOTAL_BYTES_POS)
#define USB_bTD_TOKEN_TOTAL_BYTES(n)           (((n)   << USB_bTD_TOKEN_TOTAL_BYTES_POS) & USB_bTD_TOKEN_TOTAL_BYTES_MSK)

// USB Driver State Flags
// Device State Flags
#define USBD_DRIVER_INITIALIZED                (1U           )
#define USBD_DRIVER_POWERED                    (1U     <<  1U)

// Host State Flags
#define USBH_DRIVER_INITIALIZED                (1U     <<  4U)
#define USBH_DRIVER_POWERED                    (1U     <<  5U)

#endif /* __USB_LPC18XX_H */
