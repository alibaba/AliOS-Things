/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE file
 * for complete information.
 */
#define _GNU_SOURCE
#define __USE_GNU

//#include <os_wrapper.h>
#include "lwip/opt.h"

//#include <log.h>
#include "iperf_config.h"
#include <stdarg.h>
#include <string.h>
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/tcp.h"

#include "net.h"
#include "iperf.h"
#include "iperf_api.h"
#include "iperf_udp.h"
#include "iperf_tcp.h"
#include "timer.h"

//#include "cJSON.h"
#include "units.h"
#include "iperf_util.h"
#include "iperf_locale.h"
#include "version.h"

#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif
#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1LL)
#endif

static const char *Iperf_ep;

#define Iperf_cJSON_AddNullToObject( object, name )		Iperf_cJSON_AddItemToObject( object, name, Iperf_cJSON_CreateNull() )
#define Iperf_cJSON_AddTrueToObject( object, name )		Iperf_cJSON_AddItemToObject( object, name, Iperf_cJSON_CreateTrue() )
#define Iperf_cJSON_AddFalseToObject( object, name )		Iperf_cJSON_AddItemToObject( object, name, Iperf_cJSON_CreateFalse() )
#define Iperf_cJSON_AddIntToObject( object, name, n )		Iperf_cJSON_AddItemToObject( object, name, Iperf_cJSON_CreateInt( n ) )
#ifdef IPERF_SUPPORT_DOUBLE
#define Iperf_cJSON_AddFloatToObject( object, name, n )	Iperf_cJSON_AddItemToObject( object, name, Iperf_cJSON_CreateFloat( n ) )
#endif
#define Iperf_cJSON_AddStringToObject( object, name, s ) Iperf_cJSON_AddItemToObject( object, name, Iperf_cJSON_CreateString( s ) )

const char *Iperf_cJSON_GetErrorPtr( void )
{
	return Iperf_ep;
}

static int Iperf_cJSON_strcasecmp( const char *s1, const char *s2 )
{
	if ( ! s1 )
		return ( s1 == s2 ) ? 0 : 1;
	if ( ! s2 )
		return 1;
	for ( ; TOLOWER((u8)*s1) == TOLOWER((u8)*s2); ++s1, ++s2)
		if( *s1 == 0 )
			return 0;
	return TOLOWER((u8)*s1) - TOLOWER((u8)*s2);
}


static void *(*Iperf_cJSON_malloc)( size_t ) = iperf_malloc;
static void (*Iperf_cJSON_free)( void * ) = iperf_free;

static char* Iperf_cJSON_strdup( const char* str )
{
	size_t len;
	char* copy;

	len = STRLEN( str ) + 1;
	if ( ! ( copy = (char*) Iperf_cJSON_malloc( len ) ) )
		return 0;
	MEMCPY( copy, str, len );
	return copy;
}


/* Internal constructor. */
static Iperf_cJSON *Iperf_cJSON_New_Item( void )
{
	Iperf_cJSON* node = (Iperf_cJSON*) Iperf_cJSON_malloc( sizeof(Iperf_cJSON) );
	if ( node )
		MEMSET( node, 0, sizeof(Iperf_cJSON) );
	return node;
}


/* Delete a cJSON structure. */
void Iperf_cJSON_Delete( Iperf_cJSON *c )
{
	Iperf_cJSON *next;

	while ( c ) {
		next = c->next;
		if ( ! ( c->type & Iperf_cJSON_IsReference ) && c->child )
			Iperf_cJSON_Delete( c->child );
		if ( ! ( c->type & Iperf_cJSON_IsReference ) && c->valuestring )
			Iperf_cJSON_free( c->valuestring );
		if ( c->string )
			Iperf_cJSON_free( c->string );
		Iperf_cJSON_free( c );
		c = next;
	}
}

#ifdef IPERF_SUPPORT_DOUBLE
static double Iperf_ipow( double n, int exp )
{
	double r;

	if ( exp < 0 )
		return 1.0 / Iperf_ipow( n, -exp );
	r = 1;
	while ( exp > 0 ) {
		if ( exp & 1 )
			r *= n;
		exp >>= 1;
		n *= n;
	}
	return r;
}


/* Parse the input text to generate a number, and populate the result into item. */
static const char *Iperf_parse_number( Iperf_cJSON *item, const char *num )
{
	iperf_ssize_t i = 0;
	double f = 0;
	int isint = 1;
	int sign = 1, scale = 0, subscale = 0, signsubscale = 1;

	/* Could use sscanf for this? */
	if ( *num == '-' ) {
		/* Has sign. */
		sign = -1;
		++num;
	}
	if ( *num == '0' )
		/* Is zero. */
		++num;
	if ( *num >= '1' && *num<='9' ) {
		/* Number. */
		do {
			i = ( i * 10 ) + ( *num - '0' );
			f = ( f * 10.0 ) + ( *num - '0' );
			++num;
		} while ( *num >= '0' && *num <= '9' );
	}
	if ( *num == '.' && num[1] >= '0' && num[1] <= '9' ) {
		/* Fractional part. */
		isint = 0;
		++num;
		do {
			f = ( f * 10.0 ) + ( *num++ - '0' );
			scale--;
		} while ( *num >= '0' && *num <= '9' );
	}
	if ( *num == 'e' || *num == 'E' ) {
		/* Exponent. */
		isint = 0;
		++num;
		if ( *num == '+' )
			++num;
		else if ( *num == '-' ) {
			/* With sign. */
			signsubscale = -1;
			++num;
		}
		while ( *num >= '0' && *num <= '9' )
			subscale = ( subscale * 10 ) + ( *num++ - '0' );
	}

	/* Put it together. */
	if ( isint ) {
		/* Int: number = +/- number */
		i = sign * i;
		item->valueint = i;
		item->valuefloat = i;
	} else {
		/* Float: number = +/- number.fraction * 10^+/- exponent */
		f = sign * f * Iperf_ipow( 10.0, scale + subscale * signsubscale );
		item->valueint = f;
		item->valuefloat = f;
	}

	item->type = Iperf_cJSON_Number;
	return num;
}

/* Render the number nicely from the given item into a string. */
static char *Iperf_print_number( Iperf_cJSON *item )
{
	char *str;
	double f, f2;
	iperf_ssize_t i;

	str = (char*) Iperf_cJSON_malloc( 64 );
	if ( str ) {
		f = item->valuefloat;
		i = f;
		f2 = i;
		if ( f2 == f && item->valueint >= LLONG_MIN && item->valueint <= LLONG_MAX )
			sprintf( str, "%lld", (long long) item->valueint );
		else
			sprintf( str, "%g", item->valuefloat );
	}
	return str;
}

#else

static const char *Iperf_parse_number( Iperf_cJSON *item, const char *num )
{
	iperf_ssize_t i = 0;
	int isint = 1;
	int sign = 1;
    
	/* Could use sscanf for this? */
	if ( *num == '-' ) {
		/* Has sign. */
		sign = -1;
		++num;
	}
	if ( *num == '0' )
		/* Is zero. */
		++num;
	if ( *num >= '1' && *num<='9' ) {
		/* Number. */
		do {
			i = ( i * 10 ) + ( *num - '0' );
			++num;
		} while ( *num >= '0' && *num <= '9' );
	}
	if ( *num == '.' && num[1] >= '0' && num[1] <= '9' ) {
		/* Fractional part. */
		isint = 0;
		++num;
		do {
		    num++;
		} while ( *num >= '0' && *num <= '9' );
	}
	if ( *num == 'e' || *num == 'E' ) {
		/* Exponent. */
		isint = 0;
		++num;
		if ( *num == '+' )
			++num;
		else if ( *num == '-' ) {
			/* With sign. */
			++num;
		}
		while ( *num >= '0' && *num <= '9' )
			num++;
	}

	/* Put it together. */
	if ( isint ) {
		/* Int: number = +/- number */
		i *= sign;
		item->valueint = i;
        item->type = Iperf_cJSON_Number;
	} else {
		/* Float: just keep the original string */
		item->valueint = 0;   // not support float
        item->type = Iperf_cJSON_Number;
	}

	return num;
}

/* Render the number nicely from the given item into a string. */
static char *Iperf_print_number( Iperf_cJSON *item )
{
	char *str;

	str = (char*) Iperf_cJSON_malloc( 64 );
	if ( str ) {
	#ifdef IPERF_SUPPORT_LONGLONG
		sprintf( str, "%d",(int)item->valueint );
	#else
		sprintf( str, "%d", (int)item->valueint );
	#endif
	}
	return str;
}

#endif

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char Iperf_firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const char *Iperf_parse_string( Iperf_cJSON *item, const char *str )
{
	const char *ptr = str + 1;
	char *ptr2;
	char *out;
	int len = 0;
	unsigned uc, uc2;

	if ( *str != '\"' ) {
		/* Not a string! */
		Iperf_ep = str;
		return 0;
	}
	
	/* Skip escaped quotes. */
	while ( *ptr != '\"' && *ptr && ++len )
		if ( *ptr++ == '\\' )
			ptr++;
	
	if ( ! ( out = (char*) Iperf_cJSON_malloc( len + 1 ) ) )
		return 0;
	
	ptr = str + 1;
	ptr2 = out;
	while ( *ptr != '\"' && *ptr ) {
		if ( *ptr != '\\' )
			*ptr2++ = *ptr++;
		else {
			ptr++;
			switch ( *ptr ) {
				case 'b': *ptr2++ ='\b'; break;
				case 'f': *ptr2++ ='\f'; break;
				case 'n': *ptr2++ ='\n'; break;
				case 'r': *ptr2++ ='\r'; break;
				case 't': *ptr2++ ='\t'; break;
				case 'u':
					/* Transcode utf16 to utf8. */
					/* Get the unicode char. */
					sscanf( ptr + 1,"%4x", &uc );
					ptr += 4;
					/* Check for invalid. */
					if ( ( uc >= 0xDC00 && uc <= 0xDFFF ) || uc == 0 )
						break;

					/* UTF16 surrogate pairs. */
					if ( uc >= 0xD800 && uc <= 0xDBFF ) {
						if ( ptr[1] != '\\' || ptr[2] != 'u' )
							/* Missing second-half of surrogate. */
							break;
						sscanf( ptr + 3, "%4x", &uc2 );
						ptr += 6;
						if ( uc2 < 0xDC00 || uc2 > 0xDFFF )
							/* Invalid second-half of surrogate. */
							break;
						uc = 0x10000 | ( ( uc & 0x3FF ) << 10 ) | ( uc2 & 0x3FF );
					}

					len = 4;
					if ( uc < 0x80 )
						len = 1;
					else if ( uc < 0x800 )
						len = 2;
					else if ( uc < 0x10000 )
						len = 3;
					ptr2 += len;
					
					switch ( len ) {
						case 4: *--ptr2 = ( ( uc | 0x80) & 0xBF ); uc >>= 6;
						case 3: *--ptr2 = ( ( uc | 0x80) & 0xBF ); uc >>= 6;
						case 2: *--ptr2 = ( ( uc | 0x80) & 0xBF ); uc >>= 6;
						case 1: *--ptr2 = ( uc | Iperf_firstByteMark[len] );
					}
					ptr2 += len;
					break;
				default:  *ptr2++ = *ptr; break;
			}
			++ptr;
		}
	}
	*ptr2 = 0;
	if ( *ptr == '\"' )
		++ptr;
	item->valuestring = out;
	item->type = Iperf_cJSON_String;
	return ptr;
}


/* Render the cstring provided to an escaped version that can be printed. */
static char *Iperf_print_string_ptr( const char *str )
{
	const char *ptr;
	char *ptr2, *out;
	int len = 0;
	unsigned char token;
	
	if ( ! str )
		return Iperf_cJSON_strdup( "" );
	ptr = str;
	while ( ( token = *ptr ) && ++len ) {
		if ( STRCHR( "\"\\\b\f\n\r\t", token ) )
			++len;
		else if ( token < 32 )
			len += 5;
		++ptr;
	}
	
	if ( ! ( out = (char*) Iperf_cJSON_malloc( len + 3 ) ) )
		return 0;

	ptr2 = out;
	ptr = str;
	*ptr2++ = '\"';
	while ( *ptr ) {
		if ( (unsigned char) *ptr > 31 && *ptr != '\"' && *ptr != '\\' )
			*ptr2++ = *ptr++;
		else {
			*ptr2++ = '\\';
			switch ( token = *ptr++ ) {
				case '\\': *ptr2++ = '\\'; break;
				case '\"': *ptr2++ = '\"'; break;
				case '\b': *ptr2++ = 'b'; break;
				case '\f': *ptr2++ = 'f'; break;
				case '\n': *ptr2++ = 'n'; break;
				case '\r': *ptr2++ = 'r'; break;
				case '\t': *ptr2++ = 't'; break;
				default:
				/* Escape and print. */
				sprintf( ptr2, "u%04x", token );
				ptr2 += 5;
				break;
			}
		}
	}
	*ptr2++ = '\"';
	*ptr2++ = 0;
	return out;
}



/* Predeclare these prototypes. */
static const char *Iperf_parse_value( Iperf_cJSON *item, const char *value );
static char *Iperf_print_value( Iperf_cJSON *item, int depth, int fmt );
static const char *Iperf_parse_array( Iperf_cJSON *item, const char *value );
static char *Iperf_print_array( Iperf_cJSON *item, int depth, int fmt );
static const char *Iperf_parse_object( Iperf_cJSON *item, const char *value );
static char *Iperf_print_object( Iperf_cJSON *item, int depth, int fmt );

/* Invote print_string_ptr (which is useful) on an item. */
static char *Iperf_cjson_print_string( Iperf_cJSON *item )
{
	return Iperf_print_string_ptr( item->valuestring );
}

/* Utility to jump whitespace and cr/lf. */
static const char *Iperf_skip( const char *in )
{
	while ( in && *in && (unsigned char) *in <= 32 )
		in++;
	return in;
}


/* Parse an object - create a new root, and populate. */
Iperf_cJSON *Iperf_cJSON_Parse( const char *value )
{
	Iperf_cJSON *c;
	Iperf_ep = 0;

	//PRINTF("cJSON_Parse :%s  \r\n", value);
	
	if ( ! ( c = Iperf_cJSON_New_Item() ) )
		return 0;	/* memory fail */

	if ( ! Iperf_parse_value( c, Iperf_skip( value ) ) ) {
		Iperf_cJSON_Delete( c );
		return 0;
	}
	return c;
}


