/*******************************************************************************
  File Name:
    m2m_wifi.h

  Summary:
    WILC1000 WLAN Application Interface

  Description:
    WILC1000 WLAN Application Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef __M2M_WIFI_H__
#define __M2M_WIFI_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "common/include/nm_common.h"
#include "driver/include/m2m_types.h"
#ifdef CONF_WILC_USE_3000_REV_A
#include "driver/include/m2m_coex.h"
#endif

#ifdef CONF_MGMT


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/**@defgroup  WlanEnums Enumeration/Typedef
 * @ingroup m2m_wifi
 * @{
 */
/*!
@enum	\
	tenuWifiFrameType

@brief
	Enumeration for Wi-Fi MAC frame type codes (2-bit) 
	The following types are used to identify the type of frame sent or received.
	Each frame type constitues a number of frame subtypes as defined in @ref tenuSubTypes to specify the exact type of frame.
	Values are defined as per the IEEE 802.11 standard.
	
@remarks
	The following frame types are useful for advanced user usage when @ref CONF_MGMT is defined
	and the user application requires to monitor the frame transmission and reception.
@see
	tenuSubTypes
*/
typedef enum {
	MANAGEMENT            = 0x00,
	/*!< Wi-Fi Management frame (Probe Req/Resp, Beacon, Association Req/Resp ...etc).
	*/
	CONTROL               = 0x04,
	/*!< Wi-Fi Control frame (eg. ACK frame).
	*/
	DATA_BASICTYPE        = 0x08,
	/*!< Wi-Fi Data frame.
	*/
	RESERVED              = 0x0C
}tenuWifiFrameType;


/*!
@enum	\
	tenuSubTypes

@brief
	Enumeration for Wi-Fi MAC Frame subtype code (6-bit).
	The frame subtypes fall into one of the three frame type groups as defined in @ref tenuWifiFrameType
	(MANAGEMENT, CONTROL & DATA).
	Values are defined as per the IEEE 802.11 standard.
@remarks
	The following sub-frame types are useful for advanced user usage when @ref CONF_MGMT is defined
	and the application developer requires to monitor the frame transmission and reception.
@see
    	tenuWifiFrameType
*/
typedef enum {

	/*!< Sub-Types related to Management Sub-Types */
	ASSOC_REQ             = 0x00,
	ASSOC_RSP             = 0x10,
	REASSOC_REQ           = 0x20,
	REASSOC_RSP           = 0x30,
	PROBE_REQ             = 0x40,
	PROBE_RSP             = 0x50,
	BEACON                = 0x80,
	ATIM                  = 0x90,
	DISASOC               = 0xA0,
	AUTH                  = 0xB0,
	DEAUTH                = 0xC0,
	ACTION                = 0xD0,
 
/**@{*/ 
	/* Sub-Types related to Control */
	PS_POLL               = 0xA4,
	RTS                   = 0xB4,
	CTS                   = 0xC4,
	ACK                   = 0xD4,
	CFEND                 = 0xE4,
	CFEND_ACK             = 0xF4,
	BLOCKACK_REQ          = 0x84,
	BLOCKACK              = 0x94,
/**@{*/ 
	/* Sub-Types related to Data */
	DATA                  = 0x08,
	DATA_ACK              = 0x18,
	DATA_POLL             = 0x28,
	DATA_POLL_ACK         = 0x38,
	NULL_FRAME            = 0x48,
	CFACK                 = 0x58,
	CFPOLL                = 0x68,
	CFPOLL_ACK            = 0x78,
	QOS_DATA              = 0x88,
	QOS_DATA_ACK          = 0x98,
	QOS_DATA_POLL         = 0xA8,
	QOS_DATA_POLL_ACK     = 0xB8,
	QOS_NULL_FRAME        = 0xC8,
	QOS_CFPOLL            = 0xE8,
	QOS_CFPOLL_ACK        = 0xF8
}tenuSubTypes;


/*!
@enum	\
	tenuInfoElementId

@brief
	Enumeration for the Wi-Fi Information Element(IE) IDs, which indicates the specific type of IEs.
	IEs are management frame information included in management frames.
	Values are defined as per the IEEE 802.11 standard.

@details	Available IDs are:-
	
	ISSID   :	 Service Set Identifier (SSID)
	
	ISUPRATES   :	 Supported Rates
	
	IFHPARMS     :	 FH parameter set
	
	IDSPARMS      :       DS parameter set
	
	ICFPARMS      :        CF parameter set
	
	ITIM            :    	 Traffic Information Map

	IIBPARMS        :    	 IBSS parameter set
	
	ICOUNTRY        :  	 Country element.
	
	IEDCAPARAMS     :      EDCA parameter set
	
	ITSPEC              :  	 Traffic Specification

	ITCLAS             :  	 Traffic Classification
	
	ISCHED             :     Schedule.
	
	ICTEXT              :  	 Challenge Text
	
	IPOWERCONSTRAINT   :   	 Power Constraint.
	
	IPOWERCAPABILITY    :  	 Power Capability
	
	ITPCREQUEST         :   	 TPC Request                    
	
	ITPCREPORT          :   	 TPC Report                     
	
	ISUPCHANNEL         :   Supported channel list  
	
	ICHSWANNOUNC         :  Channel Switch Announcement    
	
	IMEASUREMENTREQUEST 	:	 Measurement request            
	
	IMEASUREMENTREPORT   :  Measurement report             
	
	IQUIET               :  Quiet element Info             
	
	IIBSSDFS           :  	 IBSS DFS                       
	
	IERPINFO           :  	 ERP Information                
	
	ITSDELAY           :    	 TS Delay                       
	
	ITCLASPROCESS      :	 TCLAS Processing               
	
	IHTCAP               :  	 HT Capabilities                
	
	IQOSCAP             :  	 QoS Capability                 
	
	IRSNELEMENT         :   RSN Information Element        
	
	IEXSUPRATES         :   Extended Supported Rates       
	
	IEXCHSWANNOUNC      :  Extended Ch Switch Announcement
	
	IHTOPERATION        :  	 HT Information                 
	
	ISECCHOFF          :   Secondary Channel Offset      
	
	I2040COEX           :   Coexistence IE           
	
	I2040INTOLCHREPORT  :   Intolerant channel report
	
	IOBSSSCAN           :  	 OBSS Scan parameters           
	
	IEXTCAP             :  	 Extended capability          
	
	IWMM                :  	 WMM parameters                 
	
	IWPAELEMENT         :  WPA Information Element
	
*/
typedef enum {
	ISSID               = 0,   
	/*!< Service Set Identifier (SSID)
	*/
	ISUPRATES           = 1,   
	/*!< Supported Rates
	*/
	IFHPARMS            = 2,   
	/*!< FH parameter set
	*/
	IDSPARMS            = 3,   
	/*!< DS parameter set
	*/
	ICFPARMS            = 4,   
	/*!< CF parameter set
	*/
	ITIM                = 5,   
	/*!< Traffic Information Map
	*/
	IIBPARMS            = 6,   
	/*!< IBSS parameter set
	*/
	ICOUNTRY            = 7,   
	/*!< Country element.
	*/
	IEDCAPARAMS         = 12,  
	/*!< EDCA parameter set
	*/
	ITSPEC              = 13,  
	/*!< Traffic Specification
	*/
	ITCLAS              = 14,  
	/*!< Traffic Classification
	*/
	ISCHED              = 15,  
	/*!< Schedule.
	*/
	ICTEXT              = 16,  
	/*!< Challenge Text
	*/
	IPOWERCONSTRAINT    = 32,  
	/*!< Power Constraint.
	*/
	IPOWERCAPABILITY    = 33,  
	/*!< Power Capability
	*/
	ITPCREQUEST         = 34,  
	/*!< TPC Request                    
	*/
	ITPCREPORT          = 35,  
	/*!< TPC Report                     
	*/
	ISUPCHANNEL         = 36, 
	/* Supported channel list  
	*/
	ICHSWANNOUNC        = 37,  
	/*!< Channel Switch Announcement    
	*/
	IMEASUREMENTREQUEST = 38,  
	/*!< Measurement request            
	*/
	IMEASUREMENTREPORT  = 39,  
	/*!< Measurement report             
	*/
	IQUIET              = 40,  
	/*!< Quiet element Info             
	*/
	IIBSSDFS            = 41,  
	/*!< IBSS DFS                       
	*/
	IERPINFO            = 42,  
	/*!< ERP Information                
	*/
	ITSDELAY            = 43,  
	/*!< TS Delay                       
	*/
	ITCLASPROCESS       = 44,  
	/*!< TCLAS Processing               
	*/
	IHTCAP              = 45,  
	/*!< HT Capabilities                
	*/
	IQOSCAP             = 46,  
	/*!< QoS Capability                 
	*/
	IRSNELEMENT         = 48,  
	/*!< RSN Information Element        
	*/
	IEXSUPRATES         = 50,  
	/*!< Extended Supported Rates       
	*/
	IEXCHSWANNOUNC      = 60,  
	/*!< Extended Ch Switch Announcement
	*/
	IHTOPERATION        = 61,  
	/*!< HT Information                 
	*/
	ISECCHOFF           = 62,  
	/*!< Secondary Channel Offset      
	*/
	I2040COEX           = 72,  
	/*!< 20/40 Coexistence IE           
	*/
	I2040INTOLCHREPORT  = 73,  
	/*!< 20/40 Intolerant channel report
	*/
	IOBSSSCAN           = 74,  
	/*!< OBSS Scan parameters           
	*/
	IEXTCAP             = 127, 
	/*!< Extended capability            
	*/
	IWMM                = 221, 
	/*!< WMM parameters                 
	*/
	IWPAELEMENT         = 221 
	/*!< WPA Information Element        
	*/
}tenuInfoElementId;
 //@}

/*!
@struct	\
	tenuWifiCapability

@brief
	Enumeration for capability Information field bit. 
	The value of the capability information field from the 802.11 management frames received by the wireless LAN interface. 
	Defining the cabilites of the Wi-Fi system. Values are defined as per the IEEE 802.11 standard.

@details
	Capabilities:-
	ESS/IBSS             : Defines wether a frame is coming from an AP or not.              
	POLLABLE    		: CF Pollable                  
	POLLREQ       		: Request to be polled         
	PRIVACY      		: WEP encryption supported     
	SHORTPREAMBLE   : Short Preamble is supported  
	SHORTSLOT           : Short Slot is supported      
	PBCC       	        :PBCC                         
	CHANNELAGILITY :Channel Agility              
	SPECTRUM_MGMT  :Spectrum Management          
	DSSS_OFDM      : DSSS-OFDM                    
*/
typedef enum{
	ESS            = 0x01,   
	/*!< ESS capability               
	*/
	IBSS           = 0x02,   
	/*!< IBSS mode                    
	*/
	POLLABLE       = 0x04,   
	/*!< CF Pollable                  
	*/
	POLLREQ        = 0x08,   
	/*!< Request to be polled         
	*/
	PRIVACY        = 0x10,   
	/*!< WEP encryption supported     
	*/
	SHORTPREAMBLE  = 0x20,   
	/*!< Short Preamble is supported  
	*/
	SHORTSLOT      = 0x400,  
	/*!< Short Slot is supported      
	*/
	PBCC           = 0x40,   
	/*!< PBCC                         
	*/
	CHANNELAGILITY = 0x80,   
	/*!< Channel Agility              
	*/
	SPECTRUM_MGMT  = 0x100,  
	/*!< Spectrum Management          
	*/
	DSSS_OFDM      = 0x2000  
	/*!< DSSS-OFDM                    
	*/
}tenuWifiCapability;


