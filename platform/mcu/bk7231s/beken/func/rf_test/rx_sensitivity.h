#ifndef _RX_SENSITIVITY_H_
#define _RX_SENSITIVITY_H_

#define RS_DEBUG

#ifdef RS_DEBUG
#define RS_PRT       os_printf
#define RS_WPRT      warning_prf
#else
#define RS_PRT       os_null_printf
#define RS_WPRT      os_null_printf
#endif // RS_DEBUG

#define RS_2_4_G_CHANNEL_NUM            (14)


/* pseudo-code*/
/*
void rs_get_rx_result(void)
{
#!/bin/bash
	 echo ""
	 TXBW20=`mem 0xc0b00b70`
	 TXBW40=`mem 0xc0b00b74`
	 TXBW80=`mem 0xc0b00b78`
	 TXBW160=`mem 0xc0b00b7C`

	 TXMPDUINAMPDU=`mem 0xc0b00b34`
	 TXOCTECTINAMPDU=`mem 0xc0b00b38`
	 TXAMPDU=`mem 0xc0b00b30`

	 RXBW20=`mem 0xc0b00b80`
	 RXBW40=`mem 0xc0b00b84`
	 RXBW80=`mem 0xc0b00b88`
	 RXBW160=`mem 0xc0b00b8C`

	 RXMPDUINAMPDU=`mem 0xc0b00b48`
	 RXOCTECTINAMPDU=`mem 0xc0b00b4C`
	 RXAMPDU=`mem 0xc0b00b3C`

	 FCSERROR=`mem 0xc0b00804` 
	 PHYERROR=`mem 0xc0b00808` 
	 RXFLOWC=`mem 0xc0b0080c` 

	 mem 0xc0b00220 w 0x0 > /dev/null
	 mem 0xc0b00290 w 0x0 > /dev/null
	 mem 0xc0b00294 w 0x0 > /dev/null
	 mem 0xc0b00298 w 0x0 > /dev/null
	 mem 0xc0b00040 w 0x00011881 > /dev/null
	 
	 
	 sleep 1
	 TXBW20NEW=`mem 0xc0b00b70`
	 TXBW40NEW=`mem 0xc0b00b74`
	 TXBW80NEW=`mem 0xc0b00b78`
	 TXBW160NEW=`mem 0xc0b00b7C`

	 TXMPDUINAMPDUNEW=`mem 0xc0b00b34`
	 TXOCTECTINAMPDUNEW=`mem 0xc0b00b38`
	 TXAMPDUNEW=`mem 0xc0b00b30`

	 RXBW20NEW=`mem 0xc0b00b80`
	 RXBW40NEW=`mem 0xc0b00b84`
	 RXBW80NEW=`mem 0xc0b00b88`
	 RXBW160NEW=`mem 0xc0b00b8C`

	 RXMPDUINAMPDUNEW=`mem 0xc0b00b48`
	 RXOCTECTINAMPDUNEW=`mem 0xc0b00b4C`
	 RXAMPDUNEW=`mem 0xc0b00b3C`

	 FCSERRORNEW=`mem 0xc0b00804` 
	 PHYERRORNEW=`mem 0xc0b00808` 
	 RXFLOWCNEW=`mem 0xc0b0080c` 
	 CCAPRI=`mem 0xc0b00220`
	 CCASEC20=`mem 0xc0b00290`
	 CCASEC40=`mem 0xc0b00294`
	 CCASEC80=`mem 0xc0b00298`

	 TXBW20CUR=$((0x$TXBW20NEW - 0x$TXBW20))
	 TXBW40CUR=$((0x$TXBW40NEW - 0x$TXBW40))
	 TXBW80CUR=$((0x$TXBW80NEW - 0x$TXBW80))
	 TXBW160CUR=$((0x$TXBW160NEW - 0x$TXBW160))
	 TXTOT=$(($TXBW40CUR + $TXBW20CUR + $TXBW80CUR + $TXBW160CUR))


	 TXAMPDUCUR=$((0x$TXAMPDUNEW - 0x$TXAMPDU))
	 TXMPDUINAMPDUCUR=$((0x$TXMPDUINAMPDUNEW - 0x$TXMPDUINAMPDU))
	 TXOCTECTINAMPDUCUR=$((0x$TXOCTECTINAMPDUNEW - 0x$TXOCTECTINAMPDU))
	 #TXAMPDUCUR=$TXBW40CUR

	 if [ $TXTOT == 0 ]
	 then 
	   TXTOT=1
	 fi   

	 printf "TX\n"
	 printf "20MHz  : %0d (%0d %%)\n" $TXBW20CUR $((100*$TXBW20CUR/$TXTOT))
	 printf "40MHz  : %0d (%0d %%)\n" $TXBW40CUR $((100*$TXBW40CUR/$TXTOT))
	 printf "80MHz  : %0d (%0d %%)\n" $TXBW80CUR $((100*$TXBW80CUR/$TXTOT))
	 printf "160MHz : %0d (%0d %%)\n" $TXBW160CUR $((100*$TXBW160CUR/$TXTOT))

	 printf "AMPDU  : %0d (%0d %%)\n" $TXAMPDUCUR $((100*$TXAMPDUCUR/$TXTOT))
	 if [ $TXAMPDUCUR == 0 ]
	 then 
	   TXAMPDUCUR=1
	 fi   
	 echo "$TXMPDUINAMPDUCUR $TXAMPDUCUR" | awk '{printf "MPDU  in AMPDU : %0.1f \n", $1/$2}'
	 printf "Bytes in AMPDU : %0d\n" $(($TXOCTECTINAMPDUCUR/$TXAMPDUCUR))
	 printf "AMPDU Throughput : %0.1fMbps\n" $((8*$TXOCTECTINAMPDUCUR/1000000))



	 RXBW20CUR=$((0x$RXBW20NEW - 0x$RXBW20))
	 RXBW40CUR=$((0x$RXBW40NEW - 0x$RXBW40))
	 RXBW80CUR=$((0x$RXBW80NEW - 0x$RXBW80))
	 RXBW160CUR=$((0x$RXBW160NEW - 0x$RXBW160))
	 
	 RXAMPDUCUR=$((0x$RXAMPDUNEW - 0x$RXAMPDU))
	 RXMPDUINAMPDUCUR=$((0x$RXMPDUINAMPDUNEW - 0x$RXMPDUINAMPDU))
	 RXOCTECTINAMPDUCUR=$((0x$RXOCTECTINAMPDUNEW - 0x$RXOCTECTINAMPDU))
	 
	 FCSERRCUR=$((0x$FCSERRORNEW - 0x$FCSERROR))
	 PHYERRCUR=$((0x$PHYERRORNEW - 0x$PHYERROR))
	 RXFLOWCCUR=$((0x$RXFLOWCNEW - 0x$RXFLOWC))
	 RXTOT=$(($RXBW40CUR + $RXBW20CUR + $RXBW80CUR + $RXBW160CUR + $FCSERRCUR + $PHYERRCUR + $RXFLOWCCUR))

	 if [ $RXTOT == 0 ]
	 then 
	   RXTOT=1
	 fi   

	 printf "\nRX\n"
	 printf "20MHz  : %0d (%0d %%)\n" $RXBW20CUR $((100*$RXBW20CUR/$RXTOT))
	 printf "40MHz  : %0d (%0d %%)\n" $RXBW40CUR $((100*$RXBW40CUR/$RXTOT))
	 printf "80MHz  : %0d (%0d %%)\n" $RXBW80CUR $((100*$RXBW80CUR/$RXTOT))
	 printf "160MHz : %0d (%0d %%)\n" $RXBW160CUR $((100*$RXBW160CUR/$RXTOT))

	 printf "AMPDU  : %0d (%0d %%)\n" $RXAMPDUCUR $((100*$RXAMPDUCUR/$RXTOT))
	 if [ $RXAMPDUCUR == 0 ]
	 then 
	   RXAMPDUCUR=1
	 fi   
	 echo "$RXMPDUINAMPDUCUR $RXAMPDUCUR" | awk '{printf "MPDU  in AMPDU : %0.1f \n", $1/$2}'
	 printf "Bytes in AMPDU : %0d\n" $(($RXOCTECTINAMPDUCUR/$RXAMPDUCUR))
	 printf "AMPDU Throughput : %0.1fMbps\n" $((8*$RXOCTECTINAMPDUCUR/1000000))
	 
	 printf "FCSErr   : %0d (%0d %%)\n" $FCSERRCUR $((100*$FCSERRCUR/$RXTOT))
	 printf "PHYErr   : %0d (%0d %%)\n" $PHYERRCUR $((100*$PHYERRCUR/$RXTOT)) 
	 printf "RxFlowErr: %0d (%0d %%)\n" $RXFLOWCCUR $((100*$RXFLOWCCUR/$RXTOT)) 
	 printf "CCAPrim  Busy : %3d %%\n" $((0x$CCAPRI / 10000))
	 printf "CCASec20 Busy : %3d %%\n" $((0x$CCASEC20 / 10000))
	 printf "CCASec40 Busy : %3d %%\n" $((0x$CCASEC40 / 10000))
	 printf "CCASec80 Busy : %3d %%\n" $((0x$CCASEC80 / 10000))
	 printf "\n"
}
*/

#endif // _RX_SENSITIVITY_H_