/* Render a cJSON item/entity/structure to text. */
char *Iperf_cJSON_Print( Iperf_cJSON *item )
{
	return Iperf_print_value( item, 0, 1 );
}
char *Iperf_cJSON_PrintUnformatted( Iperf_cJSON *item )
{
	return Iperf_print_value( item, 0, 0 );
}


/* Parser core - when encountering text, process appropriately. */
static const char *Iperf_parse_value( Iperf_cJSON *item, const char *value )
{
	if ( ! value )
		return 0;	/* Fail on null. */
	if ( ! STRNCMP( value, "null", 4 ) ) {
		item->type = Iperf_cJSON_NULL;
		return value + 4;
	}
	if ( ! STRNCMP( value, "false", 5 ) ) {
		item->type = Iperf_cJSON_False;
		return value + 5;
	}
	if ( ! STRNCMP( value, "true", 4 ) ) {
		item->type = Iperf_cJSON_True;
		item->valueint = 1;
		return value + 4;
	}
	if ( *value == '\"' )
		return Iperf_parse_string( item, value );
	if ( *value == '-' || ( *value >= '0' && *value <= '9' ) )
		return Iperf_parse_number( item, value );
	if ( *value == '[' )
		return Iperf_parse_array( item, value );
	if ( *value == '{' )
		return Iperf_parse_object( item, value );

	/* Fail. */
	Iperf_ep = value;
	return 0;
}


/* Render a value to text. */
static char *Iperf_print_value( Iperf_cJSON *item, int depth, int fmt )
{
	char *out = 0;

	if ( ! item )
		return 0;

    //PRINTF("print_value:: type:%d \r\n", item->type&0xff);
    //if (item->string)
    //    PRINTF("print_value:: type:%s \r\n", item->string);
        
	switch ( ( item->type ) & 255 ) {
		case Iperf_cJSON_NULL:   out = Iperf_cJSON_strdup( "null" ); break;
		case Iperf_cJSON_False:  out = Iperf_cJSON_strdup( "false" ); break;
		case Iperf_cJSON_True:   out = Iperf_cJSON_strdup( "true" ); break;
		case Iperf_cJSON_Number: out = Iperf_print_number( item ); break;
		case Iperf_cJSON_String: out = Iperf_cjson_print_string( item ); break;
		case Iperf_cJSON_Array:  out = Iperf_print_array( item, depth, fmt ); break;
		case Iperf_cJSON_Object: out = Iperf_print_object( item, depth, fmt ); break;
	}
	return out;
}


/* Build an array from input text. */
static const char *Iperf_parse_array( Iperf_cJSON *item, const char *value )
{
	Iperf_cJSON *child;

	if ( *value != '[' ) {
		/* Not an array! */
		Iperf_ep = value;
		return 0;
	}

	item->type = Iperf_cJSON_Array;
	value = Iperf_skip( value + 1 );
	if ( *value == ']' )
		return value + 1;	/* empty array. */

	if ( ! ( item->child = child = Iperf_cJSON_New_Item() ) )
		return 0;		 /* memory fail */
	if ( ! ( value = Iperf_skip( Iperf_parse_value( child, Iperf_skip( value ) ) ) ) )
		return 0;

	while ( *value == ',' ) {
		Iperf_cJSON *new_item;
		if ( ! ( new_item = Iperf_cJSON_New_Item() ) )
			return 0;	/* memory fail */
		child->next = new_item;
		new_item->prev = child;
		child = new_item;
		if ( ! ( value = Iperf_skip( Iperf_parse_value( child, Iperf_skip( value+1 ) ) ) ) )
			return 0;	/* memory fail */
	}

	if ( *value == ']' )
		return value + 1;	/* end of array */
	/* Malformed. */
	Iperf_ep = value;
	return 0;
}


/* Render an array to text */
static char *Iperf_print_array( Iperf_cJSON *item, int depth, int fmt )
{
	char **entries;
	char *out = 0, *ptr, *ret;
	int len = 5;
	Iperf_cJSON *child = item->child;
	int numentries = 0, i = 0, fail = 0;
	
	/* How many entries in the array? */
	while ( child ) {
		++numentries;
		child = child->next;
	}
	/* Allocate an array to hold the values for each. */
	if ( ! ( entries = (char**) Iperf_cJSON_malloc( numentries * sizeof(char*) ) ) )
		return 0;
	MEMSET( entries, 0, numentries * sizeof(char*) );
	/* Retrieve all the results. */
	child = item->child;
	while ( child && ! fail ) {
		ret = Iperf_print_value( child, depth + 1, fmt );
		entries[i++] = ret;
		if ( ret )
			len += STRLEN( ret ) + 2 + ( fmt ? 1 : 0 );
		else
			fail = 1;
		child = child->next;
	}
	
	/* If we didn't fail, try to malloc the output string. */
	if ( ! fail ) {
		out = (char*) Iperf_cJSON_malloc( len );
		if ( ! out )
			fail = 1;
	}

	/* Handle failure. */
	if ( fail ) {
		for ( i = 0; i < numentries; ++i )
			if ( entries[i] )
				Iperf_cJSON_free( entries[i] );
		Iperf_cJSON_free( entries );
		return 0;
	}
	
	/* Compose the output array. */
	*out = '[';
	ptr = out + 1;
	*ptr = 0;
	for ( i = 0; i < numentries; ++i ) {
		STRCPY( ptr, entries[i] );
		ptr += STRLEN( entries[i] );
		if ( i != numentries - 1 ) {
			*ptr++ = ',';
			if ( fmt )
				*ptr++ = ' ';
			*ptr = 0;
		}
		Iperf_cJSON_free( entries[i] );
	}
	Iperf_cJSON_free( entries );
	*ptr++ = ']';
	*ptr++ = 0;
	return out;	
}


/* Build an object from the text. */
static const char *Iperf_parse_object( Iperf_cJSON *item, const char *value )
{
	Iperf_cJSON *child;

	if ( *value != '{' ) {
		/* Not an object! */
		Iperf_ep = value;
		return 0;
	}
	
	item->type = Iperf_cJSON_Object;
	value =Iperf_skip( value + 1 );
	if ( *value == '}' )
		return value + 1;	/* empty array. */
	
	if ( ! ( item->child = child = Iperf_cJSON_New_Item() ) )
		return 0;
	if ( ! ( value = Iperf_skip( Iperf_parse_string( child, Iperf_skip( value ) ) ) ) )
		return 0;
	child->string = child->valuestring;
	child->valuestring = 0;
	if ( *value != ':' ) {
		/* Fail! */
		Iperf_ep = value;
		return 0;
	}
	if ( ! ( value = Iperf_skip( Iperf_parse_value( child,Iperf_skip( value + 1 ) ) ) ) )
		return 0;
	
	while ( *value == ',' ) {
		Iperf_cJSON *new_item;
		if ( ! ( new_item = Iperf_cJSON_New_Item() ) )
			return 0;	/* memory fail */
		child->next = new_item;
		new_item->prev = child;
		child = new_item;
		if ( ! ( value = Iperf_skip( Iperf_parse_string( child, Iperf_skip( value + 1 ) ) ) ) )
			return 0;
		child->string = child->valuestring;
		child->valuestring = 0;
		if ( *value != ':' ) {
			/* Fail! */
			Iperf_ep = value;
			return 0;
		}
		if ( ! ( value = Iperf_skip( Iperf_parse_value( child, Iperf_skip( value + 1 ) ) ) ) )
			return 0;
	}
	
	if ( *value == '}' )
		return value + 1;	/* end of array */
	/* Malformed. */
	Iperf_ep = value;
	return 0;
}


/* Render an object to text. */
static char *Iperf_print_object( Iperf_cJSON *item, int depth, int fmt )
{
	char **entries = 0, **names = 0;
	char *out = 0, *ptr, *ret, *str;
	int len = 7, i = 0, j;
	Iperf_cJSON *child = item->child;
	int numentries = 0, fail = 0;

	/* Count the number of entries. */
	while ( child ) {
		++numentries;
		child = child->next;
	}
	/* Allocate space for the names and the objects. */
	if ( ! ( entries = (char**) Iperf_cJSON_malloc( numentries * sizeof(char*) ) ) )
		return 0;
	if ( ! ( names = (char**) Iperf_cJSON_malloc( numentries * sizeof(char*) ) ) ) {
		Iperf_cJSON_free( entries );
		return 0;
	}
	MEMSET( entries, 0, sizeof(char*) * numentries );
	MEMSET( names, 0, sizeof(char*) * numentries );

	/* Collect all the results into our arrays. */
	child = item->child;
	++depth;
	if ( fmt )
		len += depth;
	while ( child ) {
		names[i] = str = Iperf_print_string_ptr( child->string );
		entries[i++] = ret = Iperf_print_value( child, depth, fmt );
		if ( str && ret )
			len += STRLEN( ret ) + STRLEN( str ) + 2 + ( fmt ? 2 + depth : 0 );
		else
			fail = 1;
		child = child->next;
	}
	
	/* Try to allocate the output string. */
	if ( ! fail ) {
		out = (char*) Iperf_cJSON_malloc( len );
		if ( ! out )
			fail = 1;
	}

	/* Handle failure. */
	if ( fail ) {
		for ( i = 0; i < numentries; ++i ) {
			if ( names[i] )
				Iperf_cJSON_free( names[i] );
			if ( entries[i] )
				Iperf_cJSON_free( entries[i] );
		}
		Iperf_cJSON_free( names );
		Iperf_cJSON_free( entries );
		return 0;
	}
	
	/* Compose the output. */
	*out = '{';
	ptr = out + 1;
	if ( fmt ){
		*ptr++ = '\r';
		*ptr++ = '\n';
	}
	*ptr = 0;
	for ( i = 0; i < numentries; ++i ) {
		if ( fmt )
			for ( j = 0; j < depth; ++j )
				*ptr++ = '\t';
		STRCPY( ptr, names[i] );
		ptr += STRLEN( names[i] );
		*ptr++ = ':';
		if ( fmt )
			*ptr++ = '\t';
		STRCPY( ptr, entries[i] );
		ptr += STRLEN( entries[i] );
		if ( i != numentries - 1 )
			*ptr++ = ',';
		if ( fmt ){
    		*ptr++ = '\r';
			*ptr++ = '\n';
	    }
		*ptr = 0;
		Iperf_cJSON_free( names[i] );
		Iperf_cJSON_free( entries[i] );
	}
	
	Iperf_cJSON_free( names );
	Iperf_cJSON_free( entries );
	if ( fmt )
		for ( i = 0; i < depth - 1; ++i )
			*ptr++ = '\t';
	*ptr++ = '}';
	*ptr++ = 0;
	return out;	
}


int Iperf_cJSON_GetArraySize( Iperf_cJSON *array )
{
	Iperf_cJSON *c = array->child;
	int i = 0;
	while ( c ) {
		++i;
		c = c->next;
	}
	return i;
}


Iperf_cJSON *Iperf_cJSON_GetArrayItem( Iperf_cJSON *array, int item )
{
	Iperf_cJSON *c = array->child;
	while ( c && item > 0 ) {
		--item;
		c = c->next;
	}
	return c;
}


Iperf_cJSON *Iperf_cJSON_GetObjectItem( Iperf_cJSON *object, const char *string )
{
	Iperf_cJSON *c = object->child;
	while ( c && Iperf_cJSON_strcasecmp( c->string, string ) )
		c = c->next;
	return c;
}


/* Utility for array list handling. */
static void Iperf_suffix_object( Iperf_cJSON *prev, Iperf_cJSON *item )
{
	prev->next = item;
	item->prev = prev;
}


/* Utility for handling references. */
static Iperf_cJSON *Iperf_create_reference( Iperf_cJSON *item )
{
	Iperf_cJSON *ref;
	if ( ! ( ref = Iperf_cJSON_New_Item() ) )
		return 0;
	MEMCPY( ref, item, sizeof(Iperf_cJSON) );
	ref->string = 0;
	ref->type |= Iperf_cJSON_IsReference;
	ref->next = ref->prev = 0;
	return ref;
}


/* Add item to array/object. */
void Iperf_cJSON_AddItemToArray( Iperf_cJSON *array, Iperf_cJSON *item )
{
	Iperf_cJSON *c = array->child;
	if ( ! item )
		return;
	if ( ! c ) {
		array->child = item;
	} else {
		while ( c && c->next )
			c = c->next;
		Iperf_suffix_object( c, item );
	}
}

void Iperf_cJSON_AddItemToObject( Iperf_cJSON *object, const char *string, Iperf_cJSON *item )
{    
    //PRINTF("cJSON_AddItemToObject ...%s \r\n", string);
	if ( ! item )
		return;
	if ( item->string )
		Iperf_cJSON_free( item->string );
	item->string = Iperf_cJSON_strdup( string );
	Iperf_cJSON_AddItemToArray( object, item );
    //PRINTF("cJSON_AddItemToObject ...end\r\n");
}

void Iperf_cJSON_AddItemReferenceToArray( Iperf_cJSON *array, Iperf_cJSON *item )
{
	Iperf_cJSON_AddItemToArray( array, Iperf_create_reference( item ) );
}

void Iperf_cJSON_AddItemReferenceToObject( Iperf_cJSON *object, const char *string, Iperf_cJSON *item )
{
	Iperf_cJSON_AddItemToObject( object, string, Iperf_create_reference( item ) );
}

Iperf_cJSON *Iperf_cJSON_DetachItemFromArray( Iperf_cJSON *array, int which )
{
	Iperf_cJSON *c = array->child;
	while ( c && which > 0 ) {
		c = c->next;
		--which;
	}
	if ( ! c )
		return 0;
	if ( c->prev )
		c->prev->next = c->next;
	if ( c->next ) c->next->prev = c->prev;
	if ( c == array->child )
		array->child = c->next;
	c->prev = c->next = 0;
	return c;
}

void Iperf_cJSON_DeleteItemFromArray( Iperf_cJSON *array, int which )
{
	Iperf_cJSON_Delete( Iperf_cJSON_DetachItemFromArray( array, which ) );
}

Iperf_cJSON *Iperf_cJSON_DetachItemFromObject(Iperf_cJSON *object, const char *string )
{
	int i = 0;
	Iperf_cJSON *c = object->child;
	while ( c && Iperf_cJSON_strcasecmp( c->string, string ) ) {
		++i;
		c = c->next;
	}
	if ( c )
		return Iperf_cJSON_DetachItemFromArray( object, i );
	return 0;
}

void Iperf_cJSON_DeleteItemFromObject( Iperf_cJSON *object, const char *string )
{
	Iperf_cJSON_Delete( Iperf_cJSON_DetachItemFromObject( object, string ) );
}