#endif
#ifdef P2P_CONCURRENT_INTERFACES

/**@defgroup  WlanEnums Enumeration/Typedef
 * @ingroup m2m_wifi
 * @{
 */
/*!
@enum	\
	tenuWifiFrameType

@brief
	Enumeration for the used interfaces with concurrency operations.
	
@remarks
	
@see
	
*/

/* P2P_STA_CONCURRENCY_INTERFACE:
*  This interface is used for P2P-Station concurrency mode.
*  Both Station and P2P should be on the same social channels(M2M_WIFI_CH_1, M2M_WIFI_CH_6 or M2M_WIFI_CH_11)
*  in order to create P2P-Station concurrency mode.
*  Usage:  os_m2m_wifi_set_p2p_control_ifc(P2P_STA_CONCURRENCY_INTERFACE)
*
*  P2P_AP_CONCURRENCY_INTERFACE:
*  This interface is used for P2P-AP concurrency mode.
*  Both AP and P2P should be on the same social channels(M2M_WIFI_CH_1, M2M_WIFI_CH_6 or M2M_WIFI_CH_11)
*  in order to create P2P-AP concurrency mode.
*  Usage:  os_m2m_wifi_set_p2p_control_ifc(P2P_AP_CONCURRENCY_INTERFACE)
*/
typedef enum {
	P2P_STA_CONCURRENCY_INTERFACE	=0,
	P2P_AP_CONCURRENCY_INTERFACE	=1,
}tenuP2pControlInterface;

#endif

/*!
@typedef \
	tpfAppWifiCb

@brief	
				Wi-Fi's main callback function handler, for handling the M2M_WIFI events received on the Wi-Fi interface. 
			       Such notifications are received in response to Wi-Fi/P2P operations such as @ref m2m_wifi_request_scan,
				@ref m2m_wifi_connect. 
				Wi-Fi/P2P operations are implemented in an asynchronous mode, and all incoming information/status
				are to be handled through this callback function when the corresponding notification is received.
				Applications are expected to assign this wi-fi callback function by calling @ref m2m_wifi_init
@param [in]	u8MsgType
				Type of notification. Possible types are:
				/ref M2M_WIFI_RESP_CON_STATE_CHANGED
				/ref M2M_WIFI_RESP_CONN_INFO
				/ref M2M_WIFI_REQ_WPS
				/ref M2M_WIFI_RESP_SCAN_DONE
				/ref M2M_WIFI_RESP_SCAN_RESULT
				/ref M2M_WIFI_RESP_CURRENT_RSSI
				/ref M2M_WIFI_RESP_DEFAULT_CONNECT
		
			In case Bypass mode is defined :
				@ref M2M_WIFI_RESP_ETHERNET_RX_PACKET
		
			In case Monitoring mode is used:
				@ref M2M_WIFI_RESP_WIFI_RX_PACKET
				
@param [in]	pvMsg
				A pointer to a buffer containing the notification parameters (if any). It should be
				Casted to the correct data type corresponding to the notification type.
	
@see
	tstrM2mWifiStateChanged
	tstrM2MWPSInfo
	tstrM2mScanDone
	tstrM2mWifiscanResult
*/
typedef void (*tpfAppWifiCb) (uint8 u8MsgType, void * pvMsg);

/*!
@typedef \
	tpfAppEthCb

@brief	
	Ethernet (Bypass mode) notification callback function receiving Bypass mode events as defined in
	the Wi-Fi reponses enumeration @ref tenuM2mStaCmd. 

@param [in]	u8MsgType
	Type of notification. Possible types are:
	- [M2M_WIFI_RESP_ETHERNET_RX_PACKET](@ref M2M_WIFI_RESP_ETHERNET_RX_PACKET)

@param [in]	pvMsg
	A pointer to a buffer containing the notification parameters (if any). It should be
	casted to the correct data type corresponding to the notification type.
	For example, it could be a pointer to the buffer holding the received frame in case of @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET
	event.
	
@param [in]	pvControlBuf
	A pointer to control buffer describing the accompanied message.
	To be casted to @ref tstrM2mIpCtrlBuf in case of @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET event.

@warning
	Make sure that the bypass mode is defined before using @ref tpfAppEthCb. 

@see
	m2m_wifi_init

*/
typedef void (*tpfAppEthCb) (uint8 u8MsgType, void * pvMsg,void * pvCtrlBuf);

/*!
@typedef	\
	tpfAppMonCb

@brief	
	Wi-Fi monitoring mode callback function. This function delivers all received wi-Fi packets through the Wi-Fi interface.
       Applications requiring to operate in the monitoring should call the asynchronous function m2m_wifi_enable_monitoring_mode
       and expect to receive the wi-fi packets through this callback function, when the event ....is received.
	To disable the monitoring mode a call to @ref m2m_wifi_disable_monitoring_mode should be made.
@param [in]	pstrWifiRxPacket
				Pointer to a structure holding the Wi-Fi packet header parameters.

@param [in]	pu8Payload
				Pointer to the buffer holding the Wi-Fi packet payload information required by the application starting from the
				defined OFFSET by the application (when calling m2m_wifi_enable_monitoring_mode).
				Could hold a value of NULL, if the application does not need any data from the payload.

@param [in]	u16PayloadSize
				The size of the payload in bytes.
				
@see
	m2m_wifi_enable_monitoring_mode		
	
@warning
	u16PayloadSize should not exceed the buffer size given through m2m_wifi_enable_monitoring_mode.
	
*/
typedef void (*tpfAppMonCb) (tstrM2MWifiRxPacketInfo *pstrWifiRxPacket, uint8 * pu8Payload, uint16 u16PayloadSize);

/**
@struct 	\
	tstrEthInitParam
	
@brief		
	Structure to hold ethernet interface parameters. 
	Structure is to be defined and have its attributes set,based on the application's functiality before 
	a call is made to the initialize the wi-fi operations by calling the  @ref m2m_wifi_init function.
	Part of the wi-fi configuration structure @ref tstrWifiInitParam.
	Applications shouldn't need to define this structure, if the bypass mode is not defined.
	
@see
	tpfAppEthCb
	tpfAppWifiCb
	m2m_wifi_init

@warning
	Make sure that bypass mode is defined before using @ref tstrEthInitParam. 

*/
typedef struct {
	tpfAppWifiCb pfAppWifiCb;
	/*!<
		Callback for wifi notifications.
	*/
	tpfAppEthCb  pfAppEthCb;
	/*!<
		Callback for Ethernet interface.
	*/
	uint8 * au8ethRcvBuf;
	/*!<
		Pointer to Receive Buffer of Ethernet Packet
	*/
	uint16	u16ethRcvBufSize;
	/*!<
		Size of Receive Buffer for Ethernet Packet
	*/
} tstrEthInitParam;
/*!
@struct	\	
 	tstrM2mIpCtrlBuf
 	
@brief		
 	Structure holding the incoming buffer's data size information, indicating the data size of the buffer and the remaining buffer's data size .
 	The data of the buffer which holds the packet sent to the host when in the bypass mode, is placed in the @ref tstrEthInitParam structure in the au8ethRcvBuf attribute.
	This following information is retreived in the host when an event @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET is received
	in the wi-fi callback function @ref tpfAppWifiCb.

 	The application is expected to use this strurture's information to determine if there is still incoming data to be received from the firmware.
 	
 @see
	 tpfAppEthCb
	 tstrEthInitParam
 
 @warning
	 Make sure that bypass mode is defined before using @ref tstrM2mIpCtrlBuf

 */
typedef struct{
	uint16	u16DataSize;
	/*!<
		Size of the received data in bytes.
	*/
	uint16	u16RemainigDataSize;
	/*!<
		Size of the remaining data bytes to be delivered to host.
	*/
	uint8	u8DataOffset;
	/*!<
		Offset of the Ethernet packet inside the receive buffer.
	*/
	uint8	u8IfcId;
	/*!<
		The logical interface to which the frame belongs.
	*/
} tstrM2mIpCtrlBuf;


/**
@struct		\
	tstrWifiInitParam

@brief		
	Structure, holding the Wi-fi configuration attributes such as the wi-fi callback , monitoring mode callback and ethernet parameter initialization structure.
	Such configuration parameters are required to be set before calling the wi-fi initialization function @ref m2m_wifi_init.
	@ref pfAppWifiCb attribute must be set to handle the wi-fi callback operations.
	@ref pfAppMonCb attribute, is optional based on wether the application requires the monitoring mode configuration, and can there not
	be set before the initialization.
	@ref strEthInitParam structure, is another optional configuration based on wether the bypass mode is set.
	
*/
typedef struct {
	tpfAppWifiCb pfAppWifiCb;
	/*!<
		Callback for Wi-Fi notifications.
	*/
	tpfAppMonCb  pfAppMonCb;
	/*!<
		Callback for monitoring interface.
	*/
	tstrEthInitParam strEthInitParam ;
	/*!<
	Structure to hold Ethernet interface parameters.
	*/

} tstrWifiInitParam;


typedef union
{
	tstrM2mCoexMode 		strM2mCoexMode;
	tstrM2mCoexNullFramesMode strM2mCoexNullFramesMode;
	tstrM2mWifiConnect		strM2mWifiConnect;
	tstrM2mTxPwrLevel		strM2mTxPwrLevel;
	tstrM2mSetMacAddress	strM2mSetMacAddress;
	tstrM2MIPConfig			strM2MIPConfig;
	tstrM2mLsnInt			strM2mLsnInt;
	tstrM2MScanOption		strM2MScanOption;
	tstrM2MScanRegion		strM2MScanRegion;
	tstrM2MScanList			strM2MScanList;
	tstrM2MScan				strM2MScan;
	tstrM2MWPSConnect		strM2MWPSConnect;
	tstrM2MP2PConnect		strM2MP2PConnect;
	tstrM2MAPConfig			strM2MAPConfig;
	uint8					au8BlackListRequest[7];
	uint8					au8MacAddress[6];
	uint8					au8MacAddressDpl[12];
	tstrM2mReqScanResult	strM2mReqScanResult;
	tstrM2mPsType			strM2mPsType;
	tstrM2MDeviceNameConfig	strM2MDeviceNameConfig;
	tstrM2MWifiMonitorModeCtrl strM2MWifiMonitorModeCtrl;
	uint8 					u8ChannelID;
	tstrM2MWifiTxPacketInfo	strM2MWifiTxPacketInfo;
	uint32					u32UTCSeconds;
	tstrM2MMulticastMac		strM2MMulticastMac;
	uint8					au8CertChunk[32];
	tstrM2MIfId				strM2MIfId;
	tstrM2MUserMaxTxRate	strM2MUserMaxTxRate;
}tuCtrlStruct;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup WLANAPI Function
 *   @ingroup m2m_wifi
 */
