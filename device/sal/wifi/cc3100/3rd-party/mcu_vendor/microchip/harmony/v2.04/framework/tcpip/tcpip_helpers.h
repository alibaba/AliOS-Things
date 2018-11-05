/*******************************************************************************
Header file for tcpip_stack_helpers

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_helpers.h

  Summary:
    TCP/IP Stack helpers header file.

  Description:
    This file provides the APIs for the TCP/IP Stack helpers.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef __TCPIP_HELPERS_H_
#define __TCPIP_HELPERS_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
     bool	TCPIP_Helper_StringToIPAddress(const char* str, IPV4_ADDR* IPAddress);

  Summary:
	Converts an ASCII string to an IPV4 address.
	
  Description:
	This function parses a dotted-quad decimal IP address string into an 
	IPV4_ADDR struct.  The output result is big-endian.

  Precondition:
    None.

  Parameters:
	str         - Pointer to a dotted-quad IP address string
	IPAddress   - Pointer to IPV4_ADDR in which to store the result
	
  Returns:
  	- true - an IP address was successfully decoded
  	- false - no IP address could be found, or the format was incorrect

 Remarks:
    None.
 */

bool	TCPIP_Helper_StringToIPAddress(const char* str, IPV4_ADDR* IPAddress);

// *****************************************************************************
/*
  Function:
     bool    TCPIP_Helper_IPAddressToString(const IPV4_ADDR* IPAddress, char* buff, size_t buffSize);

  Summary:
	Converts an IPV4 address to an ASCII string.  
	
  Description:
	This function converts IPV4_ADDR to a dotted-quad decimal IP address string

  Precondition:
    None.

  Parameters:
	IPAddress   - Pointer to IPV4_ADDR to convert
	buff        - buffer to store the converted dotted-quad IP address string
    buffSize    - buffer size
	
  Returns:
  	- true - an IP address was successfully converted
  	- false - supplied buffer was not large enough

 Remarks:
    None.
 */

bool    TCPIP_Helper_IPAddressToString(const IPV4_ADDR* IPAddress, char* buff, size_t buffSize);

// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_StringToIPv6Address (const char * str, IPV6_ADDR * addr);

  Summary:
	Converts a string to an IPv6 address.

  Description:
	This function parses the text representation of an IPv6 address to an
	IPV6_ADDR struct.  The output result is big-endian.
	
  Precondition:
	None

  Parameters:
	str - Pointer to an RFC3513, Section 2.2 text representation of
        an IPv6 address
        Example:    1111:2222:3333:4444:5555:6666:AAAA:FFFF
                    1111:2222::FFFF
                    1111:2222:3333:4444:5555:6666:192.168.1.20
	addr - Pointer to IPV6_ADDR in which to store the result

  Return Values:
  	- true - an IP address was successfully decoded
  	- false - no IP address could be found, or the format was incorrect

 Remarks:
    None.    
 */

bool    TCPIP_Helper_StringToIPv6Address (const char * str, IPV6_ADDR * addr);

// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_IPv6AddressToString (const IPV6_ADDR * addr, char* buff, size_t buffSize);

  Summary:
	Converts an IPv6 address to a string representation.

  Description:
	This function converts an IPV6_ADDR to a text representation of an IPv6 address.
	
  Precondition:
	None.

  Parameters:
	addr - Pointer to IPV6_ADDR to convert
	buff - Pointer to a buffer to store the text representation
    buffSize    - buffer size
	
  Returns:
  	- true  - an IPv6 address was successfully converted
  	- false - supplied buffer was not large enough

 Remarks:
    None.
    
 */

bool    TCPIP_Helper_IPv6AddressToString (const IPV6_ADDR * addr, char* buff, size_t buffSize);


// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_IsBcastAddress(const IPV4_ADDR* IPAddress);

  Summary:
	Checks if an IPv4 address is a broadcast address.

  Description:
	This function verifies if the supplied IPv4 address is a broadcast address.
	
  Precondition:
	None.

  Parameters:
	IPAddress - Pointer to IPV4_ADDR to check
	
  Returns:
  	- true  - if the IPv4 address is a broadcast address
  	- false - if the IPv4 address is not a broadcast address

 Remarks:
    None.
 */