/* Replace array/object items with new ones. */
void Iperf_cJSON_ReplaceItemInArray( Iperf_cJSON *array, int which, Iperf_cJSON *newitem )
{
	Iperf_cJSON *c = array->child;
	while ( c && which > 0 ) {
		c = c->next;
		--which;
	}
	if ( ! c )
		return;
	newitem->next = c->next;
	newitem->prev = c->prev;
	if ( newitem->next )
		newitem->next->prev = newitem;
	if ( c == array->child )
		array->child = newitem;
	else
		newitem->prev->next = newitem;
	c->next = c->prev = 0;
	Iperf_cJSON_Delete( c );
}

void Iperf_cJSON_ReplaceItemInObject( Iperf_cJSON *object, const char *string, Iperf_cJSON *newitem )
{
	int i = 0;
	Iperf_cJSON *c = object->child;
	while ( c && Iperf_cJSON_strcasecmp( c->string, string ) ) {
		++i;
		c = c->next;
	}
	if ( c ) {
		newitem->string = Iperf_cJSON_strdup( string );
		Iperf_cJSON_ReplaceItemInArray( object, i, newitem );
	}
}


/* Create basic types: */

Iperf_cJSON *Iperf_cJSON_CreateNull( void )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item )
		item->type = Iperf_cJSON_NULL;
	return item;
}

Iperf_cJSON *Iperf_cJSON_CreateTrue( void )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item )
		item->type = Iperf_cJSON_True;
	return item;
}

Iperf_cJSON *Iperf_cJSON_CreateFalse( void )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item )
		item->type = Iperf_cJSON_False;
	return item;
}

Iperf_cJSON *Iperf_cJSON_CreateBool( int b )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item )
		item->type = b ? Iperf_cJSON_True : Iperf_cJSON_False;
	return item;
}

#ifdef IPERF_SUPPORT_DOUBLE
Iperf_cJSON *Iperf_cJSON_CreateInt( iperf_ssize_t num )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item ) {
		item->type = Iperf_cJSON_Number;
		item->valuefloat = num;
		item->valueint = num;
	}
	return item;
}

Iperf_cJSON *Iperf_cJSON_CreateFloat( double num )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item ) {
		item->type = Iperf_cJSON_Number;
		item->valuefloat = num;
		item->valueint = num;
	}
	return item;
}

#else
Iperf_cJSON *Iperf_cJSON_CreateInt( iperf_ssize_t num )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item ) {
		item->type = Iperf_cJSON_Number;
		item->valueint = num;
	}
	return item;
}
#endif

Iperf_cJSON *Iperf_cJSON_CreateString( const char *string )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item ) {
		item->type = Iperf_cJSON_String;
		item->valuestring = Iperf_cJSON_strdup( string );
	}
	return item;
}

Iperf_cJSON *Iperf_cJSON_CreateArray( void )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item )
		item->type = Iperf_cJSON_Array;
	return item;
}

Iperf_cJSON *Iperf_cJSON_CreateObject( void )
{
	Iperf_cJSON *item = Iperf_cJSON_New_Item();
	if ( item )
		item->type = Iperf_cJSON_Object;
	return item;
}
//#if (IPERF3_ENABLE == 1)
/* Forwards. */
static int send_parameters(struct iperf_test *test);
static int get_parameters(struct iperf_test *test);
static int send_results(struct iperf_test *test);
static int get_results(struct iperf_test *test);
static int JSON_write(int fd, Iperf_cJSON *json);
static void print_interval_results(struct iperf_test *test, struct iperf_stream *sp, Iperf_cJSON *json_interval_streams);
static Iperf_cJSON *JSON_read(int fd);

#ifdef IPERF_DEBUG_MEM
int iperf_mallocs = 0;
int iperf_frees = 0;
int iperf_msize = 0;
int iperf_max_mallocs = 0;
int iperf_max_msize = 0;
#endif

void *iperf_malloc(size_t size)
{
#ifdef IPERF_DEBUG_MEM
    u8 * ptr;
    ptr = (u8 *)MALLOC(size+4);
    if (ptr) {
        iperf_mallocs++;
        iperf_msize += size;
        *(int*)ptr = size;

        if (iperf_mallocs - iperf_frees > iperf_max_mallocs)
            iperf_max_mallocs = iperf_mallocs - iperf_frees;
        if (iperf_msize > iperf_max_msize)
            iperf_max_msize = iperf_msize;
        //PRINTF("iperf_malloc size: %u\r\n", size);
        ptr += 4;
    }    
    return (void*)ptr;
#else
    return (void*)MALLOC(size);
#endif    
}

void iperf_free(void *ptr)
{
#ifdef IPERF_DEBUG_MEM
    if (ptr){
        ptr = (u8 *)ptr - 4;
        iperf_frees++;
        iperf_msize -= *(int*)ptr;
        //PRINTF("iperf_free size: %u\r\n", *(int*)ptr);
        FREE(ptr);
    }
#else
    if (ptr)
        FREE(ptr);
#endif
}

/*************************** Print usage functions ****************************/

void
iperf_usage()
{
    PRINTF("Usage: iperf [-s|-c host] [options]\r\n");
    PRINTF("Try `iperf --help' for more information.\r\n");
}

void
usage_long()
{
    PRINTF("Usage: iperf3 [-s|-c host] [options]\r\n");
    PRINTF("       iperf3 [-h|--help] [-v|--version]\r\n\r\n");
    PRINTF("Server or Client:\r\n");
    PRINTF("  -p, --port      #         server port to listen on/connect to\r\n");
    PRINTF("  -i, --interval  #         seconds between periodic bandwidth reports\r\n");
    PRINTF("  -V, --verbose             more detailed output\r\n");
    PRINTF("  -d, --debug               emit debugging output\r\n");
    PRINTF("  -v, --version             show version information and quit\r\n");
    PRINTF("  -h, --help                show this message and quit\r\n");
    PRINTF("  stop                      kill the iperf threads \r\n");
    PRINTF("  list                      show the running iperf threads \r\n");
    PRINTF("Server specific:\r\n");
    PRINTF("  -s, --server              run in server mode\r\n");
    PRINTF("Client specific:\r\n");
    PRINTF("  -c, --client    <host>    run in client mode, connecting to <host>\r\n");
    PRINTF("  -u, --udp                 use UDP rather than TCP\r\n");
    PRINTF("  -b, --bandwidth #[KMG][/#] target bandwidth in bits/sec (0 for unlimited)\r\n");
    PRINTF("                            (default %d Mbit/sec for UDP, unlimited for TCP)\r\n", UDP_RATE / (1024*1024));
    PRINTF("                            (optional slash and packet count for burst mode)\r\n");
    PRINTF("  -t, --time      #         time in seconds to transmit for (default %d secs)\r\n", DURATION);
    PRINTF("  -n, --bytes     #[KMG]    number of bytes to transmit (instead of -t)\r\n");
    PRINTF("  -k, --blockcount #[KMG]   number of blocks (packets) to transmit (instead of -t or -n)\r\n");
    PRINTF("  -l, --len       #[KMG]    length of buffer to read or write\r\n");
    PRINTF("                            (default %d KB for TCP, %d Bytes for UDP)\r\n", DEFAULT_TCP_BLKSIZE/1024, DEFAULT_UDP_BLKSIZE);
    PRINTF("  -R, --reverse             run in reverse mode (server sends, client receives)\r\n");
    //PRINTF("  -w, --window    #[KMG]    set window size / socket buffer size\r\n");
    PRINTF("  -N, --no-delay            set TCP no delay, disabling Nagle's Algorithm\r\n");
    PRINTF("  -P, --parallel  #         number of parallel client streams to run\r\n");
    //PRINTF("  -O, --omit N              omit the first n seconds [0,%d]\r\n", MAX_OMIT);
    PRINTF("  -T, --title str           prefix every output line with this string\r\n");
}


void warning(char *str)
{
    PRINTF("warning: %s\r\n", str);
}


/************** Getter routines for some fields inside iperf_test *************/

int
iperf_get_verbose(struct iperf_test *ipt)
{
    return ipt->verbose;
}

int
iperf_get_control_socket(struct iperf_test *ipt)
{
    return ipt->ctrl_sck;
}

int
iperf_get_test_omit(struct iperf_test *ipt)
{
    return ipt->omit;
}

int
iperf_get_test_duration(struct iperf_test *ipt)
{
    return ipt->duration;
}

int
iperf_get_test_burst(struct iperf_test *ipt)
{
    return ipt->settings->burst;
}

char
iperf_get_test_role(struct iperf_test *ipt)
{
    return ipt->role;
}

int
iperf_get_test_reverse(struct iperf_test *ipt)
{
    return ipt->reverse;
}

int
iperf_get_test_blksize(struct iperf_test *ipt)
{
    return ipt->settings->blksize;
}

int
iperf_get_test_socket_bufsize(struct iperf_test *ipt)
{
    return ipt->settings->socket_bufsize;
}

int
iperf_get_test_num_streams(struct iperf_test *ipt)
{
    return ipt->num_streams;
}

int
iperf_get_test_server_port(struct iperf_test *ipt)
{
    return ipt->server_port;
}

char*
iperf_get_test_server_hostname(struct iperf_test *ipt)
{
    return ipt->server_hostname;
}

int
iperf_get_test_protocol_id(struct iperf_test *ipt)
{
    return ipt->protocol->id;
}


int
iperf_get_test_zerocopy(struct iperf_test *ipt)
{
    return ipt->zerocopy;
}

int
iperf_get_test_get_server_output(struct iperf_test *ipt)
{
    return ipt->get_server_output;
}

char
iperf_get_test_unit_format(struct iperf_test *ipt)
{
    return ipt->settings->unit_format;
}

char *
iperf_get_test_bind_address(struct iperf_test *ipt)
{
    return ipt->bind_address;
}

/************** Setter routines for some fields inside iperf_test *************/

void
iperf_set_verbose(struct iperf_test *ipt, int verbose)
{
    ipt->verbose = verbose;
}

void
iperf_set_control_socket(struct iperf_test *ipt, int ctrl_sck)
{
    ipt->ctrl_sck = ctrl_sck;
}

void
iperf_set_test_omit(struct iperf_test *ipt, int omit)
{
    ipt->omit = omit;
}

void
iperf_set_test_duration(struct iperf_test *ipt, int duration)
{
    ipt->duration = duration;
}

void
iperf_set_test_state(struct iperf_test *ipt, signed char state)
{
    ipt->state = state;
}

void
iperf_set_test_blksize(struct iperf_test *ipt, int blksize)
{
    ipt->settings->blksize = blksize;
}


void
iperf_set_test_burst(struct iperf_test *ipt, int burst)
{
    ipt->settings->burst = burst;
}

void
iperf_set_test_server_port(struct iperf_test *ipt, int server_port)
{
    ipt->server_port = server_port;
}

void
iperf_set_test_socket_bufsize(struct iperf_test *ipt, int socket_bufsize)
{
    ipt->settings->socket_bufsize = socket_bufsize;
}

void
iperf_set_test_num_streams(struct iperf_test *ipt, int num_streams)
{
    ipt->num_streams = num_streams;
}

static void
check_sender_has_retransmits(struct iperf_test *ipt)
{
    if (ipt->sender && ipt->protocol->id == Ptcp && has_tcpinfo_retransmits())
    	ipt->sender_has_retransmits = 1;
    else
	    ipt->sender_has_retransmits = 0;
}

void
iperf_set_test_role(struct iperf_test *ipt, char role)
{
    ipt->role = role;
    if (role == 'c')
    	ipt->sender = 1;
    else if (role == 's')
	    ipt->sender = 0;
    if (ipt->reverse)
        ipt->sender = ! ipt->sender;
    check_sender_has_retransmits(ipt);
}

char * iperf_strdup (char * source)
{
    char *str_dup;
    
    if (source == NULL)
        return NULL;

    str_dup = iperf_malloc(STRLEN(source)+1);
    if (str_dup == NULL)
        return NULL;

    STRCPY(str_dup, source);
    return str_dup;
}

void
iperf_set_test_server_hostname(struct iperf_test *ipt, char *server_hostname)
{
    ipt->server_hostname = iperf_strdup(server_hostname);
}

void
iperf_set_test_reverse(struct iperf_test *ipt, int reverse)
{
    ipt->reverse = reverse;
    if (ipt->reverse)
        ipt->sender = ! ipt->sender;
    check_sender_has_retransmits(ipt);
}

void
iperf_set_test_get_server_output(struct iperf_test *ipt, int get_server_output)
{
    ipt->get_server_output = get_server_output;
}

void
iperf_set_test_unit_format(struct iperf_test *ipt, char unit_format)
{
    ipt->settings->unit_format = unit_format;
}

void
iperf_set_test_bind_address(struct iperf_test *ipt, char *bind_address)
{
    ipt->bind_address = iperf_strdup(bind_address);
}


/********************** Get/set test protocol structure ***********************/

struct protocol *
get_protocol(struct iperf_test *test, int prot_id)
{
    struct protocol *prot;

    SLIST_FOREACH(prot, &test->protocols, protocols) {
        if (prot->id == prot_id)
            break;
    }

    if (prot == NULL)
        test->i_errno = IEPROTOCOL;

    return prot;
}

int
set_protocol(struct iperf_test *test, int prot_id)
{
    struct protocol *prot = NULL;

    SLIST_FOREACH(prot, &test->protocols, protocols) {
        if (prot->id == prot_id) {
            test->protocol = prot;
	        check_sender_has_retransmits(test);
            return 0;
        }
    }

    test->i_errno = IEPROTOCOL;
    return -1;
}


/************************** Iperf callback functions **************************/

void
iperf_on_new_stream(struct iperf_stream *sp)
{
    connect_msg(sp);
}

void
iperf_on_test_start(struct iperf_test *test)
{
    if (test->verbose) {
        if (test->settings->bytes)
        	iperf_printf(test, iperf_test_start_bytes, test->protocol->name, test->num_streams, test->settings->blksize, test->omit, test->settings->bytes);
        else if (test->settings->blocks)
        	iperf_printf(test, iperf_test_start_blocks, test->protocol->name, test->num_streams, test->settings->blksize, test->omit, test->settings->blocks);
        else
        	iperf_printf(test, iperf_test_start_time, test->protocol->name, test->num_streams, test->settings->blksize, test->omit, test->duration);
    }
}