#ifdef __cplusplus
     extern "C" {
#endif

 /** @defgroup WiFiDownloadFn m2m_wifi_download_mode
  *  @ingroup WLANAPI
  * 	 Synchronous download mode entry function that prepares the WINC board to enter the download mode, ready for the firmware or certificate download.
  *	 The WINC board is prepared for download, through initializations for the WINC driver including bus initializations and interrupt enabling, it also halts the chip, to allow for the firmware downloads.
  *	 Firmware can be downloaded through a number of interfaces, UART, I2C and SPI.
  */
  /**@{*/
 /*!
 @fn \
	 NMI_API void  m2m_wifi_download_mode(void);
 @brief  Prepares the WINC broard before downloading any data (Firmware, Certificates .. etc)
 
		 This function should called before starting to download any data to the WINC board. The WINC board is prepared for download, through initializations for the WINC driver including bus initializations 
		 and interrupt enabling, it also halts the chip, to allow for the firmware downloads Firmware can be downloaded through a number of interfaces, UART, I2C and SPI.
 
 @return	 
	 The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
 */
 NMI_API sint8	m2m_wifi_download_mode(void);
 
  /**@}*/
  

 /** @defgroup WifiInitFn m2m_wifi_init
 *  @ingroup WLANAPI
 *   Synchronous initialization function for the WINC driver. This function initializes the driver by, registering the call back function for M2M_WIFI layer(also the call back function for bypass mode/monitoring mode if defined), \n
	initializing the host interface layer and the bus interfaces. \n
	Wi-Fi callback registering is essential to allow the handling of the events received, in response to the asynchronous wi-fi operations. \n
	Following are the possible wi-fi events that are expected to be received through the call back function(provided by the application) to the M2M_WIFI layer are : \n
		@ref M2M_WIFI_RESP_CON_STATE_CHANGED \n
		@ref M2M_WIFI_RESP_CONN_INFO \n
		@ref M2M_WIFI_REQ_WPS \n
		@ref M2M_WIFI_RESP_SCAN_DONE \n
		@ref M2M_WIFI_RESP_SCAN_RESULT \n
		@ref M2M_WIFI_RESP_CURRENT_RSSI \n
		@ref M2M_WIFI_RESP_DEFAULT_CONNECT \n
	Example: \n
	In case Bypass mode is defined : \n
		@ref M2M_WIFI_RESP_ETHERNET_RX_PACKET
		
	In case Monitoring mode is used: \n
		@ref M2M_WIFI_RESP_WIFI_RX_PACKET
		
	Any application using the WINC driver must call this function at the start of its main function.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8  m2m_wifi_init(tstrWifiInitParam * pWifiInitParam);

@param [in]	pWifiInitParam
	This is a pointer to the @ref tstrWifiInitParam structure which holds the pointer to the application WIFI layer call back function,
	monitoring mode call back and @ref tstrEthInitParam strucutre containing bypass mode parameters. 
	
@pre 
	Prior to this function call, application users must provide a call back function responsible for receiving all the wi-fi events that are received on the M2M_WIFI layer.
	
@warning
	Failure to successfully complete function indicates that the driver couldn't be initialized and a fatal error will prevent the application from proceeding. 
	
@see
	m2m_wifi_deinit
	tenuM2mStaCmd

@return		
	The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
NMI_API sint8  m2m_wifi_init(tstrWifiInitParam * pWifiInitParam);
 /**@}*/
 /** @defgroup WifiDeinitFn m2m_wifi_deinit
 *  @ingroup WLANAPI
 *   Synchronous deinitialization function to the WINC1500 driver.
 *   Dinitializes the host interface and frees resources used by the M2M_WIFI layer.
 *   Must be called in the application closing function, to ensure that all resources have been correctly released. 
 *   No arguments are expected to be passed in.
 */
/**@{*/
/*!
@fn	\
	NMI_API sint8  m2m_wifi_deinit(void * arg);
	
@param [in]	arg
		Generic argument. Not used in current implementation.
@return		
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8  m2m_wifi_deinit(void * arg);
 /**@}*/
/** @defgroup WifiHandleEventsFn m2m_wifi_handle_events
*  @ingroup WLANAPI
* 	Synchronous M2M event handler function, responsible for handling interrupts received from the WINC firmware.
*     Application developers should call this function periodically in-order to receive the events that are to be handled by the
*     callback functions implemented by the application.

 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_handle_events(void * arg);

@pre
	Prior to receiving  wi-fi interrupts, the WINC driver should have been successfully initialized by calling the @ref m2m_wifi_init function.
	 
@warning
	Failure to successfully complete this function indicates bus errors and hence a fatal error that will prevent the application from proceeding.

@return		
	The function returns @ref M2M_SUCCESS for successful interrupt handling and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_handle_events(void * arg);
 /**@}*/
/** @defgroup WifiDefaultConnectFn m2m_wifi_default_connect
 *  @ingroup WLANAPI
 *  	Asynchronous wi-fi connection function.  An application calling this function will cause the firmware to correspondingly connect to the last successfully connected
*	AP from the cached connections.
*     A failure to connect will result in a response of @ref M2M_WIFI_RESP_DEFAULT_CONNECT indicating the connection error as defined in the structure @ref tstrM2MDefaultConnResp
*	Possible errors are:  The connection list is empty @ref M2M_DEFAULT_CONN_EMPTY_LIST or a mismatch 
*	for the saved AP name @ref M2M_DEFAULT_CONN_SCAN_MISMATCH.
*	
*      The only difference between this function and @ref m2m_wifi_connect, is the connection parameters. Connection using this function is expected to connect to
*      cached connection parameters.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_default_connect(void);

@pre 
	Prior to connecting, the WINC driver should have been successfully initialized by calling the @ref m2m_wifi_init function.
  
@warning
	This function must be called in station mode only.
	It's important to note that successful completion of a call to m2m_wifi_default_connect() does not guarantee success of the WIFI connection, since success of connection is known when 
	a connection response is received in the wi-fi callback function, and therefore any negative return value from this function indicates only locally-detected errors
	
@see
	m2m_wifi_connect
	
@return		
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*/
NMI_API sint8 m2m_wifi_default_connect(void);
 /**@}*/
/** @defgroup WifiConnectFn m2m_wifi_connect
 *   @ingroup WLANAPI
 *   Asynchronous wi-fi connection function to a specific AP. Prior to a successful connection, the application developers must know the SSID of the AP, the security type,
 *   the authentication information parameters and the channel number to which the connection will be established.
 *  The connection status is known when a response of @ref M2M_WIFI_RESP_CON_STATE_CHANGED is received based on the states defined in @ref tenuM2mConnState,
 *  successful connection is defined by @ref M2M_WIFI_CONNECTED
*   
 *   The only difference between this function and @ref m2m_wifi_default_connect, is the connection parameters. 
 *   Connection using this function is expected to be made to a specific AP and to a specifed channel.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, void *pvAuthInfo, uint16 u16Ch);

@param [in]	pcSsid
				A buffer holding the SSID corresponding to the requested AP.
				
@param [in]	u8SsidLen
				Length of the given SSID (not including the NULL termination).
				A length less than ZERO or greater than the maximum defined SSID @ref M2M_MAX_SSID_LEN will result in a negative error 
				@ref M2M_ERR_FAIL.
				
@param [in]	u8SecType
				Wi-Fi security type security for the network. It can be one of the following types:
				-@ref M2M_WIFI_SEC_OPEN
				-@ref M2M_WIFI_SEC_WEP
				-@ref M2M_WIFI_SEC_WPA_PSK
				-@ref M2M_WIFI_SEC_802_1X
				A value outside these possible values will result in a negative return error @ref M2M_ERR_FAIL.

@param [in]	puniAuthInfo
				Authentication parameters required for completing the connection.
				If the authentication parameters are NULL or are greater than the maximum length of the authentication parameters length as defined by
				@ref M2M_MAX_PSK_LEN a negative error will return @ref M2M_ERR_FAIL(-12) indicating connection failure.

@param [in]	u16Ch
				Wi-Fi channel number as defined in @ref tenuM2mScanCh enumeration.
				Channel number greater than @ref M2M_WIFI_CH_14 returns a negative error @ref M2M_ERR_FAIL(-12).
				Except if the value is M2M_WIFI_CH_ALL(255), since this indicates that the firmware should scan all channels to find the SSID requested to connect to.
				Failure to find the connection match will return a negative error @ref M2M_DEFAULT_CONN_SCAN_MISMATCH.
@pre
  		Prior to a successful connection request, the wi-fi driver must have been successfully intialized through the call of the @ref @m2m_wifi_init function
@see
	tuniM2MWifiAuth
	tstr1xAuthCredentials
	tstrM2mWifiWepParams
	
@warning
	-This function must be called in station mode only.
	-Successful completion of this function does not guarantee success of the WIFI connection, and
	a negative return value indicates only locally-detected errors.
	
@return	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
	
*/
NMI_API sint8 m2m_wifi_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, tuniM2MWifiAuth *puniAuthInfo, uint16 u16Ch);
 /**@}*/
/** @defgroup WifiDisconnectFn m2m_wifi_disconnect
 *   @ingroup WLANAPI
 *   Synchronous wi-fi disconnection function, requesting a Wi-Fi disconnect from the currently connected AP.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_disconnect(void);
	
@pre 
	Disconnection must be made to a successfully connected AP. If the WINC is not in the connected state, a call to this function will hold insignificant.

@warning
	This function must be called in station mode only.
	
@see
	m2m_wifi_connect
	m2m_wifi_default_connect
	
@return		
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_disconnect(void);
 /**@}*/
/** @defgroup GetConnectionInfoFn m2m_wifi_get_connection_info
 *   @ingroup WLANAPI
 *  Asynchronous connection status retrieval function, retrieves the status information of the currently connected AP. The result is passed to the Wi-Fi notification callback
*    through the event @ref M2M_WIFI_RESP_CONN_INFO. Connection information is retrieved from the structure @ref tstrM2MConnInfo.
 */
 /**@{*/