static __inline__ bool __attribute__((always_inline)) TCPIP_Helper_IsBcastAddress(const IPV4_ADDR* IPAddress)
{
    return (IPAddress->Val == 0xFFFFFFFF);
}

// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_IsMcastAddress(const IPV4_ADDR* IPAddress)

  Summary:
	Checks if an IPv4 address is a multicast address.

  Description:
	This function verifies if the supplied IPv4 address is a multicast address.
	
  Precondition:
	None.

  Parameters:
	IPAddress - Pointer to IPV4_ADDR to check
	
  Returns:
  	- true  - if the IPv4 address is a multicast address
  	- false - if the IPv4 address is not a multicast address

 Remarks:
    None.
 */

static __inline__ bool __attribute__((always_inline)) TCPIP_Helper_IsMcastAddress(const IPV4_ADDR* IPAddress)
{
    return ((IPAddress->v[0] & 0xf0) == 0xE0);
}

// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_IsPrivateAddress(uint32_t ipv4Address);

  Summary:
	Detects a private (non-routable) address.

  Description:
	This function checks if the passed in IPv4 address
    is a private or a routable address.
	
  Precondition:
	None.

  Parameters:
	ipv4Address - IPv4 address to check, network order
	
  Returns:
  	- true - the IPv4 address is a private address
  	- false - the IPv4 address is a routable address

 Remarks:
    None.
 */

bool      TCPIP_Helper_IsPrivateAddress(uint32_t ipv4Address);

// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_StringToMACAddress(const char* str, uint8_t macAddr[6]);

  Summary:
	Converts a string to an MAC address.

  Description:
	This function parses a MAC address string "aa:bb:cc:dd:ee:ff"
    or "aa-bb-cc-dd-ee-ff" into an hex MAC address.
	
  Precondition:
	None.

  Parameters:
	str - Pointer to a colon separated MAC address string
	macAddr - Pointer to buffer to store the result
	
  Returns:
  	- true - a MAC address was successfully decoded
  	- false - no MAC address could be found, or the format was incorrect

 Remarks:
    None.    
 */

bool      TCPIP_Helper_StringToMACAddress(const char* str, uint8_t macAddr[6]);

// *****************************************************************************
/*
  Function:
    bool    TCPIP_Helper_MACAddressToString(const TCPIP_MAC_ADDR* macAddr, char* buff, size_t buffSize);

  Summary:
	Converts a MAC address to a string.

  Description:
	This function will convert a MAC address to a string representation.
	
  Precondition:
	None.

  Parameters:
	macAddr  - Pointer to address to convert
	buff     - buffer to store the string representation
    buffSize - size of the buffer

  Returns:
  	- true  - a MAC address was successfully decoded
  	- false - no MAC address could be found, or the format was incorrect

 Remarks:
    None.    
 */

bool     TCPIP_Helper_MACAddressToString(const TCPIP_MAC_ADDR* macAddr, char* buff, size_t buffSize);

// helpers to convert a host long to a network long
// and reverse
//
// helpers to convert a host short to a network short
// and reverse
//


// *****************************************************************************
/*
  Function:
    uint64_t TCPIP_Helper_htonll(uint64_t hLLong);
    uint32_t TCPIP_Helper_htonl(uint32_t hLong);
    uint16_t TCPIP_Helper_htons(uint16_t hShort);
    uint64_t TCPIP_Helper_ntohll(uint64_t nLLong);
    uint32_t TCPIP_Helper_ntohl(uint32_t nLong);
    uint16_t TCPIP_Helper_ntohs(uint16_t nShort);

  Summary:
	Conversion routines from network order to host order and reverse.

  Description:
	These functions will convert a long or short quantity from the
    network order (big endian) to host order (little endian on PIC32).
	
  Precondition:
	None.

  Parameters:
	hLong/nLong     - long value (32 bits) to convert
	hShort/nShort   - short value (16 bits) to convert
	
  Returns:
  	The converted 16/32-bit quantity.

 Remarks:
    None.    
 */

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
uint32_t __attribute__((nomips16)) TCPIP_Helper_htonl(uint32_t hLong);

uint16_t __attribute__((nomips16)) TCPIP_Helper_htons(uint16_t hShort);

uint64_t __attribute__((nomips16)) TCPIP_Helper_htonll(uint64_t hLLong);