void
iperf_on_connect(struct iperf_test *test)
{
    //time_t now_secs;
    int port = 0;
    char ipr[100];
    //struct sockaddr_storage sa;
    struct sockaddr_in sa;    
    struct sockaddr_in *sa_inP;
    socklen_t len;

    //now_secs = time((time_t*) 0);
    //(void) strftime(now_str, sizeof(now_str), rfc1123_fmt, gmtime(&now_secs));
    //if (test->verbose)
	//    iperf_printf(test, report_time, now_str);

    if (test->role == 'c') {
        iperf_printf(test, iperf_report_connecting, test->server_hostname, test->server_port);
        if (test->reverse)
    	    iperf_printf(test, iperf_report_reverse, test->server_hostname);
    } else {
        len = sizeof(sa);
        getpeername(test->ctrl_sck, (struct sockaddr *) &sa, &len);
        if (getsockdomain(test->ctrl_sck) == AF_INET) {
            ip_addr_t remote_addr;
            char *pstr;
	        sa_inP = (struct sockaddr_in *) &sa;
	        inet_addr_to_ipaddr(&remote_addr, &sa_inP->sin_addr);
            pstr = ipaddr_ntoa(&remote_addr);
            STRCPY(ipr, pstr);
	        port = ntohs(sa_inP->sin_port);
        }
	    iperf_printf(test, iperf_report_accepted, ipr, port);
    }
    if (test->verbose) {
        iperf_printf(test, iperf_report_cookie, test->cookie);
        if (test->protocol->id == SOCK_STREAM) {
            if (test->settings->mss)
                iperf_printf(test, "      TCP MSS: %d\r\n", test->settings->mss);
        }
    }
}

void
iperf_on_test_finish(struct iperf_test *test)
{
}


/******************************************************************************/
/*
    return values:
        <0, error
        0, go ahead to run client or server
        100, end of iperf
*/
int
iperf_parse_arguments(struct iperf_test *test, int argc, char **argv)
{
/*
    static struct option longopts[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"interval", required_argument, NULL, 'i'},
        {"verbose", no_argument, NULL, 'V'},
        {"version", no_argument, NULL, 'v'},
        {"server", no_argument, NULL, 's'},
        {"client", required_argument, NULL, 'c'},
        {"udp", no_argument, NULL, 'u'},
        {"parallel", required_argument, NULL, 'P'},
        {"time", required_argument, NULL, 't'},        
        {"bandwidth", required_argument, NULL, 'b'},
        {"length", required_argument, NULL, 'l'},
        {"window", required_argument, NULL, 'w'},
        {"bytes", required_argument, NULL, 'n'},
        {"blockcount", required_argument, NULL, 'k'},
        {"omit", required_argument, NULL, 'O'},
        {"title", require_argument, NULL, 'T'},
        {"no-delay", no_argument, NULL, 'N'},
        {"reverse", no_argument, NULL, 'R'},
        {"debug", no_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    */
    int i;
    int blksize;
    int server_flag, client_flag, rate_flag, duration_flag;   
    char* slash;

    blksize = 0;
    server_flag = client_flag = rate_flag = duration_flag = 0;   
    for(i=1; i<argc; i++) {
        if (STRCMP(argv[i], "-p") == 0 || STRCMP(argv[i], "--port") == 0 )
        {
            if (++i >= argc) {
                return -1;
            }
            test->server_port = ATOI(argv[i]);
            continue;
        }
        else if (STRCMP(argv[i], "-i") == 0 || STRCMP(argv[i], "--interval") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            test->stats_interval = test->reporter_interval = ATOI(argv[i]);
            if ((test->stats_interval < MIN_INTERVAL || test->stats_interval > MAX_INTERVAL) && test->stats_interval != 0) {
                test->i_errno = IEINTERVAL;
                return -1;
            }
            continue;
        }
        else if (STRCMP(argv[i], "-c") == 0 || STRCMP(argv[i], "--client") == 0 )
        {
            if (++i >= argc) {
                return -1;
            }
            if (test->role == 's') {
                test->i_errno = IESERVCLIENT;
                return -1;
            }
            iperf_set_test_role(test, 'c');
            iperf_set_test_server_hostname(test, argv[i]);
            continue;
        }
        else if (STRCMP(argv[i], "-t") == 0 || STRCMP(argv[i], "--time") == 0 )
        {
            if (++i >= argc) {
                return -1;
            }
            test->duration = ATOI(argv[i]);
            if (test->duration > MAX_TIME) {
                test->i_errno = IEDURATION;
                return -1;
            }
            duration_flag = 1;
            client_flag = 1;
            continue;
        }
        else if (STRCMP(argv[i], "-P") == 0 || STRCMP(argv[i], "--parallel") == 0 )
        {
            if (++i >= argc) {
                return -1;
            }
            test->num_streams = ATOI(argv[i]);
            if (test->num_streams > MAX_STREAMS) {
                test->i_errno = IENUMSTREAMS;
                return -1;
            }
            client_flag = 1;
            continue;
        }
        else if (STRCMP(argv[i], "-b") == 0 || STRCMP(argv[i], "--bandwidth") == 0 )
        {
            if (++i >= argc) {
                return -1;
            }
            slash = (char * )STRCHR((const char *)argv[i], (char)'/');
            if (slash) {
                *slash = '\0';
                ++slash;
                test->settings->burst = ATOI(slash);
                if (test->settings->burst <= 0 || test->settings->burst > MAX_BURST) {
                    test->i_errno = IEBURST;
                    return -1;
                }
            }
            test->settings->rate = unit_atoi_rate(argv[i]);
            if (test->settings->rate > MAX_BANDWIDTH)
            {
                test->i_errno = IEBANDWIDTH;
                return -1;
            }
    		rate_flag = 1;
            client_flag = 1;
            continue;
        }
        else if (STRCMP(argv[i], "-n") == 0 || STRCMP(argv[i], "--bytes") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            test->settings->bytes = unit_atoi_rate(argv[i]);
    		client_flag = 1;
			continue;
        }
        else if (STRCMP(argv[i], "-k") == 0 || STRCMP(argv[i], "--blockcount") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            test->settings->blocks = unit_atoi_rate(argv[i]);
    		client_flag = 1;
			continue;
        }
        /*
        else if (STRCMP(argv[i], "-O") == 0 || STRCMP(argv[i], "--omit") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            test->omit = ATOI(argv[i]);
            if (test->omit < 0 || test->omit > MAX_OMIT) {
                test->i_errno = IEOMIT;
                return -1;
            }
            client_flag = 1;
			continue;
        }*/
        else if (STRCMP(argv[i], "-T") == 0 || STRCMP(argv[i], "--title") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            test->title = iperf_strdup(argv[i]);
            client_flag = 1;
			continue;
        }
        /*
        else if (STRCMP(argv[i], "-w") == 0 || STRCMP(argv[i], "--window") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            test->settings->socket_bufsize = unit_atoi_rate(argv[i]);
            if (test->settings->socket_bufsize > MAX_TCP_BUFFER) {
                test->i_errno = IEBUFSIZE;
                return -1;
            }
            client_flag = 1;
			continue;
        }*/
        else if (STRCMP(argv[i], "-l") == 0 || STRCMP(argv[i], "--len") == 0 ){
            if (++i >= argc) {
                return -1;
            }
            blksize = unit_atoi_rate(argv[i]);
    		client_flag = 1;
			continue;
        }
        else if (STRCMP(argv[i], "-V") == 0 || STRCMP(argv[i], "--verbose") == 0){
            test->verbose = 1;
            continue;
        }
        else if (STRCMP(argv[i], "-v") == 0 || STRCMP(argv[i], "--version") == 0 ){
            PRINTF("%s\r\n", iperf_version);
            return 100;
        }
        else if (STRCMP(argv[i], "-s") == 0 || STRCMP(argv[i], "--server") == 0 ){
            if (test->role == 'c') {
                test->i_errno = IESERVCLIENT;
                return -1;
            }
            iperf_set_test_role(test, 's');
            continue;
        }
        else if (STRCMP(argv[i], "-u") == 0 || STRCMP(argv[i], "--udp") == 0 ){
            set_protocol(test, Pudp);
            client_flag = 1;
            continue;
        }
        else if (STRCMP(argv[i], "-N") == 0 || STRCMP(argv[i], "--no-delay") == 0 ){
            test->no_delay = 1;
            client_flag = 1;
            continue;
        }
        
        else if (STRCMP(argv[i], "-R") == 0 || STRCMP(argv[i], "--reverse") == 0 ){
            iperf_set_test_reverse(test, 1);
            client_flag = 1;
            continue;
        }
        else if (STRCMP(argv[i], "-d") == 0 || STRCMP(argv[i], "--debug") == 0 ){
            test->debug = 1;
            continue;
        }
        else
        {
            usage_long();
            return 100;
        }
    }

    /* Check flag / role compatibility. */
    if (test->role == 'c' && server_flag) {
    	test->i_errno = IESERVERONLY;
    	return -1;
    }
    if (test->role == 's' && client_flag) {
    	test->i_errno = IECLIENTONLY;
    	return -1;
    }

    if (!test->bind_address && test->bind_port) {
        test->i_errno = IEBIND;
        return -1;
    }
    if (blksize == 0) {
    	if (test->protocol->id == Pudp)
    	    blksize = DEFAULT_UDP_BLKSIZE;
    	else if (test->protocol->id == Psctp)
    	    blksize = DEFAULT_SCTP_BLKSIZE;
    	else
    	    blksize = DEFAULT_TCP_BLKSIZE;
    }
    if (blksize < MIN_BLOCKSIZE || blksize > MAX_BLOCKSIZE) {
    	test->i_errno = IEBLOCKSIZE;
    	return -1;
    }
    if (test->protocol->id == Pudp &&
    	blksize > MAX_UDP_BLOCKSIZE) {
    	test->i_errno = IEUDPBLOCKSIZE;
    	return -1;
    }
    test->settings->blksize = blksize;

    /*
    if (blksize >= 102400L)
    {
        test->multisend = 1;
    }
    */
    
    if (!rate_flag)
    	test->settings->rate = test->protocol->id == Pudp ? UDP_RATE : 0;

    if ((test->settings->bytes != 0 || test->settings->blocks != 0) && ! duration_flag)
        test->duration = 0;

    /* Disallow specifying multiple test end conditions. The code actually
    ** works just fine without this prohibition. As soon as any one of the
    ** three possible end conditions is met, the test ends. So this check
    ** could be removed if desired.
    */
    if ((duration_flag && test->settings->bytes != 0) ||
        (duration_flag && test->settings->blocks != 0) ||
    	(test->settings->bytes != 0 && test->settings->blocks != 0)) {
        test->i_errno = IEENDCONDITIONS;
        return -1;
    }

    if ((test->role != 'c') && (test->role != 's')) {
        test->i_errno = IENOROLE;
        return -1;
    }
        
    return 0;
}

void iperf_set_state (struct iperf_test* test, signed char state)
{
    if (test->debug)
        PRINTF("iperf state changed: %d --> %d\r\n", test->state, state);
    
    test->state = state;
}


int
iperf_set_send_state(struct iperf_test *test, signed char state)
{
    iperf_set_state(test, state);
    if (Nwrite(test->ctrl_sck, (char*) &state, sizeof(state), Ptcp) < 0) {
    	test->i_errno = IESENDMESSAGE;
    	return -1;
    }

    return 0;
}

void
iperf_check_throttle(struct iperf_stream *sp, struct timeval *nowP)
{
    s32 msec;
    u32 bits_per_second;

    struct iperf_interval_results *irp, temp;
    struct iperf_stream_result *rp = NULL;

    
    if (sp->test->done)
        return;
	
    rp = sp->result;
	
    irp = TAILQ_LAST(&rp->interval_results, irlisthead);
    if (irp != NULL)
        memcpy(&temp.interval_start_time, &rp->end_time, sizeof(struct timeval));
    else
        memcpy(&temp.interval_start_time, &rp->start_time, sizeof(struct timeval));

    msec = msecond_delta(nowP, &temp.interval_start_time);
    if (msec == 0)
        return;
    bits_per_second = sp->result->bytes_sent_this_interval * 8 * 1000 / (u32)msec;    

    if (sp->test->debug){
        //PRINTF("check_id:%ld, msec:%ld, bits_per_sec:%u \r\n", check_id++, msec, bits_per_second);
    }
    
    if (bits_per_second < sp->test->settings->rate) {
        sp->green_light = 1;
        FD_SET(sp->socket, &sp->test->write_set);
    } else {
        sp->green_light = 0;
        FD_CLR(sp->socket, &sp->test->write_set);
    }
}

int
iperf_send(struct iperf_test *test, fd_set *write_setP)
{
    register int multisend, r, streams_active;
    register struct iperf_stream *sp;
    struct timeval now;

    /* Can we do multisend mode? */
    if (test->settings->burst != 0)
        multisend = test->settings->burst;
    else if (test->settings->rate == 0)
        multisend = test->multisend;
    else
        multisend = 1;	/* nope */

    for (; multisend > 0; --multisend) {
        if (test->settings->rate != 0 && test->settings->burst == 0)
            iperf_gettimeofday(&now, NULL);

        if (test->settings->bytes != 0 && test->bytes_sent >= test->settings->bytes)
            break;
        if (test->settings->blocks != 0 && test->blocks_sent >= test->settings->blocks)
            break;

        streams_active = 0;
        SLIST_FOREACH(sp, &test->streams, streams) {
            if (sp->green_light &&
                (write_setP == NULL || FD_ISSET(sp->socket, write_setP))) {
                if ((r = sp->snd(sp)) < 0) {
                    if (r == NET_SOFTERROR)
            			break;
            	    //PRINTF("iperf_send retval=%d \r\n", r);
        		    test->i_errno = IESTREAMWRITE;
        		    return r;
        		}
        		streams_active = 1;
        		test->bytes_sent += r;
        		++test->blocks_sent;
		        if (test->settings->rate != 0 && test->settings->burst == 0)
		            iperf_check_throttle(sp, &now);
        		if (multisend > 1 && test->settings->bytes != 0 && test->bytes_sent >= test->settings->bytes)
        		    break;
        		if (multisend > 1 && test->settings->blocks != 0 && test->blocks_sent >= test->settings->blocks)
        		    break;
    	    }
    	}
    	if (!streams_active)
    	    break;
    }

    if (test->settings->burst != 0) {
    	iperf_gettimeofday(&now, NULL);
    	SLIST_FOREACH(sp, &test->streams, streams)
    	    iperf_check_throttle(sp, &now);
    }
    if (write_setP != NULL)
    	SLIST_FOREACH(sp, &test->streams, streams)
    	    if (FD_ISSET(sp->socket, write_setP))
        		FD_CLR(sp->socket, write_setP);

    return 0;
}