/*!
@fn	\
	sint8 m2m_wifi_get_connection_info(void);
	
@brief
	Request the status information of the currently connected Wi-Fi AP. The result is passed to the Wi-Fi notification callback
	with the event @ref M2M_WIFI_RESP_CONN_INFO.
	
@pre	
	- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at startup. Registering the callback
	is done through passing it to the intialization @ref m2m_wifi_init function.
	- The event @ref M2M_WIFI_RESP_CONN_INFO must be handled in the callback to receive the requested connection info.

@warning
	Calling this function is valid ONLY in the STA CONNECTED state. Otherwise, the WINC SW shall ignore the request silently.
	
@see
	tpfAppWifiCb
	m2m_wifi_init
	M2M_WIFI_RESP_CONN_INFO
	tstrM2MConnInfo

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet shows an example of how wi-fi connection information is retrieved .
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"


	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_CONN_INFO:
			{
				tstrM2MConnInfo		*pstrConnInfo = (tstrM2MConnInfo*)pvMsg;
				
				printf("CONNECTED AP INFO\n");
				printf("SSID     			: %s\n",pstrConnInfo->acSSID);
				printf("SEC TYPE 			: %d\n",pstrConnInfo->u8SecType);
				printf("Signal Strength		: %d\n", pstrConnInfo->s8RSSI); 
				printf("Local IP Address	: %d.%d.%d.%d\n", 
					pstrConnInfo->au8IPAddr[0] , pstrConnInfo->au8IPAddr[1], pstrConnInfo->au8IPAddr[2], pstrConnInfo->au8IPAddr[3]);
			}
			break;

		default:
			break;
		}
	}

	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// connect to the default AP
			m2m_wifi_default_connect();
						
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode
*/
NMI_API sint8 m2m_wifi_get_connection_info(void);
 /**@}*/
 /** @defgroup WifiSetMacAddFn m2m_wifi_set_mac_address
  *   @ingroup WLANAPI
  *  Synchronous dual MAC address assigning to the WILC1500 . It is used for non-production SW.
  */
  /**@{*/
 /*!
 @fn \
	 NMI_API sint8 m2m_wifi_set_mac_address(uint8 au8MacAddress0[6],uint8 au8MacAddress1[6]);
 
 @brief
	 Assign MAC addresses to the WILC device.
 
 @param [in] au8MacAddress0
				 MAC Address to be provisioned to the WILC, address of first station interface.
 @param [in] au8MacAddress1
				 MAC Address to be provisioned to the WILC, address of AP/STA2 interface.
 
 @return	 
	 The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
 NMI_API sint8 m2m_wifi_set_mac_address(uint8 au8MacAddress0[6],uint8 au8MacAddress1[6]);
 /**@}*/
/** @defgroup WifiWpsFn m2m_wifi_wps
 *   @ingroup WLANAPI
 *   Asynchronous WPS triggering function.
 *   This function is called for the WINC to enter the WPS (Wi-Fi Protected Setup) mode. The result is passed to the Wi-Fi notification callback
*	with the event @ref M2M_WIFI_REQ_WPS.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_wps(uint8 u8TriggerType,const char * pcPinNumber);

@param [in]	u8TriggerType
				WPS Trigger method. Could be:
				- [WPS_PIN_TRIGGER](@ref WPS_PIN_TRIGGER)   Push button method
				- [WPS_PBC_TRIGGER](@ref WPS_PBC_TRIGGER)	Pin method
				
@param [in]	pcPinNumber
				PIN number for WPS PIN method. It is not used if the trigger type is WPS_PBC_TRIGGER. It must follow the rules
				stated by the WPS Standard.

@warning
	This function is not allowed in AP or P2P modes.
	
@pre	
	- A Wi-Fi notification callback of type (@ref tpfAppWifiCb MUST be implemented and registered at startup. Registering the callback
	  is done through passing it to the [m2m_wifi_init](@ref m2m_wifi_init).
	- The event [M2M_WIFI_REQ_WPS](@ref M2M_WIFI_REQ_WPS) must be handled in the callback to receive the WPS status.
	- The WINC device MUST be in IDLE or STA mode. If AP or P2P mode is active, the WPS will not be performed. 
	- The [m2m_wifi_handle_events](@ref m2m_wifi_handle_events) MUST be called to receive the responses in the callback.
@see
	tpfAppWifiCb
	m2m_wifi_init
	M2M_WIFI_REQ_WPS
	tenuWPSTrigger
	tstrM2MWPSInfo

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet shows an example of how wifi wps is triggered .
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"

	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_REQ_WPS:
			{
				tstrM2MWPSInfo	*pstrWPS = (tstrM2MWPSInfo*)pvMsg;
				if(pstrWPS->u8AuthType != 0)
				{
					printf("WPS SSID           : %s\n",pstrWPS->au8SSID);
					printf("WPS PSK            : %s\n",pstrWPS->au8PSK);
					printf("WPS SSID Auth Type : %s\n",pstrWPS->u8AuthType == M2M_WIFI_SEC_OPEN ? "OPEN" : "WPA/WPA2");
					printf("WPS Channel        : %d\n",pstrWPS->u8Ch + 1);
					
					// establish Wi-Fi connection
					m2m_wifi_connect((char*)pstrWPS->au8SSID, (uint8)m2m_strlen(pstrWPS->au8SSID),
						pstrWPS->u8AuthType, pstrWPS->au8PSK, pstrWPS->u8Ch);
				}
				else
				{
					printf("(ERR) WPS Is not enabled OR Timedout\n");
				}
			}
			break;
			
		default:
			break;
		}
	}

	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Trigger WPS in Push button mode.
			m2m_wifi_wps(WPS_PBC_TRIGGER, NULL);
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode
*/
NMI_API sint8 m2m_wifi_wps(uint8 u8TriggerType,const char  *pcPinNumber);
 /**@}*/
/** @defgroup WifiWpsDisableFn m2m_wifi_wps_disable
 *   @ingroup WLANAPI
 * Disable the NMC1500 WPS operation.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_wps_disable(void);

@brief
	Disable the WINC WPS operation.

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_wps_disable(void);
 /**@}*/
/** @defgroup WifiP2PFn m2m_wifi_p2p
 *   @ingroup WLANAPI
 *    Asynchronous  Wi-Fi direct (P2P) enabling mode function.
	The WINC supports P2P in device listening mode ONLY (intent is ZERO).
	The WINC P2P implementation does not support P2P GO (Group Owner) mode.
	Active P2P devices (e.g. phones) could find the WINC in the search list. When a device is connected to WINC, a Wi-Fi notification event
	@ref M2M_WIFI_RESP_CON_STATE_CHANGED is triggered. Refer to the code examples for a more illustrative example.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_p2p(uint8 u8Channel);

@param [in]	u8Channel
				P2P Listen RF channel. According to the P2P standard It must hold only one of the following values 1, 6 or 11.

@pre
	- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at initialization. Registering the callback
	  is done through passing it to the @ref m2m_wifi_init.
	- The events @ref M2M_WIFI_RESP_CON_STATE_CHANGED must be handled in the callback.
	- The @ref m2m_wifi_handle_events MUST be called to receive the responses in the callback.

@warning
	This function is not allowed in AP or STA modes.

@see
	tpfAppWifiCb
	m2m_wifi_init
	M2M_WIFI_RESP_CON_STATE_CHANGED
	tstrM2mWifiStateChanged

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet shown an example of how the p2p mode operates.
@code
	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
			{
				tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged*)pvMsg;
				M2M_INFO("Wifi State :: %s :: ErrCode %d\n", pstrWifiState->u8CurrState? "CONNECTED":"DISCONNECTED",pstrWifiState->u8ErrCode);
				
				// Do something
			}
			break;
			
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Trigger P2P
			m2m_wifi_p2p(1);
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode

*/
NMI_API sint8 m2m_wifi_p2p(uint8 u8Channel);
 /**@}*/
/** @defgroup WifiP2PDisconnectFn m2m_wifi_p2p_disconnect
 *   @ingroup WLANAPI
 * Disable the NMC1500 device Wi-Fi direct mode (P2P). 
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_p2p_disconnect(void);
@pre 
	The p2p mode must have be enabled and active before a disconnect can be called.
@see
         m2m_wifi_p2p
@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_p2p_disconnect(void);
 /**@}*/
/** @defgroup WifiEnableApFn m2m_wifi_enable_ap
 *   @ingroup WLANAPI
 * 	Asychronous wi-fi hotspot enabling function. 
 *    The WINC supports AP mode operation with the following limitations:
	- Only 1 STA could be associated at a time.
	- Open and WEP are the only supported security types
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_enable_ap(CONST tstrM2MAPConfig* pstrM2MAPConfig);

@param [in]	pstrM2MAPConfig
				A structure holding the AP configurations.

@warning
	This function is not allowed in P2P or STA modes.
	
@pre
	- A Wi-Fi notification callback of type @ref tpfAppWifiCb  MUST be implemented and registered at intalization. Registering the callback
	  is done through passing it to the [m2m_wifi_init](@ref m2m_wifi_init).
	- The @ref m2m_wifi_handle_events MUST be called to receive the responses in the callback.

@see
	tpfAppWifiCb
	tenuM2mSecType
	m2m_wifi_init
	tstrM2mWifiStateChanged
	tstrM2MAPConfig

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet demonstrates how the AP mode is enabled after the driver is initialized in the application's main function
@code
	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		switch(u8WiFiEvent)
		{
			
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			tstrM2MAPConfig		apConfig;
			
			strcpy(apConfig.au8SSID, "WINC_SSID");
			apConfig.u8ListenChannel 	= 1;
			apConfig.u8SecType			= M2M_WIFI_SEC_OPEN;
			apConfig.u8SsidHide			= 0;
						
			// Trigger AP
			m2m_wifi_enable_ap(&apConfig);
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}

@endcode

*/
NMI_API sint8 m2m_wifi_enable_ap(CONST tstrM2MAPConfig* pstrM2MAPConfig);
 /**@}*/
/** @defgroup WifiDisableApFn m2m_wifi_disable_ap
 *   @ingroup WLANAPI
 *    Synchronous wi-fi hotspot disabling function. Must be called only when the AP is enabled through the @ref m2m_wifi_enable_ap
 *   function. Otherwise the call to this function will not be useful.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_disable_ap(void);
@see
         m2m_wifi_enable_ap
@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_disable_ap(void);
 /**@}*/