uint64_t __attribute__((nomips16)) TCPIP_Helper_ntohll(uint64_t nLLong);
#define     TCPIP_Helper_ntohll(ll)  TCPIP_Helper_htonll(ll)

#else

static inline  uint32_t __attribute__((always_inline)) TCPIP_Helper_htonl(uint32_t hLong)
{
    return (((hLong & 0x000000ff) << 24) | ((hLong & 0x0000ff00) << 8) | ((hLong & 0x00ff0000) >> 8) | ((hLong & 0xff000000) >> 24));
}

static inline uint16_t __attribute__((always_inline)) TCPIP_Helper_htons(uint16_t hShort)
{
       return (((hShort) << 8) | ((hShort) >> 8));
}

#endif  // !defined(__PIC32MX__)

uint32_t    TCPIP_Helper_ntohl(uint32_t nLong);
#define     TCPIP_Helper_ntohl(n)   TCPIP_Helper_htonl(n)

uint16_t TCPIP_Helper_ntohs(uint16_t nShort);
#define  TCPIP_Helper_ntohs(n)  TCPIP_Helper_htons(n) 

//*****************************************************************************
/*
  Function:
	void TCPIP_Helper_FormatNetBIOSName(uint8_t Name[])

  Summary:
	Formats a string to a valid NetBIOS name.

  Description:
	This function formats a string to a valid NetBIOS name.  Names will be
	exactly 16 characters, as defined by the NetBIOS spec.  The 16th 
	character will be a 0x00 byte, while the other 15 will be the 
	provided string, padded with spaces as necessary.

  Precondition:
	None.

  Parameters:
	Name - the string to format as a NetBIOS name.  This parameter must have
	at least 16 bytes allocated.

  Returns:
	None.
*/
void    TCPIP_Helper_FormatNetBIOSName(uint8_t Name[]);



// *****************************************************************************
/* Function:
    bool TCPIP_Helper_TCPSecurePortGet(uint16_t tcpPort);

  Summary:
    Checks if the required TCP port is a secure port.
    
  Description:
    This function checks the internal data for detecting if the required TCP port is a secure port or not.
    A secure port is one that requires secure connections.
    The TCP/IP stack maintains an internal table with the ports that require secure connections.
    This table can be maintained using the TCPIP_Helper_SecurePortSet() function.

  Precondition:
    TCP/IP stack properly initialized.

  Parameters:
	tcpPort    - The TCP port to query

  Returns:
    - true	- if the port is a secure connection port
    - false - if port is not secure (the port is not found in the internal secure ports table)

  Remarks:
    The TCP/IP stack populates the internal secure port table with default values 
    as part of the stack power up procedure.
    The table is not re-initialized when the stack is brought down and then restarted.

 */

bool             TCPIP_Helper_TCPSecurePortGet(uint16_t tcpPort);

// *****************************************************************************
/* Function:
    bool TCPIP_Helper_UDPSecurePortGet(uint16_t udpPort);

  Summary:
    Checks if the required UDP port is a secure port.
    
  Description:
    This function checks the internal data for detecting if the required UDP port is a secure port or not.
    A secure port is one that requires secure connections.
    The TCP/IP stack maintains an internal table with the ports that require secure connections.
    This table can be maintained using the TCPIP_Helper_SecurePortSet() function.

  Precondition:
    TCP/IP stack properly initialized.

  Parameters:
	udpPort    - The UDP port to query

  Returns:
    - true	- if the port is a secure connection port
    - false - if port is not secure (the port is not found in the internal secure ports table)

  Remarks:
    The TCP/IP stack populates the internal secure port table with default values 
    as part of the stack power up procedure.
    The table is not re-initialized when the stack is brought down and then restarted.

 */

bool             TCPIP_Helper_UDPSecurePortGet(uint16_t udpPort);

// *****************************************************************************
/* Function:
    uint16_t TCPIP_Helper_SecurePortGetByIndex(int index, bool streamSocket, int* pnIndexes);

  Summary:
    Returns the secure port belonging to a specified index.
    
  Description:
    This function returns the secure TCP/UDP port situated at the internal port table requested index.
    It also returns the number of indexes currently in the table.

  Precondition:
    TCP/IP stack properly initialized.

  Parameters:
	index           - The port index to query.
                      0 should always be a valid index
    streamSocket    - if true, a stream/TCP port is queried
                      else a datagram/UDP port is queried
    pnIndexes       - pointer to store the number of indexes that the table currntly has.
                      Could be NULL if not needed.


  Returns:
    - a port number corresponding to the required index
    - 0 if the corresponding slot is free or the port is not associated with the requested
      stream/datagram flag.
 */