int
iperf_recv(struct iperf_test *test, fd_set *read_setP)
{
    int r;
    struct iperf_stream *sp;

    SLIST_FOREACH(sp, &test->streams, streams) {
    	if (FD_ISSET(sp->socket, read_setP)) {
    	    while ((r = sp->rcv(sp)) > 0) {
                test->bytes_sent += r;
                ++test->blocks_sent;
    	    }
    	    FD_CLR(sp->socket, read_setP);
            if (r == NET_SOFTERROR || r==0)  //next stream
                continue;

    		test->i_errno = IESTREAMREAD;
    		return r;
    	}
    }
    return 0;
}

int
iperf_init_test(struct iperf_test *test)
{
    struct timeval now;
    struct iperf_stream *sp;

    if (test->protocol->init) {
        if (test->protocol->init(test) < 0)
            return -1;
    }

    /* Init each stream. */
    if (iperf_gettimeofday(&now, NULL) < 0) {
    	test->i_errno = IEINITTEST;
    	return -1;
    }
    SLIST_FOREACH(sp, &test->streams, streams) {
    	sp->result->start_time = now;
    }

    if (test->on_test_start)
        test->on_test_start(test);

    return 0;
}

static void
send_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_stream *sp = client_data.p;

    /* All we do here is set or clear the flag saying that this stream may
    ** be sent to.  The actual sending gets done in the send proc, after
    ** checking the flag.
    */
    iperf_check_throttle(sp, nowP);
}

int
iperf_create_send_timers(struct iperf_test * test)
{
    struct iperf_stream *sp;
    TimerClientData cd;

    SLIST_FOREACH(sp, &test->streams, streams) {
        sp->green_light = 1;
    	if (test->settings->rate != 0) {
    	    cd.p = sp;
    	    sp->send_timer = tmr_create((struct timeval*) 0, send_timer_proc, cd, 100L, 1, test);
    	    /* (Repeat every tenth second - arbitrary often value.) */
    	    if (sp->send_timer == NULL) {
        		test->i_errno = IEINITTEST;
        		return -1;
    	    }
    	}
    }
    return 0;
}

/**
 * iperf_exchange_parameters - handles the param_Exchange part for client
 *
 */

int
iperf_exchange_parameters(struct iperf_test *test)
{
    int s;
    s32 err;

    if (test->role == 'c') {
        if (send_parameters(test) < 0)
            return -1;
    } else {
        if (get_parameters(test) < 0)
            return -1;

        if ((s = test->protocol->sock_listen(test)) < 0) {
    	    if (iperf_set_send_state(test, SERVER_ERROR) != 0)
                return -1;
            err = htonl(test->i_errno);
            if (Nwrite(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                test->i_errno = IECTRLWRITE;
                return -1;
            }
            err = htonl(test->t_errno);
            if (Nwrite(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                test->i_errno = IECTRLWRITE;
                return -1;
            }
            return -1;
        }
        FD_SET(s, &test->read_set);
        test->max_fd = (s > test->max_fd) ? s : test->max_fd;
        test->prot_listener = s;

        // Send the control message to create streams and start the test
    	if (iperf_set_send_state(test, CREATE_STREAMS) != 0)
            return -1;
    }

    return 0;
}

/*************************************************************/

int
iperf_exchange_results(struct iperf_test *test)
{
    if (test->role == 'c') {
        /* Send results to server. */
    	if (send_results(test) < 0)
            return -1;
        /* Get server results. */
        if (get_results(test) < 0)
            return -1;
    } else {
        /* Get client results. */
        if (get_results(test) < 0)
            return -1;
        /* Send results to client. */
	    if (send_results(test) < 0)
            return -1;
    }
    return 0;
}

/*************************************************************/

static int
send_parameters(struct iperf_test *test)
{
    int r = 0;
    Iperf_cJSON *j;

    j = Iperf_cJSON_CreateObject();
    if (j == NULL) {
    	test->i_errno = IESENDPARAMS;
    	r = -1;
    } else {
    	if (test->protocol->id == Ptcp)
    	    Iperf_cJSON_AddTrueToObject(j, "tcp");
    	else if (test->protocol->id == Pudp)
            Iperf_cJSON_AddTrueToObject(j, "udp");
        else if (test->protocol->id == Psctp)
            Iperf_cJSON_AddTrueToObject(j, "sctp");
    	Iperf_cJSON_AddIntToObject(j, "omit", test->omit);
    	if (test->duration)
    	    Iperf_cJSON_AddIntToObject(j, "time", test->duration);
    	if (test->settings->bytes)
    	    Iperf_cJSON_AddIntToObject(j, "num", test->settings->bytes);
    	if (test->settings->blocks)
    	    Iperf_cJSON_AddIntToObject(j, "blockcount", test->settings->blocks);
    	if (test->settings->mss)
    	    Iperf_cJSON_AddIntToObject(j, "MSS", test->settings->mss);
    	if (test->no_delay)
    	    Iperf_cJSON_AddTrueToObject(j, "nodelay");
    	Iperf_cJSON_AddIntToObject(j, "parallel", test->num_streams);
    	if (test->reverse)
    	    Iperf_cJSON_AddTrueToObject(j, "reverse");
    	if (test->settings->socket_bufsize)
    	    Iperf_cJSON_AddIntToObject(j, "window", test->settings->socket_bufsize);
    	if (test->settings->blksize)
    	    Iperf_cJSON_AddIntToObject(j, "len", test->settings->blksize);
    	if (test->settings->rate)
    	    Iperf_cJSON_AddIntToObject(j, "bandwidth", test->settings->rate);
    	if (test->settings->burst)
    	    Iperf_cJSON_AddIntToObject(j, "burst", test->settings->burst);
    	if (test->settings->tos)
    	    Iperf_cJSON_AddIntToObject(j, "TOS", test->settings->tos);
    	if (test->settings->flowlabel)
    	    Iperf_cJSON_AddIntToObject(j, "flowlabel", test->settings->flowlabel);
    	if (test->title)
    	    Iperf_cJSON_AddStringToObject(j, "title", test->title);
    	if (test->congestion)
    	    Iperf_cJSON_AddStringToObject(j, "congestion", test->congestion);
    	if (test->get_server_output)
    	    Iperf_cJSON_AddIntToObject(j, "get_server_output", iperf_get_test_get_server_output(test));

    	Iperf_cJSON_AddStringToObject(j, "client_version", IPERF_VERSION);

    	if (test->debug) {
    	    PRINTF("send_parameters:\r\n%s\r\n", Iperf_cJSON_Print(j));
    	}

    	if (JSON_write(test->ctrl_sck, j) < 0) {
    	    test->i_errno = IESENDPARAMS;
    	    r = -1;
    	}
    	Iperf_cJSON_Delete(j);
    }
    return r;
}

/*************************************************************/
static int
get_parameters(struct iperf_test *test)
{
    int r = 0;
    Iperf_cJSON *j;
    Iperf_cJSON *j_p;

    j = JSON_read(test->ctrl_sck);
    if (j == NULL) {
    	test->i_errno = IERECVPARAMS;
        r = -1;
    } else {
    	if (test->debug) {
    	    PRINTF("get_parameters:\r\n%s\r\n", Iperf_cJSON_Print(j));
    	}

    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "tcp")) != NULL)
    	    set_protocol(test, Ptcp);
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "udp")) != NULL)
    	    set_protocol(test, Pudp);
        if ((j_p = Iperf_cJSON_GetObjectItem(j, "sctp")) != NULL)
        {
            #if HAVE_SCTP
            set_protocol(test, Psctp);
            #else
            PRINTF("NOT support SCTP yet!\r\n");
            test->i_errno = IERECVPARAMS;
            Iperf_cJSON_Delete(j);
            return -1;
            #endif
        }
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "omit")) != NULL){
    	    test->omit = j_p->valueint;
    	    if (test->omit < 0 || test->omit > MAX_OMIT){
   	            PRINTF("omit time range: [0,%d] \r\n", MAX_OMIT);
                test->i_errno = IERECVPARAMS;
                Iperf_cJSON_Delete(j);
                return -1;
    	    }
    	}
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "time")) != NULL)
    	    test->duration = j_p->valueint;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "num")) != NULL)
    	    test->settings->bytes = j_p->valueint;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "blockcount")) != NULL)
    	    test->settings->blocks = j_p->valueint;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "MSS")) != NULL) 
    	{
            #if HAVE_TCP_MSS
            test->settings->mss = j_p->valueint;
            #else
            PRINTF("NOT support setting MSS option!\r\n");
            test->i_errno = IERECVPARAMS;
            Iperf_cJSON_Delete(j);
            return -1;
            #endif    	    
    	}
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "nodelay")) != NULL)
    	    test->no_delay = 1;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "parallel")) != NULL)	{
    	    test->num_streams = j_p->valueint;
    	    if (test->num_streams > MAX_STREAMS)
    	    {
   	            PRINTF("max streams in parallel allowed: %d \r\n", MAX_STREAMS);
                test->i_errno = IERECVPARAMS;
                Iperf_cJSON_Delete(j);
                return -1;
    	    }
    	}
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "reverse")) != NULL){
#if 0
    	    {
   	            PRINTF("NOT support reverse option in server mode \r\n");
                test->i_errno = IERECVPARAMS;
                cJSON_Delete(j);
                return -1;
    	    }
#endif
    	    iperf_set_test_reverse(test, 1);    //there is still a bug yet!
    	}
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "window")) != NULL){    	    
    	    test->settings->socket_bufsize = j_p->valueint;
    	    if (test->settings->socket_bufsize > MAX_TCP_BUFFER)
    	    {
   	            PRINTF("max tcp window: %d \r\n", MAX_TCP_BUFFER);
                test->i_errno = IERECVPARAMS;
                Iperf_cJSON_Delete(j);
                return -1;
    	    }
    	}
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "len")) != NULL){
    	    test->settings->blksize = j_p->valueint;
    	    if (test->settings->blksize > MAX_BLOCKSIZE){
   	            PRINTF("max block size allowed: %d \r\n", MAX_BLOCKSIZE);
    	        test->settings->blksize = MAX_BLOCKSIZE;
    	    }else if (test->settings->blksize < MIN_BLOCKSIZE){
   	            PRINTF("min block size allowed: %d \r\n", MIN_BLOCKSIZE);
    	        test->settings->blksize = MIN_BLOCKSIZE;
    	    }
        }
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "bandwidth")) != NULL)
    	    test->settings->rate = j_p->valueint;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "burst")) != NULL){    	
    	    test->settings->burst = j_p->valueint; 
    	    if (test->settings->burst > MAX_BURST || test->settings->burst <= 0){
   	            PRINTF("burst range: [1, %d] \r\n", MAX_BURST);
                test->i_errno = IERECVPARAMS;
                Iperf_cJSON_Delete(j);
                return -1;
            }
    	}
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "TOS")) != NULL)
    	    test->settings->tos = j_p->valueint;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "flowlabel")) != NULL)  //for ipv6
    	    test->settings->flowlabel = j_p->valueint;
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "title")) != NULL)
    	    test->title = iperf_strdup(j_p->valuestring);
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "congestion")) != NULL)  //not used ?
    	    test->congestion = iperf_strdup(j_p->valuestring);
    	if ((j_p = Iperf_cJSON_GetObjectItem(j, "get_server_output")) != NULL)
    	    iperf_set_test_get_server_output(test, 1);
    	if (test->sender && test->protocol->id == Ptcp && has_tcpinfo_retransmits())
    	    test->sender_has_retransmits = 1;
    	Iperf_cJSON_Delete(j);
    }

    return r;
}

/*************************************************************/
static int
send_results(struct iperf_test *test)
{
    int r = 0;
    Iperf_cJSON *j;
    Iperf_cJSON *j_streams;
    struct iperf_stream *sp;
    Iperf_cJSON *j_stream;
    int sender_has_retransmits;
    iperf_size_t bytes_transferred;
    int retransmits;

    j = Iperf_cJSON_CreateObject();
    if (j == NULL) {
    	test->i_errno = IEPACKAGERESULTS;
    	r = -1;
    } else {
	    Iperf_cJSON_AddStringToObject(j, "cpu_util_total", "15.98");
	    Iperf_cJSON_AddStringToObject(j, "cpu_util_user", "10.0");
	    Iperf_cJSON_AddStringToObject(j, "cpu_util_system", "5.98");
	    if ( ! test->sender )
	        sender_has_retransmits = -1;
	    else
	        sender_has_retransmits = test->sender_has_retransmits;
     	Iperf_cJSON_AddIntToObject(j, "sender_has_retransmits", sender_has_retransmits);

    	/* If on the server and sending server output, then do this */
    	if (test->role == 's' && test->get_server_output) {
    		/* Add textual output */
    		size_t buflen = 0;
            char *output;
            
    		/* Figure out how much room we need to hold the complete output string */
    		struct iperf_textline *t;
    		TAILQ_FOREACH(t, &(test->server_output_list), textlineentries) {
    		    buflen += STRLEN(t->line);
    		}

    		/* Allocate and build it up from the component lines */
    		output = iperf_malloc(buflen + 1);
		    if (output == NULL) {
                test->i_errno = IEPACKAGERESULTS;
                r = -1;
            }
    		MEMSET(output, 0, sizeof(buflen+1));
    		TAILQ_FOREACH(t, &(test->server_output_list), textlineentries) {
    		    STRNCAT(output, t->line, buflen);
    		    buflen -= STRLEN(t->line);
    		}

    		Iperf_cJSON_AddStringToObject(j, "server_output_text", output);
    	}

    	j_streams = Iperf_cJSON_CreateArray();
    	if (j_streams == NULL) {
    	    test->i_errno = IEPACKAGERESULTS;
    	    r = -1;
    	} else {
    	    Iperf_cJSON_AddItemToObject(j, "streams", j_streams);
    	    SLIST_FOREACH(sp, &test->streams, streams) {
        		j_stream = Iperf_cJSON_CreateObject();
        		if (j_stream == NULL) {
        		    test->i_errno = IEPACKAGERESULTS;
        		    r = -1;
        		} else {
        		    Iperf_cJSON_AddItemToArray(j_streams, j_stream);
        		    bytes_transferred = test->sender ? sp->result->bytes_sent : sp->result->bytes_received;
        		    retransmits = (test->sender && test->sender_has_retransmits) ? sp->result->stream_retrans : -1;
        		    Iperf_cJSON_AddIntToObject(j_stream, "id", sp->id);
        		    Iperf_cJSON_AddIntToObject(j_stream, "bytes", bytes_transferred);
        		    Iperf_cJSON_AddIntToObject(j_stream, "retransmits", retransmits);
        		    if (sp->packet_count > sp->cnt_error)
    		    #ifdef IPERF_SUPPORT_DOUBLE    		    //in seconds
                        Iperf_cJSON_AddFloatToObject(j_stream, "jitter", sp->jitter/(1000*(sp->packet_count - sp->cnt_error)));
    		    #else                                   //in ms, otherwise jitter would be zero.
            		    Iperf_cJSON_AddIntToObject(j_stream, "jitter", sp->jitter/(sp->packet_count - sp->cnt_error));
        		#endif
            		else
            		    Iperf_cJSON_AddIntToObject(j_stream, "jitter", 0);
        		    Iperf_cJSON_AddIntToObject(j_stream, "errors", sp->cnt_error);
        		    Iperf_cJSON_AddIntToObject(j_stream, "packets", sp->packet_count);
        		}
    	    }
    	    if (r == 0 && test->debug) {
        		PRINTF("send_results\r\n%s\r\n", Iperf_cJSON_Print(j));
    	    }
    	    if (r == 0 && JSON_write(test->ctrl_sck, j) < 0) {
        		test->i_errno = IESENDRESULTS;
        		r = -1;
    	    }
    	}
    	Iperf_cJSON_Delete(j);
    }
    return r;
}
/*************************************************************/