/** @defgroup GetConnectionInfoFn m2m_wifi_ap_get_assoc_info
 *   @ingroup WLANAPI
 *  Asynchronous associated status retrieval function in the AP mode, retrieves the status information of the currently connected connected stations to the WILC1000 AP. The result is passed to the Wi-Fi notification callback
*    through the event @ref M2M_WIFI_RESP_AP_ASSOC_INFO. Connection information is retrieved from the structure @ref tstrM2MAPAssocInfo.
 */
 /**@{*/
/*!
@fn	\
	sint8 m2m_wifi_ap_get_assoc_info(void);
	
@brief
	Request the status information of the currently connected stations to the WILC1000 AP. The result is passed to the Wi-Fi notification callback
	with the event @ref M2M_WIFI_RESP_AP_ASSOC_INFO.
	
@pre	
	- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at startup. Registering the callback
	is done through passing it to the intialization @ref m2m_wifi_init function.
	- The event @ref M2M_WIFI_RESP_AP_ASSOC_INFO must be handled in the callback to receive the requested connection info.

@warning
	Calling this function is valid ONLY in the AP mode. Otherwise, the WINC SW shall ignore the request silently.
	
@see
	tpfAppWifiCb
	m2m_wifi_init
	M2M_WIFI_RESP_AP_ASSOC_INFO
	tstrM2MAPAssocInfo

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet shows an example of how wi-fi connection information is retrieved .
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"


	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_AP_ASSOC_INFO:
		{
			tstrM2MAPAssocInfo* pstrAssocInfo =(tstrM2MAPAssocInfo*)pvMsg;
			printk("AP Assoc list received[%d]\n",pstrAssocInfo->u8NoConnSta);
			for(i=0;i<pstrAssocInfo->u8NoConnSta;i++)
			{
				printk("STA %x:%x:%x:%x:%x:%x connected RSSI %d\n",pstrAssocInfo->astrM2MAssocEntryInfo[i].BSSID[0],
					pstrAssocInfo->astrM2MAssocEntryInfo[i].BSSID[1],pstrAssocInfo->astrM2MAssocEntryInfo[i].BSSID[2],
					pstrAssocInfo->astrM2MAssocEntryInfo[i].BSSID[3],pstrAssocInfo->astrM2MAssocEntryInfo[i].BSSID[4],
					pstrAssocInfo->astrM2MAssocEntryInfo[i].BSSID[5],pstrAssocInfo->astrM2MAssocEntryInfo[i].s8RSSI);
			}
							
		  }		
			break;

	case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{			
			pstrConnectState = (tstrM2mWifiStateChanged*)pvMsg;
			
			if(pstrConnectState->u8CurrState)
			{
				m2m_wifi_ap_get_assoc_info();
			}
			break;
		default:
			break;
		}
	}

	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			tstrM2MAPConfig strM2MAPConfig;
			strcpy(strM2MAPConfig.au8PSK,"1234567890");			
			strcpy(strM2MAPConfig.au8SSID,"WILC1000_AP");
			strM2MAPConfig.u8ListenChannel = M2M_WIFI_CH_11;
			strM2MAPConfig.u8SecType = M2M_WIFI_SEC_WPA_PSK;//M2M_WIFI_SEC_OPEN;//;//M2M_WIFI_SEC_WEP;
			strM2MAPConfig.u8SsidHide = 0;
		
			m2m_wifi_enable_ap(&strM2MAPConfig);
						
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode
*/
NMI_API sint8 m2m_wifi_ap_get_assoc_info(void);
  /**@}*/
/** @defgroup WifiDisableApFn m2m_wifi_ap_add_black_list
 *   @ingroup WLANAPI
 *    Synchronous wi-fi hotspot add/remove station from the black List.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_ap_add_black_list(uint8 bAddNewEntry,uint8* mac_addr);
@param [in]	bAddNewEntry
				Boolean value if 1 then the given mac address would be added to the black list
				if zero the given mac address would be removed from the black list
			mac_addr
				MAC address of the station to be added/removed from the black list
				Note to remove all stations from the black list use all FF's in the mac address and set
				bAddNewEntry to zero
@warning
	The firmware will be maintaining a list of 16 MAC addresses , Requests over this number will 
	be discarded without a notice.

@see
         m2m_wifi_enable_ap
@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_ap_add_black_list(uint8 bAddNewEntry,uint8* mac_addr);
 /**@}*/
/** @defgroup WifiSetScanOptionFn m2m_wifi_set_scan_options
 *   @ingroup WLANAPI
 *   Synchronous wi-fi scan settings function. This function sets the time configuration parameters for the scan operation.
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_scan_options(uint8 u8NumOfSlot,uint8 u8SlotTime);

@param [in]	u8NumOfSlot;
	The minimum number of slots is 2 for every channel, 
	For every slot the soc will send Probe Req on air, and wait/listen for PROBE RESP/BEACONS for the u8slotTime in ms
	
@param [in]	 u8SlotTime;
		The time in ms that the Soc will wait on every channel listening for the frames on air
		when that time increases the number of APs will increase in the scan results
		Minimum time is 10 ms and the maximum is 250 ms 
@see
	tenuM2mScanCh
	m2m_wifi_request_scan
	
@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_scan_options(uint8 u8NumOfSlot,uint8 u8SlotTime);
 /**@}*/
/** @defgroup WifiSetScanRegionFn m2m_wifi_set_scan_region
 *   @ingroup WLANAPI
 *  Synchronous wi-fi scan region setting function.
 *   This function sets the scan region, which will affect the range of possible scan channels. 
 *   For 2.5GHz supported in the current release, the requested scanregion can't exceed the maximum number of channels (14).
 */
 /**@{*/
/*!
@fn	\
	sint8 m2m_wifi_set_scan_region(uint8 ScanRegion)

@param [in]	ScanRegion;
		ASIA = 14
		NORTH_AMERICA = 11
@see
	tenuM2mScanCh
	m2m_wifi_request_scan
	
@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*/
NMI_API sint8 m2m_wifi_set_scan_region(uint8  ScanRegion);
  /**@}*/
 /** @defgroup WifiSetScanRegionFn m2m_wifi_set_scan_list
  *   @ingroup WLANAPI
  *  Synchronous wi-fi scan region setting function.
  *   This function sets the scan list, which will affect the range of possible scan channels. 
  *   For 2.5GHz supported in the current release, the requested scanregion can't exceed the maximum number of channels (14).
  */
  /**@{*/
 /*!
 @fn \
	 sint8 m2m_wifi_set_scan_list(tstrM2MScanList* pstrScanList)
 
 @param [in] ScanRegion;
		 pstrScanList 
		 
 @see
 	tstrM2MScanList
	 m2m_wifi_request_scan
	 
 @return
	 The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 
 */
 NMI_API sint8 m2m_wifi_set_scan_list(tstrM2MScanList* pstrScanList);

 /**@}*/
/** @defgroup WifiRequestScanFn m2m_wifi_request_scan
 *   @ingroup WLANAPI
 *    Asynchronous wi-fi scan request on the given channel. The scan status is delivered in the wi-fi event callback and then the application
 *    is to read the scan results sequentially. 
 *    The number of  APs found (N) is returned in event @ref M2M_WIFI_RESP_SCAN_DONE with the number of found
 *     APs.
 *	The application could read the list of APs by calling the function @ref m2m_wifi_req_scan_result N times.
 */ 
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_request_scan(uint8 ch);

@param [in]	ch
		      RF Channel ID for SCAN operation. It should be set according to tenuM2mScanCh. 
		      With a value of M2M_WIFI_CH_ALL(255)), means to scan all channels.

@warning
	This function is not allowed in P2P or AP modes. It works only for STA mode (connected or disconnected).
				
@pre
	- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at intialization. Registering the callback
	  is done through passing it to the @ref m2m_wifi_init.
	- The events @ref M2M_WIFI_RESP_SCAN_DONE and @ref M2M_WIFI_RESP_SCAN_RESULT.
	  must be handled in the callback.
	- The @ref m2m_wifi_handle_events function MUST be called to receive the responses in the callback.

@see
	M2M_WIFI_RESP_SCAN_DONE
	M2M_WIFI_RESP_SCAN_RESULT
	tpfAppWifiCb
	tstrM2mWifiscanResult
	tenuM2mScanCh
	m2m_wifi_init
	m2m_wifi_handle_events
	m2m_wifi_req_scan_result

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
  the events received in response.
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		static uint8	u8ScanResultIdx = 0;
		
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_SCAN_DONE:
			{
				tstrM2mScanDone	*pstrInfo = (tstrM2mScanDone*)pvMsg;
				
				printf("Num of AP found %d\n",pstrInfo->u8NumofCh);
				if(pstrInfo->s8ScanState == M2M_SUCCESS)
				{
					u8ScanResultIdx = 0;
					if(pstrInfo->u8NumofCh >= 1)
					{
						m2m_wifi_req_scan_result(u8ScanResultIdx);
						u8ScanResultIdx ++;
					}
					else
					{
						printf("No AP Found Rescan\n");
						m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
					}
				}
				else
				{
					printf("(ERR) Scan fail with error <%d>\n",pstrInfo->s8ScanState);
				}
			}
			break;
		
		case M2M_WIFI_RESP_SCAN_RESULT:
			{
				tstrM2mWifiscanResult		*pstrScanResult =(tstrM2mWifiscanResult*)pvMsg;
				uint8						u8NumFoundAPs = m2m_wifi_get_num_ap_found();
				
				printf(">>%02d RI %d SEC %s CH %02d BSSID %02X:%02X:%02X:%02X:%02X:%02X SSID %s\n",
					pstrScanResult->u8index,pstrScanResult->s8rssi,
					pstrScanResult->u8AuthType,
					pstrScanResult->u8ch,
					pstrScanResult->au8BSSID[0], pstrScanResult->au8BSSID[1], pstrScanResult->au8BSSID[2],
					pstrScanResult->au8BSSID[3], pstrScanResult->au8BSSID[4], pstrScanResult->au8BSSID[5],
					pstrScanResult->au8SSID);
				
				if(u8ScanResultIdx < u8NumFoundAPs)
				{
					// Read the next scan result
					m2m_wifi_req_scan_result(index);
					u8ScanResultIdx ++;
				}
			}
			break;
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Scan all channels
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode
*/
NMI_API sint8 m2m_wifi_request_scan(uint8 ch);
 /**@}*/
/** @defgroup WifiRequestScanFn m2m_wifi_request_scan_ssid
 *   @ingroup WLANAPI
 *    Asynchronous wi-fi scan request on the given channel. The scan status is delivered in the wi-fi event callback and then the application
 *    is to read the scan results sequentially. 
 *    The number of  APs found (N) is returned in event @ref M2M_WIFI_RESP_SCAN_DONE with the number of found
 *     APs.
 *	The application could read the list of APs by calling the function @ref m2m_wifi_req_scan_result N times.
 * 
 */
 /**@{*/