uint16_t            TCPIP_Helper_SecurePortGetByIndex(int index, bool streamSocket, int* pnIndexes);

// *****************************************************************************
/* Function:
    bool TCPIP_Helper_SecurePortSet(uint16_t port, bool streamSocket, bool isSecure);

  Summary:
    Sets the required port secure connection status.
    
  Description:
    This function sets the internal data for deciding if the required port is a secure port or not.
    A secure port is one that requires secure connections.
    The TCP/IP internally maintained table with  the ports that require secure connections
    is manipulated with this function.
    This table can be queried using the TCPIP_Helper_TCPSecurePortGet() function.

  Precondition:
    TCP/IP stack properly initialized.

  Parameters:
	port            - The TCP/UDP port to set
    streamSocket    - if true, a stream/TCP port is queried
                      else a datagram/UDP port is queried
    isSecure        - if true, the port is set as requiring secure connection
                      if false, the port is set as not requiring a secure connection and it will be removed
                      from the secure ports table


  Returns:
    - true	- the port status successfuly changed
    - false - the port status could not be changed (no more slots in the table, port not found, etc.)

  Remarks:
    Currently there is no protection for multiple threads accessing and modifying the entries in the secure port table.
    It is thus recommended that the updates to the table occur just once at initialization time and after
    that all threads perform read-only accesses.

 */

bool             TCPIP_Helper_SecurePortSet(uint16_t port, bool streamSocket, bool isSecure);


// *****************************************************************************
/*****************************************************************************
  Function:
	uint16_t TCPIP_Helper_Base64Encode(const uint8_t* sourceData, uint16_t sourceLen,
						uint8_t* destData, uint16_t destLen)
  Summary:
    Helper function to encode to Base-64.

  Description:
	This function encodes a binary array to Base-64.
	
  Precondition:
	None

  Parameters:
	sourceData - Pointer to a string of binary data
	sourceLen	- Length of the binary source data
	destData	- Pointer to write the Base-64 encoded data
	destLen	    - Maximum length that can be written to destData

  Returns:
  	Number of encoded bytes written to destData.  This will always be
  	a multiple of 4.
  
  Remarks:
	Encoding cannot be performed in-place.
    If surceData overlaps with  destData, the behavior is undefined.
	
	The source data is padded wit 1 or 2 bytes, if needed, to make the source size a multiple
    of 3 bytes.
    Then for each 3 bytes tuple in the source 4 output bytes are generated.
    The output size needed is pad(sourceLen) * 4 / 3 bytes.

  ***************************************************************************/
uint16_t TCPIP_Helper_Base64Encode(const uint8_t* sourceData, uint16_t sourceLen, uint8_t* destData, uint16_t destLen);

// *****************************************************************************
/*****************************************************************************
  Function:
	uint16_t TCPIP_Helper_Base64Decode(const uint8_t* sourceData, uint16_t sourceLen, 
						uint8_t* destData, uint16_t destLen)

  Summary:
    Helper function to decode a Base-64 encoded array.

  Description:
	Decodes a Base-64 array to its literal representation.
	
  Precondition:
	None

  Parameters:
	sourceData  - Pointer to a string of Base-64 encoded data
	sourceLen	- Length of the Base-64 source data
	destData	- Pointer to write the decoded data
	sourceLen	- Maximum length that can be written to destData

  Returns:
  	Number of decoded bytes written to destData.
  
  Remarks:
	This function will ignore invalid Base-64 characters (CR, LF, etc).
    If sourceData is equal to destData, the data will be converted
	in-place.
    If sourceData is not equal to destData, but the regions 
	overlap, the behavior is undefined.
	
	Decoded data size is 3 / 4 the size of the encoded source data.
  ***************************************************************************/
uint16_t TCPIP_Helper_Base64Decode(const uint8_t* sourceData, uint16_t sourceLen, uint8_t* destData, uint16_t destLen);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __TCPIP_HELPERS_H_