static int
get_results(struct iperf_test *test)
{
    int r = 0;
    Iperf_cJSON *j;
    Iperf_cJSON *j_sender_has_retransmits;
    int result_has_retransmits;
    Iperf_cJSON *j_streams;
    int n, i;
    Iperf_cJSON *j_stream;
    Iperf_cJSON *j_id;
    Iperf_cJSON *j_bytes;
    Iperf_cJSON *j_retransmits;
    Iperf_cJSON *j_jitter;
    Iperf_cJSON *j_errors;
    Iperf_cJSON *j_packets;
    Iperf_cJSON *j_server_output;
    int sid, cerror, pcount;
    iperf_jit_t jitter;
    iperf_size_t bytes_transferred;
    int retransmits;
    struct iperf_stream *sp;

    j = JSON_read(test->ctrl_sck);
    if (j == NULL) {
    	test->i_errno = IERECVRESULTS;
        r = -1;
    } else {
    	j_sender_has_retransmits = Iperf_cJSON_GetObjectItem(j, "sender_has_retransmits");
    	if (j_sender_has_retransmits == NULL) {
    	    test->i_errno = IERECVRESULTS;
    	    r = -1;
    	} else {
    	    if (test->debug) {
        		PRINTF("get_results:\r\n%s\r\n", Iperf_cJSON_Print(j));
    	    }

    	    result_has_retransmits = j_sender_has_retransmits->valueint;
    	    if (! test->sender)
        		test->sender_has_retransmits = result_has_retransmits;
    	    j_streams = Iperf_cJSON_GetObjectItem(j, "streams");
    	    if (j_streams == NULL) {
        		test->i_errno = IERECVRESULTS;
        		r = -1;
    	    } else {
    	        n = Iperf_cJSON_GetArraySize(j_streams);
        		for (i=0; i<n; ++i) {
        		    j_stream = Iperf_cJSON_GetArrayItem(j_streams, i);
        		    if (j_stream == NULL) {
            			test->i_errno = IERECVRESULTS;
            			r = -1;
        		    } else {
            			j_id = Iperf_cJSON_GetObjectItem(j_stream, "id");
            			j_bytes = Iperf_cJSON_GetObjectItem(j_stream, "bytes");
            			j_retransmits = Iperf_cJSON_GetObjectItem(j_stream, "retransmits");
            			j_jitter = Iperf_cJSON_GetObjectItem(j_stream, "jitter");
            			j_errors = Iperf_cJSON_GetObjectItem(j_stream, "errors");
            			j_packets = Iperf_cJSON_GetObjectItem(j_stream, "packets");
            			if (j_id == NULL || j_bytes == NULL || j_retransmits == NULL || j_jitter == NULL || j_errors == NULL || j_packets == NULL) {
            			    test->i_errno = IERECVRESULTS;
            			    r = -1;
            			} else {
            			    sid = j_id->valueint;
            			    bytes_transferred = j_bytes->valueint;
            			    retransmits = j_retransmits->valueint;
            			    if (test->sender) {
                		        #ifdef IPERF_SUPPORT_DOUBLE
                			    jitter = j_jitter->valuefloat*1000.0;  //get in seconds, print in ms.
                			    #else
                			    jitter = 0;
                			    #endif
                			}
            			    cerror = j_errors->valueint;
            			    pcount = j_packets->valueint;
            			    SLIST_FOREACH(sp, &test->streams, streams)
            				if (sp->id == sid) break;
            			    if (sp == NULL) {
                				test->i_errno = IESTREAMID;
                				r = -1;
            			    } else {
                				if (test->sender) {
                				    sp->jitter = jitter;
                				    sp->cnt_error = cerror;
                				    sp->packet_count = pcount;
                				    sp->result->bytes_received = bytes_transferred;
                				} else {
                				    sp->result->bytes_sent = bytes_transferred;
                				    sp->result->stream_retrans = retransmits;
                				}
            			    }
            			}
        		    }
        		}
        		/*
            		 * If we're the client and we're supposed to get remote results,
            		 * look them up and process accordingly.
            		 */
        		if (test->role == 'c' && iperf_get_test_get_server_output(test)) {
        		    /* Look for JSON.  If we find it, grab the object so it doesn't get deleted. */
        		    j_server_output = Iperf_cJSON_DetachItemFromObject(j, "server_output_json");
        		    if (j_server_output != NULL) {
            			test->json_server_output = j_server_output;
        		    }
        		    else {
            			/* No JSON, look for textual output.  Make a copy of the text for later. */
            			j_server_output = Iperf_cJSON_GetObjectItem(j, "server_output_text");
            			if (j_server_output != NULL) {
            			    test->server_output_text = iperf_strdup(j_server_output->valuestring);
            			}
        		    }
        		}
    	    }
    	}
    	Iperf_cJSON_Delete(j);
    }
    return r;
}

/*************************************************************/

static int
JSON_write(int fd, Iperf_cJSON *json)
{
    u32 hsize, nsize;
    char *str;
    int r = 0;

    str = Iperf_cJSON_PrintUnformatted(json);

    if (str == NULL)
    {
        PRINTF("cJSON_PrintUnformatted failed\r\n");
    	r = -1;
    }
    else {
    	hsize = STRLEN(str);
    	nsize = htonl(hsize);
    	if (Nwrite(fd, (char*) &nsize, sizeof(nsize), Ptcp) < 0)
    	    r = -1;
    	else {
    	    if (Nwrite(fd, str, hsize, Ptcp) < 0)
    		r = -1;
    	}
    	iperf_free(str);
    }
    
    return r;
}

/*************************************************************/

static Iperf_cJSON *
JSON_read(int fd)
{
    u32 hsize, nsize;
    char *str;
    Iperf_cJSON *json = NULL;

    if (Nread(fd, (char*) &nsize, sizeof(nsize), Ptcp) >= 0) {
    	hsize = ntohl(nsize);
    	str = (char *) iperf_malloc(hsize+1);	/* +1 for EOS */
    	if (str != NULL) {
    	    if (Nread(fd, str, hsize, Ptcp) >= 0) {
        		str[hsize] = '\0';	/* add the EOS */
        		json = Iperf_cJSON_Parse(str);
    	    }
            iperf_free(str);
    	}
    }
    return json;
}

/*************************************************************/
/**
 * add_to_interval_list -- adds new interval to the interval_list
 */

void
add_to_interval_list(struct iperf_stream_result * rp, struct iperf_interval_results * new)
{
    struct iperf_interval_results *irp;

    irp = (struct iperf_interval_results *) iperf_malloc(sizeof(struct iperf_interval_results));
    if (irp != NULL){
        MEMCPY(irp, new, sizeof(struct iperf_interval_results));
        TAILQ_INSERT_TAIL(&rp->interval_results, irp, irlistentries);
    }
}


/************************************************************/

/**
 * connect_msg -- displays connection message
 * denoting sender/receiver details
 *
 */

void
connect_msg(struct iperf_stream *sp)
{
    char ipl[100], ipr[100], *pstr;
    int lport = 0, rport = 0;

    if (getsockdomain(sp->socket) == AF_INET) {
        ip_addr_t ipv4_addr;
         
        inet_addr_to_ipaddr(&ipv4_addr, &((struct sockaddr_in *) &sp->local_addr)->sin_addr);
        pstr = ipaddr_ntoa(&ipv4_addr);
        STRCPY(ipl, pstr);

        inet_addr_to_ipaddr(&ipv4_addr, &((struct sockaddr_in *) &sp->remote_addr)->sin_addr);
        pstr = ipaddr_ntoa(&ipv4_addr);
        STRCPY(ipr, pstr);
        
        lport = ntohs(((struct sockaddr_in *) &sp->local_addr)->sin_port);
        rport = ntohs(((struct sockaddr_in *) &sp->remote_addr)->sin_port);
    }

	iperf_printf(sp->test, iperf_report_connected, sp->socket, ipl, lport, ipr, rport);
}


/**************************************************************************/

struct iperf_test *
iperf_new_test()
{
    struct iperf_test *test;

    test = (struct iperf_test *) iperf_malloc(sizeof(struct iperf_test));
    if (!test) {
        return NULL;
    }
    /* initialize everything to zero */
    MEMSET(test, 0, sizeof(struct iperf_test));

    test->settings = (struct iperf_settings *) iperf_malloc(sizeof(struct iperf_settings));
    if (!test->settings) {
        iperf_free(test);
	    return NULL;
    }
    MEMSET(test->settings, 0, sizeof(struct iperf_settings));
    test->listener = test->prot_listener = test->ctrl_sck = -1;
    
    return test;
}

/**************************************************************************/

struct protocol *
protocol_new(void)
{
    struct protocol *proto;

    proto = iperf_malloc(sizeof(struct protocol));
    if(proto != NULL)
        MEMSET(proto, 0, sizeof(struct protocol));

    return proto;
}

void
protocol_free(struct protocol *proto)
{
    iperf_free(proto); 
}

/**************************************************************************/
int
iperf_defaults(struct iperf_test *testp)
{
    struct protocol *tcp, *udp;

    testp->omit = OMIT;
    testp->duration = DURATION;
	
    testp->title = NULL;
    testp->congestion = NULL;
    testp->server_port = PORT;
    testp->ctrl_sck = -1;
    testp->prot_listener = -1;

    testp->stats_callback = iperf_stats_callback;
    testp->reporter_callback = iperf_reporter_callback;

    testp->stats_interval = testp->reporter_interval = 1;
    testp->num_streams = 1;
    testp->no_delay = 0;

    testp->settings->domain = AF_INET;  //AF_UNSPEC;
    testp->settings->unit_format = 'a';
    testp->settings->socket_bufsize = 0;    /* use autotuning */
    testp->settings->blksize = DEFAULT_TCP_BLKSIZE;
    testp->settings->burst = 0;
    testp->settings->mss = 0;
    testp->settings->bytes = 0;
    testp->settings->blocks = 0;
    MEMSET(testp->cookie, 0, COOKIE_SIZE);

    testp->multisend = DEFAULT_MULTI_SEND;	/* arbitrary */

    /* Set up protocol list */
    SLIST_INIT(&testp->streams);
    SLIST_INIT(&testp->protocols);

    tcp = protocol_new();
    if (!tcp)
        return -1;

    tcp->id = Ptcp;
    tcp->name = "TCP";
    tcp->sock_accept = iperf_tcp_accept;
    tcp->sock_listen = iperf_tcp_listen;
    tcp->sock_connect = iperf_tcp_connect;
    tcp->sock_send = iperf_tcp_send;
    tcp->sock_recv = iperf_tcp_recv;
    tcp->init = NULL;
    SLIST_INSERT_HEAD(&testp->protocols, tcp, protocols);

    udp = protocol_new();
    if (!udp) {
        protocol_free(tcp);
        return -1;
    }

    udp->id = Pudp;
    udp->name = "UDP";
    udp->sock_accept = iperf_udp_accept;
    udp->sock_listen = iperf_udp_listen;
    udp->sock_connect = iperf_udp_connect;
    udp->sock_send = iperf_udp_send;
    udp->sock_recv = iperf_udp_recv;
    udp->init = iperf_udp_init;
    SLIST_INSERT_AFTER(tcp, udp, protocols);

    set_protocol(testp, Ptcp);

    testp->on_new_stream = iperf_on_new_stream;
    testp->on_test_start = iperf_on_test_start;
    testp->on_connect = iperf_on_connect;
    testp->on_test_finish = iperf_on_test_finish;

    TAILQ_INIT(&testp->server_output_list);

    return 0;
}


/**************************************************************************/
void
iperf_free_test(struct iperf_test *test)
{
    struct protocol *prot;
    struct iperf_stream *sp;
    struct iperf_textline *t;
    
    /* iperf_free streams */
    while (!SLIST_EMPTY(&test->streams)) {
        sp = SLIST_FIRST(&test->streams);
        SLIST_REMOVE_HEAD(&test->streams, streams);	
        iperf_free_stream(sp);
    }

    if (test->server_hostname)
	    iperf_free(test->server_hostname);
    if (test->bind_address)
	    iperf_free(test->bind_address);

    iperf_free(test->settings);
    if (test->title)
	    iperf_free(test->title);
    if (test->congestion)
	    iperf_free(test->congestion);
    if (test->omit_timer != NULL)
	    tmr_cancel(test->omit_timer, test);
    if (test->timer != NULL)
	    tmr_cancel(test->timer, test);
    if (test->stats_timer != NULL)
	    tmr_cancel(test->stats_timer, test);
    if (test->reporter_timer != NULL)
	    tmr_cancel(test->reporter_timer, test);
    tmr_destroy(test);

    /* iperf_free protocol list */
    while (!SLIST_EMPTY(&test->protocols)) {
        prot = SLIST_FIRST(&test->protocols);
        SLIST_REMOVE_HEAD(&test->protocols, protocols);        
        iperf_free(prot);
    }

    if (test->server_output_text) {
	    iperf_free(test->server_output_text);
	    test->server_output_text = NULL;
    }

    /* iperf_free output line buffers, if any (on the server only) */    
    while (!TAILQ_EMPTY(&test->server_output_list)) {
    	t = TAILQ_FIRST(&test->server_output_list);
    	TAILQ_REMOVE(&test->server_output_list, t, textlineentries);
    	if (t->line)
    	    iperf_free(t->line);
    	iperf_free(t);
    }

    /* XXX: Why are we setting these values to NULL? */
    // test->streams = NULL;
    test->stats_callback = NULL;
    test->reporter_callback = NULL;

    if (test->ctrl_sck >= 0)
        close (test->ctrl_sck);

    if (test->listener >= 0)
        close (test->listener);

    if (test->prot_listener >= 0)
        close(test->prot_listener);    

    iperf_free(test);
}