/*!
@fn	\
	NMI_API sint8 m2m_wifi_request_scan_ssid(uint8 ch,char* pcssid);

@param [in]	ch
		      RF Channel ID for SCAN operation. It should be set according to tenuM2mScanCh. 
		      With a value of M2M_WIFI_CH_ALL(255)), means to scan all channels.

		      pcssid
		      Pointer to a null terminated string holds the favorite SSID to include it in the probe requests  

@warning
	This function is not allowed in P2P or AP modes. It works only for STA mode (connected or disconnected).
				
@pre
	- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at intialization. Registering the callback
	  is done through passing it to the @ref m2m_wifi_init.
	- The events @ref M2M_WIFI_RESP_SCAN_DONE and @ref M2M_WIFI_RESP_SCAN_RESULT.
	  must be handled in the callback.
	- The @ref m2m_wifi_handle_events function MUST be called to receive the responses in the callback.

@see
	M2M_WIFI_RESP_SCAN_DONE
	M2M_WIFI_RESP_SCAN_RESULT
	tpfAppWifiCb
	tstrM2mWifiscanResult
	tenuM2mScanCh
	m2m_wifi_init
	m2m_wifi_handle_events
	m2m_wifi_req_scan_result

@return
	The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
  the events received in response.
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		static uint8	u8ScanResultIdx = 0;
		
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_SCAN_DONE:
			{
				tstrM2mScanDone	*pstrInfo = (tstrM2mScanDone*)pvMsg;
				
				printf("Num of AP found %d\n",pstrInfo->u8NumofCh);
				if(pstrInfo->s8ScanState == M2M_SUCCESS)
				{
					u8ScanResultIdx = 0;
					if(pstrInfo->u8NumofCh >= 1)
					{
						m2m_wifi_req_scan_result(u8ScanResultIdx);
						u8ScanResultIdx ++;
					}
					else
					{
						printf("No AP Found Rescan\n");
						m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
					}
				}
				else
				{
					printf("(ERR) Scan fail with error <%d>\n",pstrInfo->s8ScanState);
				}
			}
			break;
		
		case M2M_WIFI_RESP_SCAN_RESULT:
			{
				tstrM2mWifiscanResult		*pstrScanResult =(tstrM2mWifiscanResult*)pvMsg;
				uint8						u8NumFoundAPs = m2m_wifi_get_num_ap_found();
				
				printf(">>%02d RI %d SEC %s CH %02d BSSID %02X:%02X:%02X:%02X:%02X:%02X SSID %s\n",
					pstrScanResult->u8index,pstrScanResult->s8rssi,
					pstrScanResult->u8AuthType,
					pstrScanResult->u8ch,
					pstrScanResult->au8BSSID[0], pstrScanResult->au8BSSID[1], pstrScanResult->au8BSSID[2],
					pstrScanResult->au8BSSID[3], pstrScanResult->au8BSSID[4], pstrScanResult->au8BSSID[5],
					pstrScanResult->au8SSID);
				
				if(u8ScanResultIdx < u8NumFoundAPs)
				{
					// Read the next scan result
					m2m_wifi_req_scan_result(index);
					u8ScanResultIdx ++;
				}
			}
			break;
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Scan all channels
			
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL,"Favorite_AP_Name");
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode
*/
NMI_API sint8 m2m_wifi_request_scan_ssid(uint8 ch,char* pcssid);
/**@}*/
/** @defgroup WifiGetNumAPFoundFn m2m_wifi_get_num_ap_found
 *   @ingroup WLANAPI
 *  Synchronous function to retrieve the number of AP's found in the last scan request, 
 *  The function read the number of AP's from global variable which updated in the 
 *   wi-fi callback fucntion through the @ref M2M_WIFI_RESP_SCAN_DONE event.
 *   Function used only in STA mode only.
 */
 /**@{*/
/*!
@fn          NMI_API uint8 m2m_wifi_get_num_ap_found(void);

@see         m2m_wifi_request_scan 
		   M2M_WIFI_RESP_SCAN_DONE
		   M2M_WIFI_RESP_SCAN_RESULT         
@pre         m2m_wifi_request_scan need to be called first	
		- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at intialization. Registering the callback
		   is done through passing it to the @ref m2m_wifi_init.
		- The event @ref M2M_WIFI_RESP_SCAN_DONE must be handled in the callback to receive the requested connection information. 
@warning   This function must be called only in the wi-fi callback function when the events @ref M2M_WIFI_RESP_SCAN_DONE or @ref M2M_WIFI_RESP_SCAN_RESULT
		   are received.
		   Calling this function in any other place will result in undefined/outdated numbers.
@return      Return the number of AP's found in the last Scan Request.
		  
\section Example
  The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
  the events received in response.
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		static uint8	u8ScanResultIdx = 0;
		
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_SCAN_DONE:
			{
				tstrM2mScanDone	*pstrInfo = (tstrM2mScanDone*)pvMsg;
				
				printf("Num of AP found %d\n",pstrInfo->u8NumofCh);
				if(pstrInfo->s8ScanState == M2M_SUCCESS)
				{
					u8ScanResultIdx = 0;
					if(pstrInfo->u8NumofCh >= 1)
					{
						m2m_wifi_req_scan_result(u8ScanResultIdx);
						u8ScanResultIdx ++;
					}
					else
					{
						printf("No AP Found Rescan\n");
						m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
					}
				}
				else
				{
					printf("(ERR) Scan fail with error <%d>\n",pstrInfo->s8ScanState);
				}
			}
			break;
		
		case M2M_WIFI_RESP_SCAN_RESULT:
			{
				tstrM2mWifiscanResult		*pstrScanResult =(tstrM2mWifiscanResult*)pvMsg;
				uint8						u8NumFoundAPs = m2m_wifi_get_num_ap_found();
				
				printf(">>%02d RI %d SEC %s CH %02d BSSID %02X:%02X:%02X:%02X:%02X:%02X SSID %s\n",
					pstrScanResult->u8index,pstrScanResult->s8rssi,
					pstrScanResult->u8AuthType,
					pstrScanResult->u8ch,
					pstrScanResult->au8BSSID[0], pstrScanResult->au8BSSID[1], pstrScanResult->au8BSSID[2],
					pstrScanResult->au8BSSID[3], pstrScanResult->au8BSSID[4], pstrScanResult->au8BSSID[5],
					pstrScanResult->au8SSID);
				
				if(u8ScanResultIdx < u8NumFoundAPs)
				{
					// Read the next scan result
					m2m_wifi_req_scan_result(index);
					u8ScanResultIdx ++;
				}
			}
			break;
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Scan all channels
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode			 
*/
NMI_API uint8 m2m_wifi_get_num_ap_found(void);
/**@}*/
/** @defgroup WifiReqScanResult m2m_wifi_req_scan_result
*   @ingroup WLANAPI
*   Synchronous call to read the AP information from the SCAN Result list with the given index.
*   This function is expected to be called when the response events M2M_WIFI_RESP_SCAN_RESULT or
*   M2M_WIFI_RESP_SCAN_DONE are received in the wi-fi callback function.
*   The response information received can be obtained through the casting to the @ref tstrM2mWifiscanResult structure 	
 */
 /**@{*/
/*!
@fn          NMI_API sint8 m2m_wifi_req_scan_result(uint8 index);
@param [in]  index 
		      Index for the requested result, the index range start from 0 till number of AP's found

@see          tstrM2mWifiscanResult
		   m2m_wifi_get_num_ap_found
		   m2m_wifi_request_scan             
		   
@pre         @ref m2m_wifi_request_scan needs to be called first, then m2m_wifi_get_num_ap_found 
		   to get the number of AP's found	
			- A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at startup. Registering the callback
			is done through passing it to the @ref m2m_wifi_init function.
			- The event @ref M2M_WIFI_RESP_SCAN_RESULT must be handled in the callback to receive the requested connection information.
@warning     Function used  in STA mode only. the scan results are updated only if the scan request is called.
		     Calling this function only without a scan request will lead to firmware errors. 
		     Refrain from introducing a large delay  between the scan request and the scan result request, to prevent
		     an errors occuring.
			 
@return      The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
\section Example
  The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
  the events received in response.
@code
	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		static uint8	u8ScanResultIdx = 0;
		
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_SCAN_DONE:
			{
				tstrM2mScanDone	*pstrInfo = (tstrM2mScanDone*)pvMsg;
				
				printf("Num of AP found %d\n",pstrInfo->u8NumofCh);
				if(pstrInfo->s8ScanState == M2M_SUCCESS)
				{
					u8ScanResultIdx = 0;
					if(pstrInfo->u8NumofCh >= 1)
					{
						m2m_wifi_req_scan_result(u8ScanResultIdx);
						u8ScanResultIdx ++;
					}
					else
					{
						printf("No AP Found Rescan\n");
						m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
					}
				}
				else
				{
					printf("(ERR) Scan fail with error <%d>\n",pstrInfo->s8ScanState);
				}
			}
			break;
		
		case M2M_WIFI_RESP_SCAN_RESULT:
			{
				tstrM2mWifiscanResult		*pstrScanResult =(tstrM2mWifiscanResult*)pvMsg;
				uint8						u8NumFoundAPs = m2m_wifi_get_num_ap_found();
				
				printf(">>%02d RI %d SEC %s CH %02d BSSID %02X:%02X:%02X:%02X:%02X:%02X SSID %s\n",
					pstrScanResult->u8index,pstrScanResult->s8rssi,
					pstrScanResult->u8AuthType,
					pstrScanResult->u8ch,
					pstrScanResult->au8BSSID[0], pstrScanResult->au8BSSID[1], pstrScanResult->au8BSSID[2],
					pstrScanResult->au8BSSID[3], pstrScanResult->au8BSSID[4], pstrScanResult->au8BSSID[5],
					pstrScanResult->au8SSID);
				
				if(u8ScanResultIdx < u8NumFoundAPs)
				{
					// Read the next scan result
					m2m_wifi_req_scan_result(index);
					u8ScanResultIdx ++;
				}
			}
			break;
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Scan all channels
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}
	
@endcode 
*/
NMI_API sint8 m2m_wifi_req_scan_result(uint8 index);
/**@}*/
/** @defgroup WifiReqCurrentRssiFn m2m_wifi_req_curr_rssi
 *   @ingroup WLANAPI
 *   Asynchronous request for the current RSSI of the connected AP.
 *   The response received in through the @ref M2M_WIFI_RESP_CURRENT_RSSI event.
 */
 /**@{*/
