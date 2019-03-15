/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : iniparserapi.h
 * Description : iniparserapi
 * History :
 *
 */

#ifndef INI_PARSER_API_H
#define INI_PARSER_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------------*/
/**
  @brief    IniParser to load /system/etc/cedarx.conf config file
  @return   integer 1 parse success, 0 parse fail

  The dictionary will save int a global vale g_ini.
  This function will be called automatically when other api funcion is called
  firstly.
 **/
/*--------------------------------------------------------------------------*/
static inline int  IniParserInit() {
	return 0;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated
  @return   void

  Free all memory associated to ini dictionary.
 */
/*--------------------------------------------------------------------------*/
static inline void IniParserDestory() {

}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
/*static inline const char * IniParserGetString(const char * key, const char * def) {

}*/

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  - "42"      ->  42
  - "042"     ->  34 (octal -> decimal)
  - "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.
 */
/*--------------------------------------------------------------------------*/
static inline int IniParserGetInt(const char * key, int notfound) {
	return notfound;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a double
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
static inline double IniparserGetDouble(const char * key, double notfound) {
	return notfound;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
static inline int IniParserGetBoolean(const char * key, int notfound) {
	return notfound;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
//int IniParserFindEntry(const char * entry);

static inline int GetConfigParamterInt(const char * key, int notfound) {
	return notfound;
}

//const char * GetConfigParamterString(const char * key, const char * def);

static inline double GetConfigParamterDouble(const char * key, double notfound) {
	return notfound;
}
//int GetConfigParamterBoolean(const char * key, int notfound);

#ifdef __cplusplus
}
#endif

#endif