void
iperf_reset_test(struct iperf_test *test)
{
    struct iperf_stream *sp;
    struct iperf_textline *t;

    /* iperf_free streams */
    while (!SLIST_EMPTY(&test->streams)) {
        sp = SLIST_FIRST(&test->streams);
        SLIST_REMOVE_HEAD(&test->streams, streams);
        iperf_free_stream(sp);
    }
    if (test->omit_timer != NULL) {
    	tmr_cancel(test->omit_timer, test);
    	test->omit_timer = NULL;
    }
    if (test->timer != NULL) {
    	tmr_cancel(test->timer, test);
    	test->timer = NULL;
    }
    if (test->stats_timer != NULL) {
    	tmr_cancel(test->stats_timer, test);
    	test->stats_timer = NULL;
    }
    if (test->reporter_timer != NULL) {
    	tmr_cancel(test->reporter_timer, test);
    	test->reporter_timer = NULL;
    }
    tmr_destroy(test);
    
    test->done = 0;
    test->periods = 0;
    
    SLIST_INIT(&test->streams);

    test->role = 's';
    test->sender = 0;
    test->sender_has_retransmits = 0;
    set_protocol(test, Ptcp);
    test->omit = OMIT;
    test->duration = DURATION;
    test->title = NULL;
    iperf_set_state(test, 0);
    
	if (test->ctrl_sck >= 0)
		close(test->ctrl_sck);
	if (test->listener >= 0)
		close(test->listener);
	if (test->prot_listener >= 0)
	    close(test->prot_listener);
	
    test->ctrl_sck = -1;
    test->listener = -1;
    test->prot_listener = -1;

    test->bytes_sent = 0;
    test->blocks_sent = 0;

    test->reverse = 0;
    test->no_delay = 0;

    FD_ZERO(&test->read_set);
    FD_ZERO(&test->write_set);
    
    test->num_streams = 1;
    test->settings->socket_bufsize = 0;
    test->settings->blksize = DEFAULT_TCP_BLKSIZE;
    test->settings->burst = 0;
    test->settings->mss = 0;
    MEMSET(test->cookie, 0, COOKIE_SIZE);
    test->multisend = DEFAULT_MULTI_SEND;	/* arbitrary */

    /* iperf_free output line buffers, if any (on the server only) */
    while (!TAILQ_EMPTY(&test->server_output_list)) {
    	t = TAILQ_FIRST(&test->server_output_list);
    	TAILQ_REMOVE(&test->server_output_list, t, textlineentries);
    	iperf_free((void*)t->line);
    	iperf_free((void*)t);
    }
}

/* Reset all of a test's stats back to zero.  Called when the omitting
** period is over.
*/
void
iperf_reset_stats(struct iperf_test *test)
{
    struct timeval now;
    struct iperf_stream *sp;
    struct iperf_stream_result *rp;

    test->bytes_sent = 0;
    test->blocks_sent = 0;
    iperf_gettimeofday(&now, NULL);
    SLIST_FOREACH(sp, &test->streams, streams) {
        sp->omitted_packet_count = sp->packet_count;
        sp->jitter = 0;
        sp->outoforder_packets = 0;
        sp->cnt_error = 0;
        rp = sp->result;
        rp->bytes_sent = rp->bytes_received = 0;
        rp->bytes_sent_this_interval = rp->bytes_received_this_interval = 0;
        if (test->sender && test->sender_has_retransmits) {
            struct iperf_interval_results ir; /* temporary results structure */
            save_tcpinfo(sp, &ir);
            rp->stream_prev_total_retrans = get_total_retransmits(&ir);
        }
        rp->stream_retrans = 0;
        rp->start_time = now;
    }
}

/**************************************************************************/

/**
 * Gather statistics during a test.
 * This function works for both the client and server side.
 */
void
iperf_stats_callback(struct iperf_test *test)
{
    struct iperf_stream *sp;
    struct iperf_stream_result *rp = NULL;
    struct iperf_interval_results *irp, temp;

    if (test->state == TEST_RUNNING || test->state == TEST_END)
        test->periods++ ;

    temp.omitted = test->omitting;    
    SLIST_FOREACH(sp, &test->streams, streams) {
        rp = sp->result;
        
    	temp.bytes_transferred = test->sender ? rp->bytes_sent_this_interval : rp->bytes_received_this_interval;

     	irp = TAILQ_LAST(&rp->interval_results, irlisthead);
        /* result->end_time contains timestamp of previous interval */
        if ( irp != NULL ) /* not the 1st interval */
            memcpy(&temp.interval_start_time, &rp->end_time, sizeof(struct timeval));
        else /* or use timestamp from beginning */
            memcpy(&temp.interval_start_time, &rp->start_time, sizeof(struct timeval));
        
        iperf_gettimeofday(&rp->end_time, NULL);
        memcpy(&temp.interval_end_time, &rp->end_time, sizeof(struct timeval));

    	temp.end_time = test->periods;
        
        temp.interval_duration = timeval_diff(&temp.interval_start_time, &temp.interval_end_time);

        //PRINTF("temp=%u.%u   %u.%u   %u\r\n",temp.interval_start_time.tv_sec, temp.interval_start_time.tv_usec,
         //   temp.interval_end_time.tv_sec, temp.interval_end_time.tv_usec , temp.interval_duration);
        //temp.interval_duration = test->stats_interval;
    	if (test->protocol->id == Ptcp) {
    	    if ( has_tcpinfo()) {
    		    save_tcpinfo(sp, &temp);
        		if (test->sender && test->sender_has_retransmits) {
        		    long total_retrans = get_total_retransmits(&temp);
        		    temp.interval_retrans = total_retrans - rp->stream_prev_total_retrans;
        		    rp->stream_retrans += temp.interval_retrans;
        		    rp->stream_prev_total_retrans = total_retrans;

        		    temp.snd_cwnd = get_snd_cwnd(&temp);
        		    if (temp.snd_cwnd > rp->stream_max_snd_cwnd) {
        			    rp->stream_max_snd_cwnd = temp.snd_cwnd;
        		    }
        		    
        		    temp.rtt = get_rtt(&temp);
        		    if (temp.rtt > rp->stream_max_rtt) {
        			    rp->stream_max_rtt = temp.rtt;
        		    }
        		    if (rp->stream_min_rtt == 0 ||
            			temp.rtt < rp->stream_min_rtt) {
            			rp->stream_min_rtt = temp.rtt;
        		    }
        		    rp->stream_sum_rtt += temp.rtt;
        		    rp->stream_count_rtt++;
        		}
    	    }
    	} else {
            irp = TAILQ_LAST(&rp->interval_results, irlisthead);
    	    if (irp == NULL) {
        		temp.interval_packet_count = sp->packet_count;
        		temp.interval_outoforder_packets = sp->outoforder_packets;
        		temp.interval_cnt_error = sp->cnt_error;
        		if (temp.interval_packet_count - temp.interval_cnt_error > 0)
        		    temp.jitter = sp->jitter/(temp.interval_packet_count - temp.interval_cnt_error);
        		else
        		    temp.jitter = 0;
    	    } else {
        		temp.interval_packet_count = sp->packet_count - irp->packet_count;
        		temp.interval_outoforder_packets = sp->outoforder_packets - irp->outoforder_packets;
        		temp.interval_cnt_error = sp->cnt_error - irp->cnt_error;
        		if (temp.interval_packet_count - temp.interval_cnt_error > 0)
            		temp.jitter = (sp->jitter - irp->total_jitter)/(temp.interval_packet_count - temp.interval_cnt_error);
        		else
        		    temp.jitter = 0;
    	    }

    	    if (test->debug){
        	    //PRINTF("sp->jit:%ld, tmp.packet count:%ld\r\n", sp->jitter, temp.interval_packet_count);
        	    //if (irp){
        	    //    PRINTF("irp->jit:%ld, irp->packet cout:%ld\r\n", irp->jitter, irp->interval_packet_count);
        	    //}
        	    //PRINTF("jitter: %ld\r\n", temp.jitter);
        	}
    	    temp.packet_count = sp->packet_count;
    	    temp.outoforder_packets = sp->outoforder_packets;
    	    temp.cnt_error = sp->cnt_error;
    	    temp.total_jitter = sp->jitter;
    	}
        add_to_interval_list(rp, &temp);
        rp->bytes_sent_this_interval = rp->bytes_received_this_interval = 0;
        //iperf_gettimeofday(&rp->start_time, NULL);
    }
}

/**
 * Print intermediate results during a test (interval report).
 * Uses print_interval_results to print the results for each stream,
 * then prints an interval summary for all streams in this
 * interval.
 */
static void
iperf_print_intermediate(struct iperf_test *test)
{
    char ubuf[UNIT_LEN];
    char nbuf[UNIT_LEN];
    struct iperf_stream *sp = NULL;
    struct iperf_interval_results *irp;
    iperf_size_t bytes = 0;
    u32 bandwidth;
    int retransmits = 0;
    u32 start_time, end_time;
    int total_packets = 0, lost_packets = 0;
    s32 lost_percent;
    iperf_jit_t avg_jitter = 0;

    SLIST_FOREACH(sp, &test->streams, streams) {
        print_interval_results(test, sp, NULL);

    	/* sum up all streams */
	    irp = TAILQ_LAST(&sp->result->interval_results, irlisthead);
	    if (irp == NULL) {
	        PRINTF("iperf_print_intermediate error: interval_results is NULL \r\n");
	        return;
	    }
        bytes += irp->bytes_transferred;
	    if (test->protocol->id == Ptcp) {
	        if (test->sender && test->sender_has_retransmits) {
    		    retransmits += irp->interval_retrans;
	        }
	    } else {
            total_packets += irp->interval_packet_count;
            lost_packets += irp->interval_cnt_error;
            avg_jitter += irp->jitter;
        }
    }
    
    /* next build string with sum of all streams */
    if (test->num_streams > 1) {
        sp = SLIST_FIRST(&test->streams); /* reset back to 1st stream */
       	/* Only do this of course if there was a first stream */
       	if (sp) {
            irp = TAILQ_LAST(&sp->result->interval_results, irlisthead);    /* use 1st stream for timing info */

            unit_snprintf(ubuf, UNIT_LEN, bytes, 'A');
            bandwidth = (irp->interval_duration > 0) ? (bytes * 1000 / irp->interval_duration) : 0;
            unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);

            start_time = (irp->end_time - 1) * test->stats_interval;
            end_time = irp->end_time * test->stats_interval;
        	if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
        	    if (test->sender && test->sender_has_retransmits) {
        	        /* Interval sum, TCP with retransmits. */
    		        iperf_printf(test, iperf_report_sum_bw_retrans_format, start_time, end_time, ubuf, nbuf, retransmits, irp->omitted?iperf_report_omitted:""); /* XXX irp->omitted or test->omitting? */
        	    } else {
        		    /* Interval sum, TCP without retransmits. */
    		        iperf_printf(test, iperf_report_sum_bw_format, start_time, end_time, ubuf, nbuf, test->omitting?iperf_report_omitted:"");
        	    }
        	} else {
        	    /* Interval sum, UDP. */
        	    if (test->sender) {
    		        iperf_printf(test, iperf_report_sum_bw_udp_sender_format, start_time, end_time, ubuf, nbuf, total_packets, test->omitting?iperf_report_omitted:"");
        	    } else {
            		avg_jitter /= test->num_streams;
            		lost_percent = (total_packets > 0) ? (100 * lost_packets / total_packets) : 0;
	                iperf_printf(test, iperf_report_sum_bw_udp_format, start_time, end_time, ubuf, nbuf, avg_jitter, lost_packets, total_packets, lost_percent, test->omitting?iperf_report_omitted:"");
        	    }
        	}
    	}
    }

    /// free the memory once it's not used any futher!!!
    SLIST_FOREACH(sp, &test->streams, streams) {
        irp = TAILQ_LAST(&sp->result->interval_results, irlisthead);
        if (irp != NULL)
        {
            struct iperf_interval_results *irp_tmp;
            irp_tmp = TAILQ_PREV(irp, irlisthead, irlistentries);
            if (irp_tmp != NULL)
            {
                TAILQ_REMOVE(&sp->result->interval_results, irp_tmp, irlistentries);
                iperf_free(irp_tmp);
            }
        }
    }
}

/**
 * Print overall summary statistics at the end of a test.
 */