/*!
@fn          NMI_API sint8 m2m_wifi_req_curr_rssi(void);
@pre	   - A Wi-Fi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered before intialization. Registering the callback
			is done through passing it to the [m2m_wifi_init](@ref m2m_wifi_init) through the @ref tstrWifiInitParam intialization structure.
		   - The event @ref M2M_WIFI_RESP_CURRENT_RSSI must be handled in the callback to receive the requested connection information.       
@return      The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.	
\section Example
  The code snippet demonstrates how the rssi request is called in the application's main function and the handling of event received in the callback.
@code

	#include "m2m_wifi.h"
	#include "m2m_types.h"
	
	void wifi_event_cb(uint8 u8WiFiEvent, void * pvMsg)
	{
		static uint8	u8ScanResultIdx = 0;
		
		switch(u8WiFiEvent)
		{
		case M2M_WIFI_RESP_CURRENT_RSSI:
			{
				sint8	*rssi = (sint8*)pvMsg;
				M2M_INFO("ch rssi %d\n",*rssi);
			}
			break;
		default:
			break;
		}
	}
	
	int main()
	{
		tstrWifiInitParam 	param;
		
		param.pfAppWifiCb	= wifi_event_cb;
		if(!m2m_wifi_init(&param))
		{
			// Scan all channels
			m2m_wifi_req_curr_rssi();
			
			while(1)
			{
				m2m_wifi_handle_events(NULL);
			}
		}
	}

@endcode	

*/
NMI_API sint8 m2m_wifi_req_curr_rssi(void);
/**@}*/
/** @defgroup WifiGetOtpMacAddFn m2m_wifi_get_otp_mac_address
*   @ingroup WLANAPI
*   Request the MAC address stored on the OTP (one time programmable) memory of the device.
*   The function is blocking until the response is received.
*/
 /**@{*/
/*!
@fn          NMI_API sint8 m2m_wifi_get_otp_mac_address(uint8 *pu8MacAddr, uint8 * pu8IsValid);
@brief   	
@param [out] pu8MacAddr
			 Output MAC address buffer of 6 bytes size. Valid only if *pu8Valid=1.
@param [out] pu8IsValid
		     A output boolean value to indicate the validity of pu8MacAddr in OTP. 
		     Output zero if the OTP memory is not programmed, non-zero otherwise.
@pre         m2m_wifi_init required to call any WIFI/socket function
@see         m2m_wifi_get_mac_address             

@return      The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

*/
NMI_API sint8 m2m_wifi_get_otp_mac_address(uint8 *pu8MacAddr, uint8 * pu8IsValid);
/**@}*/
/** @defgroup WifiGetMacAddFn m2m_wifi_get_mac_address
*   @ingroup WLANAPI
*   Function to retrieve the current MAC address. The function is blocking until the response is received.
*/
 /**@{*/

/*!
@fn          NMI_API sint8 m2m_wifi_get_mac_address(uint8 *pu8MacAddr0, uint8 *pu8MacAddr1)	
@param [out] pu8MacAddr0
			 Output MAC address buffer of 6 bytes size.	
			 pu8MacAddr1
			 Output MAC address buffer of 6 bytes size.	
@pre         m2m_wifi_init required to call any WIFI/socket function
@see         m2m_wifi_get_otp_mac_address             
@return    The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*/
NMI_API sint8 m2m_wifi_get_mac_address(uint8 *pu8MacAddr0, uint8 *pu8MacAddr1);
/**@}*/
/** @defgroup SetSleepModeFn m2m_wifi_set_sleep_mode
 *   @ingroup WLANAPI
 *  Synchronous power-save mode setting function for the NMC1500. 
 */
 /**@{*/
/*!
@fn			NMI_API sint8 m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn);
@param [in]	PsTyp
			Desired power saving mode. Supported types are defined in @ref tenuPowerSaveModes.
@param [in]	BcastEn
			Broadcast reception enable flag. 
			If it is 1, the WINC1500 must be awake each DTIM beacon for receiving broadcast traffic.
			If it is 0, the WINC1500 will not wakeup at the DTIM beacon, but its wakeup depends only 
			on the the configured Listen Interval. 

@warning    The function called once after initialization.

@see	   tenuPowerSaveModes
		   m2m_wifi_get_sleep_mode

@return    The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*/
NMI_API sint8 m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn);
/**@}*/
/** @defgroup GetSleepModeFn m2m_wifi_get_sleep_mode
 *   @ingroup WLANAPI
 *  Synchronous power save mode retrieval function.
 */
 /**@{*/
/*!
@fn		    NMI_API uint8 m2m_wifi_get_sleep_mode(void);
@see	    tenuPowerSaveModes 
		    m2m_wifi_set_sleep_mode
@return	    The current operating power saving mode.

*/
NMI_API uint8 m2m_wifi_get_sleep_mode(void);
/**@}*/
/** @defgroup WifiSetDeviceNameFn m2m_wifi_set_device_name
 *   @ingroup WLANAPI
 *  Set the WINC1500 device name which is to be used as a P2P device name.
 */
 /**@{*/
/*!
@fn			NMI_API sint8 m2m_wifi_set_device_name(uint8 *pu8DeviceName, uint8 u8DeviceNameLength);		
@param [in]	pu8DeviceName
			Buffer holding the device name.
@param [in]	u8DeviceNameLength
			Length of the device name. Should not exceed the maximum device name's length M2M_DEVICE_NAME_MAX.
@warning		The function called once after initialization. 
@return		The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_device_name(uint8 *pu8DeviceName, uint8 u8DeviceNameLength);
/**@}*/
/** @defgroup WifiSetLsnIntFn m2m_wifi_set_lsn_int
 *   @ingroup WLANAPI
*	Synchronous function for setting the wi-fi listen interval for power save operation. It is represented in units
*	of AP Beacon periods. Function 
 */
 /**@{*/
/*!
@fn			NMI_API sint8 m2m_wifi_set_lsn_int(tstrM2mLsnInt * pstrM2mLsnInt);

@param [in]	pstrM2mLsnInt
			Structure holding the listen interval configurations.
@pre		Function m2m_wifi_set_sleep_mode shall be called first
@warning     	The function should be called once after initialization. 
@see		tstrM2mLsnInt
                     m2m_wifi_set_sleep_mode
@return		The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*/
NMI_API sint8 m2m_wifi_set_lsn_int(tstrM2mLsnInt *pstrM2mLsnInt);
/**@}*/
/** @defgroup WifiEnableMonitorModeFn m2m_wifi_enable_monitoring_mode
 *   @ingroup WLANAPI
 *     Asynchronous  wi-fi monitoring enable mode (Promiscuous mode) function. This function enables the monitoring mode, which starts transmission
 *    of the packets based on the filter information passed in as a parameter. All packets that meet the filtering criteria are passed to the application layer, to be handled by the assigned monitoring callback function.
 *    The monitoring callback function must be implemented before starting the monitoring mode, in-order to handle the packets received.
 *    Registering of the implemented callback function is through the callback pointer @ref tpfAppMonCb in the @ref tstrWifiInitParam structure.
 *    passed to @ref m2m_wifi_init function at initialization.
 *    
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_enable_monitoring_mode(tstrM2MWifiMonitorModeCtrl *);
 * @param [in]     pstrMtrCtrl
 *                 	Pointer to @ref tstrM2MWifiMonitorModeCtrl structure holding the Filtering parameters.
 *					A buffer to receive data should be set during initializing m2m_wifi_init(), or 
 *					m2m_wifi_set_receive_buffer()
 * @see             tstrM2MWifiMonitorModeCtrl
 * 			   tstrM2MWifiRxPacketInfo
 *			   tstrWifiInitParam
 *			   m2m_wifi_disable_monitoring_mode
 *			   m2m_wifi_init
 *			   m2m_wifi_set_receive_buffer
 * @return       The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*\section Example
*  The example demonstrates the main function where-by the monitoring enable function is called after the initialization of the driver and the packets are
*   handled in the callback function.
* @code
			
			#include "m2m_wifi.h"
			#include "m2m_types.h"

			//Declare receive buffer 
			uint8 gmgmt[1600];
	
			//Callback functions
			void wifi_cb(uint8 u8WiFiEvent, void * pvMsg)
			{
				; 
			}
			void wifi_monitoring_cb(tstrM2MWifiRxPacketInfo *pstrWifiRxPacket, uint8 *pu8Payload, uint16 u16PayloadSize)
			{
				if((NULL != pstrWifiRxPacket) && (0 != u16PayloadSize)) {
					if(MANAGEMENT == pstrWifiRxPacket->u8FrameType) {
						M2M_INFO("***# MGMT PACKET #***\n");
					} else if(DATA_BASICTYPE == pstrWifiRxPacket->u8FrameType) {
						M2M_INFO("***# DATA PACKET #***\n");
					} else if(CONTROL == pstrWifiRxPacket->u8FrameType) {
						M2M_INFO("***# CONTROL PACKET #***\n");
					}
				}
			}
			
			int main()
			{
				//Register wifi_monitoring_cb 
				tstrWifiInitParam param;
				param.pfAppWifiCb = wifi_cb;
				param.pfAppMonCb  = wifi_monitoring_cb;
				param->strEthInitParam.au8ethRcvBuf = gmgmt;
				param->strEthInitParam.u16ethRcvBufSize = sizeof(gmgmt);
				
				nm_bsp_init();
				
				if(!m2m_wifi_init(&param)) {
					//Enable Monitor Mode with filter to receive all data frames on channel 1
					tstrM2MWifiMonitorModeCtrl	strMonitorCtrl = {0};
					strMonitorCtrl.u8ChannelID		= 1;
					strMonitorCtrl.u8FrameType		= DATA_BASICTYPE;
					strMonitorCtrl.u8FrameSubtype	= M2M_WIFI_FRAME_SUB_TYPE_ANY; //Receive any subtype of data frame
					m2m_wifi_enable_monitoring_mode(&strMonitorCtrl);
					
					while(1) {
						m2m_wifi_handle_events(NULL);
					}
				}
				return 0;
			}
 * @endcode
 */
