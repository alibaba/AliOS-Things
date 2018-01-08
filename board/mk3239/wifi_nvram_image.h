
/** @file
 *  NVRAM variables which define BCM43438A1 Parameters for the
 *  MXCHIP EMW3239 module.
 *
 */

#ifndef INCLUDED_NVRAM_IMAGE_H_
#define INCLUDED_NVRAM_IMAGE_H_

#include <string.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NVRAM_GENERATED_MAC_ADDRESS   "macaddr=C8:93:46:00:00:01"
/**
 * Character array of NVRAM image
 */

    static const char wifi_nvram_image[] =
            // # The following parameter values are just placeholders, need to be updated.
            "manfid=0x2d0"                                                       "\x00"
            "prodid=0x0726"                                                      "\x00"
            "vendid=0x14e4"                                                      "\x00"
            "devid=0x43e2"                                                       "\x00"
            "boardtype=0x0726"                                                   "\x00"
            "boardrev=0x1101"                                                    "\x00"
            "boardnum=22"                                                        "\x00"
            "xtalfreq=26000"                                                     "\x00"
            "sromrev=11"                                                         "\x00"
            "boardflags=0x00404201"                                              "\x00"
            "boardflags3=0x04000000"                                             "\x00" //0x08000000  /* Force external lpo */
            NVRAM_GENERATED_MAC_ADDRESS                                          "\x00"
            "nocrc=1"                                                            "\x00"
            "ag0=255"                                                            "\x00"
            "aa2g=1"                                                             "\x00"
            "ccode=ALL"                                                          "\x00"
            //#Antenna diversity
            "swdiv_en=1"                                                         "\x00"
            "swdiv_gpio=2"                                                       "\x00"

            "pa0itssit=0x20"                                                     "\x00"
            "extpagain2g=0"                                                      "\x00"
            //#PA parameters for 2.4GHz, measured at CHIP OUTPUT
            "pa2ga0=-194,5941,-695"                                              "\x00"
            "AvVmid_c0=0x0,0xc8"                                                 "\x00"
            "cckpwroffset0=5"                                                    "\x00"
            //# PPR params
            "maxp2ga0=74"                                                        "\x00"
            "txpwrbckof=6"                                                       "\x00"
            "cckbw202gpo=0"                                                      "\x00" //0x1111
            "legofdmbw202gpo=0x44444444"                                         "\x00" //0x66666666
            "mcsbw202gpo=0x88888888"                                             "\x00" //0x88888888
            "propbw202gpo=0xdd"                                                  "\x00"
            //# OFDM IIR :
            "ofdmdigfilttype=18"                                                 "\x00"
            "ofdmdigfilttypebe=18"                                               "\x00"
            //# PAPD mode:
            "papdmode=1"                                                         "\x00"
            "papdvalidtest=1"                                                    "\x00"
            "pacalidx2g=32"                                                      "\x00"
            "papdepsoffset=-36"                                                  "\x00"
            "papdendidx=61"                                                      "\x00"
            //# LTECX flags
           // "ltecxmux=1"                                                         "\x00"
            //"ltecxpadnum=0x02030401"                                             "\x00"
           // "ltecxfnsel=0x3003"                                                  "\x00"
           // "ltecxgcigpio=0x3012"                                                "\x00"
            //#il0macaddr=00:90:4c:c5:12:38
            "wl0id=0x431b"                                                       "\x00"
            "deadman_to=0xffffffff"                                              "\x00"
            //#OOB parameters
            "hostwake=0x40"                                                       "\x00"
            "hostrdy=0x41"                                                        "\x00"
            //# muxenab: 0x1 for UART enable, 0x2 for GPIOs, 0x8 for JTAG, 0x10 for HW OOB
            "muxenab=0x11"                                                        "\x00"
            //# CLDO PWM voltage settings - 0x4 - 1.1 volt
            //#cldo_pwm=0x4                                                      "\x00"
            //#VCO freq 326.4MHz
            "spurconfig=0x3"                                                     "\x00"
            //#CE 1.8.1
            //"edonthd=-70"                                                       "\x00"
            //"edoffthd=-76"                                                      "\x00"
            "\x00\x00";


#ifdef __cplusplus
} /* extern "C" */
#endif

#else /* ifndef INCLUDED_NVRAM_IMAGE_H_ */

#error Wi-Fi NVRAM image included twice

#endif /* ifndef INCLUDED_NVRAM_IMAGE_H_ */