static void
iperf_print_results(struct iperf_test *test)
{

    int total_retransmits = 0;
    int total_packets = 0, lost_packets = 0;
    char ubuf[UNIT_LEN];
    char nbuf[UNIT_LEN];
    struct iperf_stream *sp = NULL;
    iperf_size_t bytes_sent, total_sent = 0;
    iperf_size_t bytes_received, total_received = 0;
    u32 start_time = 0, end_time = 0, real_end_time = 0, lost_percent = 0;
    iperf_jit_t avg_jitter = 0;
    u32   bandwidth;

    /* print final summary for all intervals */
	iperf_printf(test, "%s", iperf_report_bw_separator);
	if (test->verbose)
	    iperf_printf(test, "%s", iperf_report_summary);
	if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
	    if (test->sender_has_retransmits)
	    	iperf_printf(test, "%s", iperf_report_bw_retrans_header);
	    else
    		iperf_printf(test, "%s", iperf_report_bw_header);
	} else
	    iperf_printf(test, "%s", iperf_report_bw_udp_header);

    start_time = 0;
    sp = SLIST_FIRST(&test->streams);

    /* 
     * If there is at least one stream, then figure out the length of time
     * we were running the tests and print out some statistics about
     * the streams.  It's possible to not have any streams at all
     * if the client got interrupted before it got to do anything.
     */
    if (sp) {
        real_end_time = timeval_diff(&sp->result->start_time, &sp->result->end_time);
        //PRINTF("temp1=%u.%u   %u.%u   %u\r\n",sp->result->start_time.tv_sec, sp->result->start_time.tv_usec,
        //    sp->result->end_time.tv_sec, sp->result->end_time.tv_usec , end_time1);
        end_time = test->stats_interval * test->periods;
        SLIST_FOREACH(sp, &test->streams, streams) {
            bytes_sent = sp->result->bytes_sent;
            bytes_received = sp->result->bytes_received;
            total_sent += bytes_sent;
            total_received += bytes_received;

            if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
        	    if (test->sender_has_retransmits) {
            		total_retransmits += sp->result->stream_retrans;
        	    }
        	} else {
                total_packets += (sp->packet_count - sp->omitted_packet_count);
                lost_packets += sp->cnt_error;
                if (test->role == 's')  //else : client, just print what see.
                {
                    if (sp->jitter < 0)
                        sp->jitter = -sp->jitter;
                    if (total_packets > lost_packets)
                        sp->jitter /= total_packets - lost_packets;
                    else
                        sp->jitter = 0;
                    avg_jitter += sp->jitter;
                }
            }

        	unit_snprintf(ubuf, UNIT_LEN, bytes_sent, 'A');
        	bandwidth = (real_end_time > 0) ? (bytes_sent * 1000 / real_end_time) : 0;
        	unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
        	if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
        	    if (test->sender_has_retransmits) {
            		/* Summary, TCP with retransmits. */
        		    iperf_printf(test, iperf_report_bw_retrans_format, sp->socket, start_time, end_time, ubuf, nbuf, sp->result->stream_retrans, iperf_report_sender);
            	} else {
            		/* Summary, TCP without retransmits. */
        		    iperf_printf(test, iperf_report_bw_format, sp->socket, start_time, end_time, ubuf, nbuf, iperf_report_sender);
        	    }
        	} else {
        	    /* Summary, UDP. */
        	    if (sp->packet_count - sp->omitted_packet_count > 0)
        	        lost_percent = 100 * sp->cnt_error / (sp->packet_count - sp->omitted_packet_count);
        	    else
        	        lost_percent = 100;

        		iperf_printf(test, iperf_report_bw_udp_format, sp->socket, start_time, end_time, ubuf, nbuf, sp->jitter, sp->cnt_error, (sp->packet_count - sp->omitted_packet_count), lost_percent, "");
                if (test->role == 'c')
                    iperf_printf(test, iperf_report_datagrams, sp->socket, (sp->packet_count - sp->omitted_packet_count));
                if (sp->outoforder_packets > 0 )
                    iperf_printf(test, iperf_report_sum_outoforder, sp->socket, start_time, end_time, sp->outoforder_packets);
                }

        	unit_snprintf(ubuf, UNIT_LEN, bytes_received, 'A');
        	bandwidth = (real_end_time > 0) ? (bytes_received * 1000 / real_end_time) : 0;
        	unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
        	if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
        		iperf_printf(test, iperf_report_bw_format, sp->socket, start_time, end_time, ubuf, nbuf, iperf_report_receiver);
        	}
            else
            {
        		iperf_printf(test, iperf_report_bw_format, sp->socket, start_time, end_time, ubuf, nbuf, iperf_report_receiver);
            }
        }
    }

    if (test->num_streams > 1) {
        unit_snprintf(ubuf, UNIT_LEN, total_sent, 'A');
        
    	/* If no tests were run, arbitrariliy set bandwidth to 0. */
        bandwidth = (real_end_time > 0) ? (total_sent * 1000 / real_end_time) : 0;
        unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
        if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
    	    if (test->sender_has_retransmits) {
    		    /* Summary sum, TCP with retransmits. */
    		    iperf_printf(test, iperf_report_sum_bw_retrans_format, start_time, end_time, ubuf, nbuf, total_retransmits, iperf_report_sender);
    	    } else {
    		    /* Summary sum, TCP without retransmits. */
    		    iperf_printf(test, iperf_report_sum_bw_format, start_time, end_time, ubuf, nbuf, iperf_report_sender);
    	    }
            unit_snprintf(ubuf, UNIT_LEN, total_received, 'A');
    	    /* If no tests were run, set received bandwidth to 0 */
            bandwidth = (real_end_time > 0) ? (total_received * 1000 / real_end_time) : 0;
            unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
    		iperf_printf(test, iperf_report_sum_bw_format, start_time, end_time, ubuf, nbuf, iperf_report_receiver);
        } else {
    	    /* Summary sum, UDP. */
            avg_jitter /= test->num_streams;
                
    	    /* If no packets were sent, arbitrarily set loss percentage to 100. */
    	    lost_percent = (total_packets > 0) ? (100 * lost_packets / total_packets) : 0;

            iperf_printf(test, iperf_report_sum_bw_udp_format_send, start_time, end_time, ubuf, nbuf, iperf_report_sender);

            unit_snprintf(ubuf, UNIT_LEN, total_received, 'A');
            /* If no tests were run, set received bandwidth to 0 */
            bandwidth = (real_end_time > 0) ? (total_received * 1000 / real_end_time) : 0;
            unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);

            iperf_printf(test, iperf_report_sum_bw_udp_format, start_time, end_time, ubuf, nbuf, avg_jitter, lost_packets, total_packets, lost_percent, iperf_report_receiver);
        }
    }

	/* Print server output if we're on the client and it was requested/provided */
	if (test->role == 'c' && iperf_get_test_get_server_output(test)) {
	    if (test->json_server_output) {
    		iperf_printf(test, "\r\nServer JSON output:\r\n%s\r\n", Iperf_cJSON_Print(test->json_server_output));
    		Iperf_cJSON_Delete(test->json_server_output);
    		test->json_server_output = NULL;
    	}
	    if (test->server_output_text) {
    		iperf_printf(test, "\r\nServer output:\r\n%s\r\n", test->server_output_text);
    		test->server_output_text = NULL;
	    }
    }
}

/**************************************************************************/

/**
 * Main report-printing callback.
 * Prints results either during a test (interval report only) or 
 * after the entire test has been run (last interval report plus 
 * overall summary).
 */
void
iperf_reporter_callback(struct iperf_test *test)
{
    switch (test->state) {
        case TEST_RUNNING:
        case STREAM_RUNNING:
            /* print interval results for each stream */
            iperf_print_intermediate(test);
            break;
        case DISPLAY_RESULTS:
            if (test->role == 'c')
                iperf_print_intermediate(test);
            iperf_print_results(test);
            break;
    } 
}

/**
 * Print the interval results for one stream.
 * This function needs to know about the overall test so it can determine the
 * context for printing headers, separators, etc.
 */
static void
print_interval_results(struct iperf_test *test, struct iperf_stream *sp, Iperf_cJSON *json_interval_streams)
{
    char ubuf[UNIT_LEN];
    char nbuf[UNIT_LEN];
    char cbuf[UNIT_LEN];
    u32  st = 0, et = 0.;
    struct iperf_interval_results *irp = NULL;
    u32  bandwidth, lost_percent;

    irp = TAILQ_LAST(&sp->result->interval_results, irlisthead); /* get last entry in linked list */
    if (irp == NULL) {
    	PRINTF("print_interval_results error: interval_results is NULL \r\n");
        return;
    }
	/* First stream? */
	if (sp == SLIST_FIRST(&test->streams)) {
	    /* It it's the first interval, print the header;
    	    ** else if there's more than one stream, print the separator;
    	    ** else nothing.
    	    */
	    if (test->periods == 1){
    		if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
    		    if (test->sender && test->sender_has_retransmits)
        			iperf_printf(test, "%s", iperf_report_bw_retrans_cwnd_header);
    		    else
        			iperf_printf(test, "%s", iperf_report_bw_header);
    		} else {
    		    if (test->sender)
        			iperf_printf(test, "%s", iperf_report_bw_udp_sender_header);
    		    else
        			iperf_printf(test, "%s", iperf_report_bw_udp_header);
    		}
	    } else if (test->num_streams > 1)
    		iperf_printf(test, "%s", iperf_report_bw_separator);
    }

    unit_snprintf(ubuf, UNIT_LEN, irp->bytes_transferred, 'A');
    bandwidth = (irp->interval_duration > 0) ? (irp->bytes_transferred * 1000 / irp->interval_duration) : 0;
    unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
    
    st = (irp->end_time - 1) * test->stats_interval;
    et = irp->end_time * test->stats_interval;
    if (test->protocol->id == Ptcp || test->protocol->id == Psctp) {
    	if (test->sender && test->sender_has_retransmits) {
    	    /* Interval, TCP with retransmits. */
    		unit_snprintf(cbuf, UNIT_LEN, irp->snd_cwnd, 'A');
    		iperf_printf(test, iperf_report_bw_retrans_cwnd_format, sp->socket, st, et, ubuf, nbuf, irp->interval_retrans, cbuf, irp->omitted?iperf_report_omitted:"");
    	} else {
    	    /* Interval, TCP without retransmits. */
    		iperf_printf(test, iperf_report_bw_format, sp->socket, st, et, ubuf, nbuf, irp->omitted?iperf_report_omitted:"");
    	}
    } else {
    	/* Interval, UDP. */
    	if (test->sender) {
    		iperf_printf(test, iperf_report_bw_udp_sender_format, sp->socket, st, et, ubuf, nbuf, irp->interval_packet_count, irp->omitted?iperf_report_omitted:"");
    	} else {
    	    if (irp->interval_packet_count){
    	        lost_percent = 100 * irp->interval_cnt_error / irp->interval_packet_count;
	        }else{
    	        lost_percent = 100;
    	        irp->jitter = 0;
	        }
    		iperf_printf(test, iperf_report_bw_udp_format, sp->socket, st, et, ubuf, nbuf, irp->jitter, irp->interval_cnt_error, irp->interval_packet_count, lost_percent, irp->omitted?iperf_report_omitted:"");
    	}
    }
}

/**************************************************************************/
void
iperf_free_stream(struct iperf_stream *sp)
{
    struct iperf_interval_results *irp, *nirp;

    if (sp->buffer)
        iperf_free(sp->buffer);
    /* XXX: need to free interval list too! */
    for (irp = TAILQ_FIRST(&sp->result->interval_results); irp != TAILQ_END(sp->result->interval_results); irp = nirp) {
        nirp = TAILQ_NEXT(irp, irlistentries);
        iperf_free(irp);
    }
    if (sp->result)
        iperf_free(sp->result);
    if (sp->send_timer != NULL)
    	tmr_cancel(sp->send_timer, sp->test);

    if ((sp->socket >= 0) && (!is_closed(sp->socket)))
        close(sp->socket);

    iperf_free(sp);
}

/**************************************************************************/
struct iperf_stream *
iperf_new_stream(struct iperf_test *test, int s)
{
    struct iperf_stream *sp;

    sp = (struct iperf_stream *) iperf_malloc(sizeof(struct iperf_stream));
    if (!sp) {
        test->i_errno = IECREATESTREAM;
        return NULL;
    }

    MEMSET(sp, 0, sizeof(struct iperf_stream));

    sp->test = test;
    sp->settings = test->settings;
    sp->result = (struct iperf_stream_result *) iperf_malloc(sizeof(struct iperf_stream_result));
    if (!sp->result) {
        iperf_free(sp);
        test->i_errno = IECREATESTREAM;
        return NULL;
    }

    sp->jitter = 0;
    MEMSET(sp->result, 0, sizeof(struct iperf_stream_result));
    TAILQ_INIT(&sp->result->interval_results);
    
    /* Create and randomize the buffer */
    sp->buffer = (char *) iperf_malloc(test->settings->blksize);
    if (sp->buffer == NULL)
    {
        iperf_free(sp->result);
        iperf_free(sp);
        test->i_errno = IECREATESTREAM;
        return NULL;
    }
    MEMSET(sp->buffer, 'A', test->settings->blksize);

    /* Set socket */
    sp->socket = s;

    sp->snd = test->protocol->sock_send;
    sp->rcv = test->protocol->sock_recv;

    /* Initialize stream */
    if (iperf_init_stream(sp, test) < 0) {
        iperf_free (sp->buffer);
        iperf_free(sp->result);
        iperf_free(sp);
        return NULL;
    }
    iperf_add_stream(test, sp);

    return sp;
}

/**************************************************************************/
int
iperf_init_stream(struct iperf_stream *sp, struct iperf_test *test)
{
    socklen_t len;
    int opt;

    //len = sizeof(struct sockaddr_storage); 
    len = sizeof(struct sockaddr_in); 
    if (getsockname(sp->socket, (struct sockaddr *) &sp->local_addr, &len) < 0) {
        test->i_errno = IEINITSTREAM;
        return -1;
    }
    //len = sizeof(struct sockaddr_storage);
    len = sizeof(struct sockaddr_in);
    if (getpeername(sp->socket, (struct sockaddr *) &sp->remote_addr, &len) < 0) {
        test->i_errno = IEINITSTREAM;
        return -1;
    }

    /* Set IPv4 TOS */
    if ((opt = test->settings->tos)) {
        if (setsockopt(sp->socket, IPPROTO_IP, IP_TOS, &opt, sizeof(opt)) < 0) {
            test->i_errno = IESETTOS;
            return -1;
        }
    }

    return 0;
}

/**************************************************************************/
void
iperf_add_stream(struct iperf_test *test, struct iperf_stream *sp)
{
    int i;
    struct iperf_stream *n, *prev;

    if (SLIST_EMPTY(&test->streams)) {
        SLIST_INSERT_HEAD(&test->streams, sp, streams);
        sp->id = 1;
    } else {
        // for (n = test->streams, i = 2; n->next; n = n->next, ++i);
        i = 2;
        SLIST_FOREACH(n, &test->streams, streams) {
            prev = n;
            ++i;
        }
        SLIST_INSERT_AFTER(prev, sp, streams);
        sp->id = i;
    }
}


int
iperf_printf(struct iperf_test *test, const char* format, ...)
{
    va_list argp;
    int r = -1;
   	char linebuffer[200];

    /*
     * There are roughly two use cases here.  If we're the client,
     * want to print stuff directly to the output stream.
     * If we're the sender we might need to buffer up output to send
     * to the client.
     *
     * This doesn't make a whole lot of difference except there are
     * some chunks of output on the client (on particular the whole
     * of the server output with --get-server-output) that could
     * easily exceed the size of the line buffer, but which don't need
     * to be buffered up anyway.
     */
    if (test->role == 'c') {
    	if (test->title)
    	    PRINTF("%s:  ", test->title);
    	va_start(argp, format);
    	r = vsnprintf(linebuffer, sizeof(linebuffer), format, argp);
    	va_end(argp);
    	PRINTF("%s", linebuffer);
    }
    else if (test->role == 's') {
    	va_start(argp, format);
    	r = vsnprintf(linebuffer, sizeof(linebuffer), format, argp);
    	va_end(argp);
    	PRINTF("%s", linebuffer);
    	if (test->role == 's' && iperf_get_test_get_server_output(test)) {
    	    struct iperf_textline *l = (struct iperf_textline *) iperf_malloc(sizeof(struct iperf_textline));
    	    if (l != NULL){
        	    l->line = iperf_strdup(linebuffer);
        	    TAILQ_INSERT_TAIL(&(test->server_output_list), l, textlineentries);
    	    }
    	}
    }
    return r;
}
//#endif //#if(IPERF3_ENABLE==1)