NMI_API sint8 m2m_wifi_enable_monitoring_mode(tstrM2MWifiMonitorModeCtrl *pstrMtrCtrl);
/**@}*/
/** @defgroup WifiDisableMonitorModeFn m2m_wifi_disable_monitoring_mode
 *   @ingroup WLANAPI
 *   Synchronous function to disable Wi-Fi monitoring mode (Promiscuous mode). Expected to be called, if the enable monitoring mode is set, but if it was called without enabling
 *   no
 *   negative impact will reside.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_disable_monitoring_mode(void);
 * @see           m2m_wifi_enable_monitoring_mode               
 * @return      The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
NMI_API sint8 m2m_wifi_disable_monitoring_mode(void);

/**@}*/
/** @defgroup WifiDisableMonitorModeFn m2m_wifi_change_monitoring_channel
 *   @ingroup WLANAPI
 *   Synchronous function to change the Wi-Fi monitoring channel(Promiscuous mode). Expected to be called, if the enable monitoring mode is set, but if it was called without enabling
 *   no
 *   negative impact will reside.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_change_monitoring_channel(uint8 u8ChannelID);
  * @param [in]     u8ChannelID	New monitoring channel
 * @see           m2m_wifi_enable_monitoring_mode
 * @return      The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
NMI_API sint8 m2m_wifi_change_monitoring_channel(uint8 u8ChannelID);

 /**@}*/
 /** @defgroup SendWlanPktFn m2m_wifi_send_wlan_pkt
 *   @ingroup WLANAPI
 *   Synchronous function to transmit a WIFI RAW packet while the implementation of this packet is left to the application developer.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_send_wlan_pkt(uint8 *, uint16, uint16);
 
 * @param [in]     pu8WlanPacket
 *                 	     Pointer to a buffer holding the whole WIFI frame.
 * @param [in]     u16WlanHeaderLength
 * 			      The size of the WIFI packet header ONLY.
 * @param [in]     u16WlanPktSize
 *			     The size of the whole bytes in packet. 
 * @see             m2m_wifi_enable_monitoring_mode
 			   m2m_wifi_disable_monitoring_mode
 * @pre              Enable Monitoring mode first using @ref m2m_wifi_enable_monitoring_mode
 * @warning        This function available in monitoring mode ONLY.\n  
 * @note             Packets are user's responsibility.
 * @return     	    The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
NMI_API sint8 m2m_wifi_send_wlan_pkt(uint8 *pu8WlanPacket, uint16 u16WlanHeaderLength, uint16 u16WlanPktSize);
/**@}*/
/** @defgroup WifiSendEthernetPktFn m2m_wifi_send_ethernet_pkt
 *   @ingroup WLANAPI
 *   Synchronous function to transmit an Ethernet packet. Transmit a packet directly in bypass mode where the TCP/IP stack is disabled and the implementation of this packet is left to the application developer.
*	The Ethernet packet composition is left to the application developer. 
 */
 /**@{*/
/*!
 * @fn           NMI_API sint8 m2m_wifi_send_ethernet_pkt(uint8* pu8Packet, uint16 u16PacketSize, uint8	u8IfcId)
 * @param [in]     pu8Packet
 *                        Pointer to a buffer holding the whole Ethernet frame.
 * @param [in]     u16PacketSize
 * 		            The size of the whole bytes in packet.
 * @param [in]     u8IfcId
 *				    The interface selected to send Ethernet packet	(AP_INTERFACE, STATION_INTERFACE OR P2P_INTERFACE)
 * 		            The size of the whole bytes in packet.    
  * @attention     This function available in Bypass mode ONLY. Make sure that firmware version built with macro \ref ETH_MODE.\n  
 * @note             Packets are the user's responsibility.
 * @return         The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

 */
NMI_API sint8 m2m_wifi_send_ethernet_pkt(uint8* pu8Packet,uint16 u16PacketSize, uint8 u8IfcId);
/**@}*/
/** @defgroup WifiSetCustInfoElementFn m2m_wifi_set_cust_InfoElement
 *   @ingroup WLANAPI
 *   Synchronous function to Add/Remove user-defined Information Element to the WIFIBeacon and Probe Response frames while chip mode is Access Point Mode.\n
 *   According to the information element layout shown bellow, if it is required to set new data for the information elements, pass in the buffer with the
 *   information according to the sizes and ordering defined bellow. However, if it's required to delete these IEs, fill the buffer with zeros.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_set_cust_InfoElement(uint8*);
 * @param [in]     pau8M2mCustInfoElement
 *                        Pointer to Buffer containing the IE to be sent. It is the application developer's responsibility to ensure on the correctness  of the information element's ordering passed in. 
 * @warning	       - Size of All elements combined must not exceed 255 byte.\n
 *			       - Used in Access Point Mode \n
 * @note              IEs Format will be follow the following layout:\n
 * @verbatim 
               --------------- ---------- ---------- ------------------- -------- -------- ----------- ----------------------  
              | Byte[0]       | Byte[1]  | Byte[2]  | Byte[3:length1+2] | ..... | Byte[n] | Byte[n+1] | Byte[n+2:lengthx+2]  | 
              |---------------|----------|----------|-------------------|-------- --------|-----------|------------------| 
              | #of all Bytes | IE1 ID   | Length1  | Data1(Hex Coded)  | ..... | IEx ID  | Lengthx   | Datax(Hex Coded)     | 
               --------------- ---------- ---------- ------------------- -------- -------- ----------- ----------------------             
 * @return        The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 \section Example
   The example demonstrates how the information elements are set using this function.
 *@code
 *
            char elementData[21];
            static char state = 0; // To Add, Append, and Delete
            if(0 == state) {	//Add 3 IEs
                state = 1;
                //Total Number of Bytes
                elementData[0]=12;
                //First IE
                elementData[1]=200; elementData[2]=1; elementData[3]='A';
                //Second IE
                elementData[4]=201; elementData[5]=2; elementData[6]='B'; elementData[7]='C';
                //Third IE
                elementData[8]=202; elementData[9]=3; elementData[10]='D'; elementData[11]=0; elementData[12]='F';
            } else if(1 == state) {	
                //Append 2 IEs to others, Notice that we keep old data in array starting with\n
                //element 13 and total number of bytes increased to 20
                state = 2; 
                //Total Number of Bytes
                elementData[0]=20;
                //Fourth IE
                elementData[13]=203; elementData[14]=1; elementData[15]='G';
                //Fifth IE
                elementData[16]=204; elementData[17]=3; elementData[18]='X'; elementData[19]=5; elementData[20]='Z';
            } else if(2 == state) {	//Delete All IEs
                state = 0; 
                //Total Number of Bytes
                elementData[0]=0;
            }
            m2m_wifi_set_cust_InfoElement(elementData);	
 * @endcode
 */
NMI_API sint8 m2m_wifi_set_cust_InfoElement(uint8* pau8M2mCustInfoElement);
/**@}*/
#ifdef ETH_MODE
/** @defgroup WifiEnableMacMcastFn m2m_wifi_enable_mac_mcast
 *   @ingroup WLANAPI
 *   Synchronous function to Add/Remove MAC addresses in the multicast filter to receive multicast packets in bypass mode.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_enable_mac_mcast(uint8 *, uint8);
 * @brief        
 * @param [in]     pu8MulticastMacAddress
 *                        Pointer to MAC address
 * @param [in]     u8AddRemove
 *                        A flag to add or remove the MAC ADDRESS, based on the following values:
 *                        -  0 : remove MAC address
 *                        -  1 : add MAC address    
 * @attention     This function is available in bypass mode ONLY. Make sure that firmware version built with the macro @ref ETH_MODE.\n  
 * @note           Maximum number of MAC addresses that could be added is 8.
 * @return       The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
NMI_API sint8 m2m_wifi_enable_mac_mcast(uint8* pu8MulticastMacAddress, uint8 u8AddRemove);
/*!
@fn	\
	 NMI_API sint8 m2m_wifi_enable_mcast_filter(void)

@brief
	Enable Multicast filter.

@param [in]	None
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

NMI_API sint8 m2m_wifi_enable_mcast_filter(void);
/*!
@fn	\
	 NMI_API sint8 m2m_wifi_enable_mcast_filter(void)

@brief
	Enable Multicast filter.

@param [in]	None
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

NMI_API sint8 m2m_wifi_disable_mcast_filter(void);
/**@}*/
/** @defgroup SetReceiveBufferFn m2m_wifi_set_receive_buffer
 *   @ingroup WLANAPI
 *    Synchronous function for  setting or changing the receiver buffer's length.
 *    Changes are made according to the developer option in bypass mode and this function should be called in the receive callback handling.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_set_receive_buffer(void *, uint16);
     
 * @param [in]     pvBuffer
 *                 Pointer to Buffer to receive data.
 *		     NULL pointer causes a negative error @ref M2M_ERR_FAIL.
 *
 * @param [in]     u16BufferLen
 *                 Length of data to be received.  Maximum length of data should not exceed the size defined by TCP/IP
 *      	     defined as @ref SOCKET_BUFFER_MAX_LENGTH
 *		     
 * @warning      This function is available in the bypass mode ONLY. Make sure that firmware version is built with macro @ref ETH_MODE.\n  
 * @return     The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
NMI_API sint8 m2m_wifi_set_receive_buffer(void* pvBuffer,uint16 u16BufferLen);
/**@}*/
#endif

/** @defgroup 
 *   @ingroup WLANAPI
 *   Synchronous function to download the certifcate data into the WILC1000 RAM , 
 	The provided input binary should be the output of the tool "root_certificate_downloader" .
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_download_cert(uint8* pCertData,uint32 u32CertSize);
 * @brief        
 * @param [in]     	pCertData
 *                        Pointer to the certificate data
 					u32CertSize
						Size of the certificate data
 * @return       The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */

sint8 m2m_wifi_download_cert(uint8* pCertData,uint32 u32CertSize);

#ifdef P2P_CONCURRENT_INTERFACES
/** @defgroup WifiEnableMacMcastFn m2m_wifi_p2p_set_control_ifc
 *   @ingroup WLANAPI
 *   Synchronous function to set on which interface the next control function would be applied.
 */
 /**@{*/
/*!
 * @fn             NMI_API sint8 m2m_wifi_set_control_ifc(uint8 u8IfcId);
 * @brief        
 * @param [in]     u8IfcId
 *                        interface ID , either 1 for interface 1 or 2 for interface 2
 * @return       The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
sint8 m2m_wifi_set_p2p_control_ifc(uint8 u8IfcId);
#endif
/*!
@fn			NMI_API sint8 m2m_wifi_set_tx_power(uint8 u8TxPwrLevel);
@brief		set the TX power tenuM2mTxPwrLevel
@param [in]	u8TxPwrLevel
			change the TX power tenuM2mTxPwrLevel
@return		The function SHALL return M2M_SUCCESE for success and a negative value otherwise.
@sa			tenuM2mTxPwrLevel
@pre		m2m_wifi_init
@warning	
*/
NMI_API sint8 m2m_wifi_set_tx_power(uint8 u8TxPwrLevel);

/*!
@fn			NMI_API sint8 m2m_wifi_set_max_tx_rate(tenuTxDataRate enuMaxTxDataRate);
@brief		Set the maximum Tx data rate used
@param [in]	enuMaxTxDataRate
			Maximum Tx power
@return		The function SHALL return M2M_SUCCESE for success and a negative value otherwise.
@sa			tenuTxDataRate
@pre		m2m_wifi_init
@warning	
*/
NMI_API sint8 m2m_wifi_set_max_tx_rate(tenuTxDataRate enuMaxTxDataRate);

#ifdef __cplusplus
}
#endif
#endif /* __M2M_WIFI_H__ */
