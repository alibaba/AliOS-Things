#!/bin/sh

PATH_SYS_FILE="../../Library/StdDriver/inc/sys.h"


PrintHead () {
echo "/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the \"License\");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an \"AS IS\" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/ "
}

PrintInclude () {
echo "#include \"PeripheralPins.h\""
}

# $1: "PinMap variable name" 
# $2: filter rule
# ex:
#
# M487
# SYS_GPB_MFPL_PB0MFP_EADC0_CH0
# { PB_0, ADC_0_0, SYS_GPB_MFPL_PB0MFP_EADC0_CH0  },
#
# NANO130
# SYS_PA_L_MFP_PA0_MFP_ADC_CH0
# { PB_0, ADC_0_0, SYS_PA_L_MFP_PA0_MFP_ADC_CH0  },

PrintPinMapStructure () {
	echo "//*** $1 ***"
	echo "const PinMap PinMap_"$1"[] = { "

#	grep -r $2 $PATH_SYS_FILE | awk -F' ' '{split($2,a,"_");  split(a[4],b,"MFP"); split(a[5],c,/[0-9]/); split($2,d,c[1]); split(d[2],e,"_"); split(a[6],f,"CH");  print "    { "substr(a[4],1,2)"_"substr(b[1],3)", \t",c[1]"_"e[1],", \t",$2," },"}'

grep -r $2 $PATH_SYS_FILE | awk -F' ' '{split($2,a,"_"); split(a[8],f,"CH"); split(a[7],c,/[0-9]/); split($2,d,c[1]); split(d[2],e,"_"); print "    { "substr(a[5],1,2)"_"substr(a[5],3)", \t",c[1]"_"e[1],", \t",$2," },"}'

	echo "    { NC,   NC,    0 }"
	echo "};"
	echo ""
}

OutCSource () {
	PrintHead
	PrintInclude

	PrintPinMapStructure "ADC" "MFP_ADC[0-9]*_CH"
	PrintPinMapStructure "DAC"  "MFP_DA_OUT[0-9]"

	PrintPinMapStructure "I2C_SDA" "MFP_I2C[0-9]*_SDA"
	PrintPinMapStructure "I2C_SCL" "MFP_I2C[0-9]*_SCL"

	PrintPinMapStructure "PWM" "MFP_PWM[0-9]*_CH"

	PrintPinMapStructure "UART_TX" "MFP_UART[0-9]*_TX"
	PrintPinMapStructure "UART_RX" "MFP_UART[0-9]*_RX"
	PrintPinMapStructure "UART_RTS"	"MFP_UART[0-9]*_RTS"
	PrintPinMapStructure "UART_CTS"	"MFP_UART[0-9]*_CTS"

	PrintPinMapStructure "SPI_MOSI" "MFP_SPI[0-9]*_MOSI"
	PrintPinMapStructure "SPI_MISO" "MFP_SPI[0-9]*_MISO"
	PrintPinMapStructure "SPI_SCLK"	"MFP_SPI[0-9]*_SCLK"
	PrintPinMapStructure "SPI_SSEL"	"MFP_SPI[0-9]*_SS"

}

PrintPinMapDef () {
	echo "//*** $1 ***"
	echo "extern const PinMap PinMap_"$1"[];"
	echo ""
}

OutCHeader () {

	PrintHead
	
echo "
#ifndef SOC_PERIPHERALPINS_H
#define SOC_PERIPHERALPINS_H

#include \"pinmap.h\"
#include \"PeripheralNames.h\"

#ifdef __cplusplus
extern \"C\" {
#endif
"
	PrintPinMapDef "ADC"
	PrintPinMapDef "DAC"

	PrintPinMapDef "I2C_SDA"
	PrintPinMapDef "I2C_SCL"

	PrintPinMapDef "PWM"

	PrintPinMapDef "UART_TX"
	PrintPinMapDef "UART_RX"
	PrintPinMapDef "UART_RTS"
	PrintPinMapDef "UART_CTS"

	PrintPinMapDef "SPI_MOSI"
	PrintPinMapDef "SPI_MISO"
	PrintPinMapDef "SPI_SCLK"
	PrintPinMapDef "SPI_SSEL"

echo "#ifdef __cplusplus
}
#endif

#endif //SOC_PERIPHERALPINS_H
"
	
}

OutCSource > __PeripheralPins.c
sed -i 's/I_2C/I2C_/g' __PeripheralPins.c

OutCHeader > __PeripheralPins.h
