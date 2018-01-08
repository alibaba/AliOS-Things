
#ifndef __StringUtils_h__
#define __StringUtils_h__

#include <stdarg.h>
#include <stdint.h>
#include <typedef.h>

/** @addtogroup MICO_Middleware_Interface
  * @{
  */

/** @defgroup MICO_String_Utils MiCO String Utils
  * @brief Provide String operations APIs.
  * @{
  */

// ==== STRING SIZE UTILS ====
//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    sizeof_string
    @abstract   Determines the size of a constant C string, excluding the null terminator.
*/
#define sizeof_string( X )      ( sizeof( ( X ) ) - 1 )

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    kSizeCString
    @abstract   Constant for describing the size of a null terminated string
*/
#define kSizeCString ( (size_t) -1 )

// ==== MAC String Tools ====   
void formatMACAddr(char *destAddr, char *srcAddr);

/* @brief  transform src string to hex mode 
*          Example: "aabbccddee" => 0xaabbccddee
*          each char in the string must 0~9 a~f A~F, otherwise return 0
*          return the real obuf length
*
*  @param  ibuf:?
*  @param  obuf:?
*  @param  olen:?
*
*  @retval ?
*/
unsigned int str2hex(unsigned char *ibuf, unsigned char *obuf, unsigned int olen);

/**
  * @brief  Convert an Integer to a string
  *
  * @param  str: The string
  * @param  intnum: The integer to be converted
  *
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum);


/**
  * @brief  Convert a string to an integer
  *
  * @param  inputstr: The string to be converted
  * @param  intnum: The integer value
  *
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);


/**
  * @brief  Allocate a memory to store the string
  *
  * @param  src: ?
  * 
  * @retval ?
  */
char *__strdup(const char *src);



/**
  * @brief  Allocate a memory to store the string, and transfer '.' to '\.'
  *
  * @param  src: ?
  * 
  * @retval ?
  */
char *__strdup_trans_dot(char *src);

// ==== NETWORKING STRING UTILS ====
#define TextToMACAddress( TEXT, SIZE, ADDR )    TextToHardwareAddress( TEXT, SIZE, 6, ADDR )


/**
  * @brief     ?
  *
  * @param  inText    : ?
  * @param  inTextSize: ?
  * @param  inAddrSize: ?
  * @param  outAddr   : ?
  * 
  * @retval ?
  */
int TextToHardwareAddress( const void *inText, size_t inTextSize, size_t inAddrSize, void *outAddr );

unsigned int str2hex(unsigned char *ibuf, unsigned char *obuf, unsigned int olen);



// ==== BYTE BUFFER TO STRING CONVERSION UTILS ====
/**
  * @brief  
  *
  * @param  inBuf   :?
  * @param  inBufLen:?
  * 
  * @retval ?
  */
char* DataToCString( const uint8_t *inBuf, size_t inBufLen );


/**
  * @brief  ?
  *
  * @param  inBuf   :?
  * @param  inBufLen:?
  * 
  * @retval ?
  */
char* DataToHexString( const uint8_t *inBuf, size_t inBufLen );


/**
  * @brief  ?
  *
  * @param  inBuf   :?
  * @param  inBufLen:?
  * 
  * @retval ?
  */
char* DataToHexStringWithSpaces( const uint8_t *inBuf, size_t inBufLen );


/**
  * @brief  ?
  *
  * @param  inBuf   :?
  * @param  inBufLen:?
  * 
  * @retval ?
  */
char* DataToHexStringWithColons( const uint8_t *inBuf, size_t inBufLen );

// ==== STRING COMPARE UTILS ====
/**
  * @brief  ?
  *
  * @param  inStr   :?
  * @param  inMaxLen:?
  * @param  inSuffix:?
  * 
  * @retval ?
  */
int strnicmp_suffix( const void *inStr, size_t inMaxLen, const char *inSuffix );



/**
  * @brief  ?
  *
  * @param  inS1   :?
  * @param  inS2   :?
  * @param  inMax  :?
  * 
  * @retval ?
  */
int strnicmp( const char *inS1, const char *inS2, size_t inMax );


/**
  * @brief  ?
  *
  * @param  inS1   :?
  * @param  inN    :?
  * @param  inS2   :?
  * 
  * @retval ?
  */
int strnicmpx( const void *inS1, size_t inN, const char *inS2 );


/**
  * @brief  ?
  *
  * @param  inStr   :?
  * @param  inMaxLen:?
  * @param  inSuffix:?
  * 
  * @retval ?
  */
char * strnstr_suffix( const char *inStr, size_t inMaxLen, const char *inSuffix);


/**
  * @brief  ?
  *
  * @param  inStr   :  ?
  * @param  inMaxLen:   ?
  * @param  inSuffix:  ?
  * 
  * @retval ?
  */
int VSNScanF( const void *inString, size_t inSize, const char *inFormat, va_list inArgs );

/**
  * @brief  ?
  *
  * @param  value   :  ?
  * @param  output:   ?
  * @param  min_length:  ?
  * @param  max_length:  ?
  * 
  * @retval ?
  */
uint8_t unsigned_to_hex_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length );

/**
  * @brief  ?
  *
  * @param  string   :  ?
  * @param  str_length:   ?
  * @param  value_out:  ?
  * @param  is_hex:  ?
  * 
  * @retval ?
  */
uint8_t string_to_unsigned( const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex );


/**
  * @brief  ?
  *
  * @param  start   :?
  * @param  s_len   :?
  * @param  find    :?
  * @param  f_len   :?
  * 
  * @retval none
  */
void *memmem(void *start, unsigned int s_len, void *find, unsigned int f_len);

uint8_t unsigned_to_decimal_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length );

#if defined (__CC_ARM)
#define strdup __strdup

/**
  * @brief  ?
  *
  * @param  s       :
  * @param  count   :
  * 
  * @retval ?
  */
size_t strnlen(const char *s, size_t count);
#endif

/**
  * @}
  */

/**
  * @}
  */


#endif // __StringUtils_h__

