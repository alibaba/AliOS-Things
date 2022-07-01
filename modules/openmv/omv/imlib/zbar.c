/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * This file is part of the ZBar Bar Code Reader library.
 */
#include <limits.h>
#include "imlib.h"
#ifdef IMLIB_ENABLE_BARCODES
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define free(ptr) ({ umm_free(ptr); })
#define malloc(size) ({ void *_r = umm_malloc(size); if(!_r) fb_alloc_fail(); _r; })
#define realloc(ptr, size) ({ void *_r = umm_realloc((ptr), (size)); if(!_r) fb_alloc_fail(); _r; })
#define calloc(num, item_size) ({ void *_r = umm_calloc((num), (item_size)); if(!_r) fb_alloc_fail(); _r; })
#define assert(expression)
#define zprintf(...)
#define dbprintf(...) while(0)
#define zassert(condition, retval, format, ...) do { if(!(condition)) return(retval); } while(0)

#define zbar_image_write_png(...)
#define svg_open(...)
#define svg_image(...)
#define svg_group_start(...)
#define svg_path_start(...)
#define svg_path_end(...)
#define svg_group_end(...)
#define svg_close(...)

#define NO_STATS
#define ENABLE_EAN
#define FIXME_ADDON_SYNC
#define ENABLE_I25
#define ENABLE_DATABAR
#define ENABLE_CODABAR
#define ENABLE_CODE39
#define ENABLE_CODE93
#define ENABLE_CODE128
#define ENABLE_PDF417

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "zbar.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/** @file
 * ZBar Barcode Reader C API definition
 */

/** @mainpage
 *
 * interface to the barcode reader is available at several levels.
 * most applications will want to use the high-level interfaces:
 *
 * @section high-level High-Level Interfaces
 *
 * these interfaces wrap all library functionality into an easy-to-use
 * package for a specific toolkit:
 * - the "GTK+ 2.x widget" may be used with GTK GUI applications.  a
 *   Python wrapper is included for PyGtk
 * - the @ref zbar::QZBar "Qt4 widget" may be used with Qt GUI
 *   applications
 * - the Processor interface (in @ref c-processor "C" or @ref
 *   zbar::Processor "C++") adds a scanning window to an application
 *   with no GUI.
 *
 * @section mid-level Intermediate Interfaces
 *
 * building blocks used to construct high-level interfaces:
 * - the ImageScanner (in @ref c-imagescanner "C" or @ref
 *   zbar::ImageScanner "C++") looks for barcodes in a library defined
 *   image object
 * - the Window abstraction (in @ref c-window "C" or @ref
 *   zbar::Window "C++") sinks library images, displaying them on the
 *   platform display
 * - the Video abstraction (in @ref c-video "C" or @ref zbar::Video
 *   "C++") sources library images from a video device
 *
 * @section low-level Low-Level Interfaces
 *
 * direct interaction with barcode scanning and decoding:
 * - the Scanner (in @ref c-scanner "C" or @ref zbar::Scanner "C++")
 *   looks for barcodes in a linear intensity sample stream
 * - the Decoder (in @ref c-decoder "C" or @ref zbar::Decoder "C++")
 *   extracts barcodes from a stream of bar and space widths
 */

/** @name Global library interfaces */
/*@{*/

/** "color" of element: bar or space. */
typedef enum zbar_color_e {
    ZBAR_SPACE = 0,    /**< light area or space between bars */
    ZBAR_BAR = 1,      /**< dark area or colored bar segment */
} zbar_color_t;

/** decoded symbol type. */
typedef enum zbar_symbol_type_e {
    ZBAR_NONE        =      0,  /**< no symbol decoded */
    ZBAR_PARTIAL     =      1,  /**< intermediate status */
    ZBAR_EAN2        =      2,  /**< GS1 2-digit add-on */
    ZBAR_EAN5        =      5,  /**< GS1 5-digit add-on */
    ZBAR_EAN8        =      8,  /**< EAN-8 */
    ZBAR_UPCE        =      9,  /**< UPC-E */
    ZBAR_ISBN10      =     10,  /**< ISBN-10 (from EAN-13). @since 0.4 */
    ZBAR_UPCA        =     12,  /**< UPC-A */
    ZBAR_EAN13       =     13,  /**< EAN-13 */
    ZBAR_ISBN13      =     14,  /**< ISBN-13 (from EAN-13). @since 0.4 */
    ZBAR_COMPOSITE   =     15,  /**< EAN/UPC composite */
    ZBAR_I25         =     25,  /**< Interleaved 2 of 5. @since 0.4 */
    ZBAR_DATABAR     =     34,  /**< GS1 DataBar (RSS). @since 0.11 */
    ZBAR_DATABAR_EXP =     35,  /**< GS1 DataBar Expanded. @since 0.11 */
    ZBAR_CODABAR     =     38,  /**< Codabar. @since 0.11 */
    ZBAR_CODE39      =     39,  /**< Code 39. @since 0.4 */
    ZBAR_PDF417      =     57,  /**< PDF417. @since 0.6 */
    ZBAR_QRCODE      =     64,  /**< QR Code. @since 0.10 */
    ZBAR_CODE93      =     93,  /**< Code 93. @since 0.11 */
    ZBAR_CODE128     =    128,  /**< Code 128 */

    /** mask for base symbol type.
     * @deprecated in 0.11, remove this from existing code
     */
    ZBAR_SYMBOL      = 0x00ff,
    /** 2-digit add-on flag.
     * @deprecated in 0.11, a ::ZBAR_EAN2 component is used for
     * 2-digit GS1 add-ons
     */
    ZBAR_ADDON2      = 0x0200,
    /** 5-digit add-on flag.
     * @deprecated in 0.11, a ::ZBAR_EAN5 component is used for
     * 5-digit GS1 add-ons
     */
    ZBAR_ADDON5      = 0x0500,
    /** add-on flag mask.
     * @deprecated in 0.11, GS1 add-ons are represented using composite
     * symbols of type ::ZBAR_COMPOSITE; add-on components use ::ZBAR_EAN2
     * or ::ZBAR_EAN5
     */
    ZBAR_ADDON       = 0x0700,
} zbar_symbol_type_t;

/** decoded symbol coarse orientation.
 * @since 0.11
 */
typedef enum zbar_orientation_e {
    ZBAR_ORIENT_UNKNOWN = -1,   /**< unable to determine orientation */
    ZBAR_ORIENT_UP,             /**< upright, read left to right */
    ZBAR_ORIENT_RIGHT,          /**< sideways, read top to bottom */
    ZBAR_ORIENT_DOWN,           /**< upside-down, read right to left */
    ZBAR_ORIENT_LEFT,           /**< sideways, read bottom to top */
} zbar_orientation_t;

/** decoder configuration options.
 * @since 0.4
 */
typedef enum zbar_config_e {
    ZBAR_CFG_ENABLE = 0,        /**< enable symbology/feature */
    ZBAR_CFG_ADD_CHECK,         /**< enable check digit when optional */
    ZBAR_CFG_EMIT_CHECK,        /**< return check digit when present */
    ZBAR_CFG_ASCII,             /**< enable full ASCII character set */
    ZBAR_CFG_NUM,               /**< number of boolean decoder configs */

    ZBAR_CFG_MIN_LEN = 0x20,    /**< minimum data length for valid decode */
    ZBAR_CFG_MAX_LEN,           /**< maximum data length for valid decode */

    ZBAR_CFG_UNCERTAINTY = 0x40,/**< required video consistency frames */

    ZBAR_CFG_POSITION = 0x80,   /**< enable scanner to collect position data */

    ZBAR_CFG_X_DENSITY = 0x100, /**< image scanner vertical scan density */
    ZBAR_CFG_Y_DENSITY,         /**< image scanner horizontal scan density */
} zbar_config_t;

/** decoder symbology modifier flags.
 * @since 0.11
 */
typedef enum zbar_modifier_e {
    /** barcode tagged as GS1 (EAN.UCC) reserved
     * (eg, FNC1 before first data character).
     * data may be parsed as a sequence of GS1 AIs
     */
    ZBAR_MOD_GS1 = 0,

    /** barcode tagged as AIM reserved
     * (eg, FNC1 after first character or digit pair)
     */
    ZBAR_MOD_AIM,

    /** number of modifiers */
    ZBAR_MOD_NUM,
} zbar_modifier_t;

/** retrieve string name for symbol encoding.
 * @param sym symbol type encoding
 * @returns the static string name for the specified symbol type,
 * or "UNKNOWN" if the encoding is not recognized
 */
extern const char *zbar_get_symbol_name(zbar_symbol_type_t sym);

/** retrieve string name for addon encoding.
 * @param sym symbol type encoding
 * @returns static string name for any addon, or the empty string
 * if no addons were decoded
 * @deprecated in 0.11
 */
extern const char *zbar_get_addon_name(zbar_symbol_type_t sym);

/** retrieve string name for configuration setting.
 * @param config setting to name
 * @returns static string name for config,
 * or the empty string if value is not a known config
 */
extern const char *zbar_get_config_name(zbar_config_t config);

/** retrieve string name for modifier.
 * @param modifier flag to name
 * @returns static string name for modifier,
 * or the empty string if the value is not a known flag
 */
extern const char *zbar_get_modifier_name(zbar_modifier_t modifier);

/** retrieve string name for orientation.
 * @param orientation orientation encoding
 * @returns the static string name for the specified orientation,
 * or "UNKNOWN" if the orientation is not recognized
 * @since 0.11
 */
extern const char *zbar_get_orientation_name(zbar_orientation_t orientation);

/** parse a configuration string of the form "[symbology.]config[=value]".
 * the config must match one of the recognized names.
 * the symbology, if present, must match one of the recognized names.
 * if symbology is unspecified, it will be set to 0.
 * if value is unspecified it will be set to 1.
 * @returns 0 if the config is parsed successfully, 1 otherwise
 * @since 0.4
 */
extern int zbar_parse_config(const char *config_string,
                             zbar_symbol_type_t *symbology,
                             zbar_config_t *config,
                             int *value);

/** consistently compute fourcc values across architectures
 * (adapted from v4l2 specification)
 * @since 0.11
 */
#define zbar_fourcc(a, b, c, d)                 \
        ((unsigned long)(a) |                   \
         ((unsigned long)(b) << 8) |            \
         ((unsigned long)(c) << 16) |           \
         ((unsigned long)(d) << 24))

/** parse a fourcc string into its encoded integer value.
 * @since 0.11
 */
static inline unsigned long zbar_fourcc_parse (const char *format)
{
    unsigned long fourcc = 0;
    if(format) {
        int i;
        for(i = 0; i < 4 && format[i]; i++)
            fourcc |= ((unsigned long)format[i]) << (i * 8);
    }
    return(fourcc);
}

/*@}*/

struct zbar_symbol_s;
typedef struct zbar_symbol_s zbar_symbol_t;

struct zbar_symbol_set_s;
typedef struct zbar_symbol_set_s zbar_symbol_set_t;


/*------------------------------------------------------------*/
/** @name Symbol interface
 * decoded barcode symbol result object.  stores type, data, and image
 * location of decoded symbol.  all memory is owned by the library
 */
/*@{*/

/** @typedef zbar_symbol_t
 * opaque decoded symbol object.
 */

/** symbol reference count manipulation.
 * increment the reference count when you store a new reference to the
 * symbol.  decrement when the reference is no longer used.  do not
 * refer to the symbol once the count is decremented and the
 * containing image has been recycled or destroyed.
 * @note the containing image holds a reference to the symbol, so you
 * only need to use this if you keep a symbol after the image has been
 * destroyed or reused.
 * @since 0.9
 */
extern void zbar_symbol_ref(const zbar_symbol_t *symbol,
                            int refs);

/** retrieve type of decoded symbol.
 * @returns the symbol type
 */
extern zbar_symbol_type_t zbar_symbol_get_type(const zbar_symbol_t *symbol);

/** retrieve symbology boolean config settings.
 * @returns a bitmask indicating which configs were set for the detected
 * symbology during decoding.
 * @since 0.11
 */
extern unsigned int zbar_symbol_get_configs(const zbar_symbol_t *symbol);

/** retrieve symbology modifier flag settings.
 * @returns a bitmask indicating which characteristics were detected
 * during decoding.
 * @since 0.11
 */
extern unsigned int zbar_symbol_get_modifiers(const zbar_symbol_t *symbol);

/** retrieve data decoded from symbol.
 * @returns the data string
 */
extern const char *zbar_symbol_get_data(const zbar_symbol_t *symbol);

/** retrieve length of binary data.
 * @returns the length of the decoded data
 */
extern unsigned int zbar_symbol_get_data_length(const zbar_symbol_t *symbol);

/** retrieve a symbol confidence metric.
 * @returns an unscaled, relative quantity: larger values are better
 * than smaller values, where "large" and "small" are application
 * dependent.
 * @note expect the exact definition of this quantity to change as the
 * metric is refined.  currently, only the ordered relationship
 * between two values is defined and will remain stable in the future
 * @since 0.9
 */
extern int zbar_symbol_get_quality(const zbar_symbol_t *symbol);

/** retrieve current cache count.  when the cache is enabled for the
 * image_scanner this provides inter-frame reliability and redundancy
 * information for video streams.
 * @returns < 0 if symbol is still uncertain.
 * @returns 0 if symbol is newly verified.
 * @returns > 0 for duplicate symbols
 */
extern int zbar_symbol_get_count(const zbar_symbol_t *symbol);

/** retrieve the number of points in the location polygon.  the
 * location polygon defines the image area that the symbol was
 * extracted from.
 * @returns the number of points in the location polygon
 * @note this is currently not a polygon, but the scan locations
 * where the symbol was decoded
 */
extern unsigned zbar_symbol_get_loc_size(const zbar_symbol_t *symbol);

/** retrieve location polygon x-coordinates.
 * points are specified by 0-based index.
 * @returns the x-coordinate for a point in the location polygon.
 * @returns -1 if index is out of range
 */
extern int zbar_symbol_get_loc_x(const zbar_symbol_t *symbol,
                                 unsigned index);

/** retrieve location polygon y-coordinates.
 * points are specified by 0-based index.
 * @returns the y-coordinate for a point in the location polygon.
 * @returns -1 if index is out of range
 */
extern int zbar_symbol_get_loc_y(const zbar_symbol_t *symbol,
                                 unsigned index);

/** retrieve general orientation of decoded symbol.
 * @returns a coarse, axis-aligned indication of symbol orientation or
 * ::ZBAR_ORIENT_UNKNOWN if unknown
 * @since 0.11
 */
extern zbar_orientation_t
zbar_symbol_get_orientation(const zbar_symbol_t *symbol);

/** iterate the set to which this symbol belongs (there can be only one).
 * @returns the next symbol in the set, or
 * @returns NULL when no more results are available
 */
extern const zbar_symbol_t *zbar_symbol_next(const zbar_symbol_t *symbol);

/** retrieve components of a composite result.
 * @returns the symbol set containing the components
 * @returns NULL if the symbol is already a physical symbol
 * @since 0.10
 */
extern const zbar_symbol_set_t*
zbar_symbol_get_components(const zbar_symbol_t *symbol);

/** iterate components of a composite result.
 * @returns the first physical component symbol of a composite result
 * @returns NULL if the symbol is already a physical symbol
 * @since 0.10
 */
extern const zbar_symbol_t*
zbar_symbol_first_component(const zbar_symbol_t *symbol);

/** print XML symbol element representation to user result buffer.
 * @see http://zbar.sourceforge.net/2008/barcode.xsd for the schema.
 * @param symbol is the symbol to print
 * @param buffer is the inout result pointer, it will be reallocated
 * with a larger size if necessary.
 * @param buflen is inout length of the result buffer.
 * @returns the buffer pointer
 * @since 0.6
 */
extern char *zbar_symbol_xml(const zbar_symbol_t *symbol,
                             char **buffer,
                             unsigned *buflen);

/*@}*/

/*------------------------------------------------------------*/
/** @name Symbol Set interface
 * container for decoded result symbols associated with an image
 * or a composite symbol.
 * @since 0.10
 */
/*@{*/

/** @typedef zbar_symbol_set_t
 * opaque symbol iterator object.
 * @since 0.10
 */

/** reference count manipulation.
 * increment the reference count when you store a new reference.
 * decrement when the reference is no longer used.  do not refer to
 * the object any longer once references have been released.
 * @since 0.10
 */
extern void zbar_symbol_set_ref(const zbar_symbol_set_t *symbols,
                                int refs);

/** retrieve set size.
 * @returns the number of symbols in the set.
 * @since 0.10
 */
extern int zbar_symbol_set_get_size(const zbar_symbol_set_t *symbols);

/** set iterator.
 * @returns the first decoded symbol result in a set
 * @returns NULL if the set is empty
 * @since 0.10
 */
extern const zbar_symbol_t*
zbar_symbol_set_first_symbol(const zbar_symbol_set_t *symbols);

/** raw result iterator.
 * @returns the first decoded symbol result in a set, *before* filtering
 * @returns NULL if the set is empty
 * @since 0.11
 */
extern const zbar_symbol_t*
zbar_symbol_set_first_unfiltered(const zbar_symbol_set_t *symbols);

/*@}*/

/*------------------------------------------------------------*/
/** @name Image interface
 * stores image data samples along with associated format and size
 * metadata
 */
/*@{*/

struct zbar_image_s;
/** opaque image object. */
typedef struct zbar_image_s zbar_image_t;

/** cleanup handler callback function.
 * called to free sample data when an image is destroyed.
 */
typedef void (zbar_image_cleanup_handler_t)(zbar_image_t *image);

/** data handler callback function.
 * called when decoded symbol results are available for an image
 */
typedef void (zbar_image_data_handler_t)(zbar_image_t *image,
                                         const void *userdata);

/*@}*/

/*------------------------------------------------------------*/
/** @name Image Scanner interface
 * @anchor c-imagescanner
 * mid-level image scanner interface.
 * reads barcodes from 2-D images
 */
/*@{*/

struct zbar_image_scanner_s;
/** opaque image scanner object. */
typedef struct zbar_image_scanner_s zbar_image_scanner_t;

/** constructor. */
extern zbar_image_scanner_t *zbar_image_scanner_create(void);

/** destructor. */
extern void zbar_image_scanner_destroy(zbar_image_scanner_t *scanner);

/** setup result handler callback.
 * the specified function will be called by the scanner whenever
 * new results are available from a decoded image.
 * pass a NULL value to disable callbacks.
 * @returns the previously registered handler
 */
extern zbar_image_data_handler_t*
zbar_image_scanner_set_data_handler(zbar_image_scanner_t *scanner,
                                    zbar_image_data_handler_t *handler,
                                    const void *userdata);


/** set config for indicated symbology (0 for all) to specified value.
 * @returns 0 for success, non-0 for failure (config does not apply to
 * specified symbology, or value out of range)
 * @see zbar_decoder_set_config()
 * @since 0.4
 */
extern int zbar_image_scanner_set_config(zbar_image_scanner_t *scanner,
                                         zbar_symbol_type_t symbology,
                                         zbar_config_t config,
                                         int value);

/** parse configuration string using zbar_parse_config()
 * and apply to image scanner using zbar_image_scanner_set_config().
 * @returns 0 for success, non-0 for failure
 * @see zbar_parse_config()
 * @see zbar_image_scanner_set_config()
 * @since 0.4
 */
static inline int
zbar_image_scanner_parse_config (zbar_image_scanner_t *scanner,
                                 const char *config_string)
{
    zbar_symbol_type_t sym;
    zbar_config_t cfg;
    int val;
    return(zbar_parse_config(config_string, &sym, &cfg, &val) ||
           zbar_image_scanner_set_config(scanner, sym, cfg, val));
}

/** enable or disable the inter-image result cache (default disabled).
 * mostly useful for scanning video frames, the cache filters
 * duplicate results from consecutive images, while adding some
 * consistency checking and hysteresis to the results.
 * this interface also clears the cache
 */
extern void zbar_image_scanner_enable_cache(zbar_image_scanner_t *scanner,
                                            int enable);

/** remove any previously decoded results from the image scanner and the
 * specified image.  somewhat more efficient version of
 * zbar_image_set_symbols(image, NULL) which may retain memory for
 * subsequent decodes
 * @since 0.10
 */
extern void zbar_image_scanner_recycle_image(zbar_image_scanner_t *scanner,
                                             zbar_image_t *image);

/** retrieve decode results for last scanned image.
 * @returns the symbol set result container or NULL if no results are
 * available
 * @note the symbol set does not have its reference count adjusted;
 * ensure that the count is incremented if the results may be kept
 * after the next image is scanned
 * @since 0.10
 */
extern const zbar_symbol_set_t*
zbar_image_scanner_get_results(const zbar_image_scanner_t *scanner);

/** scan for symbols in provided image.  The image format must be
 * "Y800" or "GRAY".
 * @returns >0 if symbols were successfully decoded from the image,
 * 0 if no symbols were found or -1 if an error occurs
 * @see zbar_image_convert()
 * @since 0.9 - changed to only accept grayscale images
 */
extern int zbar_scan_image(zbar_image_scanner_t *scanner,
                           zbar_image_t *image);

/*@}*/

/*------------------------------------------------------------*/
/** @name Decoder interface
 * @anchor c-decoder
 * low-level bar width stream decoder interface.
 * identifies symbols and extracts encoded data
 */
/*@{*/

struct zbar_decoder_s;
/** opaque decoder object. */
typedef struct zbar_decoder_s zbar_decoder_t;

/** decoder data handler callback function.
 * called by decoder when new data has just been decoded
 */
typedef void (zbar_decoder_handler_t)(zbar_decoder_t *decoder);

/** constructor. */
extern zbar_decoder_t *zbar_decoder_create(void);

/** destructor. */
extern void zbar_decoder_destroy(zbar_decoder_t *decoder);

/** set config for indicated symbology (0 for all) to specified value.
 * @returns 0 for success, non-0 for failure (config does not apply to
 * specified symbology, or value out of range)
 * @since 0.4
 */
extern int zbar_decoder_set_config(zbar_decoder_t *decoder,
                                   zbar_symbol_type_t symbology,
                                   zbar_config_t config,
                                   int value);

/** parse configuration string using zbar_parse_config()
 * and apply to decoder using zbar_decoder_set_config().
 * @returns 0 for success, non-0 for failure
 * @see zbar_parse_config()
 * @see zbar_decoder_set_config()
 * @since 0.4
 */
static inline int zbar_decoder_parse_config (zbar_decoder_t *decoder,
                                             const char *config_string)
{
    zbar_symbol_type_t sym;
    zbar_config_t cfg;
    int val;
    return(zbar_parse_config(config_string, &sym, &cfg, &val) ||
           zbar_decoder_set_config(decoder, sym, cfg, val));
}

/** retrieve symbology boolean config settings.
 * @returns a bitmask indicating which configs are currently set for the
 * specified symbology.
 * @since 0.11
 */
extern unsigned int zbar_decoder_get_configs(const zbar_decoder_t *decoder,
                                             zbar_symbol_type_t symbology);

/** clear all decoder state.
 * any partial symbols are flushed
 */
extern void zbar_decoder_reset(zbar_decoder_t *decoder);

/** mark start of a new scan pass.
 * clears any intra-symbol state and resets color to ::ZBAR_SPACE.
 * any partially decoded symbol state is retained
 */
extern void zbar_decoder_new_scan(zbar_decoder_t *decoder);

/** process next bar/space width from input stream.
 * the width is in arbitrary relative units.  first value of a scan
 * is ::ZBAR_SPACE width, alternating from there.
 * @returns appropriate symbol type if width completes
 * decode of a symbol (data is available for retrieval)
 * @returns ::ZBAR_PARTIAL as a hint if part of a symbol was decoded
 * @returns ::ZBAR_NONE (0) if no new symbol data is available
 */
extern zbar_symbol_type_t zbar_decode_width(zbar_decoder_t *decoder,
                                            unsigned width);

/** retrieve color of @em next element passed to
 * zbar_decode_width(). */
extern zbar_color_t zbar_decoder_get_color(const zbar_decoder_t *decoder);

/** retrieve last decoded data.
 * @returns the data string or NULL if no new data available.
 * the returned data buffer is owned by library, contents are only
 * valid between non-0 return from zbar_decode_width and next library
 * call
 */
extern const char *zbar_decoder_get_data(const zbar_decoder_t *decoder);

/** retrieve length of binary data.
 * @returns the length of the decoded data or 0 if no new data
 * available.
 */
extern unsigned int
zbar_decoder_get_data_length(const zbar_decoder_t *decoder);

/** retrieve last decoded symbol type.
 * @returns the type or ::ZBAR_NONE if no new data available
 */
extern zbar_symbol_type_t
zbar_decoder_get_type(const zbar_decoder_t *decoder);

/** retrieve modifier flags for the last decoded symbol.
 * @returns a bitmask indicating which characteristics were detected
 * during decoding.
 * @since 0.11
 */
extern unsigned int zbar_decoder_get_modifiers(const zbar_decoder_t *decoder);

/** retrieve last decode direction.
 * @returns 1 for forward and -1 for reverse
 * @returns 0 if the decode direction is unknown or does not apply
 * @since 0.11
 */
extern int zbar_decoder_get_direction(const zbar_decoder_t *decoder);

/** setup data handler callback.
 * the registered function will be called by the decoder
 * just before zbar_decode_width() returns a non-zero value.
 * pass a NULL value to disable callbacks.
 * @returns the previously registered handler
 */
extern zbar_decoder_handler_t*
zbar_decoder_set_handler(zbar_decoder_t *decoder,
                         zbar_decoder_handler_t *handler);

/** associate user specified data value with the decoder. */
extern void zbar_decoder_set_userdata(zbar_decoder_t *decoder,
                                      void *userdata);

/** return user specified data value associated with the decoder. */
extern void *zbar_decoder_get_userdata(const zbar_decoder_t *decoder);

/*@}*/

/*------------------------------------------------------------*/
/** @name Scanner interface
 * @anchor c-scanner
 * low-level linear intensity sample stream scanner interface.
 * identifies "bar" edges and measures width between them.
 * optionally passes to bar width decoder
 */
/*@{*/

struct zbar_scanner_s;
/** opaque scanner object. */
typedef struct zbar_scanner_s zbar_scanner_t;

/** constructor.
 * if decoder is non-NULL it will be attached to scanner
 * and called automatically at each new edge
 * current color is initialized to ::ZBAR_SPACE
 * (so an initial BAR->SPACE transition may be discarded)
 */
extern zbar_scanner_t *zbar_scanner_create(zbar_decoder_t *decoder);

/** destructor. */
extern void zbar_scanner_destroy(zbar_scanner_t *scanner);

/** clear all scanner state.
 * also resets an associated decoder
 */
extern zbar_symbol_type_t zbar_scanner_reset(zbar_scanner_t *scanner);

/** mark start of a new scan pass. resets color to ::ZBAR_SPACE.
 * also updates an associated decoder.
 * @returns any decode results flushed from the pipeline
 * @note when not using callback handlers, the return value should
 * be checked the same as zbar_scan_y()
 * @note call zbar_scanner_flush() at least twice before calling this
 * method to ensure no decode results are lost
 */
extern zbar_symbol_type_t zbar_scanner_new_scan(zbar_scanner_t *scanner);

/** flush scanner processing pipeline.
 * forces current scanner position to be a scan boundary.
 * call multiple times (max 3) to completely flush decoder.
 * @returns any decode/scan results flushed from the pipeline
 * @note when not using callback handlers, the return value should
 * be checked the same as zbar_scan_y()
 * @since 0.9
 */
extern zbar_symbol_type_t zbar_scanner_flush(zbar_scanner_t *scanner);

/** process next sample intensity value.
 * intensity (y) is in arbitrary relative units.
 * @returns result of zbar_decode_width() if a decoder is attached,
 * otherwise @returns (::ZBAR_PARTIAL) when new edge is detected
 * or 0 (::ZBAR_NONE) if no new edge is detected
 */
extern zbar_symbol_type_t zbar_scan_y(zbar_scanner_t *scanner,
                                      int y);

/** process next sample from RGB (or BGR) triple. */
static inline zbar_symbol_type_t zbar_scan_rgb24 (zbar_scanner_t *scanner,
                                                    unsigned char *rgb)
{
    return(zbar_scan_y(scanner, rgb[0] + rgb[1] + rgb[2]));
}

/** retrieve last scanned width. */
extern unsigned zbar_scanner_get_width(const zbar_scanner_t *scanner);

/** retrieve sample position of last edge.
 * @since 0.10
 */
extern unsigned zbar_scanner_get_edge(const zbar_scanner_t *scn,
                                      unsigned offset,
                                      int prec);

/** retrieve last scanned color. */
extern zbar_color_t zbar_scanner_get_color(const zbar_scanner_t *scanner);

/*@}*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "config.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

int zbar_parse_config (const char *cfgstr,
                       zbar_symbol_type_t *sym,
                       zbar_config_t *cfg,
                       int *val)
{
    const char *dot, *eq;
    int len;
    char negate;
    if(!cfgstr)
        return(1);

    dot = strchr(cfgstr, '.');
    if(dot) {
        int len = dot - cfgstr;
        if(!len || (len == 1 && !strncmp(cfgstr, "*", len)))
            *sym = 0;
        else if(len < 2)
            return(1);
        else if(!strncmp(cfgstr, "qrcode", len))
            *sym = ZBAR_QRCODE;
        else if(!strncmp(cfgstr, "db", len))
            *sym = ZBAR_DATABAR;
        else if(len < 3)
            return(1);
        else if(!strncmp(cfgstr, "upca", len))
            *sym = ZBAR_UPCA;
        else if(!strncmp(cfgstr, "upce", len))
            *sym = ZBAR_UPCE;
        else if(!strncmp(cfgstr, "ean13", len))
            *sym = ZBAR_EAN13;
        else if(!strncmp(cfgstr, "ean8", len))
            *sym = ZBAR_EAN8;
        else if(!strncmp(cfgstr, "ean5", len))
            *sym = ZBAR_EAN5;
        else if(!strncmp(cfgstr, "ean2", len))
            *sym = ZBAR_EAN2;
        else if(!strncmp(cfgstr, "composite", len))
            *sym = ZBAR_COMPOSITE;
        else if(!strncmp(cfgstr, "i25", len))
            *sym = ZBAR_I25;
        else if(len < 4)
            return(1);
        else if(!strncmp(cfgstr, "scanner", len))
            *sym = ZBAR_PARTIAL; /* FIXME lame */
        else if(!strncmp(cfgstr, "isbn13", len))
            *sym = ZBAR_ISBN13;
        else if(!strncmp(cfgstr, "isbn10", len))
            *sym = ZBAR_ISBN10;
        else if(!strncmp(cfgstr, "db-exp", len))
            *sym = ZBAR_DATABAR_EXP;
        else if(!strncmp(cfgstr, "codabar", len))
            *sym = ZBAR_CODABAR;
        else if(len < 6)
            return(1);
        else if(!strncmp(cfgstr, "code93", len))
            *sym = ZBAR_CODE93;
        else if(!strncmp(cfgstr, "code39", len))
            *sym = ZBAR_CODE39;
        else if(!strncmp(cfgstr, "pdf417", len))
            *sym = ZBAR_PDF417;
        else if(len < 7)
            return(1);
        else if(!strncmp(cfgstr, "code128", len))
            *sym = ZBAR_CODE128;
        else if(!strncmp(cfgstr, "databar", len))
            *sym = ZBAR_DATABAR;
        else if(!strncmp(cfgstr, "databar-exp", len))
            *sym = ZBAR_DATABAR_EXP;
        else
            return(1);
        cfgstr = dot + 1;
    }
    else
        *sym = 0;

    len = strlen(cfgstr);
    eq = strchr(cfgstr, '=');
    if(eq)
        len = eq - cfgstr;
    else
        *val = 1;  /* handle this here so we can override later */
    negate = 0;

    if(len > 3 && !strncmp(cfgstr, "no-", 3)) {
        negate = 1;
        cfgstr += 3;
        len -= 3;
    }

    if(len < 1)
        return(1);
    else if(!strncmp(cfgstr, "y-density", len))
        *cfg = ZBAR_CFG_Y_DENSITY;
    else if(!strncmp(cfgstr, "x-density", len))
        *cfg = ZBAR_CFG_X_DENSITY;
    else if(len < 2)
        return(1);
    else if(!strncmp(cfgstr, "enable", len))
        *cfg = ZBAR_CFG_ENABLE;
    else if(len < 3)
        return(1);
    else if(!strncmp(cfgstr, "disable", len)) {
        *cfg = ZBAR_CFG_ENABLE;
        negate = !negate; /* no-disable ?!? */
    }
    else if(!strncmp(cfgstr, "min-length", len))
        *cfg = ZBAR_CFG_MIN_LEN;
    else if(!strncmp(cfgstr, "max-length", len))
        *cfg = ZBAR_CFG_MAX_LEN;
    else if(!strncmp(cfgstr, "ascii", len))
        *cfg = ZBAR_CFG_ASCII;
    else if(!strncmp(cfgstr, "add-check", len))
        *cfg = ZBAR_CFG_ADD_CHECK;
    else if(!strncmp(cfgstr, "emit-check", len))
        *cfg = ZBAR_CFG_EMIT_CHECK;
    else if(!strncmp(cfgstr, "uncertainty", len))
        *cfg = ZBAR_CFG_UNCERTAINTY;
    else if(!strncmp(cfgstr, "position", len))
        *cfg = ZBAR_CFG_POSITION;
    else
        return(1);

    if(eq) {
#ifdef HAVE_ERRNO_H
        errno = 0;
#endif
        *val = strtol(eq + 1, NULL, 0);
#ifdef HAVE_ERRNO_H
        if(errno)
            return(1);
#endif
    }
    if(negate)
        *val = !*val;

    return(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "image.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define fourcc zbar_fourcc

struct zbar_image_s {
    uint32_t format;            /* fourcc image format code */
    unsigned width, height;     /* image size */
    const void *data;           /* image sample data */
    unsigned long datalen;      /* allocated/mapped size of data */
    unsigned crop_x, crop_y;    /* crop rectangle */
    unsigned crop_w, crop_h;
    void *userdata;             /* user specified data associated w/image */

    unsigned seq;               /* page/frame sequence number */
    zbar_symbol_set_t *syms;    /* decoded result set */
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "refcnt.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

typedef int refcnt_t;

static inline int _zbar_refcnt (refcnt_t *cnt,
                                int delta)
{
    int rc = (*cnt += delta);
    assert(rc >= 0);
    return(rc);
}

void _zbar_refcnt_init(void);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "refcnt.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

void _zbar_refcnt_init ()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "symbol.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define NUM_SYMS  20

typedef struct zbar_point_s {
    int x, y;
} zbar_point_t;

struct zbar_symbol_set_s {
    refcnt_t refcnt;
    int nsyms;                  /* number of filtered symbols */
    zbar_symbol_t *head;        /* first of decoded symbol results */
    zbar_symbol_t *tail;        /* last of unfiltered symbol results */
};

struct zbar_symbol_s {
    zbar_symbol_type_t type;    /* symbol type */
    unsigned int configs;       /* symbology boolean config bitmask */
    unsigned int modifiers;     /* symbology modifier bitmask */
    unsigned int data_alloc;    /* allocation size of data */
    unsigned int datalen;       /* length of binary symbol data */
    char *data;                 /* symbol data */

    unsigned pts_alloc;         /* allocation size of pts */
    unsigned npts;              /* number of points in location polygon */
    zbar_point_t *pts;          /* list of points in location polygon */
    zbar_orientation_t orient;  /* coarse orientation */

    refcnt_t refcnt;            /* reference count */
    zbar_symbol_t *next;        /* linked list of results (or siblings) */
    zbar_symbol_set_t *syms;    /* components of composite result */
    unsigned long time;         /* relative symbol capture time */
    int cache_count;            /* cache state */
    int quality;                /* relative symbol reliability metric */
};

extern int _zbar_get_symbol_hash(zbar_symbol_type_t);

extern void _zbar_symbol_free(zbar_symbol_t*);

extern zbar_symbol_set_t *_zbar_symbol_set_create(void);
extern void _zbar_symbol_set_free(zbar_symbol_set_t*);

static inline void sym_add_point (zbar_symbol_t *sym,
                                  int x,
                                  int y)
{
    int i = sym->npts;
    if(++sym->npts >= sym->pts_alloc)
        sym->pts = realloc(sym->pts, ++sym->pts_alloc * sizeof(zbar_point_t));
    sym->pts[i].x = x;
    sym->pts[i].y = y;
}

static inline void _zbar_symbol_refcnt (zbar_symbol_t *sym,
                                        int delta)
{
    if(!_zbar_refcnt(&sym->refcnt, delta) && delta <= 0)
        _zbar_symbol_free(sym);
}

static inline void _zbar_symbol_set_add (zbar_symbol_set_t *syms,
                                         zbar_symbol_t *sym)
{
    sym->next = syms->head;
    syms->head = sym;
    syms->nsyms++;

    _zbar_symbol_refcnt(sym, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "symbol.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

const char *zbar_get_symbol_name (zbar_symbol_type_t sym)
{
    switch(sym & ZBAR_SYMBOL) {
    case ZBAR_EAN2: return("EAN-2");
    case ZBAR_EAN5: return("EAN-5");
    case ZBAR_EAN8: return("EAN-8");
    case ZBAR_UPCE: return("UPC-E");
    case ZBAR_ISBN10: return("ISBN-10");
    case ZBAR_UPCA: return("UPC-A");
    case ZBAR_EAN13: return("EAN-13");
    case ZBAR_ISBN13: return("ISBN-13");
    case ZBAR_COMPOSITE: return("COMPOSITE");
    case ZBAR_I25: return("I2/5");
    case ZBAR_DATABAR: return("DataBar");
    case ZBAR_DATABAR_EXP: return("DataBar-Exp");
    case ZBAR_CODABAR: return("Codabar");
    case ZBAR_CODE39: return("CODE-39");
    case ZBAR_CODE93: return("CODE-93");
    case ZBAR_CODE128: return("CODE-128");
    case ZBAR_PDF417: return("PDF417");
    case ZBAR_QRCODE: return("QR-Code");
    default: return("UNKNOWN");
    }
}

const char *zbar_get_addon_name (zbar_symbol_type_t sym)
{
    return("");
}

const char *zbar_get_config_name (zbar_config_t cfg)
{
    switch(cfg) {
    case ZBAR_CFG_ENABLE: return("ENABLE");
    case ZBAR_CFG_ADD_CHECK: return("ADD_CHECK");
    case ZBAR_CFG_EMIT_CHECK: return("EMIT_CHECK");
    case ZBAR_CFG_ASCII: return("ASCII");
    case ZBAR_CFG_MIN_LEN: return("MIN_LEN");
    case ZBAR_CFG_MAX_LEN: return("MAX_LEN");
    case ZBAR_CFG_UNCERTAINTY: return("UNCERTAINTY");
    case ZBAR_CFG_POSITION: return("POSITION");
    case ZBAR_CFG_X_DENSITY: return("X_DENSITY");
    case ZBAR_CFG_Y_DENSITY: return("Y_DENSITY");
    default: return("");
    }
}

const char *zbar_get_modifier_name (zbar_modifier_t mod)
{
    switch(mod) {
    case ZBAR_MOD_GS1: return("GS1");
    case ZBAR_MOD_AIM: return("AIM");
    default: return("");
    }
}

const char *zbar_get_orientation_name (zbar_orientation_t orient)
{
    switch(orient) {
    case ZBAR_ORIENT_UP: return("UP");
    case ZBAR_ORIENT_RIGHT: return("RIGHT");
    case ZBAR_ORIENT_DOWN: return("DOWN");
    case ZBAR_ORIENT_LEFT: return("LEFT");
    default: return("UNKNOWN");
    }
}

int _zbar_get_symbol_hash (zbar_symbol_type_t sym)
{
    static const signed char hash[0x20] = {
        0x00, 0x01, 0x10, 0x11,   -1, 0x11, 0x16, 0x0c,
    0x05, 0x06, 0x08,   -1, 0x04, 0x03, 0x07, 0x12,
      -1,   -1,   -1,   -1,   -1,   -1,   -1, 0x02,
      -1, 0x00, 0x12, 0x0c, 0x0b, 0x1d, 0x0a, 0x00,
     };
    int g0 = hash[sym & 0x1f];
    int g1 = hash[~(sym >> 4) & 0x1f];
    assert(g0 >= 0 && g1 >= 0);
    if(g0 < 0 || g1 < 0)
        return(0);
    return((g0 + g1) & 0x1f);
}

void _zbar_symbol_free (zbar_symbol_t *sym)
{
    if(sym->syms) {
        zbar_symbol_set_ref(sym->syms, -1);
        sym->syms = NULL;
    }
    if(sym->pts)
        free(sym->pts);
    if(sym->data_alloc && sym->data)
        free(sym->data);
    free(sym);
}

void zbar_symbol_ref (const zbar_symbol_t *sym,
                      int refs)
{
    zbar_symbol_t *ncsym = (zbar_symbol_t*)sym;
    _zbar_symbol_refcnt(ncsym, refs);
}

zbar_symbol_type_t zbar_symbol_get_type (const zbar_symbol_t *sym)
{
    return(sym->type);
}

unsigned int zbar_symbol_get_configs (const zbar_symbol_t *sym)
{
    return(sym->configs);
}

unsigned int zbar_symbol_get_modifiers (const zbar_symbol_t *sym)
{
    return(sym->modifiers);
}

const char *zbar_symbol_get_data (const zbar_symbol_t *sym)
{
    return(sym->data);
}

unsigned int zbar_symbol_get_data_length (const zbar_symbol_t *sym)
{
    return(sym->datalen);
}

int zbar_symbol_get_count (const zbar_symbol_t *sym)
{
    return(sym->cache_count);
}

int zbar_symbol_get_quality (const zbar_symbol_t *sym)
{
    return(sym->quality);
}

unsigned zbar_symbol_get_loc_size (const zbar_symbol_t *sym)
{
    return(sym->npts);
}

int zbar_symbol_get_loc_x (const zbar_symbol_t *sym,
                           unsigned idx)
{
    if(idx < sym->npts)
        return(sym->pts[idx].x);
    else
        return(-1);
}

int zbar_symbol_get_loc_y (const zbar_symbol_t *sym,
                           unsigned idx)
{
    if(idx < sym->npts)
        return(sym->pts[idx].y);
    else
        return(-1);
}

zbar_orientation_t zbar_symbol_get_orientation (const zbar_symbol_t *sym)
{
    return(sym->orient);
}

const zbar_symbol_t *zbar_symbol_next (const zbar_symbol_t *sym)
{
    return((sym) ? sym->next : NULL);
}

const zbar_symbol_set_t*
zbar_symbol_get_components (const zbar_symbol_t *sym)
{
    return(sym->syms);
}

const zbar_symbol_t *zbar_symbol_first_component (const zbar_symbol_t *sym)
{
    return((sym && sym->syms) ? sym->syms->head : NULL);
}

zbar_symbol_set_t *_zbar_symbol_set_create ()
{
    zbar_symbol_set_t *syms = calloc(1, sizeof(*syms));
    _zbar_refcnt(&syms->refcnt, 1);
    return(syms);
}

inline void _zbar_symbol_set_free (zbar_symbol_set_t *syms)
{
    zbar_symbol_t *sym, *next;
    for(sym = syms->head; sym; sym = next) {
        next = sym->next;
        sym->next = NULL;
        _zbar_symbol_refcnt(sym, -1);
    }
    syms->head = NULL;
    free(syms);
}

void zbar_symbol_set_ref (const zbar_symbol_set_t *syms,
                          int delta)
{
    zbar_symbol_set_t *ncsyms = (zbar_symbol_set_t*)syms;
    if(!_zbar_refcnt(&ncsyms->refcnt, delta) && delta <= 0)
        _zbar_symbol_set_free(ncsyms);
}

int zbar_symbol_set_get_size (const zbar_symbol_set_t *syms)
{
    return(syms->nsyms);
}

const zbar_symbol_t*
zbar_symbol_set_first_symbol (const zbar_symbol_set_t *syms)
{
    zbar_symbol_t *sym = syms->tail;
    if(sym)
        return(sym->next);
    return(syms->head);
}

const zbar_symbol_t*
zbar_symbol_set_first_unfiltered (const zbar_symbol_set_t *syms)
{
    return(syms->head);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "img_scanner.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* internal image scanner APIs for 2D readers */

extern zbar_symbol_t *_zbar_image_scanner_alloc_sym(zbar_image_scanner_t*,
                                                    zbar_symbol_type_t,
                                                    int);
extern void _zbar_image_scanner_add_sym(zbar_image_scanner_t*,
                                        zbar_symbol_t*);
extern void _zbar_image_scanner_recycle_syms(zbar_image_scanner_t*,
                                             zbar_symbol_t*);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "img_scanner.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#if 0
# define ASSERT_POS \
    assert(p == data + x + y * (intptr_t)w)
#else
# define ASSERT_POS
#endif

/* FIXME cache setting configurability */

/* time interval for which two images are considered "nearby"
 */
#define CACHE_PROXIMITY   1000 /* ms */

/* time that a result must *not* be detected before
 * it will be reported again
 */
#define CACHE_HYSTERESIS  2000 /* ms */

/* time after which cache entries are invalidated
 */
#define CACHE_TIMEOUT     (CACHE_HYSTERESIS * 2) /* ms */

#define NUM_SCN_CFGS (ZBAR_CFG_Y_DENSITY - ZBAR_CFG_X_DENSITY + 1)

#define CFG(iscn, cfg) ((iscn)->configs[(cfg) - ZBAR_CFG_X_DENSITY])
#define TEST_CFG(iscn, cfg) (((iscn)->config >> ((cfg) - ZBAR_CFG_POSITION)) & 1)

#ifndef NO_STATS
# define STAT(x) iscn->stat_##x++
#else
# define STAT(...)
# define dump_stats(...)
#endif

#define RECYCLE_BUCKETS     5

typedef struct recycle_bucket_s {
    int nsyms;
    zbar_symbol_t *head;
} recycle_bucket_t;

/* image scanner state */
struct zbar_image_scanner_s {
    zbar_scanner_t *scn;        /* associated linear intensity scanner */
    zbar_decoder_t *dcode;      /* associated symbol decoder */
#ifdef ENABLE_QRCODE
    qr_reader *qr;              /* QR Code 2D reader */
#endif

    const void *userdata;       /* application data */
    /* user result callback */
    zbar_image_data_handler_t *handler;

    unsigned long time;         /* scan start time */
    zbar_image_t *img;          /* currently scanning image *root* */
    int dx, dy, du, umin, v;    /* current scan direction */
    zbar_symbol_set_t *syms;    /* previous decode results */
    /* recycled symbols in 4^n size buckets */
    recycle_bucket_t recycle[RECYCLE_BUCKETS];

    int enable_cache;           /* current result cache state */
    zbar_symbol_t *cache;       /* inter-image result cache entries */

    /* configuration settings */
    unsigned config;            /* config flags */
    unsigned ean_config;
    int configs[NUM_SCN_CFGS];  /* int valued configurations */
    int sym_configs[1][NUM_SYMS]; /* per-symbology configurations */

#ifndef NO_STATS
    int stat_syms_new;
    int stat_iscn_syms_inuse, stat_iscn_syms_recycle;
    int stat_img_syms_inuse, stat_img_syms_recycle;
    int stat_sym_new;
    int stat_sym_recycle[RECYCLE_BUCKETS];
#endif
};

void _zbar_image_scanner_recycle_syms (zbar_image_scanner_t *iscn,
                                       zbar_symbol_t *sym)
{
    zbar_symbol_t *next = NULL;
    for(; sym; sym = next) {
        next = sym->next;
        if(sym->refcnt && _zbar_refcnt(&sym->refcnt, -1)) {
            /* unlink referenced symbol */
            /* FIXME handle outstanding component refs (currently unsupported)
             */
            assert(sym->data_alloc);
            sym->next = NULL;
        }
        else {
            int i;
            recycle_bucket_t *bucket;
            /* recycle unreferenced symbol */
            if(!sym->data_alloc) {
                sym->data = NULL;
                sym->datalen = 0;
            }
            if(sym->syms) {
                if(_zbar_refcnt(&sym->syms->refcnt, -1))
                    assert(0);
                _zbar_image_scanner_recycle_syms(iscn, sym->syms->head);
                sym->syms->head = NULL;
                _zbar_symbol_set_free(sym->syms);
                sym->syms = NULL;
            }
            for(i = 0; i < RECYCLE_BUCKETS; i++)
                if(sym->data_alloc < 1 << (i * 2))
                    break;
            if(i == RECYCLE_BUCKETS) {
                assert(sym->data);
                free(sym->data);
                sym->data = NULL;
                sym->data_alloc = 0;
                i = 0;
            }
            bucket = &iscn->recycle[i];
            /* FIXME cap bucket fill */
            bucket->nsyms++;
            sym->next = bucket->head;
            bucket->head = sym;
        }
    }
}

static inline int recycle_syms (zbar_image_scanner_t *iscn,
                                zbar_symbol_set_t *syms)
{
    if(_zbar_refcnt(&syms->refcnt, -1))
        return(1);

    _zbar_image_scanner_recycle_syms(iscn, syms->head);
    syms->head = syms->tail = NULL;
    syms->nsyms = 0;
    return(0);
}

inline void zbar_image_scanner_recycle_image (zbar_image_scanner_t *iscn,
                                              zbar_image_t *img)
{
    zbar_symbol_set_t *syms = iscn->syms;
    if(syms && syms->refcnt) {
        if(recycle_syms(iscn, syms)) {
            STAT(iscn_syms_inuse);
            iscn->syms = NULL;
        }
        else
            STAT(iscn_syms_recycle);
    }

    syms = img->syms;
    img->syms = NULL;
    if(syms && recycle_syms(iscn, syms))
        STAT(img_syms_inuse);
    else if(syms) {
        STAT(img_syms_recycle);

        /* select one set to resurrect, destroy the other */
        if(iscn->syms)
            _zbar_symbol_set_free(syms);
        else
            iscn->syms = syms;
    }
}

inline zbar_symbol_t*
_zbar_image_scanner_alloc_sym (zbar_image_scanner_t *iscn,
                               zbar_symbol_type_t type,
                               int datalen)
{
    /* recycle old or alloc new symbol */
    zbar_symbol_t *sym = NULL;
    int i;
    for(i = 0; i < RECYCLE_BUCKETS - 1; i++)
        if(datalen <= 1 << (i * 2))
            break;

    for(; i > 0; i--)
        if((sym = iscn->recycle[i].head)) {
            STAT(sym_recycle[i]);
            break;
        }

    if(sym) {
        iscn->recycle[i].head = sym->next;
        sym->next = NULL;
        assert(iscn->recycle[i].nsyms);
        iscn->recycle[i].nsyms--;
    }
    else {
        sym = calloc(1, sizeof(zbar_symbol_t));
        STAT(sym_new);
    }

    /* init new symbol */
    sym->type = type;
    sym->quality = 1;
    sym->npts = 0;
    sym->orient = ZBAR_ORIENT_UNKNOWN;
    sym->cache_count = 0;
    sym->time = iscn->time;
    assert(!sym->syms);

    if(datalen > 0) {
        sym->datalen = datalen - 1;
        if(sym->data_alloc < datalen) {
            if(sym->data)
                free(sym->data);
            sym->data_alloc = datalen;
            sym->data = malloc(datalen);
        }
    }
    else {
        if(sym->data)
            free(sym->data);
        sym->data = NULL;
        sym->datalen = sym->data_alloc = 0;
    }
    return(sym);
}

static inline zbar_symbol_t *cache_lookup (zbar_image_scanner_t *iscn,
                                           zbar_symbol_t *sym)
{
    /* search for matching entry in cache */
    zbar_symbol_t **entry = &iscn->cache;
    while(*entry) {
        if((*entry)->type == sym->type &&
           (*entry)->datalen == sym->datalen &&
           !memcmp((*entry)->data, sym->data, sym->datalen))
            break;
        if((sym->time - (*entry)->time) > CACHE_TIMEOUT) {
            /* recycle stale cache entry */
            zbar_symbol_t *next = (*entry)->next;
            (*entry)->next = NULL;
            _zbar_image_scanner_recycle_syms(iscn, *entry);
            *entry = next;
        }
        else
            entry = &(*entry)->next;
    }
    return(*entry);
}

static inline void cache_sym (zbar_image_scanner_t *iscn,
                              zbar_symbol_t *sym)
{
    if(iscn->enable_cache) {
        uint32_t age, near_thresh, far_thresh, dup;
        zbar_symbol_t *entry = cache_lookup(iscn, sym);
        if(!entry) {
            /* FIXME reuse sym */
            entry = _zbar_image_scanner_alloc_sym(iscn, sym->type,
                                                  sym->datalen + 1);
            entry->configs = sym->configs;
            entry->modifiers = sym->modifiers;
            memcpy(entry->data, sym->data, sym->datalen);
            entry->time = sym->time - CACHE_HYSTERESIS;
            entry->cache_count = 0;
            /* add to cache */
            entry->next = iscn->cache;
            iscn->cache = entry;
        }

        /* consistency check and hysteresis */
        age = sym->time - entry->time;
        entry->time = sym->time;
        near_thresh = (age < CACHE_PROXIMITY);
        far_thresh = (age >= CACHE_HYSTERESIS);
        dup = (entry->cache_count >= 0);
        if((!dup && !near_thresh) || far_thresh) {
            int type = sym->type;
            int h = _zbar_get_symbol_hash(type);
            entry->cache_count = -iscn->sym_configs[0][h];
        }
        else if(dup || near_thresh)
            entry->cache_count++;

        sym->cache_count = entry->cache_count;
    }
    else
        sym->cache_count = 0;
}

void _zbar_image_scanner_add_sym(zbar_image_scanner_t *iscn,
                                 zbar_symbol_t *sym)
{
    zbar_symbol_set_t *syms;
    cache_sym(iscn, sym);

    syms = iscn->syms;
    if(sym->cache_count || !syms->tail) {
        sym->next = syms->head;
        syms->head = sym;
    }
    else {
        sym->next = syms->tail->next;
        syms->tail->next = sym;
    }

    if(!sym->cache_count)
        syms->nsyms++;
    else if(!syms->tail)
        syms->tail = sym;

    _zbar_symbol_refcnt(sym, 1);
}

#ifdef ENABLE_QRCODE
extern qr_finder_line *_zbar_decoder_get_qr_finder_line(zbar_decoder_t*);

# define QR_FIXED(v, rnd) ((((v) << 1) + (rnd)) << (QR_FINDER_SUBPREC - 1))
# define PRINT_FIXED(val, prec) \
    ((val) >> (prec)),         \
        (1000 * ((val) & ((1 << (prec)) - 1)) / (1 << (prec)))

static inline void qr_handler (zbar_image_scanner_t *iscn)
{
    unsigned u;
    int vert;
    qr_finder_line *line = _zbar_decoder_get_qr_finder_line(iscn->dcode);
    assert(line);
    u = zbar_scanner_get_edge(iscn->scn, line->pos[0],
                              QR_FINDER_SUBPREC);
    line->boffs = u - zbar_scanner_get_edge(iscn->scn, line->boffs,
                                            QR_FINDER_SUBPREC);
    line->len = zbar_scanner_get_edge(iscn->scn, line->len,
                                      QR_FINDER_SUBPREC);
    line->eoffs = zbar_scanner_get_edge(iscn->scn, line->eoffs,
                                        QR_FINDER_SUBPREC) - line->len;
    line->len -= u;

    u = QR_FIXED(iscn->umin, 0) + iscn->du * u;
    if(iscn->du < 0) {
        int tmp = line->boffs;
        line->boffs = line->eoffs;
        line->eoffs = tmp;
        u -= line->len;
    }
    vert = !iscn->dx;
    line->pos[vert] = u;
    line->pos[!vert] = QR_FIXED(iscn->v, 1);

    _zbar_qr_found_line(iscn->qr, vert, line);
}
#endif

static void symbol_handler (zbar_decoder_t *dcode)
{
    zbar_image_scanner_t *iscn = zbar_decoder_get_userdata(dcode);
    zbar_symbol_type_t type = zbar_decoder_get_type(dcode);
    int x = 0, y = 0, dir;
    const char *data;
    unsigned datalen;
    zbar_symbol_t *sym;

#ifdef ENABLE_QRCODE
    if(type == ZBAR_QRCODE) {
        qr_handler(iscn);
        return;
    }
#else
    assert(type != ZBAR_QRCODE);
#endif

    if(TEST_CFG(iscn, ZBAR_CFG_POSITION)) {
        /* tmp position fixup */
        int w = zbar_scanner_get_width(iscn->scn);
        int u = iscn->umin + iscn->du * zbar_scanner_get_edge(iscn->scn, w, 0);
        if(iscn->dx) {
            x = u;
            y = iscn->v;
        }
        else {
            x = iscn->v;
            y = u;
        }
    }

    /* FIXME debug flag to save/display all PARTIALs */
    if(type <= ZBAR_PARTIAL) {
        zprintf(256, "partial symbol @(%d,%d)\n", x, y);
        return;
    }

    data = zbar_decoder_get_data(dcode);
    datalen = zbar_decoder_get_data_length(dcode);

    /* FIXME need better symbol matching */
    for(sym = iscn->syms->head; sym; sym = sym->next)
        if(sym->type == type &&
           sym->datalen == datalen &&
           !memcmp(sym->data, data, datalen)) {
            sym->quality++;
            zprintf(224, "dup symbol @(%d,%d): dup %s: %.20s\n",
                    x, y, zbar_get_symbol_name(type), data);
            if(TEST_CFG(iscn, ZBAR_CFG_POSITION))
                /* add new point to existing set */
                /* FIXME should be polygon */
                sym_add_point(sym, x, y);
            return;
        }

    sym = _zbar_image_scanner_alloc_sym(iscn, type, datalen + 1);
    sym->configs = zbar_decoder_get_configs(dcode, type);
    sym->modifiers = zbar_decoder_get_modifiers(dcode);
    /* FIXME grab decoder buffer */
    memcpy(sym->data, data, datalen + 1);

    /* initialize first point */
    if(TEST_CFG(iscn, ZBAR_CFG_POSITION)) {
        zprintf(192, "new symbol @(%d,%d): %s: %.20s\n",
                x, y, zbar_get_symbol_name(type), data);
        sym_add_point(sym, x, y);
    }

    dir = zbar_decoder_get_direction(dcode);
    if(dir)
        sym->orient = (iscn->dy != 0) + ((iscn->du ^ dir) & 2);

    _zbar_image_scanner_add_sym(iscn, sym);
}

zbar_image_scanner_t *zbar_image_scanner_create ()
{
    zbar_image_scanner_t *iscn = calloc(1, sizeof(zbar_image_scanner_t));
    if(!iscn)
        return(NULL);
    iscn->dcode = zbar_decoder_create();
    iscn->scn = zbar_scanner_create(iscn->dcode);
    if(!iscn->dcode || !iscn->scn) {
        zbar_image_scanner_destroy(iscn);
        return(NULL);
    }
    zbar_decoder_set_userdata(iscn->dcode, iscn);
    zbar_decoder_set_handler(iscn->dcode, symbol_handler);

#ifdef ENABLE_QRCODE
    iscn->qr = _zbar_qr_create();
#endif

    /* apply default configuration */
    CFG(iscn, ZBAR_CFG_X_DENSITY) = 1;
    CFG(iscn, ZBAR_CFG_Y_DENSITY) = 1;
    zbar_image_scanner_set_config(iscn, 0, ZBAR_CFG_POSITION, 1);
    zbar_image_scanner_set_config(iscn, 0, ZBAR_CFG_UNCERTAINTY, 2);
    zbar_image_scanner_set_config(iscn, ZBAR_QRCODE, ZBAR_CFG_UNCERTAINTY, 0);
    zbar_image_scanner_set_config(iscn, ZBAR_CODE128, ZBAR_CFG_UNCERTAINTY, 0);
    zbar_image_scanner_set_config(iscn, ZBAR_CODE93, ZBAR_CFG_UNCERTAINTY, 0);
    zbar_image_scanner_set_config(iscn, ZBAR_CODE39, ZBAR_CFG_UNCERTAINTY, 0);
    zbar_image_scanner_set_config(iscn, ZBAR_CODABAR, ZBAR_CFG_UNCERTAINTY, 1);
    zbar_image_scanner_set_config(iscn, ZBAR_COMPOSITE, ZBAR_CFG_UNCERTAINTY, 0);
    return(iscn);
}

#ifndef NO_STATS
static inline void dump_stats (const zbar_image_scanner_t *iscn)
{
    int i;
    zprintf(1, "symbol sets allocated   = %-4d\n", iscn->stat_syms_new);
    zprintf(1, "    scanner syms in use = %-4d\trecycled  = %-4d\n",
            iscn->stat_iscn_syms_inuse, iscn->stat_iscn_syms_recycle);
    zprintf(1, "    image syms in use   = %-4d\trecycled  = %-4d\n",
            iscn->stat_img_syms_inuse, iscn->stat_img_syms_recycle);
    zprintf(1, "symbols allocated       = %-4d\n", iscn->stat_sym_new);
    for(i = 0; i < RECYCLE_BUCKETS; i++)
        zprintf(1, "     recycled[%d]        = %-4d\n",
                i, iscn->stat_sym_recycle[i]);
}
#endif

void zbar_image_scanner_destroy (zbar_image_scanner_t *iscn)
{
    int i;
    dump_stats(iscn);
    if(iscn->syms) {
        if(iscn->syms->refcnt)
            zbar_symbol_set_ref(iscn->syms, -1);
        else
            _zbar_symbol_set_free(iscn->syms);
        iscn->syms = NULL;
    }
    if(iscn->scn)
        zbar_scanner_destroy(iscn->scn);
    iscn->scn = NULL;
    if(iscn->dcode)
        zbar_decoder_destroy(iscn->dcode);
    iscn->dcode = NULL;
    for(i = 0; i < RECYCLE_BUCKETS; i++) {
        zbar_symbol_t *sym, *next;
        for(sym = iscn->recycle[i].head; sym; sym = next) {
            next = sym->next;
            _zbar_symbol_free(sym);
        }
    }
#ifdef ENABLE_QRCODE
    if(iscn->qr) {
        _zbar_qr_destroy(iscn->qr);
        iscn->qr = NULL;
    }
#endif
    free(iscn);
}

zbar_image_data_handler_t*
zbar_image_scanner_set_data_handler (zbar_image_scanner_t *iscn,
                                     zbar_image_data_handler_t *handler,
                                     const void *userdata)
{
    zbar_image_data_handler_t *result = iscn->handler;
    iscn->handler = handler;
    iscn->userdata = userdata;
    return(result);
}

int zbar_image_scanner_set_config (zbar_image_scanner_t *iscn,
                                   zbar_symbol_type_t sym,
                                   zbar_config_t cfg,
                                   int val)
{
    if((sym == 0 || sym == ZBAR_COMPOSITE) && cfg == ZBAR_CFG_ENABLE) {
        iscn->ean_config = !!val;
        if(sym)
            return(0);
    }

    if(cfg < ZBAR_CFG_UNCERTAINTY)
        return(zbar_decoder_set_config(iscn->dcode, sym, cfg, val));

    if(cfg < ZBAR_CFG_POSITION) {
        int c, i;
        if(cfg > ZBAR_CFG_UNCERTAINTY)
            return(1);
        c = cfg - ZBAR_CFG_UNCERTAINTY;
        if(sym > ZBAR_PARTIAL) {
            i = _zbar_get_symbol_hash(sym);
            iscn->sym_configs[c][i] = val;
        }
        else
            for(i = 0; i < NUM_SYMS; i++)
                iscn->sym_configs[c][i] = val;
        return(0);
    }

    if(sym > ZBAR_PARTIAL)
        return(1);

    if(cfg >= ZBAR_CFG_X_DENSITY && cfg <= ZBAR_CFG_Y_DENSITY) {
        CFG(iscn, cfg) = val;
        return(0);
    }

    if(cfg > ZBAR_CFG_POSITION)
        return(1);
    cfg -= ZBAR_CFG_POSITION;

    if(!val)
        iscn->config &= ~(1 << cfg);
    else if(val == 1)
        iscn->config |= (1 << cfg);
    else
        return(1);

    return(0);
}

void zbar_image_scanner_enable_cache (zbar_image_scanner_t *iscn,
                                      int enable)
{
    if(iscn->cache) {
        /* recycle all cached syms */
        _zbar_image_scanner_recycle_syms(iscn, iscn->cache);
        iscn->cache = NULL;
    }
    iscn->enable_cache = (enable) ? 1 : 0;
}

const zbar_symbol_set_t *
zbar_image_scanner_get_results (const zbar_image_scanner_t *iscn)
{
    return(iscn->syms);
}

static inline void quiet_border (zbar_image_scanner_t *iscn)
{
    /* flush scanner pipeline */
    zbar_scanner_t *scn = iscn->scn;
    zbar_scanner_flush(scn);
    zbar_scanner_flush(scn);
    zbar_scanner_new_scan(scn);
}

#define movedelta(dx, dy) do {                  \
        x += (dx);                              \
        y += (dy);                              \
        p += (dx) + ((uintptr_t)(dy) * w);       \
    } while(0);

int zbar_scan_image (zbar_image_scanner_t *iscn,
                     zbar_image_t *img)
{
    zbar_symbol_set_t *syms;
    const uint8_t *data;
    zbar_scanner_t *scn = iscn->scn;
    unsigned w, h, cx1, cy1;
    int density;

    /* timestamp image
     * FIXME prefer video timestamp
     */
    iscn->time = 0;//_zbar_timer_now();

#ifdef ENABLE_QRCODE
    _zbar_qr_reset(iscn->qr);
#endif

    /* image must be in grayscale format */
    if(img->format != fourcc('Y','8','0','0') &&
       img->format != fourcc('G','R','E','Y'))
        return(-1);
    iscn->img = img;

    /* recycle previous scanner and image results */
    zbar_image_scanner_recycle_image(iscn, img);
    syms = iscn->syms;
    if(!syms) {
        syms = iscn->syms = _zbar_symbol_set_create();
        STAT(syms_new);
        zbar_symbol_set_ref(syms, 1);
    }
    else
        zbar_symbol_set_ref(syms, 2);
    img->syms = syms;

    w = img->width;
    h = img->height;
    cx1 = img->crop_x + img->crop_w;
    assert(cx1 <= w);
    cy1 = img->crop_y + img->crop_h;
    assert(cy1 <= h);
    data = img->data;

    zbar_image_write_png(img, "debug.png");
    svg_open("debug.svg", 0, 0, w, h);
    svg_image("debug.png", w, h);

    zbar_scanner_new_scan(scn);

    density = CFG(iscn, ZBAR_CFG_Y_DENSITY);
    if(density > 0) {
        const uint8_t *p = data;
        int x = 0, y = 0;

        int border = (((img->crop_h - 1) % density) + 1) / 2;
        if(border > img->crop_h / 2)
            border = img->crop_h / 2;
        border += img->crop_y;
        assert(border <= h);
        svg_group_start("scanner", 0, 1, 1, 0, 0);
        iscn->dy = 0;

        movedelta(img->crop_x, border);
        iscn->v = y;

        while(y < cy1) {
            int cx0 = img->crop_x;;
            zprintf(128, "img_x+: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", 1. / 32, 0, y + 0.5);
            iscn->dx = iscn->du = 1;
            iscn->umin = cx0;
            while(x < cx1) {
                uint8_t d = *p;
                movedelta(1, 0);
                zbar_scan_y(scn, d);
            }
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(-1, density);
            iscn->v = y;
            if(y >= cy1)
                break;

            zprintf(128, "img_x-: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", -1. / 32, w, y + 0.5);
            iscn->dx = iscn->du = -1;
            iscn->umin = cx1;
            while(x >= cx0) {
                uint8_t d = *p;
                movedelta(-1, 0);
                zbar_scan_y(scn, d);
            }
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(1, density);
            iscn->v = y;
        }
        svg_group_end();
    }
    iscn->dx = 0;

    density = CFG(iscn, ZBAR_CFG_X_DENSITY);
    if(density > 0) {
        const uint8_t *p = data;
        int x = 0, y = 0;

        int border = (((img->crop_w - 1) % density) + 1) / 2;
        if(border > img->crop_w / 2)
            border = img->crop_w / 2;
        border += img->crop_x;
        assert(border <= w);
        svg_group_start("scanner", 90, 1, -1, 0, 0);
        movedelta(border, img->crop_y);
        iscn->v = x;

        while(x < cx1) {
            int cy0 = img->crop_y;
            zprintf(128, "img_y+: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", 1. / 32, 0, x + 0.5);
            iscn->dy = iscn->du = 1;
            iscn->umin = cy0;
            while(y < cy1) {
                uint8_t d = *p;
                movedelta(0, 1);
                zbar_scan_y(scn, d);
            }
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(density, -1);
            iscn->v = x;
            if(x >= cx1)
                break;

            zprintf(128, "img_y-: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", -1. / 32, h, x + 0.5);
            iscn->dy = iscn->du = -1;
            iscn->umin = cy1;
            while(y >= cy0) {
                uint8_t d = *p;
                movedelta(0, -1);
                zbar_scan_y(scn, d);
            }
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(density, 1);
            iscn->v = x;
        }
        svg_group_end();
    }
    iscn->dy = 0;
    iscn->img = NULL;

#ifdef ENABLE_QRCODE
    _zbar_qr_decode(iscn->qr, iscn, img);
#endif

    /* FIXME tmp hack to filter bad EAN results */
    /* FIXME tmp hack to merge simple case EAN add-ons */
    char filter = (!iscn->enable_cache &&
                   (density == 1 || CFG(iscn, ZBAR_CFG_Y_DENSITY) == 1));
    int nean = 0, naddon = 0;
    if(syms->nsyms) {
        zbar_symbol_t **symp;
        for(symp = &syms->head; *symp; ) {
            zbar_symbol_t *sym = *symp;
            if(sym->cache_count <= 0 &&
               ((sym->type < ZBAR_COMPOSITE && sym->type > ZBAR_PARTIAL) ||
                sym->type == ZBAR_DATABAR ||
                sym->type == ZBAR_DATABAR_EXP ||
                sym->type == ZBAR_CODABAR))
            {
            if((sym->type == ZBAR_CODABAR || filter) && sym->quality < 4) {
                    if(iscn->enable_cache) {
                        /* revert cache update */
                        zbar_symbol_t *entry = cache_lookup(iscn, sym);
                        if(entry)
                            entry->cache_count--;
                        else
                            assert(0);
                    }

                    /* recycle */
                    *symp = sym->next;
                    syms->nsyms--;
                    sym->next = NULL;
                    _zbar_image_scanner_recycle_syms(iscn, sym);
                    continue;
                }
                else if(sym->type < ZBAR_COMPOSITE &&
                        sym->type != ZBAR_ISBN10)
                {
                    if(sym->type > ZBAR_EAN5)
                        nean++;
                    else
                        naddon++;
                }
            }
            symp = &sym->next;
        }

        if(nean == 1 && naddon == 1 && iscn->ean_config) {
            /* create container symbol for composite result */
            zbar_symbol_t *ean = NULL, *addon = NULL;
            for(symp = &syms->head; *symp; ) {
                zbar_symbol_t *sym = *symp;
                if(sym->type < ZBAR_COMPOSITE && sym->type > ZBAR_PARTIAL) {
                    /* move to composite */
                    *symp = sym->next;
                    syms->nsyms--;
                    sym->next = NULL;
                    if(sym->type <= ZBAR_EAN5)
                        addon = sym;
                    else
                        ean = sym;
                }
                else
                    symp = &sym->next;
            }
            assert(ean);
            assert(addon);

            int datalen = ean->datalen + addon->datalen + 1;
            zbar_symbol_t *ean_sym =
                _zbar_image_scanner_alloc_sym(iscn, ZBAR_COMPOSITE, datalen);
            ean_sym->orient = ean->orient;
            ean_sym->syms = _zbar_symbol_set_create();
            memcpy(ean_sym->data, ean->data, ean->datalen);
            memcpy(ean_sym->data + ean->datalen,
                   addon->data, addon->datalen + 1);
            ean_sym->syms->head = ean;
            ean->next = addon;
            ean_sym->syms->nsyms = 2;
            _zbar_image_scanner_add_sym(iscn, ean_sym);
        }
    }

    if(syms->nsyms && iscn->handler)
        iscn->handler(img, iscn->userdata);

    svg_close();
    return(syms->nsyms);
}

#undef TEST_CFG
#undef CFG

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_CFGS (ZBAR_CFG_MAX_LEN - ZBAR_CFG_MIN_LEN + 1)

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "ean.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* state of each parallel decode attempt */
typedef struct ean_pass_s {
    signed char state;          /* module position of w[idx] in symbol */
#define STATE_REV   0x80        /*   scan direction reversed */
#define STATE_ADDON 0x40        /*   scanning add-on */
#define STATE_IDX   0x3f        /*   element offset into symbol */
    unsigned width;             /* width of last character */
    unsigned char raw[7];       /* decode in process */
} ean_pass_t;

/* EAN/UPC specific decode state */
typedef struct ean_decoder_s {
    ean_pass_t pass[4];         /* state of each parallel decode attempt */
    zbar_symbol_type_t left;    /* current holding buffer contents */
    zbar_symbol_type_t right;
    int direction;              /* scan direction */
    unsigned s4, width;         /* character width */
    signed char buf[18];        /* holding buffer */

    signed char enable;
    unsigned ean13_config;
    unsigned ean8_config;
    unsigned upca_config;
    unsigned upce_config;
    unsigned isbn10_config;
    unsigned isbn13_config;
    unsigned ean5_config;
    unsigned ean2_config;
} ean_decoder_t;

/* reset EAN/UPC pass specific state */
static inline void ean_new_scan (ean_decoder_t *ean)
{
    ean->pass[0].state = ean->pass[1].state = -1;
    ean->pass[2].state = ean->pass[3].state = -1;
    ean->s4 = 0;
}

/* reset all EAN/UPC state */
static inline void ean_reset (ean_decoder_t *ean)
{
    ean_new_scan(ean);
    ean->left = ean->right = ZBAR_NONE;
}

static inline unsigned ean_get_config (ean_decoder_t *ean,
                                       zbar_symbol_type_t sym)
{
    switch(sym) {
    case ZBAR_EAN2:   return(ean->ean2_config);
    case ZBAR_EAN5:   return(ean->ean5_config);
    case ZBAR_EAN8:   return(ean->ean8_config);
    case ZBAR_UPCE:   return(ean->upce_config);
    case ZBAR_ISBN10: return(ean->isbn10_config);
    case ZBAR_UPCA:   return(ean->upca_config);
    case ZBAR_EAN13:  return(ean->ean13_config);
    case ZBAR_ISBN13: return(ean->isbn13_config);
    default:          return(0);
    }
}

/* decode EAN/UPC symbols */
zbar_symbol_type_t _zbar_decode_ean(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "i25.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* interleaved 2 of 5 specific decode state */
typedef struct i25_decoder_s {
    unsigned direction : 1;     /* scan direction: 0=fwd/space, 1=rev/bar */
    unsigned element : 4;       /* element offset 0-8 */
    int character : 12;         /* character position in symbol */
    unsigned s10;               /* current character width */
    unsigned width;             /* last character width */
    unsigned char buf[4];       /* initial scan buffer */

    unsigned config;
    int configs[NUM_CFGS];      /* int valued configurations */
} i25_decoder_t;

/* reset interleaved 2 of 5 specific state */
static inline void i25_reset (i25_decoder_t *i25)
{
    i25->direction = 0;
    i25->element = 0;
    i25->character = -1;
    i25->s10 = 0;
}

/* decode interleaved 2 of 5 symbols */
zbar_symbol_type_t _zbar_decode_i25(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "databar.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define DATABAR_MAX_SEGMENTS 32

/* active DataBar (partial) segment entry */
typedef struct databar_segment_s {
    signed finder : 5;          /* finder pattern */
    unsigned exp : 1;           /* DataBar expanded finder */
    unsigned color : 1;         /* finder coloring */
    unsigned side : 1;          /* data character side of finder */

    unsigned partial : 1;       /* unpaired partial segment */
    unsigned count : 7;         /* times encountered */
    unsigned epoch : 8;         /* age, in characters scanned */
    unsigned check : 8;         /* bar checksum */
    signed short data;          /* decoded character data */
    unsigned short width;       /* measured width of finder (14 modules) */
} databar_segment_t;

/* DataBar specific decode state */
typedef struct databar_decoder_s {
    unsigned config;            /* decoder configuration flags */
    unsigned config_exp;

    unsigned csegs : 8;         /* allocated segments */
    unsigned epoch : 8;         /* current scan */

    databar_segment_t *segs;    /* active segment list */
    signed char chars[16];      /* outstanding character indices */
} databar_decoder_t;

/* reset DataBar segment decode state */
static inline void databar_new_scan (databar_decoder_t *db)
{
    int i;
    for(i = 0; i < 16; i++)
        if(db->chars[i] >= 0) {
            databar_segment_t *seg = db->segs + db->chars[i];
            if(seg->partial)
                seg->finder = -1;
            db->chars[i] = -1;
        }
}

/* reset DataBar accumulated segments */
static inline void databar_reset (databar_decoder_t *db)
{
    int i, n = db->csegs;
    databar_new_scan(db);
    for(i = 0; i < n; i++)
        db->segs[i].finder = -1;
}

/* decode DataBar symbols */
zbar_symbol_type_t _zbar_decode_databar(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "codabar.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2011 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* Codabar specific decode state */
typedef struct codabar_decoder_s {
    unsigned direction : 1;     /* scan direction: 0=fwd, 1=rev */
    unsigned element : 4;       /* element offset 0-7 */
    int character : 12;         /* character position in symbol */
    unsigned s7;                /* current character width */
    unsigned width;             /* last character width */
    unsigned char buf[6];       /* initial scan buffer */

    unsigned config;
    int configs[NUM_CFGS];      /* int valued configurations */
} codabar_decoder_t;

/* reset Codabar specific state */
static inline void codabar_reset (codabar_decoder_t *codabar)
{
    codabar->direction = 0;
    codabar->element = 0;
    codabar->character = -1;
    codabar->s7 = 0;
}

/* decode Codabar symbols */
zbar_symbol_type_t _zbar_decode_codabar(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "code39.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* Code 39 specific decode state */
typedef struct code39_decoder_s {
    unsigned direction : 1;     /* scan direction: 0=fwd, 1=rev */
    unsigned element : 4;       /* element offset 0-8 */
    int character : 12;         /* character position in symbol */
    unsigned s9;                /* current character width */
    unsigned width;             /* last character width */

    unsigned config;
    int configs[NUM_CFGS];      /* int valued configurations */
} code39_decoder_t;

/* reset Code 39 specific state */
static inline void code39_reset (code39_decoder_t *dcode39)
{
    dcode39->direction = 0;
    dcode39->element = 0;
    dcode39->character = -1;
    dcode39->s9 = 0;
}

/* decode Code 39 symbols */
zbar_symbol_type_t _zbar_decode_code39(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "code93.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* Code 93 specific decode state */
typedef struct code93_decoder_s {
    unsigned direction : 1;     /* scan direction: 0=fwd/space, 1=rev/bar */
    unsigned element : 3;       /* element offset 0-5 */
    int character : 12;         /* character position in symbol */
    unsigned width;             /* last character width */
    unsigned char buf;          /* first character */

    unsigned config;
    int configs[NUM_CFGS];      /* int valued configurations */
} code93_decoder_t;

/* reset Code 93 specific state */
static inline void code93_reset (code93_decoder_t *dcode93)
{
    dcode93->direction = 0;
    dcode93->element = 0;
    dcode93->character = -1;
}

/* decode Code 93 symbols */
zbar_symbol_type_t _zbar_decode_code93(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "code128.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* Code 128 specific decode state */
typedef struct code128_decoder_s {
    unsigned direction : 1;     /* scan direction: 0=fwd/space, 1=rev/bar */
    unsigned element : 3;       /* element offset 0-5 */
    int character : 12;         /* character position in symbol */
    unsigned char start;        /* start character */
    unsigned s6;                /* character width */
    unsigned width;             /* last character width */

    unsigned config;
    int configs[NUM_CFGS];      /* int valued configurations */
} code128_decoder_t;

/* reset Code 128 specific state */
static inline void code128_reset (code128_decoder_t *dcode128)
{
    dcode128->direction = 0;
    dcode128->element = 0;
    dcode128->character = -1;
    dcode128->s6 = 0;
}

/* decode Code 128 symbols */
zbar_symbol_type_t _zbar_decode_code128(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "pdf417.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* PDF417 specific decode state */
typedef struct pdf417_decoder_s {
    unsigned direction : 1;     /* scan direction: 0=fwd/space, 1=rev/bar */
    unsigned element : 3;       /* element offset 0-7 */
    int character : 12;         /* character position in symbol */
    unsigned s8;                /* character width */

    unsigned config;
    int configs[NUM_CFGS];      /* int valued configurations */
} pdf417_decoder_t;

/* reset PDF417 specific state */
static inline void pdf417_reset (pdf417_decoder_t *pdf417)
{
    pdf417->direction = 0;
    pdf417->element = 0;
    pdf417->character = -1;
    pdf417->s8 = 0;
}

/* decode PDF417 symbols */
zbar_symbol_type_t _zbar_decode_pdf417(zbar_decoder_t *dcode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "decoder.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* size of bar width history (implementation assumes power of two) */
#ifndef DECODE_WINDOW
# define DECODE_WINDOW  16
#endif

/* initial data buffer allocation */
#ifndef BUFFER_MIN
# define BUFFER_MIN   0x20
#endif

/* maximum data buffer allocation
 * (longer symbols are rejected)
 */
#ifndef BUFFER_MAX
# define BUFFER_MAX  0x100
#endif

/* buffer allocation increment */
#ifndef BUFFER_INCR
# define BUFFER_INCR  0x10
#endif

#define CFG(dcode, cfg) ((dcode).configs[(cfg) - ZBAR_CFG_MIN_LEN])
#define TEST_CFG(config, cfg) (((config) >> (cfg)) & 1)
#define MOD(mod) (1 << (mod))

/* symbology independent decoder state */
struct zbar_decoder_s {
    unsigned char idx;                  /* current width index */
    unsigned w[DECODE_WINDOW];          /* window of last N bar widths */
    zbar_symbol_type_t type;            /* type of last decoded data */
    zbar_symbol_type_t lock;            /* buffer lock */
    unsigned modifiers;                 /* symbology modifier */
    int direction;                      /* direction of last decoded data */
    unsigned s6;                        /* 6-element character width */

    /* everything above here is automatically reset */
    unsigned buf_alloc;                 /* dynamic buffer allocation */
    unsigned buflen;                    /* binary data length */
    unsigned char *buf;                 /* decoded characters */
    void *userdata;                     /* application data */
    zbar_decoder_handler_t *handler;    /* application callback */

    /* symbology specific state */
#ifdef ENABLE_EAN
    ean_decoder_t ean;                  /* EAN/UPC parallel decode attempts */
#endif
#ifdef ENABLE_I25
    i25_decoder_t i25;                  /* Interleaved 2 of 5 decode state */
#endif
#ifdef ENABLE_DATABAR
    databar_decoder_t databar;          /* DataBar decode state */
#endif
#ifdef ENABLE_CODABAR
    codabar_decoder_t codabar;          /* Codabar decode state */
#endif
#ifdef ENABLE_CODE39
    code39_decoder_t code39;            /* Code 39 decode state */
#endif
#ifdef ENABLE_CODE93
    code93_decoder_t code93;            /* Code 93 decode state */
#endif
#ifdef ENABLE_CODE128
    code128_decoder_t code128;          /* Code 128 decode state */
#endif
#ifdef ENABLE_PDF417
    pdf417_decoder_t pdf417;            /* PDF417 decode state */
#endif
#ifdef ENABLE_QRCODE
    qr_finder_t qrf;                    /* QR Code finder state */
#endif
};

/* return current element color */
static inline char get_color (const zbar_decoder_t *dcode)
{
    return(dcode->idx & 1);
}

/* retrieve i-th previous element width */
static inline unsigned get_width (const zbar_decoder_t *dcode,
                                  unsigned char offset)
{
    return(dcode->w[(dcode->idx - offset) & (DECODE_WINDOW - 1)]);
}

/* retrieve bar+space pair width starting at offset i */
static inline unsigned pair_width (const zbar_decoder_t *dcode,
                                   unsigned char offset)
{
    return(get_width(dcode, offset) + get_width(dcode, offset + 1));
}

/* calculate total character width "s"
 *   - start of character identified by context sensitive offset
 *     (<= DECODE_WINDOW - n)
 *   - size of character is n elements
 */
static inline unsigned calc_s (const zbar_decoder_t *dcode,
                               unsigned char offset,
                               unsigned char n)
{
    /* FIXME check that this gets unrolled for constant n */
    unsigned s = 0;
    while(n--)
        s += get_width(dcode, offset++);
    return(s);
}

/* fixed character width decode assist
 * bar+space width are compared as a fraction of the reference dimension "x"
 *   - +/- 1/2 x tolerance
 *   - measured total character width (s) compared to symbology baseline (n)
 *     (n = 7 for EAN/UPC, 11 for Code 128)
 *   - bar+space *pair width* "e" is used to factor out bad "exposures"
 *     ("blooming" or "swelling" of dark or light areas)
 *     => using like-edge measurements avoids these issues
 *   - n should be > 3
 */
static inline int decode_e (unsigned e,
                            unsigned s,
                            unsigned n)
{
    /* result is encoded number of units - 2
     * (for use as zero based index)
     * or -1 if invalid
     */
    unsigned char E = ((e * n * 2 + 1) / s - 3) / 2;
    return((E >= n - 3) ? -1 : E);
}

/* sort three like-colored elements and return ordering
 */
static inline unsigned decode_sort3 (zbar_decoder_t *dcode,
                                     int i0)
{
    unsigned w0 = get_width(dcode, i0);
    unsigned w2 = get_width(dcode, i0 + 2);
    unsigned w4 = get_width(dcode, i0 + 4);
    if(w0 < w2) {
        if(w2 < w4)
            return((i0 << 8) | ((i0 + 2) << 4) | (i0 + 4));
        if(w0 < w4)
            return((i0 << 8) | ((i0 + 4) << 4) | (i0 + 2));
        return(((i0 + 4) << 8) | (i0 << 4) | (i0 + 2));
    }
    if(w4 < w2)
        return(((i0 + 4) << 8) | ((i0 + 2) << 4) | i0);
    if(w0 < w4)
        return(((i0 + 2) << 8) | (i0 << 4) | (i0 + 4));
    return(((i0 + 2) << 8) | ((i0 + 4) << 4) | i0);
}

/* sort N like-colored elements and return ordering
 */
static inline unsigned decode_sortn (zbar_decoder_t *dcode,
                                     int n,
                                     int i0)
{
    unsigned mask = 0, sort = 0;
    int i;
    for(i = n - 1; i >= 0; i--) {
        unsigned wmin = UINT_MAX;
        int jmin = -1, j;
        for(j = n - 1; j >= 0; j--) {
            if((mask >> j) & 1)
                continue;
            unsigned w = get_width(dcode, i0 + j * 2);
            if(wmin >= w) {
                wmin = w;
                jmin = j;
            }
        }
        zassert(jmin >= 0, 0, "sortn(%d,%d) jmin=%d",
                n, i0, jmin);
        sort <<= 4;
        mask |= 1 << jmin;
        sort |= i0 + jmin * 2;
    }
    return(sort);
}

/* acquire shared state lock */
static inline char acquire_lock (zbar_decoder_t *dcode,
                                 zbar_symbol_type_t req)
{
    if(dcode->lock) {
        dbprintf(2, " [locked %d]\n", dcode->lock);
        return(1);
    }
    dcode->lock = req;
    return(0);
}

/* check and release shared state lock */
static inline char release_lock (zbar_decoder_t *dcode,
                                 zbar_symbol_type_t req)
{
    zassert(dcode->lock == req, 1, "lock=%d req=%d\n",
            dcode->lock, req);
    dcode->lock = 0;
    return(0);
}

/* ensure output buffer has sufficient allocation for request */
static inline char size_buf (zbar_decoder_t *dcode,
                             unsigned len)
{
    unsigned char *buf;
    if(len <= BUFFER_MIN)
        return(0);
    if(len < dcode->buf_alloc)
        /* FIXME size reduction heuristic? */
        return(0);
    if(len > BUFFER_MAX)
        return(1);
    if(len < dcode->buf_alloc + BUFFER_INCR) {
        len = dcode->buf_alloc + BUFFER_INCR;
        if(len > BUFFER_MAX)
            len = BUFFER_MAX;
    }
    buf = realloc(dcode->buf, len);
    if(!buf)
        return(1);
    dcode->buf = buf;
    dcode->buf_alloc = len;
    return(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "ean.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* partial decode symbol location */
typedef enum symbol_partial_e {
    EAN_LEFT   = 0x0000,
    EAN_RIGHT  = 0x1000,
} symbol_partial_t;

/* convert compact encoded D2E1E2 to character (bit4 is parity) */
static const unsigned char digits[] = {  /* E1   E2 */
    0x06, 0x10, 0x04, 0x13,              /*  2  2-5 */
    0x19, 0x08, 0x11, 0x05,              /*  3  2-5 (d2 <= thr) */
    0x09, 0x12, 0x07, 0x15,              /*  4  2-5 (d2 <= thr) */
    0x16, 0x00, 0x14, 0x03,              /*  5  2-5 */
    0x18, 0x01, 0x02, 0x17,              /* E1E2=43,44,33,34 (d2 > thr) */
};

static const unsigned char parity_decode[] = {
    0xf0, /* [xx] BBBBBB = RIGHT half EAN-13 */

    /* UPC-E check digit encoding */
    0xff,
    0xff,
    0x0f, /* [07] BBBAAA = 0 */
    0xff,
    0x1f, /* [0b] BBABAA = 1 */
    0x2f, /* [0d] BBAABA = 2 */
    0xf3, /* [0e] BBAAAB = 3 */
    0xff,
    0x4f, /* [13] BABBAA = 4 */
    0x7f, /* [15] BABABA = 7 */
    0xf8, /* [16] BABAAB = 8 */
    0x5f, /* [19] BAABBA = 5 */
    0xf9, /* [1a] BAABAB = 9 */
    0xf6, /* [1c] BAAABB = 6 */
    0xff,

    /* LEFT half EAN-13 leading digit */
    0xff,
    0x6f, /* [23] ABBBAA = 6 */
    0x9f, /* [25] ABBABA = 9 */
    0xf5, /* [26] ABBAAB = 5 */
    0x8f, /* [29] ABABBA = 8 */
    0xf7, /* [2a] ABABAB = 7 */
    0xf4, /* [2c] ABAABB = 4 */
    0xff,
    0x3f, /* [31] AABBBA = 3 */
    0xf2, /* [32] AABBAB = 2 */
    0xf1, /* [34] AABABB = 1 */
    0xff,
    0xff,
    0xff,
    0xff,
    0x0f, /* [3f] AAAAAA = 0 */
};

static inline int check_width (unsigned w0,
                               unsigned w1)
{
    unsigned dw0 = w0;
    w0 *= 8;
    w1 *= 8;
    return(w0 - dw0 <= w1 && w1 <= w0 + dw0);
}

/* evaluate previous N (>= 2) widths as auxiliary pattern,
 * using preceding 4 as character width
 */
static inline signed char aux_end (zbar_decoder_t *dcode,
                                   unsigned char fwd)
{
    signed char code, i;

    /* reference width from previous character */
    unsigned s = calc_s(dcode, 4 + fwd, 4);

    /* check quiet zone */
    unsigned qz = get_width(dcode, 0);
    if(!fwd && qz && qz <= s * 3 / 4) {
        dbprintf(2, " [invalid quiet]");
        return(-1);
    }

    dbprintf(2, " (");
    code = 0;
    for(i = 1 - fwd; i < 3 + fwd; i++) {
        unsigned e = get_width(dcode, i) + get_width(dcode, i + 1);
        dbprintf(2, " %d", e);
        code = (code << 2) | decode_e(e, s, 7);
        if(code < 0) {
            dbprintf(2, " [invalid end guard]");
            return(-1);
        }
    }
    dbprintf(2, ") s=%d aux=%x", s, code);
    return(code);
}

/* determine possible auxiliary pattern
 * using current 4 as possible character
 */
static inline signed char aux_start (zbar_decoder_t *dcode)
{
    /* FIXME NB add-on has no guard in reverse */
    unsigned e1, e2 = get_width(dcode, 5) + get_width(dcode, 6);
    unsigned char E1;
    if(dcode->ean.s4 < 6)
        return(-1);

    if(decode_e(e2, dcode->ean.s4, 7)) {
        dbprintf(2, " [invalid any]");
        return(-1);
    }

    e1 = get_width(dcode, 4) + get_width(dcode, 5);
    E1 = decode_e(e1, dcode->ean.s4, 7);

    if(get_color(dcode) == ZBAR_BAR) {
        /* check for quiet-zone */
        unsigned qz = get_width(dcode, 7);
        if(!qz || qz > dcode->ean.s4 * 3 / 4) {
            if(!E1) {
                dbprintf(2, " [valid normal]");
                return(0); /* normal symbol start */
            }
            else if(E1 == 1) {
                dbprintf(2, " [valid add-on]");
                return(STATE_ADDON); /* add-on symbol start */
            }
        }
        dbprintf(2, " [invalid start]");
        return(-1);
    }

    if(!E1) {
        /* attempting decode from SPACE => validate center guard */
        unsigned e3 = get_width(dcode, 6) + get_width(dcode, 7);
        unsigned e4 = get_width(dcode, 7) + get_width(dcode, 8);
        if(!decode_e(e3, dcode->ean.s4, 7) &&
           !decode_e(e4, dcode->ean.s4, 7)) {
            dbprintf(2, " [valid center]");
            return(0); /* start after center guard */
        }
    }
    dbprintf(2, " [invalid center]");
    return(-1);
}

/* check addon delimiter using current 4 as character
 */
static inline signed char aux_mid (zbar_decoder_t *dcode)
{
    unsigned e = get_width(dcode, 4) + get_width(dcode, 5);
    return(decode_e(e, dcode->ean.s4, 7));
}

/* attempt to decode previous 4 widths (2 bars and 2 spaces) as a character */
static inline signed char decode4 (zbar_decoder_t *dcode)
{
    signed char code;

    /* calculate similar edge measurements */
    unsigned e1 = ((get_color(dcode) == ZBAR_BAR)
                   ? get_width(dcode, 0) + get_width(dcode, 1)
                   : get_width(dcode, 2) + get_width(dcode, 3));
    unsigned e2 = get_width(dcode, 1) + get_width(dcode, 2);
    dbprintf(2, "\n        e1=%d e2=%d", e1, e2);

    if(dcode->ean.s4 < 6)
        return(-1);

    /* create compacted encoding for direct lookup */
    code = ((decode_e(e1, dcode->ean.s4, 7) << 2) |
             decode_e(e2, dcode->ean.s4, 7));
    if(code < 0)
        return(-1);
    dbprintf(2, " code=%x", code);

    /* 4 combinations require additional determinant (D2)
       E1E2 == 34 (0110)
       E1E2 == 43 (1001)
       E1E2 == 33 (0101)
       E1E2 == 44 (1010)
     */
    if((1 << code) & 0x0660) {
        unsigned char mid, alt;
        /* use sum of bar widths */
        unsigned d2 = ((get_color(dcode) == ZBAR_BAR)
                       ? get_width(dcode, 0) + get_width(dcode, 2)
                       : get_width(dcode, 1) + get_width(dcode, 3));
        d2 *= 7;
        mid = (((1 << code) & 0x0420)
                             ? 3     /* E1E2 in 33,44 */
                             : 4);   /* E1E2 in 34,43 */
        alt = d2 > (mid * dcode->ean.s4);
        if(alt)
            code = ((code >> 1) & 3) | 0x10; /* compress code space */
        dbprintf(2, " (d2=%d(%d) alt=%d)", d2, mid * dcode->ean.s4, alt);
    }
    dbprintf(2, " char=%02x", digits[(unsigned char)code]);
    zassert(code < 0x14, -1, "code=%02x e1=%x e2=%x s4=%x color=%x\n",
            code, e1, e2, dcode->ean.s4, get_color(dcode));
    return(code);
}

static inline char ean_part_end2 (ean_decoder_t *ean,
                                  ean_pass_t *pass)
{
    if(!TEST_CFG(ean->ean2_config, ZBAR_CFG_ENABLE))
        return(ZBAR_NONE);

    /* extract parity bits */
    unsigned char par = ((pass->raw[1] & 0x10) >> 3 |
                         (pass->raw[2] & 0x10) >> 4);
    /* calculate "checksum" */
    unsigned char chk = ~((pass->raw[1] & 0xf) * 10 +
                          (pass->raw[2] & 0xf)) & 0x3;
    dbprintf(2, " par=%x chk=%x", par, chk);
    if(par != chk)
        return(ZBAR_NONE);

    dbprintf(2, "\n");
    dbprintf(1, "decode2=%x%x\n",
             pass->raw[1] & 0xf, pass->raw[2] & 0xf);
    return(ZBAR_EAN2);
}

static inline zbar_symbol_type_t ean_part_end4 (ean_pass_t *pass,
                                                unsigned char fwd)
{
    /* extract parity bits */
    unsigned char par = ((pass->raw[1] & 0x10) >> 1 |
                         (pass->raw[2] & 0x10) >> 2 |
                         (pass->raw[3] & 0x10) >> 3 |
                         (pass->raw[4] & 0x10) >> 4);

    dbprintf(2, " par=%x", par);
    if(par && par != 0xf)
        /* invalid parity combination */
        return(ZBAR_NONE);

    if((!par) == fwd) {
        /* reverse sampled digits */
        unsigned char tmp = pass->raw[1];
        pass->state |= STATE_REV;
        pass->raw[1] = pass->raw[4];
        pass->raw[4] = tmp;
        tmp = pass->raw[2];
        pass->raw[2] = pass->raw[3];
        pass->raw[3] = tmp;
    }

    dbprintf(2, "\n");
    dbprintf(1, "decode4=%x%x%x%x\n",
             pass->raw[1] & 0xf, pass->raw[2] & 0xf,
             pass->raw[3] & 0xf, pass->raw[4] & 0xf);
    if(!par)
        return(ZBAR_EAN8 | EAN_RIGHT);
    return(ZBAR_EAN8 | EAN_LEFT);
}

static inline char ean_part_end5 (ean_decoder_t *ean,
                                  ean_pass_t *pass)
{
    if(!TEST_CFG(ean->ean5_config, ZBAR_CFG_ENABLE))
        return(ZBAR_NONE);

    /* extract parity bits */
    unsigned char par = ((pass->raw[1] & 0x10) |
                         (pass->raw[2] & 0x10) >> 1 |
                         (pass->raw[3] & 0x10) >> 2 |
                         (pass->raw[4] & 0x10) >> 3 |
                         (pass->raw[5] & 0x10) >> 4);
    /* calculate checksum */
    unsigned char chk = (((pass->raw[1] & 0x0f) +
                          (pass->raw[2] & 0x0f) * 3 +
                          (pass->raw[3] & 0x0f) +
                          (pass->raw[4] & 0x0f) * 3 +
                          (pass->raw[5] & 0x0f)) * 3) % 10;

    unsigned char parchk = parity_decode[par >> 1];
    if(par & 1)
        parchk >>= 4;
    parchk &= 0xf;
    dbprintf(2, " par=%x(%d) chk=%d", par, parchk, chk);
    if(parchk != chk)
        return(ZBAR_NONE);

    dbprintf(2, "\n");
    dbprintf(1, "decode5=%x%x%x%x%x\n",
             pass->raw[1] & 0xf, pass->raw[2] & 0xf,
             pass->raw[3] & 0xf, pass->raw[4] & 0xf,
             pass->raw[5] & 0xf);

    return(ZBAR_EAN5);
}

static inline zbar_symbol_type_t ean_part_end7 (ean_decoder_t *ean,
                                                ean_pass_t *pass,
                                                unsigned char fwd)
{
    /* calculate parity index */
    unsigned char par = ((fwd)
                         ? ((pass->raw[1] & 0x10) << 1 |
                            (pass->raw[2] & 0x10) |
                            (pass->raw[3] & 0x10) >> 1 |
                            (pass->raw[4] & 0x10) >> 2 |
                            (pass->raw[5] & 0x10) >> 3 |
                            (pass->raw[6] & 0x10) >> 4)
                         : ((pass->raw[1] & 0x10) >> 4 |
                            (pass->raw[2] & 0x10) >> 3 |
                            (pass->raw[3] & 0x10) >> 2 |
                            (pass->raw[4] & 0x10) >> 1 |
                            (pass->raw[5] & 0x10) |
                            (pass->raw[6] & 0x10) << 1));

    /* lookup parity combination */
    pass->raw[0] = parity_decode[par >> 1];
    if(par & 1)
        pass->raw[0] >>= 4;
    pass->raw[0] &= 0xf;
    dbprintf(2, " par=%02x(%x)", par, pass->raw[0]);

    if(pass->raw[0] == 0xf)
        /* invalid parity combination */
        return(ZBAR_NONE);

    if((!par) == fwd) {
        unsigned char i;
        pass->state |= STATE_REV;
        /* reverse sampled digits */
        for(i = 1; i < 4; i++) {
            unsigned char tmp = pass->raw[i];
            pass->raw[i] = pass->raw[7 - i];
            pass->raw[7 - i] = tmp;
        }
    }

    dbprintf(2, "\n");
    dbprintf(1, "decode=%x%x%x%x%x%x%x(%02x)\n",
             pass->raw[0] & 0xf, pass->raw[1] & 0xf,
             pass->raw[2] & 0xf, pass->raw[3] & 0xf,
             pass->raw[4] & 0xf, pass->raw[5] & 0xf,
             pass->raw[6] & 0xf, par);

    if(TEST_CFG(ean->ean13_config, ZBAR_CFG_ENABLE)) {
        if(!par)
            return(ZBAR_EAN13 | EAN_RIGHT);
        if(par & 0x20)
            return(ZBAR_EAN13 | EAN_LEFT);
    }
    if(par && !(par & 0x20))
        return(ZBAR_UPCE);

    return(ZBAR_NONE);
}

/* update state for one of 4 parallel passes */
static inline zbar_symbol_type_t decode_pass (zbar_decoder_t *dcode,
                                              ean_pass_t *pass)
{
    unsigned char idx, fwd;
    pass->state++;
    idx = pass->state & STATE_IDX;
    fwd = pass->state & 1;

    if(get_color(dcode) == ZBAR_SPACE) {
        if(pass->state & STATE_ADDON) {
            dbprintf(2, " i=%d", idx);
            if(idx == 0x09 || idx == 0x21) {
                unsigned qz = get_width(dcode, 0);
                unsigned s = calc_s(dcode, 1, 4);
                zbar_symbol_type_t part = !qz || (qz >= s * 3 / 4);
                if(part && idx == 0x09)
                    part = ean_part_end2(&dcode->ean, pass);
                else if(part)
                    part = ean_part_end5(&dcode->ean, pass);

                if(part || idx == 0x21) {
                    dcode->ean.direction = 0;
                    pass->state = -1;
                    return(part);
                }
            }
            if((idx & 7) == 1) {
                dbprintf(2, " +");
                pass->state += 2;
                idx += 2;
            }
        }
        else if((idx == 0x10 || idx == 0x11) &&
                TEST_CFG(dcode->ean.ean8_config, ZBAR_CFG_ENABLE) &&
                !aux_end(dcode, fwd)) {
            dbprintf(2, " fwd=%x", fwd);
            zbar_symbol_type_t part = ean_part_end4(pass, fwd);
            if(part)
                dcode->ean.direction = (pass->state & STATE_REV) != 0;
            pass->state = -1;
            return(part);
        }
    else if((idx == 0x18 || idx == 0x19)) {
      zbar_symbol_type_t part = ZBAR_NONE;
      dbprintf(2, " fwd=%x", fwd);
      if(!aux_end(dcode, fwd) && pass->raw[5] != 0xff)
        part = ean_part_end7(&dcode->ean, pass, fwd);
      if(part)
        dcode->ean.direction = (pass->state & STATE_REV) != 0;
      pass->state = -1;
      return(part);
        }
    }

    if(pass->state & STATE_ADDON)
        idx >>= 1;

    if(!(idx & 0x03) && idx <= 0x14) {
        signed char code = -1;
        unsigned w = pass->width;
        if(!dcode->ean.s4)
            return(0);
        /* validate guard bars before decoding first char of symbol */
        if(!pass->state) {
            pass->state = aux_start(dcode);
            pass->width = dcode->ean.s4;
            if(pass->state < 0)
                return(0);
            idx = pass->state & STATE_IDX;
        }
        else {
            w = check_width(w, dcode->ean.s4);
            if(w)
                pass->width = (pass->width + dcode->ean.s4 * 3) / 4;
        }

        if(w)
            code = decode4(dcode);
        else
            dbprintf(2, " [bad width]");

        if((code < 0 && idx != 0x10) ||
           (idx > 0 && (pass->state & STATE_ADDON) && aux_mid(dcode)))
            pass->state = -1;
        else if(code < 0)
            pass->raw[5] = 0xff;
        else {
            dbprintf(2, "\n        raw[%x]=%02x =>", idx >> 2,
                     digits[(unsigned char)code]);
            pass->raw[(idx >> 2) + 1] = digits[(unsigned char)code];
            dbprintf(2, " raw=%d%d%d%d%d%d%d",
                     pass->raw[0] & 0xf, pass->raw[1] & 0xf,
                     pass->raw[2] & 0xf, pass->raw[3] & 0xf,
                     pass->raw[4] & 0xf, pass->raw[5] & 0xf,
                     pass->raw[6] & 0xf);
        }
    }
    return(0);
}

static inline signed char ean_verify_checksum (ean_decoder_t *ean,
                                               int n)
{
    unsigned char chk = 0;
    unsigned char i, d;
    for(i = 0; i < n; i++) {
        unsigned char d = ean->buf[i];
        zassert(d < 10, -1, "i=%x d=%x chk=%x %s\n", i, d, chk,
                _zbar_decoder_buf_dump((void*)ean->buf, 18));
        chk += d;
        if((i ^ n) & 1) {
            chk += d << 1;
            if(chk >= 20)
                chk -= 20;
        }
        if(chk >= 10)
            chk -= 10;
    }
    zassert(chk < 10, -1, "chk=%x n=%x %s", chk, n,
            _zbar_decoder_buf_dump((void*)ean->buf, 18));
    if(chk)
        chk = 10 - chk;
    d = ean->buf[n];
    zassert(d < 10, -1, "n=%x d=%x chk=%x %s\n", n, d, chk,
            _zbar_decoder_buf_dump((void*)ean->buf, 18));
    if(chk != d) {
        dbprintf(1, "\nchecksum mismatch %d != %d (%s)\n",
                 chk, d, dsprintbuf(ean));
        return(-1);
    }
    return(0);
}

static inline unsigned char isbn10_calc_checksum (ean_decoder_t *ean)
{
    unsigned int chk = 0;
    unsigned char w;
    for(w = 10; w > 1; w--) {
        unsigned char d = ean->buf[13 - w];
        zassert(d < 10, '?', "w=%x d=%x chk=%x %s\n", w, d, chk,
                _zbar_decoder_buf_dump((void*)ean->buf, 18));
        chk += d * w;
    }
    chk = chk % 11;
    if(!chk)
        return('0');
    chk = 11 - chk;
    if(chk < 10)
        return(chk + '0');
    return('X');
}

static inline void ean_expand_upce (ean_decoder_t *ean,
                                    ean_pass_t *pass)
{
    int i = 0;
    unsigned char decode;
    /* parity encoded digit is checksum */
    ean->buf[12] = pass->raw[i++];

    decode = pass->raw[6] & 0xf;
    ean->buf[0] = 0;
    ean->buf[1] = 0;
    ean->buf[2] = pass->raw[i++] & 0xf;
    ean->buf[3] = pass->raw[i++] & 0xf;
    ean->buf[4] = (decode < 3) ? decode : pass->raw[i++] & 0xf;
    ean->buf[5] = (decode < 4) ? 0 : pass->raw[i++] & 0xf;
    ean->buf[6] = (decode < 5) ? 0 : pass->raw[i++] & 0xf;
    ean->buf[7] = 0;
    ean->buf[8] = 0;
    ean->buf[9] = (decode < 3) ? pass->raw[i++] & 0xf : 0;
    ean->buf[10] = (decode < 4) ? pass->raw[i++] & 0xf : 0;
    ean->buf[11] = (decode < 5) ? pass->raw[i] & 0xf : decode;
}

static inline zbar_symbol_type_t integrate_partial (ean_decoder_t *ean,
                                                    ean_pass_t *pass,
                                                    zbar_symbol_type_t part)
{
    /* copy raw data into holding buffer */
    /* if same partial is not consistent, reset others */
    dbprintf(2, " integrate part=%x (%s)", part, dsprintbuf(ean));
    signed char i, j;

    if((ean->left && ((part & ZBAR_SYMBOL) != ean->left)) ||
       (ean->right && ((part & ZBAR_SYMBOL) != ean->right))) {
        /* partial mismatch - reset collected parts */
        dbprintf(2, " rst(type %x %x)", ean->left, ean->right);
        ean->left = ean->right = ZBAR_NONE;
    }

    if((ean->left || ean->right) &&
       !check_width(ean->width, pass->width)) {
      dbprintf(2, " rst(width %d)", pass->width);
      ean->left = ean->right = ZBAR_NONE;
    }


    if(part & EAN_RIGHT) {
        part &= ZBAR_SYMBOL;
        j = part - 1;
        for(i = part >> 1; i; i--, j--) {
            unsigned char digit = pass->raw[i] & 0xf;
            if(ean->right && ean->buf[j] != digit) {
                /* partial mismatch - reset collected parts */
                dbprintf(2, " rst(right)");
                ean->left = ean->right = ZBAR_NONE;
            }
            ean->buf[j] = digit;
        }
        ean->right = part;
        part &= ean->left;  /* FIXME!? */
    }
    else if(part == ZBAR_EAN13 || part == ZBAR_EAN8) /* EAN_LEFT */ {
        j = (part - 1) >> 1;
        for(i = part >> 1; j >= 0; i--, j--) {
            unsigned char digit = pass->raw[i] & 0xf;
            if(ean->left && ean->buf[j] != digit) {
                /* partial mismatch - reset collected parts */
                dbprintf(2, " rst(left)");
                ean->left = ean->right = ZBAR_NONE;
            }
            ean->buf[j] = digit;
        }
        ean->left = part;
        part &= ean->right;  /* FIXME!? */
    }
    else if(part != ZBAR_UPCE) /* add-ons */ {
        for(i = part; i > 0; i--)
            ean->buf[i - 1] = pass->raw[i] & 0xf;
        ean->left = part;
    }
    else
        ean_expand_upce(ean, pass);

    ean->width = pass->width;

    if(!part)
        part = ZBAR_PARTIAL;

    if(((part == ZBAR_EAN13 ||
         part == ZBAR_UPCE) && ean_verify_checksum(ean, 12)) ||
       (part == ZBAR_EAN8 && ean_verify_checksum(ean, 7))) {
        /* invalid checksum */
        if(ean->right)
      ean->left = ZBAR_NONE;
        else
      ean->right = ZBAR_NONE;
        part = ZBAR_NONE;
    }

    if(part == ZBAR_EAN13) {
        /* special case EAN-13 subsets */
        if(!ean->buf[0] && TEST_CFG(ean->upca_config, ZBAR_CFG_ENABLE))
            part = ZBAR_UPCA;
        else if(ean->buf[0] == 9 && ean->buf[1] == 7) {
            /* ISBN-10 has priority over ISBN-13(?) */
            if(ean->buf[2] == 8 &&
               TEST_CFG(ean->isbn10_config, ZBAR_CFG_ENABLE))
                part = ZBAR_ISBN10;
            else if((ean->buf[2] == 8 || ean->buf[2] == 9) &&
               TEST_CFG(ean->isbn13_config, ZBAR_CFG_ENABLE))
                part = ZBAR_ISBN13;
        }
    }
    else if(part == ZBAR_UPCE) {
        if(TEST_CFG(ean->upce_config, ZBAR_CFG_ENABLE)) {
            /* UPC-E was decompressed for checksum verification,
             * but user requested compressed result
             */
            ean->buf[0] = ean->buf[1] = 0;
            for(i = 2; i < 8; i++)
                ean->buf[i] = pass->raw[i - 1] & 0xf;
            ean->buf[i] = pass->raw[0] & 0xf;
        }
        else if(TEST_CFG(ean->upca_config, ZBAR_CFG_ENABLE))
            /* UPC-E reported as UPC-A has priority over EAN-13 */
            part = ZBAR_UPCA;
        else if(TEST_CFG(ean->ean13_config, ZBAR_CFG_ENABLE))
            part = ZBAR_EAN13;
        else
            part = ZBAR_NONE;
    }

    dbprintf(2, " dir=%d %x/%x=%x",
             ean->direction, ean->left, ean->right, part);
    return(part);
}

/* copy result to output buffer */
static inline void ean_postprocess (zbar_decoder_t *dcode,
                                    zbar_symbol_type_t sym)
{
    ean_decoder_t *ean = &dcode->ean;
    zbar_symbol_type_t base = sym;
    int i = 0, j = 0;
    if(base > ZBAR_PARTIAL) {
        if(base == ZBAR_UPCA)
            i = 1;
        else if(base == ZBAR_UPCE) {
            i = 1;
            base--;
        }
        else if(base == ZBAR_ISBN13)
            base = ZBAR_EAN13;
        else if(base == ZBAR_ISBN10)
            i = 3;

        if(base == ZBAR_ISBN10 ||
           (base > ZBAR_EAN5 &&
            !TEST_CFG(ean_get_config(ean, sym), ZBAR_CFG_EMIT_CHECK)))
            base--;

        for(; j < base && ean->buf[i] >= 0; i++, j++)
            dcode->buf[j] = ean->buf[i] + '0';

        if(sym == ZBAR_ISBN10 && j == 9 &&
           TEST_CFG(ean->isbn10_config, ZBAR_CFG_EMIT_CHECK))
            /* recalculate ISBN-10 check digit */
            dcode->buf[j++] = isbn10_calc_checksum(ean);
    }
    dcode->buflen = j;
    dcode->buf[j] = '\0';
    dcode->direction = 1 - 2 * ean->direction;
    dcode->modifiers = 0;
    dbprintf(2, " base=%d j=%d (%s)", base, j, dcode->buf);
}

zbar_symbol_type_t _zbar_decode_ean (zbar_decoder_t *dcode)
{
    /* process upto 4 separate passes */
    zbar_symbol_type_t sym = ZBAR_NONE;
    unsigned char pass_idx = dcode->idx & 3;
    unsigned char i;

    /* update latest character width */
    dcode->ean.s4 -= get_width(dcode, 4);
    dcode->ean.s4 += get_width(dcode, 0);

    for(i = 0; i < 4; i++) {
        ean_pass_t *pass = &dcode->ean.pass[i];
        if(pass->state >= 0 ||
           i == pass_idx)
        {
            zbar_symbol_type_t part;
            dbprintf(2, "      ean[%x/%x]: idx=%x st=%d s=%d",
                     i, pass_idx, dcode->idx, pass->state, dcode->ean.s4);
            part = decode_pass(dcode, pass);
            if(part) {
                /* update accumulated data from new partial decode */
                sym = integrate_partial(&dcode->ean, pass, part);
                if(sym) {
                    /* this pass valid => _reset_ all passes */
                    dbprintf(2, " sym=%x", sym);
                    dcode->ean.pass[0].state = dcode->ean.pass[1].state = -1;
                    dcode->ean.pass[2].state = dcode->ean.pass[3].state = -1;
                    if(sym > ZBAR_PARTIAL) {
                        if(!acquire_lock(dcode, sym))
                            ean_postprocess(dcode, sym);
                        else {
                            dbprintf(1, " [locked %d]", dcode->lock);
                            sym = ZBAR_PARTIAL;
                        }
                    }
                }
            }
            dbprintf(2, "\n");
        }
    }
    return(sym);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "i25.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

static inline unsigned char i25_decode1 (unsigned char enc,
                                         unsigned e,
                                         unsigned s)
{
    unsigned char E = decode_e(e, s, 45);
    if(E > 7)
        return(0xff);
    enc <<= 1;
    if(E > 2)
        enc |= 1;
    return(enc);
}

static inline unsigned char i25_decode10 (zbar_decoder_t *dcode,
                                          unsigned char offset)
{
    i25_decoder_t *dcode25 = &dcode->i25;
    dbprintf(2, " s=%d", dcode25->s10);
    if(dcode25->s10 < 10)
        return(0xff);

    /* threshold bar width ratios */
    unsigned char enc = 0, par = 0;
    signed char i;
    for(i = 8; i >= 0; i -= 2) {
        unsigned char j = offset + ((dcode25->direction) ? i : 8 - i);
        enc = i25_decode1(enc, get_width(dcode, j), dcode25->s10);
        if(enc == 0xff)
            return(0xff);
        if(enc & 1)
            par++;
    }

    dbprintf(2, " enc=%02x par=%x", enc, par);

    /* parity check */
    if(par != 2) {
        dbprintf(2, " [bad parity]");
        return(0xff);
    }

    /* decode binary weights */
    enc &= 0xf;
    if(enc & 8) {
        if(enc == 12)
            enc = 0;
        else if(--enc > 9) {
            dbprintf(2, " [invalid encoding]");
            return(0xff);
        }
    }

    dbprintf(2, " => %x", enc);
    return(enc);
}

static inline signed char i25_decode_start (zbar_decoder_t *dcode)
{
    i25_decoder_t *dcode25 = &dcode->i25;
    if(dcode25->s10 < 10)
        return(ZBAR_NONE);

    unsigned char enc = 0;
    unsigned char i = 10;
    enc = i25_decode1(enc, get_width(dcode, i++), dcode25->s10);
    enc = i25_decode1(enc, get_width(dcode, i++), dcode25->s10);
    enc = i25_decode1(enc, get_width(dcode, i++), dcode25->s10);

    if((get_color(dcode) == ZBAR_BAR)
       ? enc != 4
       : (enc = i25_decode1(enc, get_width(dcode, i++), dcode25->s10))) {
        dbprintf(4, "      i25: s=%d enc=%x [invalid]\n", dcode25->s10, enc);
        return(ZBAR_NONE);
    }

    /* check leading quiet zone - spec is 10n(?)
     * we require 5.25n for w=2n to 6.75n for w=3n
     * (FIXME should really factor in w:n ratio)
     */
    unsigned quiet = get_width(dcode, i);
    if(quiet && quiet < dcode25->s10 * 3 / 8) {
        dbprintf(3, "      i25: s=%d enc=%x q=%d [invalid qz]\n",
                 dcode25->s10, enc, quiet);
        return(ZBAR_NONE);
    }

    dcode25->direction = get_color(dcode);
    dcode25->element = 1;
    dcode25->character = 0;
    return(ZBAR_PARTIAL);
}

static inline int i25_acquire_lock (zbar_decoder_t *dcode)
{
    int i;
    /* lock shared resources */
    if(acquire_lock(dcode, ZBAR_I25)) {
        dcode->i25.character = -1;
        return(1);
    }

    /* copy holding buffer */
    for(i = 4; --i >= 0; )
        dcode->buf[i] = dcode->i25.buf[i];
    return(0);
}

static inline signed char i25_decode_end (zbar_decoder_t *dcode)
{
    i25_decoder_t *dcode25 = &dcode->i25;

    /* check trailing quiet zone */
    unsigned quiet = get_width(dcode, 0);
    if((quiet && quiet < dcode25->width * 3 / 8) ||
       decode_e(get_width(dcode, 1), dcode25->width, 45) > 2 ||
       decode_e(get_width(dcode, 2), dcode25->width, 45) > 2) {
        dbprintf(3, "      i25: s=%d q=%d [invalid qz]\n",
                 dcode25->width, quiet);
        return(ZBAR_NONE);
    }

    /* check exit condition */
    unsigned char E = decode_e(get_width(dcode, 3), dcode25->width, 45);
    if((!dcode25->direction)
       ? E - 3 > 4
       : (E > 2 ||
          decode_e(get_width(dcode, 4), dcode25->width, 45) > 2))
        return(ZBAR_NONE);

    if(dcode25->character <= 4 &&
       i25_acquire_lock(dcode))
        return(ZBAR_PARTIAL);

    dcode->direction = 1 - 2 * dcode25->direction;
    if(dcode25->direction) {
        /* reverse buffer */
        dbprintf(2, " (rev)");
        int i;
        for(i = 0; i < dcode25->character / 2; i++) {
            unsigned j = dcode25->character - 1 - i;
            char c = dcode->buf[i];
            dcode->buf[i] = dcode->buf[j];
            dcode->buf[j] = c;
        }
    }

    if(dcode25->character < CFG(*dcode25, ZBAR_CFG_MIN_LEN) ||
       (CFG(*dcode25, ZBAR_CFG_MAX_LEN) > 0 &&
        dcode25->character > CFG(*dcode25, ZBAR_CFG_MAX_LEN))) {
        dbprintf(2, " [invalid len]\n");
        release_lock(dcode, ZBAR_I25);
        dcode25->character = -1;
        return(ZBAR_NONE);
    }

    zassert(dcode25->character < dcode->buf_alloc, ZBAR_NONE, "i=%02x %s\n",
            dcode25->character,
            _zbar_decoder_buf_dump(dcode->buf, dcode25->character));
    dcode->buflen = dcode25->character;
    dcode->buf[dcode25->character] = '\0';
    dcode->modifiers = 0;
    dbprintf(2, " [valid end]\n");
    dcode25->character = -1;
    return(ZBAR_I25);
}

zbar_symbol_type_t _zbar_decode_i25 (zbar_decoder_t *dcode)
{
    i25_decoder_t *dcode25 = &dcode->i25;

    /* update latest character width */
    dcode25->s10 -= get_width(dcode, 10);
    dcode25->s10 += get_width(dcode, 0);

    if(dcode25->character < 0 &&
       !i25_decode_start(dcode))
        return(ZBAR_NONE);

    if(--dcode25->element == 6 - dcode25->direction)
        return(i25_decode_end(dcode));
    else if(dcode25->element)
        return(ZBAR_NONE);

    /* FIXME check current character width against previous */
    dcode25->width = dcode25->s10;

    dbprintf(2, "      i25[%c%02d+%x]",
             (dcode25->direction) ? '<' : '>',
             dcode25->character, dcode25->element);

    if(dcode25->character == 4 && i25_acquire_lock(dcode))
        return(ZBAR_PARTIAL);

    unsigned char c = i25_decode10(dcode, 1);
    dbprintf(2, " c=%x", c);
    if(c > 9) {
        dbprintf(2, " [aborted]\n");
        goto reset;
    }

    if(size_buf(dcode, dcode25->character + 3)) {
        dbprintf(2, " [overflow]\n");
        goto reset;
    }

    unsigned char *buf;
    if(dcode25->character >= 4)
        buf = dcode->buf;
    else
        buf = dcode25->buf;
    buf[dcode25->character++] = c + '0';

    c = i25_decode10(dcode, 0);
    dbprintf(2, " c=%x", c);
    if(c > 9) {
        dbprintf(2, " [aborted]\n");
        goto reset;
    }
    else
        dbprintf(2, "\n");

    buf[dcode25->character++] = c + '0';
    dcode25->element = 10;
    return((dcode25->character == 2) ? ZBAR_PARTIAL : ZBAR_NONE);

reset:
    if(dcode25->character >= 4)
        release_lock(dcode, ZBAR_I25);
    dcode25->character = -1;
    return(ZBAR_NONE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "codabar.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2011 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define NIBUF 6 /* initial scan buffer size */

static const signed char codabar_lo[12] = {
    0x0, 0x1, 0x4, 0x5, 0x2, 0xa, 0xb, 0x9,
    0x6, 0x7, 0x8, 0x3
};

static const unsigned char codabar_hi[8] = {
    0x1, 0x4, 0x7, 0x6, 0x2, 0x3, 0x0, 0x5
};

static const unsigned char codabar_characters[20] =
    "0123456789-$:/.+ABCD";

static inline int
codabar_check_width (unsigned ref,
                     unsigned w)
{
    unsigned dref = ref;
    ref *= 4;
    w *= 4;
    return(ref - dref <= w && w <= ref + dref);
}

static inline signed char
codabar_decode7 (zbar_decoder_t *dcode)
{
    codabar_decoder_t *codabar = &dcode->codabar;
    unsigned s = codabar->s7;
    dbprintf(2, " s=%d", s);
    if(s < 7)
        return(-1);

    /* check width */
    if(!codabar_check_width(codabar->width, s)) {
        dbprintf(2, " [width]");
        return(-1);
    }

    /* extract min/max bar */
    unsigned ibar = decode_sortn(dcode, 4, 1);
    dbprintf(2, " bar=%04x", ibar);

    unsigned wbmax = get_width(dcode, ibar & 0xf);
    unsigned wbmin = get_width(dcode, ibar >> 12);
    if(8 * wbmin < wbmax ||
       3 * wbmin > 2 * wbmax)
    {
        dbprintf(2, " [bar outer ratio]");
        return(-1);
    }

    unsigned wb1 = get_width(dcode, (ibar >> 8) & 0xf);
    unsigned wb2 = get_width(dcode, (ibar >> 4) & 0xf);
    unsigned long b0b3 = wbmin * wbmax;
    unsigned long b1b2 = wb1 * wb2;
    if(b1b2 + b1b2 / 8 < b0b3) {
        /* single wide bar combinations */
        if(8 * wbmin < 5 * wb1 ||
           8 * wb1 < 5 * wb2 ||
           4 * wb2 > 3 * wbmax ||
           wb2 * wb2 >= wb1 * wbmax)
        {
            dbprintf(2, " [1bar inner ratios]");
            return(-1);
        }
        ibar = (ibar >> 1) & 0x3;
    }
    else if(b1b2 > b0b3 + b0b3 / 8) {
        /* three wide bars, no wide spaces */
        if(4 * wbmin > 3 * wb1 ||
           8 * wb1 < 5 * wb2 ||
           8 * wb2 < 5 * wbmax ||
           wbmin * wb2 >= wb1 * wb1)
        {
            dbprintf(2, " [3bar inner ratios]");
            return(-1);
        }
        ibar = (ibar >> 13) + 4;
    }
    else {
        dbprintf(2, " [bar inner ratios]");
        return(-1);
    }

    unsigned ispc = decode_sort3(dcode, 2);
    dbprintf(2, "(%x) spc=%03x", ibar, ispc);

    unsigned wsmax = get_width(dcode, ispc & 0xf);
    unsigned wsmid = get_width(dcode, (ispc >> 4) & 0xf);
    unsigned wsmin = get_width(dcode, (ispc >> 8) & 0xf);
    if(ibar >> 2) {
        /* verify no wide spaces */
        if(8 * wsmin < wsmax ||
           8 * wsmin < 5 * wsmid ||
           8 * wsmid < 5 * wsmax)
        {
            dbprintf(2, " [0space inner ratios]");
            return(-1);
        }
        ibar &= 0x3;
        if(codabar->direction)
            ibar = 3 - ibar;
        int c = (0xfcde >> (ibar << 2)) & 0xf;
        dbprintf(2, " ex[%d]=%x", ibar, c);
        return(c);
    }
    else if(8 * wsmin < wsmax ||
            3 * wsmin > 2 * wsmax)
    {
        dbprintf(2, " [space outer ratio]");
        return(-1);
    }

    unsigned long s0s2 = wsmin * wsmax;
    unsigned long s1s1 = wsmid * wsmid;
    if(s1s1 + s1s1 / 8 < s0s2) {
        /* single wide space */
        if(8 * wsmin < 5 * wsmid ||
           4 * wsmid > 3 * wsmax)
        {
            dbprintf(2, " [1space inner ratios]");
            return(-1);
        }
        ispc = ((ispc & 0xf) >> 1) - 1;
        unsigned ic = (ispc << 2) | ibar;
        if(codabar->direction)
            ic = 11 - ic;
        int c = codabar_lo[ic];
        dbprintf(2, "(%d) lo[%d]=%x", ispc, ic, c);
        return(c);
    }
    else if(s1s1 > s0s2 + s0s2 / 8) {
        /* two wide spaces, check start/stop */
        if(4 * wsmin > 3 * wsmid ||
           8 * wsmid < 5 * wsmax)
        {
            dbprintf(2, " [2space inner ratios]");
            return(-1);
        }
        if((ispc >> 8) == 4) {
            dbprintf(2, " [space comb]");
            return(-1);
        }
        ispc >>= 10;
        dbprintf(2, "(%d)", ispc);
        unsigned ic = ispc * 4 + ibar;
        zassert(ic < 8, -1, "ic=%d ispc=%d ibar=%d", ic, ispc, ibar);
        unsigned char c = codabar_hi[ic];
        if(c >> 2 != codabar->direction) {
            dbprintf(2, " [invalid stop]");
            return(-1);
        }
        c = (c & 0x3) | 0x10;
        dbprintf(2, " hi[%d]=%x", ic, c);
        return(c);
    }
    else {
        dbprintf(2, " [space inner ratios]");
        return(-1);
    }
}

static inline signed char
codabar_decode_start (zbar_decoder_t *dcode)
{
    codabar_decoder_t *codabar = &dcode->codabar;
    unsigned s = codabar->s7;
    if(s < 8)
        return(ZBAR_NONE);
    dbprintf(2, "      codabar: s=%d", s);

    /* check leading quiet zone - spec is 10x */
    unsigned qz = get_width(dcode, 8);
    if((qz && qz * 2 < s) ||
       4 * get_width(dcode, 0) > 3 * s)
    {
        dbprintf(2, " [invalid qz/ics]\n");
        return(ZBAR_NONE);
    }

    /* check space ratios first */
    unsigned ispc = decode_sort3(dcode, 2);
    dbprintf(2, " spc=%03x", ispc);
    if((ispc >> 8) == 4) {
        dbprintf(2, " [space comb]\n");
        return(ZBAR_NONE);
    }

    /* require 2 wide and 1 narrow spaces */
    unsigned wsmax = get_width(dcode, ispc & 0xf);
    unsigned wsmin = get_width(dcode, ispc >> 8);
    unsigned wsmid = get_width(dcode, (ispc >> 4) & 0xf);
    if(8 * wsmin < wsmax ||
       3 * wsmin > 2 * wsmax ||
       4 * wsmin > 3 * wsmid ||
       8 * wsmid < 5 * wsmax ||
       wsmid * wsmid <= wsmax * wsmin)
    {
        dbprintf(2, " [space ratio]\n");
        return(ZBAR_NONE);
    }
    ispc >>= 10;
    dbprintf(2, "(%d)", ispc);

    /* check bar ratios */
    unsigned ibar = decode_sortn(dcode, 4, 1);
    dbprintf(2, " bar=%04x", ibar);

    unsigned wbmax = get_width(dcode, ibar & 0xf);
    unsigned wbmin = get_width(dcode, ibar >> 12);
    if(8 * wbmin < wbmax ||
       3 * wbmin > 2 * wbmax)
    {
        dbprintf(2, " [bar outer ratio]\n");
        return(ZBAR_NONE);
    }

    /* require 1 wide & 3 narrow bars */
    unsigned wb1 = get_width(dcode, (ibar >> 8) & 0xf);
    unsigned wb2 = get_width(dcode, (ibar >> 4) & 0xf);
    if(8 * wbmin < 5 * wb1 ||
       8 * wb1 < 5 * wb2 ||
       4 * wb2 > 3 * wbmax ||
       wb1 * wb2 >= wbmin * wbmax ||
       wb2 * wb2 >= wb1 * wbmax)
    {
        dbprintf(2, " [bar inner ratios]\n");
        return(ZBAR_NONE);
    }
    ibar = ((ibar & 0xf) - 1) >> 1;
    dbprintf(2, "(%d)", ibar);

    /* decode combination */
    int ic = ispc * 4 + ibar;
    zassert(ic < 8, ZBAR_NONE, "ic=%d ispc=%d ibar=%d", ic, ispc, ibar);
    int c = codabar_hi[ic];
    codabar->buf[0] = (c & 0x3) | 0x10;

    /* set character direction */
    codabar->direction = c >> 2;

    codabar->element = 4;
    codabar->character = 1;
    codabar->width = codabar->s7;
    dbprintf(1, " start=%c dir=%x [valid start]\n",
             codabar->buf[0] + 0x31, codabar->direction);
    return(ZBAR_PARTIAL);
}

static inline int
codabar_checksum (zbar_decoder_t *dcode,
                  unsigned n)
{
    unsigned chk = 0;
    unsigned char *buf = dcode->buf;
    while(n--)
        chk += *(buf++);
    return(!!(chk & 0xf));
}

static inline zbar_symbol_type_t
codabar_postprocess (zbar_decoder_t *dcode)
{
    codabar_decoder_t *codabar = &dcode->codabar;
    int dir = codabar->direction;
    dcode->direction = 1 - 2 * dir;
    int i, n = codabar->character;
    for(i = 0; i < NIBUF; i++)
        dcode->buf[i] = codabar->buf[i];
    if(dir)
        /* reverse buffer */
        for(i = 0; i < n / 2; i++) {
            unsigned j = n - 1 - i;
            char code = dcode->buf[i];
            dcode->buf[i] = dcode->buf[j];
            dcode->buf[j] = code;
        }

    if(TEST_CFG(codabar->config, ZBAR_CFG_ADD_CHECK)) {
        /* validate checksum */
        if(codabar_checksum(dcode, n))
            return(ZBAR_NONE);
        if(!TEST_CFG(codabar->config, ZBAR_CFG_EMIT_CHECK)) {
            dcode->buf[n - 2] = dcode->buf[n - 1];
            n--;
        }
    }

    for(i = 0; i < n; i++) {
        unsigned c = dcode->buf[i];
        dcode->buf[i] = ((c < 0x14)
                         ? codabar_characters[c]
                         : '?');
    }
    dcode->buflen = i;
    dcode->buf[i] = '\0';
    dcode->modifiers = 0;

    codabar->character = -1;
    return(ZBAR_CODABAR);
}

zbar_symbol_type_t
_zbar_decode_codabar (zbar_decoder_t *dcode)
{
    codabar_decoder_t *codabar = &dcode->codabar;

    /* update latest character width */
    codabar->s7 -= get_width(dcode, 8);
    codabar->s7 += get_width(dcode, 1);

    if(get_color(dcode) != ZBAR_SPACE)
        return(ZBAR_NONE);
    if(codabar->character < 0)
        return(codabar_decode_start(dcode));
    if(codabar->character < 2 &&
       codabar_decode_start(dcode))
        return(ZBAR_PARTIAL);
    if(--codabar->element)
        return(ZBAR_NONE);
    codabar->element = 4;

    dbprintf(1, "      codabar[%c%02d+%x]",
             (codabar->direction) ? '<' : '>',
             codabar->character, codabar->element);

    signed char c = codabar_decode7(dcode);
    dbprintf(1, " %d", c);
    if(c < 0) {
        dbprintf(1, " [aborted]\n");
        goto reset;
    }

    unsigned char *buf;
    if(codabar->character < NIBUF)
        buf = codabar->buf;
    else {
        if(codabar->character >= BUFFER_MIN &&
           size_buf(dcode, codabar->character + 1))
        {
            dbprintf(1, " [overflow]\n");
            goto reset;
        }
        buf = dcode->buf;
    }
    buf[codabar->character++] = c;

    /* lock shared resources */
    if(codabar->character == NIBUF &&
       acquire_lock(dcode, ZBAR_CODABAR))
    {
        codabar->character = -1;
        return(ZBAR_PARTIAL);
    }

    unsigned s = codabar->s7;
    if(c & 0x10) {
        unsigned qz = get_width(dcode, 0);
        if(qz && qz * 2 < s) {
            dbprintf(2, " [invalid qz]\n");
            goto reset;
        }
        unsigned n = codabar->character;
        if(n < CFG(*codabar, ZBAR_CFG_MIN_LEN) ||
           (CFG(*codabar, ZBAR_CFG_MAX_LEN) > 0 &&
            n > CFG(*codabar, ZBAR_CFG_MAX_LEN)))
        {
            dbprintf(2, " [invalid len]\n");
            goto reset;
        }
        if(codabar->character < NIBUF &&
           acquire_lock(dcode, ZBAR_CODABAR))
        {
            codabar->character = -1;
            return(ZBAR_PARTIAL);
        }
        dbprintf(2, " stop=%c", c + 0x31);

        zbar_symbol_type_t sym = codabar_postprocess(dcode);
        if(sym > ZBAR_PARTIAL)
            dbprintf(2, " [valid stop]");
        else {
            release_lock(dcode, ZBAR_CODABAR);
            codabar->character = -1;
        }
        dbprintf(2, "\n");
        return(sym);
    }
    else if(4 * get_width(dcode, 0) > 3 * s) {
        dbprintf(2, " [ics]\n");
        goto reset;
    }

    dbprintf(2, "\n");
    return(ZBAR_NONE);

reset:
    if(codabar->character >= NIBUF)
        release_lock(dcode, ZBAR_CODABAR);
    codabar->character = -1;
    return(ZBAR_NONE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "databar.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define GS ('\035')

enum { SCH_NUM, SCH_ALNUM, SCH_ISO646 };

static const signed char finder_hash[0x20] = {
    0x16, 0x1f, 0x02, 0x00, 0x03, 0x00, 0x06, 0x0b,
    0x1f, 0x0e, 0x17, 0x0c, 0x0b, 0x14, 0x11, 0x0c,
    0x1f, 0x03, 0x13, 0x08, 0x00, 0x0a,   -1, 0x16,
    0x0c, 0x09,   -1, 0x1a, 0x1f, 0x1c, 0x00,   -1,
};

/* DataBar character encoding groups */
struct group_s {
    unsigned short sum;
    unsigned char wmax;
    unsigned char todd;
    unsigned char teven;
} groups[] = {
    /* (17,4) DataBar Expanded character groups */
    {    0, 7,  87,   4 },
    {  348, 5,  52,  20 },
    { 1388, 4,  30,  52 },
    { 2948, 3,  10, 104 },
    { 3988, 1,   1, 204 },

    /* (16,4) DataBar outer character groups */
    {    0, 8, 161,   1 },
    {  161, 6,  80,  10 },
    {  961, 4,  31,  34 },
    { 2015, 3,  10,  70 },
    { 2715, 1,   1, 126 },

    /* (15,4) DataBar inner character groups */
    { 1516, 8,  81,   1 },
    { 1036, 6,  48,  10 },
    {  336, 4,  20,  35 },
    {    0, 2,   4,  84 },
};

static const unsigned char exp_sequences[] = {
    /* sequence Group 1 */
    0x01,
    0x23,
    0x25, 0x07,
    0x29, 0x47,
    0x29, 0x67, 0x0b,
    0x29, 0x87, 0xab,
    /* sequence Group 2 */
    0x21, 0x43, 0x65, 0x07,
    0x21, 0x43, 0x65, 0x89,
    0x21, 0x43, 0x65, 0xa9, 0x0b,
    0x21, 0x43, 0x67, 0x89, 0xab
};

/* DataBar expanded checksum multipliers */
static const unsigned char exp_checksums[] = {
    1, 189, 62, 113, 46, 43, 109, 134, 6, 79, 161, 45
};

static inline void
append_check14 (unsigned char *buf)
{
    unsigned char chk = 0, d;
    int i;
    for(i = 13; --i >= 0; ) {
        d = *(buf++) - '0';
        chk += d;
        if(!(i & 1))
            chk += d << 1;
    }
    chk %= 10;
    if(chk)
        chk = 10 - chk;
    *buf = chk + '0';
}

static inline void
decode10 (unsigned char *buf,
          unsigned long n,
          int i)
{
    buf += i;
    while(--i >= 0) {
        unsigned char d = n % 10;
        n /= 10;
        *--buf = '0' + d;
    }
}

#define VAR_MAX(l, i) ((((l) * 12 + (i)) * 2 + 6) / 7)

#define FEED_BITS(b)                         \
    while(i < (b) && len) {                  \
        d = (d << 12) | (*(data++) & 0xfff); \
        i += 12;                             \
        len--;                               \
        dbprintf(2, " %03lx", d & 0xfff);     \
    }

#define PUSH_CHAR(c) \
    *(buf++) = (c)

#define PUSH_CHAR4(c0, c1, c2, c3) do { \
        PUSH_CHAR(c0);                  \
        PUSH_CHAR(c1);                  \
        PUSH_CHAR(c2);                  \
        PUSH_CHAR(c3);                  \
    } while(0);

static inline int
databar_postprocess_exp (zbar_decoder_t *dcode,
                         int *data)
{
    int i = 0, enc;
    unsigned n;
    unsigned char *buf;
    unsigned long d = *(data++);
    int len = d / 211 + 4, buflen;

    /* grok encodation method */
    d = *(data++);
    dbprintf(2, "\n    len=%d %03lx", len, d & 0xfff);
    n = (d >> 4) & 0x7f;
    if(n >= 0x40) {
        i = 10;
        enc = 1;
        buflen = 2 + 14 + VAR_MAX(len, 10 - 2 - 44 + 6) + 2;
    }
    else if(n >= 0x38) {
        i = 4;
        enc = 6 + (n & 7);
        buflen = 2 + 14 + 4 + 6 + 2 + 6 + 2;
    }
    else if(n >= 0x30) {
        i = 6;
        enc = 2 + ((n >> 2) & 1);
        buflen = 2 + 14 + 4 + 3 + VAR_MAX(len, 6 - 2 - 44 - 2 - 10) + 2;
    }
    else if(n >= 0x20) {
        i = 7;
        enc = 4 + ((n >> 3) & 1);
        buflen = 2 + 14 + 4 + 6;
    }
    else {
        i = 9;
        enc = 0;
        buflen = VAR_MAX(len, 9 - 2) + 2;
    }
    dbprintf(2, " buflen=%d enc=%d", buflen, enc);
    zassert(buflen > 2, -1, "buflen=%d\n", buflen);

    if(enc < 4) {
        /* grok variable length symbol bit field */
        if((len ^ (d >> (--i))) & 1)
            /* even/odd length mismatch */
            return(-1);
        if(((d >> (--i)) & 1) != (len > 14))
            /* size group mismatch */
            return(-1);
    }
    len -= 2;
    dbprintf(2, " [%d+%d]", i, len);

    if(size_buf(dcode, buflen))
        return(-1);
    buf = dcode->buf;

    /* handle compressed fields */
    if(enc) {
        PUSH_CHAR('0');
        PUSH_CHAR('1');
    }

    if(enc == 1) {
        i -= 4;
        n = (d >> i) & 0xf;
        if(i >= 10)
            return(-1);
        PUSH_CHAR('0' + n);
    }
    else if(enc)
        PUSH_CHAR('9');

    if(enc) {
        int j;
        for(j = 0; j < 4; j++) {
            FEED_BITS(10);
            i -= 10;
            n = (d >> i) & 0x3ff;
            if(n >= 1000)
                return(-1);
            decode10(buf, n, 3);
            buf += 3;
        }
        append_check14(buf - 13);
        buf++;
    }

    switch(enc)
    {
    case 2: /* 01100: AI 392x */
        FEED_BITS(2);
        i -= 2;
        n = (d >> i) & 0x3;
        PUSH_CHAR4('3', '9', '2', '0' + n);
        break;

    case 3: /* 01101: AI 393x */
        FEED_BITS(12);
        i -= 2;
        n = (d >> i) & 0x3;
        PUSH_CHAR4('3', '9', '3', '0' + n);
        i -= 10;
        n = (d >> i) & 0x3ff;
        if(n >= 1000)
            return(-1);
        decode10(buf, n, 3);
        buf += 3;
        break;

    case 4: /* 0100: AI 3103 */
        FEED_BITS(15);
        i -= 15;
        n = (d >> i) & 0x7fff;
        PUSH_CHAR4('3', '1', '0', '3');
        decode10(buf, n, 6);
        buf += 6;
        break;

    case 5: /* 0101: AI 3202/3203 */
        FEED_BITS(15);
        i -= 15;
        n = (d >> i) & 0x7fff;
        dbprintf(2, " v=%d", n);
        PUSH_CHAR4('3', '2', '0', (n >= 10000) ? '3' : '2' );
        if(n >= 10000)
            n -= 10000;
        decode10(buf, n, 6);
        buf += 6;
        break;
    }
    if(enc >= 6) {
        /* 0111000 - 0111111: AI 310x/320x + AI 11/13/15/17 */
        PUSH_CHAR4('3', '1' + (enc & 1), '0', 'x');
        FEED_BITS(20);
        i -= 20;
        n = (d >> i) & 0xfffff;
        dbprintf(2, " [%d+%d] %d", i, len, n);
        if(n >= 1000000)
            return(-1);
        decode10(buf, n, 6);
        *(buf - 1) = *buf;
        *buf = '0';
        buf += 6;

        FEED_BITS(16);
        i -= 16;
        n = (d >> i) & 0xffff;
        if(n < 38400) {
            int dd, mm, yy;
            dd = n % 32;
            n /= 32;
            mm = n % 12 + 1;
            n /= 12;
            yy = n;
            PUSH_CHAR('1');
            PUSH_CHAR('0' + ((enc - 6) | 1));
            decode10(buf, yy, 2);
            buf += 2;
            decode10(buf, mm, 2);
            buf += 2;
            decode10(buf, dd, 2);
            buf += 2;
        }
        else if(n > 38400)
            return(-1);
    }

    if(enc < 4) {
        /* remainder is general-purpose data compaction */
        int scheme = SCH_NUM;
        while(i > 0 || len > 0) {
            FEED_BITS(8);
            dbprintf(2, " [%d+%d]", i, len);

            if(scheme == SCH_NUM) {
                int n1;
                i -= 4;
                if(i < 0)
                    break;
                if(!((d >> i) & 0xf)) {
                    scheme = SCH_ALNUM;
                    dbprintf(2, ">A");
                    continue;
                }
                if(!len && i < 3) {
                    /* special case last digit */
                    n = ((d >> i) & 0xf) - 1;
                    if(n > 9)
                        return(-1);
                    *(buf++) = '0' + n;
                    break;
                }
                i -= 3;
                zassert(i >= 0, -1, "\n");
                n = ((d >> i) & 0x7f) - 8;
                n1 = n % 11;
                n = n / 11;
                dbprintf(2, "N%d%d", n, n1);
                *(buf++) = (n < 10) ? '0' + n : GS;
                *(buf++) = (n1 < 10) ? '0' + n1 : GS;
            }
            else  {
                unsigned c = 0;
                i -= 3;
                if(i < 0)
                    break;
                if(!((d >> i) & 0x7)) {
                    scheme = SCH_NUM;
                    continue;
                }
                i -= 2;
                if(i < 0)
                    break;
                n = (d >> i) & 0x1f;
                if(n == 0x04) {
                    scheme ^= 0x3;
                    dbprintf(2, ">%d", scheme);
                }
                else if(n == 0x0f)
                    c = GS;
                else if(n < 0x0f)
                    c = 43 + n;
                else if(scheme == SCH_ALNUM) {
                    i--;
                    if(i < 0)
                        return(-1);
                    n = (d >> i) & 0x1f;
                    if(n < 0x1a)
                        c = 'A' + n;
                    else if(n == 0x1a)
                        c = '*';
                    else if(n < 0x1f)
                        c = ',' + n - 0x1b;
                    else
                        return(-1);
                }
                else if(scheme == SCH_ISO646 && n < 0x1d) {
                    i -= 2;
                    if(i < 0)
                        return(-1);
                    n = (d >> i) & 0x3f;
                    if(n < 0x1a)
                        c = 'A' + n;
                    else if(n < 0x34)
                        c = 'a' + n - 0x1a;
                    else
                        return(-1);
                }
                else if(scheme == SCH_ISO646) {
                    i -= 3;
                    if(i < 0)
                        return(-1);
                    n = ((d >> i) & 0x1f);
                    dbprintf(2, "(%02x)", n);
                    if(n < 0xa)
                        c = '!' + n - 8;
                    else if(n < 0x15)
                        c = '%' + n - 0xa;
                    else if(n < 0x1b)
                        c = ':' + n - 0x15;
                    else if(n == 0x1b)
                        c = '_';
                    else if(n == 0x1c)
                        c = ' ';
                    else
                        return(-1);
                }
                else
                    return(-1);

                if(c) {
                    dbprintf(2, "%d%c", scheme, c);
                    *(buf++) = c;
                }
            }
        }
        /* FIXME check pad? */
    }

    i = buf - dcode->buf;
    zassert(i < dcode->buf_alloc, -1, "i=%02x %s\n", i,
            _zbar_decoder_buf_dump(dcode->buf, i));

    *buf = 0;
    dcode->buflen = i;
    if(i && *--buf == GS) {
        *buf = 0;
        dcode->buflen--;
    }

    dbprintf(2, "\n    %s", _zbar_decoder_buf_dump(dcode->buf, dcode->buflen));
    return(0);
}
#undef FEED_BITS

/* convert from heterogeneous base {1597,2841}
 * to base 10 character representation
 */
static inline void
databar_postprocess (zbar_decoder_t *dcode,
                     unsigned d[4])
{
    databar_decoder_t *db = &dcode->databar;
    int i;
    unsigned c, chk = 0;
    unsigned char *buf = dcode->buf;
    *(buf++) = '0';
    *(buf++) = '1';
    buf += 15;
    *--buf = '\0';
    *--buf = '\0';

    dbprintf(2, "\n    d={%d,%d,%d,%d}", d[0], d[1], d[2], d[3]);
    unsigned long r = d[0] * 1597 + d[1];
    d[1] = r / 10000;
    r %= 10000;
    r = r * 2841 + d[2];
    d[2] = r / 10000;
    r %= 10000;
    r = r * 1597 + d[3];
    d[3] = r / 10000;
    dbprintf(2, " r=%ld", r);

    for(i = 4; --i >= 0; ) {
        c = r % 10;
        chk += c;
        if(i & 1)
            chk += c << 1;
        *--buf = c + '0';
        if(i)
            r /= 10;
    }

    dbprintf(2, " d={%d,%d,%d}", d[1], d[2], d[3]);
    r = d[1] * 2841 + d[2];
    d[2] = r / 10000;
    r %= 10000;
    r = r * 1597 + d[3];
    d[3] = r / 10000;
    dbprintf(2, " r=%ld", r);

    for(i = 4; --i >= 0; ) {
        c = r % 10;
        chk += c;
        if(i & 1)
            chk += c << 1;
        *--buf = c + '0';
        if(i)
            r /= 10;
    }

    r = d[2] * 1597 + d[3];
    dbprintf(2, " d={%d,%d} r=%ld", d[2], d[3], r);

    for(i = 5; --i >= 0; ) {
        c = r % 10;
        chk += c;
        if(!(i & 1))
            chk += c << 1;
        *--buf = c + '0';
        if(i)
            r /= 10;
    }

    /* NB linkage flag not supported */
    if(TEST_CFG(db->config, ZBAR_CFG_EMIT_CHECK)) {
        chk %= 10;
        if(chk)
            chk = 10 - chk;
        buf[13] = chk + '0';
        dcode->buflen = buf - dcode->buf + 14;
    }
    else
        dcode->buflen = buf - dcode->buf + 13;

    dbprintf(2, "\n    %s", _zbar_decoder_buf_dump(dcode->buf, 16));
}

static inline int
databar_check_width (unsigned wf,
                     unsigned wd,
                     unsigned n)
{
    unsigned dwf = wf * 3;
    wd *= 14;
    wf *= n;
    return(wf - dwf <= wd && wd <= wf + dwf);
}

static inline void
merge_segment (databar_decoder_t *db,
               databar_segment_t *seg)
{
    unsigned csegs = db->csegs;
    int i;
    for(i = 0; i < csegs; i++) {
        databar_segment_t *s = db->segs + i;
        if(s != seg && s->finder == seg->finder && s->exp == seg->exp &&
           s->color == seg->color && s->side == seg->side &&
           s->data == seg->data && s->check == seg->check &&
           databar_check_width(seg->width, s->width, 14)) {
            /* merge with existing segment */
            unsigned cnt = s->count;
            if(cnt < 0x7f)
                cnt++;
            seg->count = cnt;
            seg->partial &= s->partial;
            seg->width = (3 * seg->width + s->width + 2) / 4;
            s->finder = -1;
            dbprintf(2, " dup@%d(%d,%d)",
                     i, cnt, (db->epoch - seg->epoch) & 0xff);
        }
        else if(s->finder >= 0) {
            unsigned age = (db->epoch - s->epoch) & 0xff;
            if(age >= 248 || (age >= 128 && s->count < 2))
                s->finder = -1;
        }
    }
}

static inline zbar_symbol_type_t
match_segment (zbar_decoder_t *dcode,
               databar_segment_t *seg)
{
    databar_decoder_t *db = &dcode->databar;
    unsigned csegs = db->csegs, maxage = 0xfff;
    int i0, i1, i2, maxcnt = 0;
    databar_segment_t *smax[3] = { NULL, };

    if(seg->partial && seg->count < 4)
        return(ZBAR_PARTIAL);

    for(i0 = 0; i0 < csegs; i0++) {
        databar_segment_t *s0 = db->segs + i0;
        if(s0 == seg || s0->finder != seg->finder || s0->exp ||
           s0->color != seg->color || s0->side == seg->side ||
           (s0->partial && s0->count < 4) ||
           !databar_check_width(seg->width, s0->width, 14))
            continue;

        for(i1 = 0; i1 < csegs; i1++) {
            databar_segment_t *s1 = db->segs + i1;
            int chkf, chks, chk;
            unsigned age1;
            if(i1 == i0 || s1->finder < 0 || s1->exp ||
               s1->color == seg->color ||
               (s1->partial && s1->count < 4) ||
               !databar_check_width(seg->width, s1->width, 14))
                continue;
            dbprintf(2, "\n\t[%d,%d] f=%d(0%xx)/%d(%x%x%x)",
                     i0, i1, seg->finder, seg->color,
                     s1->finder, s1->exp, s1->color, s1->side);

            if(seg->color)
                chkf = seg->finder + s1->finder * 9;
            else
                chkf = s1->finder + seg->finder * 9;
            if(chkf > 72)
                chkf--;
            if(chkf > 8)
                chkf--;

            chks = (seg->check + s0->check + s1->check) % 79;

            if(chkf >= chks)
                chk = chkf - chks;
            else
                chk = 79 + chkf - chks;

            dbprintf(2, " chk=(%d,%d) => %d", chkf, chks, chk);
            age1 = (((db->epoch - s0->epoch) & 0xff) +
                    ((db->epoch - s1->epoch) & 0xff));

            for(i2 = i1 + 1; i2 < csegs; i2++) {
                databar_segment_t *s2 = db->segs + i2;
                unsigned cnt, age2, age;
                if(i2 == i0 || s2->finder != s1->finder || s2->exp ||
                   s2->color != s1->color || s2->side == s1->side ||
                   s2->check != chk ||
                   (s2->partial && s2->count < 4) ||
                   !databar_check_width(seg->width, s2->width, 14))
                    continue;
                age2 = (db->epoch - s2->epoch) & 0xff;
                age = age1 + age2;
                cnt = s0->count + s1->count + s2->count;
                dbprintf(2, " [%d] MATCH cnt=%d age=%d", i2, cnt, age);
                if(maxcnt < cnt ||
                   (maxcnt == cnt && maxage > age)) {
                    maxcnt = cnt;
                    maxage = age;
                    smax[0] = s0;
                    smax[1] = s1;
                    smax[2] = s2;
                }
            }
        }
    }

    if(!smax[0])
        return(ZBAR_PARTIAL);

    unsigned d[4];
    d[(seg->color << 1) | seg->side] = seg->data;
    for(i0 = 0; i0 < 3; i0++) {
        d[(smax[i0]->color << 1) | smax[i0]->side] = smax[i0]->data;
        if(!--(smax[i0]->count))
            smax[i0]->finder = -1;
    }
    seg->finder = -1;

    if(size_buf(dcode, 18))
        return(ZBAR_PARTIAL);

    if(acquire_lock(dcode, ZBAR_DATABAR))
        return(ZBAR_PARTIAL);

    databar_postprocess(dcode, d);
    dcode->modifiers = MOD(ZBAR_MOD_GS1);
    dcode->direction = 1 - 2 * (seg->side ^ seg->color ^ 1);
    return(ZBAR_DATABAR);
}

static inline unsigned
lookup_sequence (databar_segment_t *seg,
                 int fixed,
                 int seq[22])
{
    unsigned n = seg->data / 211, i;
    const unsigned char *p;
    i = (n + 1) / 2 + 1;
    n += 4;
    i = (i * i) / 4;
    dbprintf(2, " {%d,%d:", i, n);
    p = exp_sequences + i;

    fixed >>= 1;
    seq[0] = 0;
    seq[1] = 1;
    for(i = 2; i < n; ) {
        int s = *p;
        if(!(i & 2)) {
            p++;
            s >>= 4;
        }
        else
            s &= 0xf;
        if(s == fixed)
            fixed = -1;
        s <<= 1;
        dbprintf(2, "%x", s);
        seq[i++] = s++;
        seq[i++] = s;
    }
    dbprintf(2, "}");
    seq[n] = -1;
    return(fixed < 1);
}

#define IDX(s) \
    (((s)->finder << 2) | ((s)->color << 1) | ((s)->color ^ (s)->side))

static inline zbar_symbol_type_t
match_segment_exp (zbar_decoder_t *dcode,
                   databar_segment_t *seg,
                   int dir)
{
    databar_decoder_t *db = &dcode->databar;
    int bestsegs[22], i = 0, segs[22], seq[22];
    int ifixed = seg - db->segs, fixed = IDX(seg), maxcnt = 0;
    int iseg[DATABAR_MAX_SEGMENTS];
    unsigned csegs = db->csegs, width = seg->width, maxage = 0x7fff;

    bestsegs[0] = segs[0] = seq[1] = -1;
    seq[0] = 0;

    dbprintf(2, "\n    fixed=%d@%d: ", fixed, ifixed);
    for(i = csegs, seg = db->segs + csegs - 1; --i >= 0; seg--) {
        if(seg->exp && seg->finder >= 0 &&
           (!seg->partial || seg->count >= 4))
            iseg[i] = IDX(seg);
        else
            iseg[i] = -1;
        dbprintf(2, " %d", iseg[i]);
    }

    for(i = 0; ; i--) {
        if(!i)
            dbprintf(2, "\n   ");
        for(; i >= 0 && seq[i] >= 0; i--) {
            int j;
            dbprintf(2, " [%d]%d", i, seq[i]);

            if(seq[i] == fixed) {
                seg = db->segs + ifixed;
                if(segs[i] < 0 && databar_check_width(width, seg->width, 14)) {
                    dbprintf(2, "*");
                    j = ifixed;
                }
                else
                    continue;
            }
            else {
                for(j = segs[i] + 1; j < csegs; j++) {
                    if(iseg[j] == seq[i] &&
                       (!i || databar_check_width(width, db->segs[j].width, 14))) {
                        seg = db->segs + j;
                        break;
                    }
                }
                if(j == csegs)
                    continue;
            }

            if(!i) {
                if(!lookup_sequence(seg, fixed, seq)) {
                    dbprintf(2, "[nf]");
                    continue;
                }
                width = seg->width;
                dbprintf(2, " A00@%d", j);
            }
            else {
                width = (width + seg->width) / 2;
                dbprintf(2, " %c%x%x@%d",
                         'A' + seg->finder, seg->color, seg->side, j);
            }
            segs[i++] = j;
            segs[i++] = -1;
        }
        if(i < 0)
            break;

        seg = db->segs + segs[0];
        unsigned cnt = 0, chk = 0, age = (db->epoch - seg->epoch) & 0xff;
        for(i = 1; segs[i] >= 0; i++) {
            seg = db->segs + segs[i];
            chk += seg->check;
            cnt += seg->count;
            age += (db->epoch - seg->epoch) & 0xff;
        }

        unsigned data0 = db->segs[segs[0]].data;
        unsigned chk0 = data0 % 211;
        chk %= 211;

        dbprintf(2, " chk=%d ?= %d", chk, chk0);
        if(chk != chk0)
            continue;

        dbprintf(2, " cnt=%d age=%d", cnt, age);
        if(maxcnt > cnt || (maxcnt == cnt && maxage <= age))
            continue;

        dbprintf(2, " !");
        maxcnt = cnt;
        maxage = age;
        for(i = 0; segs[i] >= 0; i++)
            bestsegs[i] = segs[i];
        bestsegs[i] = -1;
    }

    if(bestsegs[0] < 0)
        return(ZBAR_PARTIAL);

    if(acquire_lock(dcode, ZBAR_DATABAR_EXP))
        return(ZBAR_PARTIAL);

    for(i = 0; bestsegs[i] >= 0; i++)
        segs[i] = db->segs[bestsegs[i]].data;

    if(databar_postprocess_exp(dcode, segs)) {
        release_lock(dcode, ZBAR_DATABAR_EXP);
        return(ZBAR_PARTIAL);
    }

    for(i = 0; bestsegs[i] >= 0; i++)
        if(bestsegs[i] != ifixed) {
            seg = db->segs + bestsegs[i];
            if(!--seg->count)
                seg->finder = -1;
        }

    /* FIXME stacked rows are frequently reversed,
     * so direction is impossible to determine at this level
     */
    dcode->direction = (1 - 2 * (seg->side ^ seg->color)) * dir;
    dcode->modifiers = MOD(ZBAR_MOD_GS1);
    return(ZBAR_DATABAR_EXP);
}
#undef IDX

static inline unsigned
databar_calc_check (unsigned sig0,
                    unsigned sig1,
                    unsigned side,
                    unsigned mod)
{
    unsigned chk = 0;
    int i;
    for(i = 4; --i >= 0; ) {
        chk = (chk * 3 + (sig1 & 0xf) + 1) * 3 + (sig0 & 0xf) + 1;
        sig1 >>= 4;
        sig0 >>= 4;
        if(!(i & 1))
            chk %= mod;
    }
    dbprintf(2, " chk=%d", chk);

    if(side)
        chk = (chk * (6561 % mod)) % mod;
    return(chk);
}

static inline int
calc_value4 (unsigned sig,
             unsigned n,
             unsigned wmax,
             unsigned nonarrow)
{
    unsigned v = 0;
    n--;

    unsigned w0 = (sig >> 12) & 0xf;
    if(w0 > 1) {
        if(w0 > wmax)
            return(-1);
        unsigned n0 = n - w0;
        unsigned sk20 = (n - 1) * n * (2 * n - 1);
        unsigned sk21 = n0 * (n0 + 1) * (2 * n0 + 1);
        v = sk20 - sk21 - 3 * (w0 - 1) * (2 * n - w0);

        if(!nonarrow && w0 > 2 && n > 4) {
            unsigned k = (n - 2) * (n - 1) * (2 * n - 3) - sk21;
            k -= 3 * (w0 - 2) * (14 * n - 7 * w0 - 31);
            v -= k;
        }

        if(n - 2 > wmax) {
            unsigned wm20 = 2 * wmax * (wmax + 1);
            unsigned wm21 = (2 * wmax + 1);
            unsigned k = sk20;
            if(n0 > wmax) {
                k -= sk21;
                k += 3 * (w0 - 1) * (wm20 - wm21 * (2 * n - w0));
            }
            else {
                k -= (wmax + 1) * (wmax + 2) * (2 * wmax + 3);
                k += 3 * (n - wmax - 2) * (wm20 - wm21 * (n + wmax + 1));
            }
            k *= 3;
            v -= k;
        }
        v /= 12;
    }
    else
        nonarrow = 1;
    n -= w0;

    unsigned w1 = (sig >> 8) & 0xf;
    if(w1 > 1) {
        if(w1 > wmax)
            return(-1);
        v += (2 * n - w1) * (w1 - 1) / 2;
        if(!nonarrow && w1 > 2 && n > 3)
            v -= (2 * n - w1 - 5) * (w1 - 2) / 2;
        if(n - 1 > wmax) {
            if(n - w1 > wmax)
                v -= (w1 - 1) * (2 * n - w1 - 2 * wmax);
            else
                v -= (n - wmax) * (n - wmax - 1);
        }
    }
    else
        nonarrow = 1;
    n -= w1;

    unsigned w2 = (sig >> 4) & 0xf;
    if(w2 > 1) {
        if(w2 > wmax)
            return(-1);
        v += w2 - 1;
        if(!nonarrow && w2 > 2 && n > 2)
            v -= n - 2;
        if(n > wmax)
            v -= n - wmax;
    }
    else
        nonarrow = 1;

    unsigned w3 = sig & 0xf;
    if(w3 == 1)
        nonarrow = 1;
    else if(w3 > wmax)
        return(-1);

    if(!nonarrow)
        return(-1);

    return(v);
}

static inline zbar_symbol_type_t
decode_char (zbar_decoder_t *dcode,
             databar_segment_t *seg,
             int off,
             int dir)
{
    databar_decoder_t *db = &dcode->databar;
    unsigned s = calc_s(dcode, (dir > 0) ? off : off - 6, 8);
    int n, i, emin[2] = { 0, }, sum = 0;
    unsigned sig0 = 0, sig1 = 0;

    if(seg->exp)
        n = 17;
    else if(seg->side)
        n = 15;
    else
        n = 16;
    emin[1] = -n;

    dbprintf(2, "\n        char[%c%d]: n=%d s=%d w=%d sig=",
             (dir < 0) ? '>' : '<', off, n, s, seg->width);
    if(s < 13 || !databar_check_width(seg->width, s, n))
        return(ZBAR_NONE);

    for(i = 4; --i >= 0; ) {
        int e = decode_e(pair_width(dcode, off), s, n);
        if(e < 0)
            return(ZBAR_NONE);
        dbprintf(2, "%d", e);
        sum = e - sum;
        off += dir;
        sig1 <<= 4;
        if(emin[1] < -sum)
            emin[1] = -sum;
        sig1 += sum;
        if(!i)
            break;

        e = decode_e(pair_width(dcode, off), s, n);
        if(e < 0)
            return(ZBAR_NONE);
        dbprintf(2, "%d", e);
        sum = e - sum;
        off += dir;
        sig0 <<= 4;
        if(emin[0] > sum)
            emin[0] = sum;
        sig0 += sum;
    }

    int diff = emin[~n & 1];
    diff = diff + (diff << 4);
    diff = diff + (diff << 8);

    sig0 -= diff;
    sig1 += diff;

    dbprintf(2, " emin=%d,%d el=%04x/%04x", emin[0], emin[1], sig0, sig1);

    unsigned sum0 = sig0 + (sig0 >> 8);
    unsigned sum1 = sig1 + (sig1 >> 8);
    sum0 += sum0 >> 4;
    sum1 += sum1 >> 4;
    sum0 &= 0xf;
    sum1 &= 0xf;

    dbprintf(2, " sum=%d/%d", sum0, sum1);

    if(sum0 + sum1 + 8 != n) {
        dbprintf(2, " [SUM]");
        return(ZBAR_NONE);
    }

    if(((sum0 ^ (n >> 1)) | (sum1 ^ (n >> 1) ^ n)) & 1) {
        dbprintf(2, " [ODD]");
        return(ZBAR_NONE);
    }

    i = ((n & 0x3) ^ 1) * 5 + (sum1 >> 1);
    zassert(i < sizeof(groups) / sizeof(*groups), -1,
            "n=%d sum=%d/%d sig=%04x/%04x g=%d",
            n, sum0, sum1, sig0, sig1, i);
    struct group_s *g = groups + i;
    dbprintf(2, "\n            g=%d(%d,%d,%d/%d)",
             i, g->sum, g->wmax, g->todd, g->teven);

    int vodd = calc_value4(sig0 + 0x1111, sum0 + 4, g->wmax, ~n & 1);
    dbprintf(2, " v=%d", vodd);
    if(vodd < 0 || vodd > g->todd)
        return(ZBAR_NONE);

    int veven = calc_value4(sig1 + 0x1111, sum1 + 4, 9 - g->wmax, n & 1);
    dbprintf(2, "/%d", veven);
    if(veven < 0 || veven > g->teven)
        return(ZBAR_NONE);

    int v = g->sum;
    if(n & 2)
        v += vodd + veven * g->todd;
    else
        v += veven + vodd * g->teven;

    dbprintf(2, " f=%d(%x%x%x)", seg->finder, seg->exp, seg->color, seg->side);

    unsigned chk = 0;
    if(seg->exp) {
        unsigned side = seg->color ^ seg->side ^ 1;
        if(v >= 4096)
            return(ZBAR_NONE);
        /* skip A1 left */
        chk = databar_calc_check(sig0, sig1, side, 211);
        if(seg->finder || seg->color || seg->side) {
            i = (seg->finder << 1) - side + seg->color;
            zassert(i >= 0 && i < 12, ZBAR_NONE,
                    "f=%d(%x%x%x) side=%d i=%d\n",
                    seg->finder, seg->exp, seg->color, seg->side, side, i);
            chk = (chk * exp_checksums[i]) % 211;
        }
        else if(v >= 4009)
            return(ZBAR_NONE);
        else
            chk = 0;
    }
    else {
        chk = databar_calc_check(sig0, sig1, seg->side, 79);
        if(seg->color)
            chk = (chk * 16) % 79;
    }
    dbprintf(2, " => %d val=%d", chk, v);

    seg->check = chk;
    seg->data = v;

    merge_segment(db, seg);

    if(seg->exp)
        return(match_segment_exp(dcode, seg, dir));
    else if(dir > 0)
        return(match_segment(dcode, seg));
    return(ZBAR_PARTIAL);
}

static inline int
alloc_segment (databar_decoder_t *db)
{
    unsigned maxage = 0, csegs = db->csegs;
    int i, old = -1;
    for(i = 0; i < csegs; i++) {
        databar_segment_t *seg = db->segs + i;
        unsigned age;
        if(seg->finder < 0) {
            dbprintf(2, " free@%d", i);
            return(i);
        }
        age = (db->epoch - seg->epoch) & 0xff;
        if(age >= 128 && seg->count < 2) {
            seg->finder = -1;
            dbprintf(2, " stale@%d (%d - %d = %d)",
                     i, db->epoch, seg->epoch, age);
            return(i);
        }

        /* score based on both age and count */
        if(age > seg->count)
            age = age - seg->count + 1;
        else
            age = 1;

        if(maxage < age) {
            maxage = age;
            old = i;
            dbprintf(2, " old@%d(%u)", i, age);
        }
    }

    if(csegs < DATABAR_MAX_SEGMENTS) {
        dbprintf(2, " new@%d", i);
        i = csegs;
        csegs *= 2;
        if(csegs > DATABAR_MAX_SEGMENTS)
            csegs = DATABAR_MAX_SEGMENTS;
        if(csegs != db->csegs) {
            databar_segment_t *seg;
            db->segs = realloc(db->segs, csegs * sizeof(*db->segs));
            db->csegs = csegs;
            seg = db->segs + csegs;
            while(--seg, --csegs >= i) {
                seg->finder = -1;
                seg->exp = 0;
                seg->color = 0;
                seg->side = 0;
                seg->partial = 0;
                seg->count = 0;
                seg->epoch = 0;
                seg->check = 0;
            }
            return(i);
        }
    }
    zassert(old >= 0, -1, "\n");

    db->segs[old].finder = -1;
    return(old);
}

static inline zbar_symbol_type_t
decode_finder (zbar_decoder_t *dcode)
{
    databar_decoder_t *db = &dcode->databar;
    databar_segment_t *seg;
    unsigned e0 = pair_width(dcode, 1);
    unsigned e2 = pair_width(dcode, 3);
    unsigned e1, e3, s, finder, dir;
    int sig, iseg;
    dbprintf(2, "      databar: e0=%d e2=%d", e0, e2);
    if(e0 < e2) {
        unsigned e = e2 * 4;
        if(e < 15 * e0 || e > 34 * e0)
            return(ZBAR_NONE);
        dir = 0;
        e3 = pair_width(dcode, 4);
    }
    else {
        unsigned e = e0 * 4;
        if(e < 15 * e2 || e > 34 * e2)
            return(ZBAR_NONE);
        dir = 1;
        e2 = e0;
        e3 = pair_width(dcode, 0);
    }
    e1 = pair_width(dcode, 2);

    s = e1 + e3;
    dbprintf(2, " e1=%d e3=%d dir=%d s=%d", e1, e3, dir, s);
    if(s < 12)
        return(ZBAR_NONE);

    sig = ((decode_e(e3, s, 14) << 8) | (decode_e(e2, s, 14) << 4) |
           decode_e(e1, s, 14));
    dbprintf(2, " sig=%04x", sig & 0xfff);
    if(sig < 0 ||
       ((sig >> 4) & 0xf) < 8 ||
       ((sig >> 4) & 0xf) > 10 ||
       (sig & 0xf) >= 10 ||
       ((sig >> 8) & 0xf) >= 10 ||
       (((sig >> 8) + sig) & 0xf) != 10)
        return(ZBAR_NONE);

    finder = (finder_hash[(sig - (sig >> 5)) & 0x1f] +
              finder_hash[(sig >> 1) & 0x1f]) & 0x1f;
    dbprintf(2, " finder=%d", finder);
    if(finder == 0x1f ||
       !TEST_CFG((finder < 9) ? db->config : db->config_exp, ZBAR_CFG_ENABLE))
        return(ZBAR_NONE);

    zassert(finder >= 0, ZBAR_NONE, "dir=%d sig=%04x f=%d\n",
            dir, sig & 0xfff, finder);

    iseg = alloc_segment(db);
    if(iseg < 0)
        return(ZBAR_NONE);

    seg = db->segs + iseg;
    seg->finder = (finder >= 9) ? finder - 9 : finder;
    seg->exp = (finder >= 9);
    seg->color = get_color(dcode) ^ dir ^ 1;
    seg->side = dir;
    seg->partial = 0;
    seg->count = 1;
    seg->width = s;
    seg->epoch = db->epoch;

    int rc = decode_char(dcode, seg, 12 - dir, -1);
    if(!rc)
        seg->partial = 1;
    else
        db->epoch++;

    int i = (dcode->idx + 8 + dir) & 0xf;
    zassert(db->chars[i] == -1, ZBAR_NONE, "\n");
    db->chars[i] = iseg;
    return(rc);
}

zbar_symbol_type_t
_zbar_decode_databar (zbar_decoder_t *dcode)
{
    databar_decoder_t *db = &dcode->databar;
    databar_segment_t *seg, *pair;
    zbar_symbol_type_t sym;
    int iseg, i = dcode->idx & 0xf;

    sym = decode_finder(dcode);
    dbprintf(2, "\n");

    iseg = db->chars[i];
    if(iseg < 0)
        return(sym);

    db->chars[i] = -1;
    seg = db->segs + iseg;
    dbprintf(2, "        databar: i=%d part=%d f=%d(%x%x%x)",
             iseg, seg->partial, seg->finder, seg->exp, seg->color, seg->side);
    zassert(seg->finder >= 0, ZBAR_NONE, "i=%d f=%d(%x%x%x) part=%x\n",
            iseg, seg->finder, seg->exp, seg->color, seg->side, seg->partial);

    if(seg->partial) {
        pair = NULL;
        seg->side = !seg->side;
    }
    else {
        int jseg = alloc_segment(db);
        pair = db->segs + iseg;
        seg = db->segs + jseg;
        seg->finder = pair->finder;
        seg->exp = pair->exp;
        seg->color = pair->color;
        seg->side = !pair->side;
        seg->partial = 0;
        seg->count = 1;
        seg->width = pair->width;
        seg->epoch = db->epoch;
    }

    sym = decode_char(dcode, seg, 1, 1);
    if(!sym) {
        seg->finder = -1;
        if(pair)
            pair->partial = 1;
    }
    else
        db->epoch++;
    dbprintf(2, "\n");

    return(sym);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "code39.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define NUM_CHARS (0x2c)

static const unsigned char code39_hi[32] = {
    0x80 | 0x00,  /* 2 next */
    0x40 | 0x02,  /* 4 */
    0x80 | 0x06,  /* 2 next */
    0xc0 | 0x08,  /* 2 skip */
    0x40 | 0x0a,  /* 4 */
    0x80 | 0x0e,  /* 2 next */
    0xc0 | 0x10,  /* 2 skip */
    0x00 | 0x12,  /* direct */

    0x80 | 0x13,  /* 2 next */
    0xc0 | 0x15,  /* 2 skip */
    0x80 | 0x17,  /* 2 next */
    0xff,
    0xc0 | 0x19,  /* 2 skip */
    0x00 | 0x1b,  /* direct */
    0xff,
    0xff,

    0x40 | 0x1c,  /* 4 */
    0x80 | 0x20,  /* 2 next */
    0xc0 | 0x22,  /* 2 skip */
    0x00 | 0x24,  /* direct */
    0x80 | 0x25,  /* 2 next */
    0xff,
    0x00 | 0x27,  /* direct */
    0xff,

    0xc0 | 0x28,  /* 2 skip */
    0x00 | 0x2a,  /* direct */
    0xff,
    0xff,
    0x00 | 0x2b,  /* direct */
    0xff,
    0xff,
    0xff,
};

typedef struct char39_s {
    unsigned char chk, rev, fwd;
} char39_t;

static const char39_t code39_encodings[NUM_CHARS] = {
    { 0x07, 0x1a, 0x20 }, /* 00 */
    { 0x0d, 0x10, 0x03 }, /* 01 */
    { 0x13, 0x17, 0x22 }, /* 02 */
    { 0x16, 0x1d, 0x23 }, /* 03 */
    { 0x19, 0x0d, 0x05 }, /* 04 */
    { 0x1c, 0x13, 0x06 }, /* 05 */
    { 0x25, 0x07, 0x0c }, /* 06 */
    { 0x2a, 0x2a, 0x27 }, /* 07 */
    { 0x31, 0x04, 0x0e }, /* 08 */
    { 0x34, 0x00, 0x0f }, /* 09 */
    { 0x43, 0x15, 0x25 }, /* 0a */
    { 0x46, 0x1c, 0x26 }, /* 0b */
    { 0x49, 0x0b, 0x08 }, /* 0c */
    { 0x4c, 0x12, 0x09 }, /* 0d */
    { 0x52, 0x19, 0x2b }, /* 0e */
    { 0x58, 0x0f, 0x00 }, /* 0f */
    { 0x61, 0x02, 0x11 }, /* 10 */
    { 0x64, 0x09, 0x12 }, /* 11 */
    { 0x70, 0x06, 0x13 }, /* 12 */
    { 0x85, 0x24, 0x16 }, /* 13 */
    { 0x8a, 0x29, 0x28 }, /* 14 */
    { 0x91, 0x21, 0x18 }, /* 15 */
    { 0x94, 0x2b, 0x19 }, /* 16 */
    { 0xa2, 0x28, 0x29 }, /* 17 */
    { 0xa8, 0x27, 0x2a }, /* 18 */
    { 0xc1, 0x1f, 0x1b }, /* 19 */
    { 0xc4, 0x26, 0x1c }, /* 1a */
    { 0xd0, 0x23, 0x1d }, /* 1b */
    { 0x03, 0x14, 0x1e }, /* 1c */
    { 0x06, 0x1b, 0x1f }, /* 1d */
    { 0x09, 0x0a, 0x01 }, /* 1e */
    { 0x0c, 0x11, 0x02 }, /* 1f */
    { 0x12, 0x18, 0x21 }, /* 20 */
    { 0x18, 0x0e, 0x04 }, /* 21 */
    { 0x21, 0x01, 0x0a }, /* 22 */
    { 0x24, 0x08, 0x0b }, /* 23 */
    { 0x30, 0x05, 0x0d }, /* 24 */
    { 0x42, 0x16, 0x24 }, /* 25 */
    { 0x48, 0x0c, 0x07 }, /* 26 */
    { 0x60, 0x03, 0x10 }, /* 27 */
    { 0x81, 0x1e, 0x14 }, /* 28 */
    { 0x84, 0x25, 0x15 }, /* 29 */
    { 0x90, 0x22, 0x17 }, /* 2a */
    { 0xc0, 0x20, 0x1a }, /* 2b */
};

static const unsigned char code39_characters[NUM_CHARS] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%*";

static inline unsigned char code39_decode1 (unsigned char enc,
                                            unsigned e,
                                            unsigned s)
{
    unsigned char E = decode_e(e, s, 72);
    if(E > 18)
        return(0xff);
    enc <<= 1;
    if(E > 6) {
        enc |= 1;
        dbprintf(2, "1");
    }
    else
        dbprintf(2, "0");
    return(enc);
}

static inline signed char code39_decode9 (zbar_decoder_t *dcode)
{
    code39_decoder_t *dcode39 = &dcode->code39;

    if(dcode39->s9 < 9)
        return(-1);

    /* threshold bar width ratios */
    unsigned char i, enc = 0;
    for(i = 0; i < 5; i++) {
        enc = code39_decode1(enc, get_width(dcode, i), dcode39->s9);
        if(enc == 0xff)
            return(-1);
    }
    zassert(enc < 0x20, -1, " enc=%x s9=%x\n", enc, dcode39->s9);

    /* lookup first 5 encoded widths for coarse decode */
    unsigned char idx = code39_hi[enc];
    if(idx == 0xff)
        return(-1);

    /* encode remaining widths (NB first encoded width is lost) */
    for(; i < 9; i++) {
        enc = code39_decode1(enc, get_width(dcode, i), dcode39->s9);
        if(enc == 0xff)
            return(-1);
    }

    if((idx & 0xc0) == 0x80)
        idx = (idx & 0x3f) + ((enc >> 3) & 1);
    else if((idx & 0xc0) == 0xc0)
        idx = (idx & 0x3f) + ((enc >> 2) & 1);
    else if(idx & 0xc0)
        idx = (idx & 0x3f) + ((enc >> 2) & 3);
    zassert(idx < 0x2c, -1, " idx=%x enc=%x s9=%x\n", idx, enc, dcode39->s9);

    const char39_t *c = &code39_encodings[idx];
    dbprintf(2, " i=%02x chk=%02x c=%02x/%02x", idx, c->chk, c->fwd, c->rev);
    if(enc != c->chk)
        return(-1);

    dcode39->width = dcode39->s9;
    return((dcode39->direction) ? c->rev : c->fwd);
}

static inline signed char code39_decode_start (zbar_decoder_t *dcode)
{
    code39_decoder_t *dcode39 = &dcode->code39;
    dbprintf(2, " s=%d ", dcode39->s9);

    signed char c = code39_decode9(dcode);
    if(c != 0x19 && c != 0x2b) {
        dbprintf(2, "\n");
        return(ZBAR_NONE);
    }
    dcode39->direction ^= (c == 0x19);

    /* check leading quiet zone - spec is 10x */
    unsigned quiet = get_width(dcode, 9);
    if(quiet && quiet < dcode39->s9 / 2) {
        dbprintf(2, " [invalid quiet]\n");
        return(ZBAR_NONE);
    }

    dcode39->element = 9;
    dcode39->character = 0;
    dbprintf(1, " dir=%x [valid start]\n", dcode39->direction);
    return(ZBAR_PARTIAL);
}

static inline int code39_postprocess (zbar_decoder_t *dcode)
{
    code39_decoder_t *dcode39 = &dcode->code39;
    dcode->direction = 1 - 2 * dcode39->direction;
    int i;
    if(dcode39->direction) {
        /* reverse buffer */
        dbprintf(2, " (rev)");
        for(i = 0; i < dcode39->character / 2; i++) {
            unsigned j = dcode39->character - 1 - i;
            char code = dcode->buf[i];
            dcode->buf[i] = dcode->buf[j];
            dcode->buf[j] = code;
        }
    }
    for(i = 0; i < dcode39->character; i++)
        dcode->buf[i] = ((dcode->buf[i] < 0x2b)
                         ? code39_characters[(unsigned)dcode->buf[i]]
                         : '?');
    zassert(i < dcode->buf_alloc, -1, "i=%02x %s\n", i,
            _zbar_decoder_buf_dump(dcode->buf, dcode39->character));
    dcode->buflen = i;
    dcode->buf[i] = '\0';
    dcode->modifiers = 0;
    return(0);
}

static inline int
c39_check_width (unsigned ref,
                 unsigned w)
{
    unsigned dref = ref;
    ref *= 4;
    w *= 4;
    return(ref - dref <= w && w <= ref + dref);
}

zbar_symbol_type_t _zbar_decode_code39 (zbar_decoder_t *dcode)
{
    code39_decoder_t *dcode39 = &dcode->code39;

    /* update latest character width */
    dcode39->s9 -= get_width(dcode, 9);
    dcode39->s9 += get_width(dcode, 0);

    if(dcode39->character < 0) {
        if(get_color(dcode) != ZBAR_BAR)
            return(ZBAR_NONE);
        dbprintf(2, "      code39:");
        return(code39_decode_start(dcode));
    }

    if(++dcode39->element < 9)
        return(ZBAR_NONE);

    dbprintf(2, "      code39[%c%02d+%x]",
             (dcode39->direction) ? '<' : '>',
             dcode39->character, dcode39->element);

    if(dcode39->element == 10) {
        unsigned space = get_width(dcode, 0);
        if(dcode39->character &&
           dcode->buf[dcode39->character - 1] == 0x2b) {  /* STOP */
            /* trim STOP character */
            dcode39->character--;
            zbar_symbol_type_t sym = ZBAR_NONE;

            /* trailing quiet zone check */
            if(space && space < dcode39->width / 2)
                dbprintf(2, " [invalid qz]\n");
            else if(dcode39->character < CFG(*dcode39, ZBAR_CFG_MIN_LEN) ||
                    (CFG(*dcode39, ZBAR_CFG_MAX_LEN) > 0 &&
                     dcode39->character > CFG(*dcode39, ZBAR_CFG_MAX_LEN)))
                dbprintf(2, " [invalid len]\n");
            else if(!code39_postprocess(dcode)) {
                /* FIXME checksum */
                dbprintf(2, " [valid end]\n");
                sym = ZBAR_CODE39;
            }
            dcode39->character = -1;
            if(!sym)
                release_lock(dcode, ZBAR_CODE39);
            return(sym);
        }
        if(space > dcode39->width / 2) {
            /* inter-character space check failure */
            dbprintf(2, " ics>%d [invalid ics]", dcode39->width);
            if(dcode39->character)
                release_lock(dcode, ZBAR_CODE39);
            dcode39->character = -1;
        }
        dcode39->element = 0;
        dbprintf(2, "\n");
        return(ZBAR_NONE);
    }

    dbprintf(2, " s=%d ", dcode39->s9);
    if(!c39_check_width(dcode39->width, dcode39->s9)) {
        dbprintf(2, " [width]\n");
        if(dcode39->character)
            release_lock(dcode, ZBAR_CODE39);
        dcode39->character = -1;
        return(ZBAR_NONE);
    }

    signed char c = code39_decode9(dcode);
    dbprintf(2, " c=%d", c);

    /* lock shared resources */
    if(!dcode39->character && acquire_lock(dcode, ZBAR_CODE39)) {
        dcode39->character = -1;
        return(ZBAR_PARTIAL);
    }

    if(c < 0 || size_buf(dcode, dcode39->character + 1)) {
        dbprintf(1, (c < 0) ? " [aborted]\n" : " [overflow]\n");
        release_lock(dcode, ZBAR_CODE39);
        dcode39->character = -1;
        return(ZBAR_NONE);
    }
    else {
        zassert(c < 0x2c, ZBAR_NONE, "c=%02x s9=%x\n", c, dcode39->s9);
        dbprintf(2, "\n");
    }

    dcode->buf[dcode39->character++] = c;

    return(ZBAR_NONE);
}

#undef NUM_CHARS

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "code93.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

static const signed char code93_hash[0x40] = {
    0x0f, 0x2b, 0x30, 0x38, 0x13, 0x1b, 0x11, 0x2a,
    0x0a,   -1, 0x2f, 0x0f, 0x38, 0x38, 0x2f, 0x37,
    0x24, 0x3a, 0x1b, 0x36, 0x18, 0x26, 0x02, 0x2c,
    0x2b, 0x05, 0x21, 0x3b, 0x04, 0x15, 0x12, 0x0c,
    0x00, 0x26, 0x23, 0x00,   -1, 0x2e, 0x3f, 0x13,
    0x2e, 0x36,   -1, 0x08, 0x09,   -1, 0x15, 0x14,
      -1, 0x00, 0x21, 0x3b,   -1, 0x33, 0x00,   -1,
    0x2d, 0x0c, 0x1b, 0x0a, 0x3f, 0x3f, 0x29, 0x1c,
};

static inline int
c93_check_width (unsigned cur,
                 unsigned prev)
{
    unsigned dw;
    if(prev > cur)
        dw = prev - cur;
    else
        dw = cur - prev;
    dw *= 4;
    return(dw > prev);
}

static inline int
encode6 (zbar_decoder_t *dcode)
{
    /* build edge signature of character */
    unsigned s = dcode->s6;
    int sig = 0, i;

    dbprintf(2, " s=%d ", s);
    if(s < 9)
        return(-1);

    for(i = 6; --i > 0; ) {
        unsigned c = decode_e(pair_width(dcode, i), s, 9);
        if(c > 3)
            return(-1);
        sig = (sig << 2) | c;
        dbprintf(2, "%d", c);
    }
    dbprintf(2, " sig=%03x", sig);

    return(sig);
}

static inline int
validate_sig (int sig)
{
    int i, sum = 0, emin = 0, sig0 = 0, sig1 = 0;
    dbprintf(3, " sum=0");
    for(i = 3; --i >= 0; ) {
        int e = sig & 3;
        sig >>= 2;
        sum = e - sum;
        sig1 <<= 4;
        sig1 += sum;
        dbprintf(3, "%d", sum);
        if(!i)
            break;

        e = sig & 3;
        sig >>= 2;
        sum = e - sum;
        sig0 <<= 4;
        if(emin > sum)
            emin = sum;
        sig0 += sum;
        dbprintf(3, "%d", sum);
    }

    dbprintf(3, " emin=%d sig=%03x/%03x", emin, sig1 & 0xfff, sig0 & 0xfff);

    emin = emin + (emin << 4) + (emin << 8);
    sig0 -= emin;
    sig1 += emin;

    dbprintf(3, "=%03x/%03x", sig1 & 0xfff, sig0 & 0xfff);
    return((sig0 | sig1) & 0x888);
}

static inline int
c93_decode6 (zbar_decoder_t *dcode)
{
    int sig = encode6(dcode);
    int g0, g1, c;
    if(sig < 0 ||
       (sig & 0x3) + ((sig >> 4) & 0x3) + ((sig >> 8) & 0x3) != 3 ||
       validate_sig(sig))
        return(-1);

    if(dcode->code93.direction) {
        /* reverse signature */
        unsigned tmp = sig & 0x030;
        sig = ((sig & 0x3c0) >> 6) | ((sig & 0x00f) << 6);
        sig = ((sig & 0x30c) >> 2) | ((sig & 0x0c3) << 2) | tmp;
    }

    g0 = code93_hash[(sig - (sig >> 4)) & 0x3f];
    g1 = code93_hash[((sig >> 2) - (sig >> 7)) & 0x3f];
    zassert(g0 >= 0 && g1 >= 0, -1,
            "dir=%x sig=%03x g0=%03x g1=%03x %s\n",
            dcode->code93.direction, sig, g0, g1,
            _zbar_decoder_buf_dump(dcode->buf, dcode->code93.character));

    c = (g0 + g1) & 0x3f;
    dbprintf(2, " g0=%x g1=%x c=%02x", g0, g1, c);
    return(c);
}

static inline zbar_symbol_type_t
decode_start (zbar_decoder_t *dcode)
{
    code93_decoder_t *dcode93 = &dcode->code93;
    unsigned dir, qz, s = dcode->s6;
    int c;

    dbprintf(2, "      code93:");
    c = encode6(dcode);
    if(c < 0 || (c != 0x00f && c != 0x0f0))
        return(ZBAR_NONE);

    dir = (c >> 7);

    if(dir) {
        if(decode_e(pair_width(dcode, 0), s, 9))
            return(ZBAR_NONE);
        qz = get_width(dcode, 8);
    }

    qz = get_width(dcode, 7);
    if(qz && qz < (s * 3) / 4) {
        dbprintf(2, " [invalid qz %d]", qz);
        return(ZBAR_NONE);
    }

    /* decoded valid start/stop - initialize state */
    dcode93->direction = dir;
    dcode93->element = (!dir) ? 0 : 7;
    dcode93->character = 0;
    dcode93->width = s;

    dbprintf(2, " dir=%x [valid start]", dir);
    return(ZBAR_PARTIAL);
}

static inline zbar_symbol_type_t
decode_abort (zbar_decoder_t *dcode,
              const char *reason)
{
    code93_decoder_t *dcode93 = &dcode->code93;
    if(dcode93->character > 1)
        release_lock(dcode, ZBAR_CODE93);
    dcode93->character = -1;
    if(reason)
        dbprintf(1, " [%s]\n", reason);
    return(ZBAR_NONE);
}

static inline zbar_symbol_type_t
check_stop (zbar_decoder_t *dcode)
{
    code93_decoder_t *dcode93 = &dcode->code93;
    unsigned n = dcode93->character, s = dcode->s6;
    int max_len = CFG(*dcode93, ZBAR_CFG_MAX_LEN);
    if(n < 2 ||
       n < CFG(*dcode93, ZBAR_CFG_MIN_LEN) ||
       (max_len && n > max_len))
        return(decode_abort(dcode, "invalid len"));

    if(dcode93->direction) {
        unsigned qz = get_width(dcode, 0);
        if(qz && qz < (s * 3) / 4)
            return(decode_abort(dcode, "invalid qz"));
    }
    else if(decode_e(pair_width(dcode, 0), s, 9))
        /* FIXME forward-trailing QZ check */
        return(decode_abort(dcode, "invalid stop"));

    return(ZBAR_CODE93);
}

#define CHKMOD (47)

static inline int
plusmod47 (int acc,
           int add)
{
    acc += add;
    if(acc >= CHKMOD)
        acc -= CHKMOD;
    return(acc);
}

static inline int
validate_checksums (zbar_decoder_t *dcode)
{
    code93_decoder_t *dcode93 = &dcode->code93;
    unsigned d, i, n = dcode93->character;
    unsigned sum_c = 0, acc_c = 0, i_c = (n - 2) % 20;
    unsigned sum_k = 0, acc_k = 0, i_k = (n - 1) % 15;

    for(i = 0; i < n - 2; i++) {
        d = dcode->buf[(dcode93->direction) ? n - 1 - i : i];

        if(!i_c--) {
            acc_c = 0;
            i_c = 19;
        }
        acc_c = plusmod47(acc_c, d);
        sum_c = plusmod47(sum_c, acc_c);

        if(!i_k--) {
            acc_k = 0;
            i_k = 14;
        }
        acc_k = plusmod47(acc_k, d);
        sum_k = plusmod47(sum_k, acc_k);
    }

    d = dcode->buf[(dcode93->direction) ? 1 : n - 2];
    dbprintf(2, " C=%02x?=%02x", d, sum_c);
    if(d != sum_c)
        return(1);

    acc_k = plusmod47(acc_k, sum_c);
    sum_k = plusmod47(sum_k, acc_k);
    d = dcode->buf[(dcode93->direction) ? 0 : n - 1];
    dbprintf(2, " K=%02x?=%02x", d, sum_k);
    if(d != sum_k)
        return(1);

    return(0);
}

/* resolve scan direction and convert to ASCII */
static inline int
c93_postprocess (zbar_decoder_t *dcode)
{
    code93_decoder_t *dcode93 = &dcode->code93;
    unsigned i, j, n = dcode93->character;
    static const unsigned char code93_graph[] = "-. $/+%";
    static const unsigned char code93_s2[] =
        "\x1b\x1c\x1d\x1e\x1f;<=>?[\\]^_{|}~\x7f\x00\x40`\x7f\x7f\x7f";

    dbprintf(2, "\n    postproc len=%d", n);
    dcode->direction = 1 - 2 * dcode93->direction;
    if(dcode93->direction) {
        /* reverse buffer */
        dbprintf(2, " (rev)");
        for(i = 0; i < n / 2; i++) {
            unsigned j = n - 1 - i;
            unsigned char d = dcode->buf[i];
            dcode->buf[i] = dcode->buf[j];
            dcode->buf[j] = d;
        }
    }

    n -= 2;
    for(i = 0, j = 0; i < n; ) {
        unsigned char d = dcode->buf[i++];
        if(d < 0xa)
            d = '0' + d;
        else if(d < 0x24)
            d = 'A' + d - 0xa;
        else if(d < 0x2b)
            d = code93_graph[d - 0x24];
        else {
            unsigned shift = d;
            zassert(shift < 0x2f, -1, "%s\n",
                    _zbar_decoder_buf_dump(dcode->buf, dcode93->character));
            d = dcode->buf[i++];
            if(d < 0xa || d >= 0x24)
                return(1);
            d -= 0xa;
            switch(shift)
            {
            case 0x2b: d++; break;
            case 0x2c: d = code93_s2[d]; break;
            case 0x2d: d += 0x21; break;
            case 0x2e: d += 0x61; break;
            default: return(1);
            }
        }
        dcode->buf[j++] = d;
    }

    zassert(j < dcode->buf_alloc, 1,
            "j=%02x %s\n", j,
            _zbar_decoder_buf_dump(dcode->buf, dcode->code93.character));
    dcode->buflen = j;
    dcode->buf[j] = '\0';
    dcode->modifiers = 0;
    return(0);
}

zbar_symbol_type_t
_zbar_decode_code93 (zbar_decoder_t *dcode)
{
    code93_decoder_t *dcode93 = &dcode->code93;
    int c;

    if(dcode93->character < 0) {
        zbar_symbol_type_t sym;
        if(get_color(dcode) != ZBAR_BAR)
            return(ZBAR_NONE);
        sym = decode_start(dcode);
        dbprintf(2, "\n");
        return(sym);
    }

    if(/* process every 6th element of active symbol */
       ++dcode93->element != 6 ||
       /* decode color based on direction */
       get_color(dcode) == dcode93->direction)
        return(ZBAR_NONE);

    dcode93->element = 0;

    dbprintf(2, "      code93[%c%02d+%x]:",
             (dcode93->direction) ? '<' : '>',
             dcode93->character, dcode93->element);

    if(c93_check_width(dcode->s6, dcode93->width))
        return(decode_abort(dcode, "width var"));

    c = c93_decode6(dcode);
    if(c < 0)
        return(decode_abort(dcode, "aborted"));

    if(c == 0x2f) {
        if(!check_stop(dcode))
            return(ZBAR_NONE);
        if(validate_checksums(dcode))
            return(decode_abort(dcode, "checksum error"));
        if(c93_postprocess(dcode))
            return(decode_abort(dcode, "invalid encoding"));

        dbprintf(2, " [valid end]\n");
        dbprintf(3, "    %s\n",
                 _zbar_decoder_buf_dump(dcode->buf, dcode93->character));

        dcode93->character = -1;
        return(ZBAR_CODE93);
    }

    if(size_buf(dcode, dcode93->character + 1))
        return(decode_abort(dcode, "overflow"));

    dcode93->width = dcode->s6;

    if(dcode93->character == 1) {
        /* lock shared resources */
        if(acquire_lock(dcode, ZBAR_CODE93))
            return(decode_abort(dcode, NULL));
        dcode->buf[0] = dcode93->buf;
    }

    if(!dcode93->character)
        dcode93->buf = c;
    else
        dcode->buf[dcode93->character] = c;
    dcode93->character++;

    dbprintf(2, "\n");
    return(ZBAR_NONE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "code128.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define NUM_CHARS 108           /* total number of character codes */

typedef enum code128_char_e {
    FNC3        = 0x60,
    FNC2        = 0x61,
    SHIFT       = 0x62,
    CODE_C      = 0x63,
    CODE_B      = 0x64,
    CODE_A      = 0x65,
    FNC1        = 0x66,
    START_A     = 0x67,
    START_B     = 0x68,
    START_C     = 0x69,
    STOP_FWD    = 0x6a,
    STOP_REV    = 0x6b,
    FNC4        = 0x6c,
} code128_char_t;

static const unsigned char characters[NUM_CHARS] = {
    0x5c, 0xbf, 0xa1,                                           /* [00] 00 */
    0x2a, 0xc5, 0x0c, 0xa4,                                     /* [03] 01 */
    0x2d, 0xe3, 0x0f,                                           /* [07] 02 */
    0x5f, 0xe4,                                                 /* [0a] 03 */

    0x6b, 0xe8, 0x69, 0xa7, 0xe7,                               /* [0c] 10 */
    0xc1, 0x51, 0x1e, 0x83, 0xd9, 0x00, 0x84, 0x1f,             /* [11] 11 */
    0xc7, 0x0d, 0x33, 0x86, 0xb5, 0x0e, 0x15, 0x87,             /* [19] 12 */
    0x10, 0xda, 0x11,                                           /* [21] 13 */

    0x36, 0xe5, 0x18, 0x37,                                     /* [24] 20 */
    0xcc, 0x13, 0x39, 0x89, 0x97, 0x14, 0x1b, 0x8a, 0x3a, 0xbd, /* [28] 21 */
    0xa2, 0x5e, 0x01, 0x85, 0xb0, 0x02, 0xa3,                   /* [32] 22 */
    0xa5, 0x2c, 0x16, 0x88, 0xbc, 0x12, 0xa6,                   /* [39] 23 */

    0x61, 0xe6, 0x56, 0x62,                                     /* [40] 30 */
    0x19, 0xdb, 0x1a,                                           /* [44] 31 */
    0xa8, 0x32, 0x1c, 0x8b, 0xcd, 0x1d, 0xa9,                   /* [47] 32 */
    0xc3, 0x20, 0xc4,                                           /* [4e] 33 */

    0x50, 0x5d, 0xc0,       /* [51] 0014 0025 0034 */
    0x2b, 0xc6,             /* [54] 0134 0143 */
    0x2e,                   /* [56] 0243 */
    0x53, 0x60,             /* [57] 0341 0352 */
    0x31,                   /* [59] 1024 */
    0x52, 0xc2,             /* [5a] 1114 1134 */
    0x34, 0xc8,             /* [5c] 1242 1243 */
    0x55,                   /* [5e] 1441 */

    0x57, 0x3e, 0xce,       /* [5f] 4100 5200 4300 */
    0x3b, 0xc9,             /* [62] 4310 3410 */
    0x6a,                   /* [64] 3420 */
    0x54, 0x4f,             /* [65] 1430 2530 */
    0x38,                   /* [67] 4201 */
    0x58, 0xcb,             /* [68] 4111 4311 */
    0x2f, 0xca,             /* [6a] 2421 3421 */
};

static const unsigned char lo_base[8] = {
    0x00, 0x07, 0x0c, 0x19, 0x24, 0x32, 0x40, 0x47
};

static const unsigned char lo_offset[0x80] = {
    0xff, 0xf0, 0xff, 0x1f, 0xff, 0xf2, 0xff, 0xff,     /* 00 [00] */
    0xff, 0xff, 0xff, 0x3f, 0xf4, 0xf5, 0xff, 0x6f,     /* 01 */
    0xff, 0xff, 0xff, 0xff, 0xf0, 0xf1, 0xff, 0x2f,     /* 02 [07] */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x4f,     /* 03 */
    0xff, 0x0f, 0xf1, 0xf2, 0xff, 0x3f, 0xff, 0xf4,     /* 10 [0c] */
    0xf5, 0xf6, 0xf7, 0x89, 0xff, 0xab, 0xff, 0xfc,     /* 11 */
    0xff, 0xff, 0x0f, 0x1f, 0x23, 0x45, 0xf6, 0x7f,     /* 12 [19] */
    0xff, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xf9, 0xaf,     /* 13 */

    0xf0, 0xf1, 0xff, 0x2f, 0xff, 0xf3, 0xff, 0xff,     /* 20 [24] */
    0x4f, 0x5f, 0x67, 0x89, 0xfa, 0xbf, 0xff, 0xcd,     /* 21 */
    0xf0, 0xf1, 0xf2, 0x3f, 0xf4, 0x56, 0xff, 0xff,     /* 22 [32] */
    0xff, 0xff, 0x7f, 0x8f, 0x9a, 0xff, 0xbc, 0xdf,     /* 23 */
    0x0f, 0x1f, 0xf2, 0xff, 0xff, 0x3f, 0xff, 0xff,     /* 30 [40] */
    0xf4, 0xff, 0xf5, 0x6f, 0xff, 0xff, 0xff, 0xff,     /* 31 */
    0x0f, 0x1f, 0x23, 0xff, 0x45, 0x6f, 0xff, 0xff,     /* 32 [47] */
    0xf7, 0xff, 0xf8, 0x9f, 0xff, 0xff, 0xff, 0xff,     /* 33 */
};

static inline signed char decode_lo (int sig)
{
    unsigned char offset = (((sig >> 1) & 0x01) |
                            ((sig >> 3) & 0x06) |
                            ((sig >> 5) & 0x18) |
                            ((sig >> 7) & 0x60));
    unsigned char idx = lo_offset[offset];
    unsigned char base, c;

    if(sig & 1)
        idx &= 0xf;
    else
        idx >>= 4;
    if(idx == 0xf)
        return(-1);

    base = (sig >> 11) | ((sig >> 9) & 1);
    zassert(base < 8, -1, "sig=%x offset=%x idx=%x base=%x\n",
            sig, offset, idx, base);
    idx += lo_base[base];

    zassert(idx <= 0x50, -1, "sig=%x offset=%x base=%x idx=%x\n",
            sig, offset, base, idx);
    c = characters[idx];
    dbprintf(2, " %02x(%x(%02x)/%x(%02x)) => %02x",
             idx, base, lo_base[base], offset, lo_offset[offset],
             (unsigned char)c);
    return(c);
}

static inline signed char decode_hi (int sig)
{
    unsigned char rev = (sig & 0x4400) != 0;
    unsigned char idx, c;
    if(rev)
        sig = (((sig >> 12) & 0x000f) |
               ((sig >>  4) & 0x00f0) |
               ((sig <<  4) & 0x0f00) |
               ((sig << 12) & 0xf000));
    dbprintf(2, " rev=%x", rev != 0);

    switch(sig) {
    case 0x0014: idx = 0x0; break;
    case 0x0025: idx = 0x1; break;
    case 0x0034: idx = 0x2; break;
    case 0x0134: idx = 0x3; break;
    case 0x0143: idx = 0x4; break;
    case 0x0243: idx = 0x5; break;
    case 0x0341: idx = 0x6; break;
    case 0x0352: idx = 0x7; break;
    case 0x1024: idx = 0x8; break;
    case 0x1114: idx = 0x9; break;
    case 0x1134: idx = 0xa; break;
    case 0x1242: idx = 0xb; break;
    case 0x1243: idx = 0xc; break;
    case 0x1441: idx = 0xd; rev = 0; break;
    default: return(-1);
    }
    if(rev)
        idx += 0xe;
    c = characters[0x51 + idx];
    dbprintf(2, " %02x => %02x", idx, c);
    return(c);
}

static inline unsigned char calc_check (unsigned char c)
{
    if(!(c & 0x80))
        return(0x18);
    c &= 0x7f;
    if(c < 0x3d)
        return((c < 0x30 && c != 0x17) ? 0x10 : 0x20);
    if(c < 0x50)
        return((c == 0x4d) ? 0x20 : 0x10);
    return((c < 0x67) ? 0x20 : 0x10);
}

static inline signed char decode6 (zbar_decoder_t *dcode)
{
    int sig;
    signed char c, chk;
    unsigned bars;

    /* build edge signature of character */
    unsigned s = dcode->code128.s6;

    dbprintf(2, " s=%d", s);
    if(s < 5)
        return(-1);
    /* calculate similar edge measurements */
    sig = (get_color(dcode) == ZBAR_BAR)
        ? ((decode_e(get_width(dcode, 0) + get_width(dcode, 1), s, 11) << 12) |
           (decode_e(get_width(dcode, 1) + get_width(dcode, 2), s, 11) << 8) |
           (decode_e(get_width(dcode, 2) + get_width(dcode, 3), s, 11) << 4) |
           (decode_e(get_width(dcode, 3) + get_width(dcode, 4), s, 11)))
        : ((decode_e(get_width(dcode, 5) + get_width(dcode, 4), s, 11) << 12) |
           (decode_e(get_width(dcode, 4) + get_width(dcode, 3), s, 11) << 8) |
           (decode_e(get_width(dcode, 3) + get_width(dcode, 2), s, 11) << 4) |
           (decode_e(get_width(dcode, 2) + get_width(dcode, 1), s, 11)));
    if(sig < 0)
        return(-1);
    dbprintf(2, " sig=%04x", sig);
    /* lookup edge signature */
    c = (sig & 0x4444) ? decode_hi(sig) : decode_lo(sig);
    if(c == -1)
        return(-1);

    /* character validation */
    bars = (get_color(dcode) == ZBAR_BAR)
        ? (get_width(dcode, 0) + get_width(dcode, 2) + get_width(dcode, 4))
        : (get_width(dcode, 1) + get_width(dcode, 3) + get_width(dcode, 5));
    bars = bars * 11 * 4 / s;
    chk = calc_check(c);
    dbprintf(2, " bars=%d chk=%d", bars, chk);
    if(chk - 7 > bars || bars > chk + 7)
        return(-1);

    return(c & 0x7f);
}

static inline unsigned char validate_checksum (zbar_decoder_t *dcode)
{
    unsigned idx, sum, i, acc = 0;
    unsigned char check, err;

    code128_decoder_t *dcode128 = &dcode->code128;
    if(dcode128->character < 3)
        return(1);

    /* add in irregularly weighted start character */
    idx = (dcode128->direction) ? dcode128->character - 1 : 0;
    sum = dcode->buf[idx];
    if(sum >= 103)
        sum -= 103;

    /* calculate sum in reverse to avoid multiply operations */
    for(i = dcode128->character - 3; i; i--) {
        zassert(sum < 103, -1, "dir=%x i=%x sum=%x acc=%x %s\n",
                dcode128->direction, i, sum, acc,
                _zbar_decoder_buf_dump(dcode->buf, dcode128->character));
        idx = (dcode128->direction) ? dcode128->character - 1 - i : i;
        acc += dcode->buf[idx];
        if(acc >= 103)
            acc -= 103;
        zassert(acc < 103, -1, "dir=%x i=%x sum=%x acc=%x %s\n",
                dcode128->direction, i, sum, acc,
                _zbar_decoder_buf_dump(dcode->buf, dcode128->character));
        sum += acc;
        if(sum >= 103)
            sum -= 103;
    }

    /* and compare to check character */
    idx = (dcode128->direction) ? 1 : dcode128->character - 2;
    check = dcode->buf[idx];
    dbprintf(2, " chk=%02x(%02x)", sum, check);
    err = (sum != check);
    if(err)
        dbprintf(1, " [checksum error]\n");
    return(err);
}

/* expand and decode character set C */
static inline unsigned postprocess_c (zbar_decoder_t *dcode,
                                      unsigned start,
                                      unsigned end,
                                      unsigned dst)
{
    unsigned i, j;

    /* expand buffer to accomodate 2x set C characters (2 digits per-char) */
    unsigned delta = end - start;
    unsigned newlen = dcode->code128.character + delta;
    size_buf(dcode, newlen);

    /* relocate unprocessed data to end of buffer */
    memmove(dcode->buf + start + delta, dcode->buf + start,
            dcode->code128.character - start);
    dcode->code128.character = newlen;

    for(i = 0, j = dst; i < delta; i++, j += 2) {
        /* convert each set C character into two ASCII digits */
        unsigned char code = dcode->buf[start + delta + i];
        dcode->buf[j] = '0';
        if(code >= 50) {
            code -= 50;
            dcode->buf[j] += 5;
        }
        if(code >= 30) {
            code -= 30;
            dcode->buf[j] += 3;
        }
        if(code >= 20) {
            code -= 20;
            dcode->buf[j] += 2;
        }
        if(code >= 10) {
            code -= 10;
            dcode->buf[j] += 1;
        }
        zassert(dcode->buf[j] <= '9', delta,
                "start=%x end=%x i=%x j=%x %s\n", start, end, i, j,
                _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));
        zassert(code <= 9, delta,
                "start=%x end=%x i=%x j=%x %s\n", start, end, i, j,
                _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));
        dcode->buf[j + 1] = '0' + code;
    }
    return(delta);
}

/* resolve scan direction and convert to ASCII */
static inline unsigned char postprocess (zbar_decoder_t *dcode)
{
    unsigned i, j, cexp;
    unsigned char code = 0, charset;
    code128_decoder_t *dcode128 = &dcode->code128;
    dbprintf(2, "\n    postproc len=%d", dcode128->character);
    dcode->modifiers = 0;
    dcode->direction = 1 - 2 * dcode128->direction;
    if(dcode128->direction) {
        /* reverse buffer */
        dbprintf(2, " (rev)");
        for(i = 0; i < dcode128->character / 2; i++) {
            unsigned j = dcode128->character - 1 - i;
            code = dcode->buf[i];
            dcode->buf[i] = dcode->buf[j];
            dcode->buf[j] = code;
        }
        zassert(dcode->buf[dcode128->character - 1] == STOP_REV, 1,
                "dir=%x %s\n", dcode128->direction,
                _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));
    }
    else
        zassert(dcode->buf[dcode128->character - 1] == STOP_FWD, 1,
                "dir=%x %s\n", dcode128->direction,
                _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));

    code = dcode->buf[0];
    zassert(code >= START_A && code <= START_C, 1, "%s\n",
            _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));

    charset = code - START_A;
    cexp = (code == START_C) ? 1 : 0;
    dbprintf(2, " start=%c", 'A' + charset);

    for(i = 1, j = 0; i < dcode128->character - 2; i++) {
        unsigned char code = dcode->buf[i];
        zassert(!(code & 0x80), 1,
                "i=%x j=%x code=%02x charset=%x cexp=%x %s\n",
                i, j, code, charset, cexp,
                _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));

        if((charset & 0x2) && (code < 100))
            /* defer character set C for expansion */
            continue;
        else if(code < 0x60) {
            /* convert character set B to ASCII */
            code = code + 0x20;
            if((!charset || (charset == 0x81)) && (code >= 0x60))
                /* convert character set A to ASCII */
                code -= 0x60;
            dcode->buf[j++] = code;
            if(charset & 0x80)
                charset &= 0x7f;
        }
        else {
            dbprintf(2, " %02x", code);
            if(charset & 0x2) {
                unsigned delta;
                /* expand character set C to ASCII */
                zassert(cexp, 1, "i=%x j=%x code=%02x charset=%x cexp=%x %s\n",
                        i, j, code, charset, cexp,
                        _zbar_decoder_buf_dump(dcode->buf,
                                                dcode->code128.character));
                delta = postprocess_c(dcode, cexp, i, j);
                i += delta;
                j += delta * 2;
                cexp = 0;
            }
            if(code < CODE_C) {
                if(code == SHIFT)
                    charset |= 0x80;
                else if(code == FNC2) {
                    /* FIXME FNC2 - message append */
                }
                else if(code == FNC3) {
                    /* FIXME FNC3 - initialize */
                }
            }
            else if(code == FNC1) {
                /* FNC1 - Code 128 subsets or ASCII 0x1d */
                if(i == 1)
                    dcode->modifiers |= MOD(ZBAR_MOD_GS1);
                else if(i == 2)
                    dcode->modifiers |= MOD(ZBAR_MOD_AIM);
                else if(i < dcode->code128.character - 3)
                    dcode->buf[j++] = 0x1d;
                /*else drop trailing FNC1 */
            }
            else if(code >= START_A) {
                dbprintf(1, " [truncated]\n");
                return(1);
            }
            else {
                unsigned char newset = CODE_A - code;
                zassert(code >= CODE_C && code <= CODE_A, 1,
                        "i=%x j=%x code=%02x charset=%x cexp=%x %s\n",
                        i, j, code, charset, cexp,
                        _zbar_decoder_buf_dump(dcode->buf,
                                                dcode->code128.character));
                if(newset != charset)
                    charset = newset;
                else {
                    /* FIXME FNC4 - extended ASCII */
                }
            }
            if(charset & 0x2)
                cexp = i + 1;
        }
    }
    if(charset & 0x2) {
        zassert(cexp, 1, "i=%x j=%x code=%02x charset=%x cexp=%x %s\n",
                i, j, code, charset, cexp,
                _zbar_decoder_buf_dump(dcode->buf,
                                        dcode->code128.character));
        j += postprocess_c(dcode, cexp, i, j) * 2;
    }
    zassert(j < dcode->buf_alloc, 1, "j=%02x %s\n", j,
            _zbar_decoder_buf_dump(dcode->buf, dcode->code128.character));
    dcode->buflen = j;
    dcode->buf[j] = '\0';
    dcode->code128.character = j;
    return(0);
}

zbar_symbol_type_t _zbar_decode_code128 (zbar_decoder_t *dcode)
{
    code128_decoder_t *dcode128 = &dcode->code128;
    signed char c;

    /* update latest character width */
    dcode128->s6 -= get_width(dcode, 6);
    dcode128->s6 += get_width(dcode, 0);

    if((dcode128->character < 0)
       ? get_color(dcode) != ZBAR_SPACE
       : (/* process every 6th element of active symbol */
          ++dcode128->element != 6 ||
          /* decode color based on direction */
          get_color(dcode) != dcode128->direction))
        return(0);
    dcode128->element = 0;

    dbprintf(2, "      code128[%c%02d+%x]:",
             (dcode128->direction) ? '<' : '>',
             dcode128->character, dcode128->element);

    c = decode6(dcode);
    if(dcode128->character < 0) {
        unsigned qz;
        dbprintf(2, " c=%02x", c);
        if(c < START_A || c > STOP_REV || c == STOP_FWD) {
            dbprintf(2, " [invalid]\n");
            return(0);
        }
        qz = get_width(dcode, 6);
        if(qz && qz < (dcode128->s6 * 3) / 4) {
            dbprintf(2, " [invalid qz %d]\n", qz);
            return(0);
        }
        /* decoded valid start/stop */
        /* initialize state */
        dcode128->character = 1;
        if(c == STOP_REV) {
            dcode128->direction = ZBAR_BAR;
            dcode128->element = 7;
        }
        else
            dcode128->direction = ZBAR_SPACE;
        dcode128->start = c;
        dcode128->width = dcode128->s6;
        dbprintf(2, " dir=%x [valid start]\n", dcode128->direction);
        return(0);
    }
    else if(c < 0 || size_buf(dcode, dcode128->character + 1)) {
        dbprintf(1, (c < 0) ? " [aborted]\n" : " [overflow]\n");
        if(dcode128->character > 1)
            release_lock(dcode, ZBAR_CODE128);
        dcode128->character = -1;
        return(0);
    }
    else {
        unsigned dw;
        if(dcode128->width > dcode128->s6)
            dw = dcode128->width - dcode128->s6;
        else
            dw = dcode128->s6 - dcode128->width;
        dw *= 4;
        if(dw > dcode128->width) {
            dbprintf(1, " [width var]\n");
            if(dcode128->character > 1)
                release_lock(dcode, ZBAR_CODE128);
            dcode128->character = -1;
            return(0);
        }
    }
    dcode128->width = dcode128->s6;

    zassert(dcode->buf_alloc > dcode128->character, 0,
            "alloc=%x idx=%x c=%02x %s\n",
            dcode->buf_alloc, dcode128->character, c,
            _zbar_decoder_buf_dump(dcode->buf, dcode->buf_alloc));

    if(dcode128->character == 1) {
        /* lock shared resources */
        if(acquire_lock(dcode, ZBAR_CODE128)) {
            dcode128->character = -1;
            return(0);
        }
        dcode->buf[0] = dcode128->start;
    }

    dcode->buf[dcode128->character++] = c;

    if(dcode128->character > 2 &&
       ((dcode128->direction)
        ? c >= START_A && c <= START_C
        : c == STOP_FWD)) {
        /* FIXME STOP_FWD should check extra bar (and QZ!) */
        zbar_symbol_type_t sym = ZBAR_CODE128;
        if(validate_checksum(dcode) || postprocess(dcode))
            sym = ZBAR_NONE;
        else if(dcode128->character < CFG(*dcode128, ZBAR_CFG_MIN_LEN) ||
                (CFG(*dcode128, ZBAR_CFG_MAX_LEN) > 0 &&
                 dcode128->character > CFG(*dcode128, ZBAR_CFG_MAX_LEN))) {
            dbprintf(2, " [invalid len]\n");
            sym = ZBAR_NONE;
        }
        else
            dbprintf(2, " [valid end]\n");
        dcode128->character = -1;
        if(!sym)
            release_lock(dcode, ZBAR_CODE128);
        return(sym);
    }

    dbprintf(2, "\n");
    return(0);
}

#undef NUM_CHARS

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "pdf417_hash.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

/* PDF417 bar to codeword decode table */

#define PDF417_HASH_MASK 0xfff

static const signed short pdf417_hash[PDF417_HASH_MASK + 1] = {
 0x170, 0xd8e, 0x02e, 0x000, 0xa21, 0xc99, 0x000, 0xf06,
 0xdaa, 0x7a1, 0xc5f, 0x7ff, 0xbcf, 0xac8, 0x000, 0xc51,
 0x49a, 0x5c7, 0x000, 0xef2, 0x000, 0x7dd, 0x9ee, 0xe32,
 0x1b7, 0x489, 0x3b7, 0xe70, 0x9c8, 0xe5e, 0xdf4, 0x599,
 0x4e0, 0x608, 0x639, 0xead, 0x0ac, 0x57c, 0x000, 0x20d,
 0x61b, 0x000, 0x7d1, 0x80f, 0x803, 0x000, 0x946, 0x093,
 0x79c, 0xf9c, 0xb34, 0x6d8, 0x4f1, 0x975, 0x886, 0x313,
 0xe8a, 0xf20, 0x3c9, 0xa92, 0xb90, 0xa1d, 0x091, 0x0ac,
 0xb50, 0x3af, 0x90a, 0x45a, 0x815, 0xf29, 0xb20, 0xb6d,
 0xc5c, 0x1cd, 0x1e2, 0x1bf, 0x963, 0x80b, 0xa7c, 0x9b7,
 0xb65, 0x6b7, 0x117, 0xc04, 0x000, 0x18e, 0x000, 0x77f,
 0xe0e, 0xf48, 0x370, 0x818, 0x379, 0x000, 0x090, 0xe77,
 0xd99, 0x8b8, 0xb95, 0x8a9, 0x94c, 0xc48, 0x679, 0x000,
 0x41a, 0x9ea, 0xb0e, 0x9c1, 0x1b4, 0x000, 0x630, 0x811,
 0x4b1, 0xc05, 0x98f, 0xa68, 0x485, 0x706, 0xfff, 0x0d9,
 0xddc, 0x000, 0x83f, 0x54e, 0x290, 0xfe7, 0x64f, 0xf36,
 0x000, 0x151, 0xb9b, 0x5cd, 0x961, 0x690,    -1, 0xa7a,
 0x328, 0x707, 0xe6d, 0xe1f,    -1, 0x6a0, 0xf3e, 0xb27,
 0x315, 0xc8c, 0x6de, 0x996, 0x2f9, 0xc4c, 0x90f,    -1,
 0xaa7, 0x9e9, 0xfff, 0x0bb, 0x33b, 0xbc6, 0xe17, 0x000,
 0x85d, 0x912, 0x5f7, 0x000, 0xff1, 0xba1, 0x086, 0xa1e,
 0x85a, 0x4cf, 0xd47, 0x5a9, 0x5dc, 0x0bc,    -1, 0x544,
 0x522, 0x1ff, 0xfa6, 0xa83, 0xc7d, 0x545, 0xd75, 0xb6f,
 0x284, 0xf11, 0xe46,    -1, 0x900, 0x0f3, 0xe31, 0x705,
 0x06d, 0xd59, 0x67b, 0xe56,    -1, 0xde2, 0x000, 0xd42,
    -1, 0x24b, 0x000, 0xf87, 0x842,    -1, 0xbb9, 0x065,
 0x626, 0x86a, 0x9f8,    -1, 0x7ac, 0xe20, 0xbe9, 0x357,
 0xfff, 0xf82, 0x219, 0x9d4, 0x269, 0x8a6, 0x251, 0x0af,
 0xd02, 0x09a, 0x803, 0x0a5, 0xfed, 0x278,    -1, 0x338,
 0x1e5, 0xcad, 0xf9e, 0x73e, 0xb39, 0xe48, 0x754,    -1,
 0x680, 0xd99, 0x4d4, 0x80b, 0x4be, 0xb0d, 0x5f2,    -1,
 0x4b1, 0x38a, 0xff5, 0x000, 0xa1b, 0xece, 0xa06, 0x8e6,
 0xdcb, 0xcb8, 0xc63, 0x98c, 0x346, 0x69c, 0x299, 0xa52,
 0xfff, 0x000,    -1, 0x7b2, 0xbf8, 0x2d1, 0xaff, 0x2f2,
 0xd69, 0xf20,    -1, 0xdcf, 0x9fb, 0x68f, 0x24e, 0xfd7,
 0xfdb, 0x894, 0xc8f, 0x615, 0xa25, 0x36d, 0x1bb, 0x064,
 0xb80, 0x280, 0xd7a,    -1, 0xd75, 0xc90, 0xdce, 0xdce,
 0x011, 0x869, 0xb2f, 0xd24, 0xe26, 0x492, 0xe0a, 0xcae,
    -1, 0x2ac, 0x38c, 0x0b9, 0xc4f,    -1, 0x32b, 0x415,
 0x49c, 0x11c, 0x816, 0xd08, 0xf5c, 0x356, 0x2b3, 0xfbf,
 0x7ff, 0x35d, 0x276, 0x292, 0x4f5, 0x0e2, 0xc68, 0x4c4,
 0x000, 0xb5e, 0xd0b, 0xca7, 0x624, 0x247, 0xf0d, 0x017,
 0x7ec, 0x2a6, 0x62c, 0x192, 0x610, 0xd98, 0x7a4, 0xfa3,
 0x80b, 0x043, 0xd7b, 0x301, 0x69d, 0x7e4, 0x10c, 0xacb,
 0x6eb, 0xea7, 0xe65, 0x75d, 0x4f5, 0x5b0, 0xa50, 0x7b6,
 0x0ec,    -1, 0xcf9, 0x4b4, 0x639, 0x111, 0xbdf, 0xe89,
 0x9fa, 0x76b, 0xdf6, 0x2d0, 0x857, 0x3a3, 0x000, 0xa3e,
 0x8cb, 0x35f, 0x4f0, 0x022, 0xb38, 0xc12, 0x93c, 0x2fc,
 0x546, 0xe6e, 0x91f, 0x145, 0xfff, 0x1af, 0x957, 0xbde,
 0x09d, 0xfd2, 0x9df, 0x2dc, 0x07f, 0x115, 0x7bf, 0xa35,
 0x061, 0x9bf, 0xc85, 0x918, 0x0c8, 0x317, 0xce5, 0xf28,
 0x108, 0x51b, 0x621, 0x188, 0x000, 0x28c, 0xf67, 0x6ef,
 0x000, 0xd72, 0xce2, 0x1be, 0x000, 0x000, 0x282, 0x357,
    -1, 0x4e5, 0x246, 0x859, 0x66c, 0x5d3, 0x9fd, 0x000,
 0x000, 0x82f, 0xc29, 0x331, 0xa93, 0x000, 0xae4, 0x48a,
 0x254, 0x000, 0x0ba, 0xe83, 0x7c7, 0xb6e, 0x88e, 0x774,
 0xf6f, 0x85d, 0x47f, 0xcd6, 0xe41, 0xdb6, 0x000, 0x0f4,
 0xb4d, 0x77f, 0x000, 0x901, 0x1a2, 0x44a, 0x482, 0x000,
 0xe99, 0xa75, 0x000, 0x7ab, 0x000, 0x0b6, 0x35c, 0x306,
 0x11c, 0x08e, 0x6eb, 0x11c, 0x771, 0xff9, 0x1c8, 0x63b,
 0x58b, 0x9d2, 0x250, 0x198, 0xfe7, 0xebc, 0x000, 0xa97,
 0xacc, 0xd4b, 0x28b, 0x892, 0x150, 0xcf4, 0xbc1, 0x000,
 0x662, 0xdd8, 0x61f, 0x903, 0x083, 0x000, 0xc55, 0x02f,
 0xc29, 0x4f5, 0xbcf, 0xe27, 0x9e3, 0xb13, 0xadc, 0x845,
 0x415, 0x0ae, 0x000, 0xe30, 0x931, 0x84a, 0xb09, 0x250,
 0x631, 0x7aa, 0x026, 0xdc9, 0x486, 0x3a7, 0xab0, 0xe04,
 0xe1a, 0xe17, 0x611, 0x556, 0xfac, 0x3c6, 0x5ab, 0x002,
 0xc16, 0xe60,    -1, 0xc51, 0x772, 0x67f, 0xfa9, 0x83c,
 0x974, 0x96a, 0xe94, 0x250, 0xa20, 0xc95, 0x65b, 0x479,
 0xe48, 0xa35, 0x23f, 0x5cf, 0x40a, 0xcf0, 0xe82, 0x1da,
 0x390, 0xc86, 0xa92, 0x433, 0xbed, 0x4a7, 0x09a, 0x15a,
 0xb8d, 0x9c7, 0x5fb, 0x8a0, 0x000, 0xf9a, 0xf3c, 0x11c,
 0x20c, 0xf23, 0x79d, 0xc79, 0xb71, 0x7af, 0xc5b, 0x771,
 0x629, 0x834, 0xb34, 0x20c, 0x940, 0x2ca, 0x60b, 0x000,
 0x4cb, 0x70b, 0x000, 0x000, 0x9e8, 0x000, 0xdca, 0x000,
 0x1ae, 0xb21, 0xfe3, 0x191, 0x9e1, 0x7f6, 0x04f, 0x64a,
 0xba2, 0x59e, 0x1ae, 0x000, 0x728, 0x000, 0x081, 0xecd,
 0x946, 0x000, 0xdee, 0x3ff, 0xdf9, 0x1bf, 0x01a, 0x1a9,
 0xc58, 0xe05, 0x3bf, 0x5e8, 0x39d, 0xbfa, 0x23f, 0xb8d,
    -1, 0x000, 0x779, 0x540, 0xf2c, 0x7cc, 0x340, 0x77a,
 0xa8e, 0xe8d, 0x2fd, 0xfed, 0x5d1, 0x308, 0x00f, 0xf4a,
 0x39b, 0xbe2, 0x0e5,    -1, 0xf4d, 0x1fe, 0xf00, 0x867,
 0x195, 0x2de, 0x712, 0x000, 0x00c, 0x0a3, 0x1f3, 0x4ee,
 0x317, 0x665, 0x000, 0x5d8, 0x291, 0x6c4, 0xa46, 0x492,
 0x8d4, 0x647, 0x57f, 0x000, 0x259, 0xd87, 0x5c2, 0x1d8,
 0xfad,    -1,    -1, 0x79f, 0x43a, 0xfd1, 0x164, 0x6e1,
 0x350, 0xf00, 0x0e9, 0xac4, 0xe35, 0x307, 0xfff, 0xabb,
 0xc1a, 0x768, 0x000, 0x372, 0x839, 0xf4b, 0x1c3, 0xab0,
 0xcb6, 0x943, 0xbe9, 0x20f, 0xddc, 0xe18, 0x4eb, 0x21d,
 0x530, 0x24c, 0x000, 0xf79,    -1, 0x1bd,    -1, 0x155,
 0x435,    -1, 0x132, 0x5c2, 0xb3d, 0x802, 0x733,    -1,
 0x336, 0xf19, 0xfea, 0xd2a, 0x07f, 0x8e9, 0x000, 0xdab,
    -1, 0x088, 0x4b1, 0x7ac, 0x000, 0xe66, 0xde0, 0x73c,
 0xfff, 0x02f,    -1, 0x000,    -1, 0x000, 0x562, 0x389,
 0xb20, 0x9ea,    -1, 0x3f8, 0x567, 0x035, 0xa55, 0x255,
 0xc98, 0x65f,    -1, 0x1ac, 0x571, 0x13d, 0xf57, 0x32a,
 0xbdb, 0x0ec, 0x47d, 0x43a,    -1, 0x1aa, 0x9d6, 0x843,
    -1, 0x244, 0xb03, 0xd0d, 0x579, 0x1b1, 0xea7, 0x000,
 0x062,    -1, 0x533, 0x1db, 0xf1f, 0x2f7, 0x2df, 0x3e5,
 0xdec, 0xc5c, 0x55a, 0xf6c, 0x4c1, 0x5a8, 0xcd4, 0x6fd,
 0x1a6, 0x4b8, 0x98a, 0xe17, 0xeb9, 0xfd1,    -1, 0x175,
 0x4d6, 0xba2, 0x000, 0x614, 0x147, 0x429, 0xfee,    -1,
 0x0d8,    -1, 0x98a, 0xdd2, 0xedd, 0x255, 0xef3, 0x345,
 0x000, 0xf3e,    -1,    -1, 0x210, 0x88a, 0x699,    -1,
 0x02c, 0xfee, 0x1c1, 0xb38, 0x000, 0x7cc, 0x165, 0x536,
    -1, 0x1ae, 0xefb, 0x734,    -1, 0x1a4, 0x984, 0x804,
 0x487,    -1,    -1, 0x31e, 0x9f2, 0x966, 0x000, 0xcb0,
 0x552, 0x0c9,    -1, 0x750, 0x650, 0x064, 0xffe, 0xe84,
 0x537, 0xee7, 0x834,    -1, 0x998, 0xa03,    -1, 0xcdf,
 0x4be, 0x310, 0x051, 0xf3f, 0x040, 0x973, 0x925, 0x000,
 0x000, 0xe51, 0x8b1, 0x468, 0xe11, 0xd4f, 0x374, 0x33a,
 0x126, 0x88b, 0x43a, 0xc9b, 0xdb9, 0x3c2, 0x3bd, 0x1ae,
 0x000, 0xc4a, 0x000, 0x4c4, 0x859, 0xe5a, 0x000, 0xeb4,
 0xd40, 0x87d, 0xc79, 0xe13, 0x50b,    -1, 0x724, 0x000,
 0x7be, 0x062, 0xe7f, 0xad0, 0x5f3, 0x69e, 0x381, 0x272,
 0x50f, 0xac8, 0x053, 0x55e, 0xf19, 0xd71, 0x75b, 0xbf2,
 0x000, 0x3ac, 0xdf0, 0xd75, 0x7e3, 0xe75, 0xa13, 0xfd8,
 0xbdc, 0x1d9, 0x15f, 0x8cc, 0xba4, 0xb79, 0xb7f, 0x812,
 0xfe6, 0x000, 0x2d3, 0xd7b, 0x5d4, 0xad2, 0x316, 0x908,
 0x323, 0x758, 0xb0b, 0x965, 0x1a9, 0xdce, 0x660, 0x625,
 0xeff, 0x0ed, 0x000, 0x323, 0x986, 0x831, 0x5c5, 0x22f,
 0xd49, 0xec6, 0x90e, 0x234, 0x000, 0x80f, 0x16c, 0x528,
 0x1f8, 0x2bd, 0x97d, 0xe20, 0xf29, 0x97d, 0x3a0, 0x7fc,
 0x086, 0x720, 0x1f9, 0x3eb, 0xf67, 0x423, 0xa55, 0x69e,
 0xede, 0x206, 0x7fa, 0x809, 0xfa8, 0xe22, 0x15e, 0x2a0,
 0x04a, 0xf7b, 0x4ea, 0xd9a,    -1, 0x1d8, 0x0b4, 0xb87,
 0x406,    -1, 0xcdf, 0x187, 0xf6d, 0x914, 0x4b1, 0x000,
 0x104, 0x67e, 0xc74, 0x6da, 0xe67, 0x7d2, 0xd1f, 0x64c,
 0x19d, 0x000, 0xa17, 0xfd5, 0x000, 0x8ad, 0xf38, 0xd65,
 0xabd, 0x75e, 0x667, 0x632, 0x346, 0xc48, 0xa77, 0x45e,
 0x2b5, 0xded, 0x7da, 0x160, 0x560,    -1, 0xf4e, 0xb0c,
 0xdb0, 0x287, 0x34a, 0x065, 0x439, 0x2ec, 0x679, 0xefa,
 0x208, 0xeb1, 0x1b0, 0x8c8, 0xca6, 0x62c, 0xa10, 0x673,
 0x000, 0x000, 0xc6a, 0x7b2, 0xbd7, 0xb2b, 0x17a, 0x6f3,
 0x1ab, 0xffa, 0x5e0, 0x1fa, 0xb8f, 0xe5c, 0xcab, 0xdbc,
 0x10f, 0x000, 0x000, 0xefe, 0x34b, 0x1d9, 0x834, 0x52f,
 0xb58, 0x82b, 0x6e8, 0x1f3, 0x719, 0x64e, 0xf55, 0xccd,
 0x531, 0x0de, 0x3aa, 0x150, 0x89a, 0x3b9, 0x26e, 0xebc,
 0x7ae, 0x670, 0x315, 0x8a9, 0x03b, 0x896, 0x247, 0x2f4,
 0x450, 0xd10, 0xb79, 0x0ed, 0x041,    -1, 0x707, 0x9e1,
 0xed6, 0x6d2, 0x000, 0xfff, 0xb1a, 0x084, 0xaf3, 0x47f,
 0x02f, 0xac3, 0x751, 0x8c4, 0x291, 0xadd, 0x000, 0xea1,
 0x8ec, 0xf9f, 0x5c2, 0x000, 0xd6b, 0x71e, 0x000, 0xcea,
 0x971, 0x5f8, 0x4b9, 0x7c6, 0xb7e, 0x353, 0xd25, 0x423,
 0x6ec, 0xb71, 0xf93, 0x000, 0x795, 0xc43, 0xaa2, 0x96a,
 0xcbd, 0xb55, 0x184, 0xdf0, 0x3d9, 0xbfe, 0xf79, 0x8f0,
 0x22c, 0xeeb, 0x000, 0xa4b, 0xe07, 0xf34, 0xc9d, 0x4be,
 0x95b, 0x371, 0x78c, 0x9e9, 0xde6, 0x072, 0xf0d, 0x60b,
 0x5a5, 0xab1, 0x000, 0x260, 0x000, 0xd2a, 0xd90, 0x154,
 0x4c6, 0x438, 0x5d9, 0x736, 0x062, 0x000, 0x000, 0xb84,
 0x72e, 0x0b7, 0x000, 0x050, 0x063, 0xa95, 0x89b, 0x917,
 0x049, 0xb14, 0x9a0, 0x734, 0x0c3, 0xd50, 0x917, 0xb02,
 0x8cf, 0x453, 0x0af, 0x8e5, 0x000, 0x7aa, 0x5d5, 0x81b,
 0x788, 0xb9c, 0x01a, 0x974, 0x000, 0x000, 0x37f, 0xd9f,
 0x000, 0xec4, 0x4f4, 0xbff, 0x4fe, 0x860, 0x11c, 0x74e,
 0x34a, 0x281, 0x52f, 0xb05, 0xa89, 0xbee, 0x6ad, 0x9fc,
 0x9ba, 0xb0b, 0x515, 0x1c7, 0x330, 0xfde, 0x97e, 0x6e7,
 0xc45,    -1, 0x658, 0x710, 0x28a, 0x921, 0x1de, 0x4a1,
 0x9d7, 0xe32, 0xa2d, 0xb0f, 0x545, 0xd6f, 0x329, 0x9b8,
 0xb4d, 0x9a0, 0x938, 0x783, 0xfa7, 0xd0a, 0xdc9, 0x0fe,
 0x000, 0x249, 0x000, 0x8cd, 0x922, 0x7cd, 0x021, 0xa89,
 0x3d5, 0xcee, 0x0a1, 0x6d6, 0x000,    -1, 0x48b, 0x000,
 0x87a, 0x8bb, 0x9ed, 0x01f, 0xe20, 0xb7f,    -1, 0xe95,
 0x593, 0x1da, 0x57a,    -1, 0xf3a, 0x000, 0x000,    -1,
    -1, 0x160, 0x501, 0x7a3, 0xb59,    -1,    -1, 0xc7f,
    -1, 0xf79,    -1,    -1, 0x48d, 0x781,    -1,    -1,
 0xb74,    -1, 0x3c4, 0xbe9,    -1,    -1, 0x9a4, 0x9ae,
 0xa75,    -1,    -1, 0x9cd, 0x000,    -1,    -1,    -1,
 0xc3c, 0x2d4,    -1, 0x173, 0xf38, 0x000,    -1, 0xee9,
    -1, 0xb91, 0xcc1, 0x86d, 0x8ab, 0xeb0, 0xec7, 0x687,
 0xd98, 0xa95, 0x744, 0xe7c, 0x826, 0x80e, 0x599, 0x3d9,
 0xf2f,    -1, 0x96a, 0xfd1, 0x174,    -1, 0x000, 0x1aa,
 0x50e,    -1, 0x5a2, 0xbcd, 0x000,    -1, 0x019, 0x588,
 0x18d, 0x470, 0x812, 0xeec, 0xf63, 0x05c,    -1, 0x000,
 0xb7f, 0x357, 0x436, 0xbb4, 0x1fb, 0x425, 0x1ed, 0xe13,
 0x66c, 0x555, 0xb11, 0x7b5, 0x48d, 0x38d, 0xf72, 0x000,
 0x000, 0xa66, 0x4fa, 0xf36, 0x1eb, 0x000, 0x95f, 0x000,
 0xd9a, 0x82f, 0x07f, 0x253, 0x70f, 0x915,    -1, 0x12d,
 0x040, 0x2ca, 0x446, 0x90a, 0x7a8, 0x687, 0x000, 0x04e,
 0x74f, 0x1ca, 0x793, 0x3c7, 0x3f0, 0x4c7, 0x000, 0xc30,
 0x533, 0x889, 0x9ef, 0xebd, 0x984, 0x18f, 0xfe1, 0x8ea,
 0x185, 0x410, 0x107, 0x000, 0x73e, 0xd4b, 0x8fc, 0xd34,
 0x1e6, 0x4bf, 0xbac, 0x7c3, 0x000, 0x7c8, 0xb2f, 0x02c,
 0xa46, 0x000, 0x0f9, 0x680, 0x94d, 0x6ad, 0x767, 0xfeb,
 0x6c7, 0x2d5, 0x43f, 0x9af, 0x261, 0xe83, 0xfa7, 0xb7b,
 0xf2d, 0x2f5, 0x4d7, 0x494, 0xbc2, 0x45b, 0x000, 0x17d,
 0x5c6, 0xe2b, 0xb20, 0x19e, 0x6ba, 0x973, 0xedd, 0xea8,
 0x000, 0x9f3, 0xd9a, 0x7fa, 0xb78, 0x556, 0xbb6, 0xc58,
 0x210, 0x000, 0xf9a, 0x56d, 0x48b, 0xf12, 0x000, 0x54d,
 0x5f4, 0x1ad, 0x86e, 0xe16, 0x6ff, 0xa35, 0x47e, 0x4c7,
 0x93c,    -1,    -1, 0xc98, 0xd3f, 0x000, 0x788, 0x6ef,
 0x959, 0xec2, 0x45e, 0xa4d, 0xa90, 0x000, 0x768, 0x8bb,
 0x6ee, 0x7f5, 0x770, 0xfa8, 0xba4, 0xf49, 0x7b8, 0x616,
 0x2bd, 0x23f, 0xe8c, 0x9fa, 0xa49, 0x213, 0x98a, 0x2c1,
 0x595, 0x885, 0x6de, 0x057, 0x1bc, 0x000, 0xc58, 0x7a8,
 0x5c1, 0x3d0, 0xa78, 0xb80, 0x000, 0xc06,    -1, 0x428,
 0xe92, 0xfa3, 0x341,    -1, 0x000, 0x000, 0x1ca, 0x27c,
 0xdeb, 0x835, 0x4c8, 0xdb3, 0x000, 0xf9d, 0x000, 0xe81,
 0xc22, 0xfce,    -1, 0xe6e, 0x96e, 0x161,    -1, 0x3b9,
 0x945, 0xa95, 0x13d, 0x748, 0x184, 0x588, 0x636, 0xf7e,
 0xb44, 0x2b7, 0x217, 0xee5, 0x65a, 0xc47,    -1, 0xca3,
 0x83e, 0x431, 0xc64, 0x636, 0x06e, 0x404, 0x993,    -1,
 0xeb3, 0x134, 0x8a3, 0xca9,    -1,    -1, 0x2ab, 0x000,
 0x8ed, 0x877, 0x1a8, 0xc89, 0x000, 0x000, 0xf94, 0x000,
 0x709, 0x249, 0x9ac, 0x22a, 0x605, 0x000, 0x000, 0x6b4,
 0x00c, 0xc53, 0xf23, 0x005, 0x29f, 0x865, 0xf79, 0x000,
 0x5fa, 0x764, 0xe51, 0xbdc, 0xb64, 0x0f3, 0xf29, 0x2f7,
 0x5da, 0x000, 0x16f, 0xb8b, 0x255, 0x9cc, 0xe43, 0x279,
 0x2c2, 0x483,    -1, 0xf7d, 0x7bb, 0x000, 0x9e3, 0xd84,
 0xe36, 0x6e6, 0x000,    -1, 0x33f, 0x41d, 0x5b5, 0x83e,
 0x2f4, 0xf5b, 0x9fc, 0xb1e,    -1, 0x8f4, 0xb26, 0x856,
 0x3b6, 0x126, 0x4c2, 0x274, 0x0c1, 0xfa9, 0x57d, 0x000,
 0x100, 0x7af, 0xc62, 0x000, 0xa55, 0x416, 0x93f, 0x78c,
 0xfba, 0x5a2, 0x0c2, 0x4d4, 0xa3e, 0xcc3, 0xe73, 0xd02,
 0x8df, 0x3e9, 0xe9a, 0x0f6, 0x32c, 0x23d, 0xdab, 0xf50,
 0xfc2, 0x000, 0x065, 0xc23, 0xd3d, 0xc84, 0x35e, 0x000,
 0xa24, 0x634, 0x4b4, 0xa52, 0x098, 0xb39, 0x9a4, 0xe71,
 0x8aa, 0x741, 0x000, 0xb16, 0x5c2, 0xea1, 0xc01, 0x5c1,
 0x30d, 0xca4, 0x201, 0xc9c, 0x717, 0x000, 0xba0, 0x537,
 0x619, 0x000, 0xfd9, 0x6dc, 0xdaa, 0x1da, 0xe51, 0xd39,
 0xb4c, 0x8a1, 0x098, 0x2f8, 0x191, 0x9dc, 0xdb0, 0x5e1,
 0x000, 0xe97, 0xef1, 0x8d3, 0xb0d, 0xfce, 0x336, 0xee1,
 0x7a2, 0xbc8, 0x494, 0x580, 0xba7, 0x000, 0x62a, 0x96a,
 0x527, 0x859, 0x811, 0xef0, 0x429, 0xef4, 0xf3d, 0x000,
 0x9d6, 0xb71, 0x000, 0x14b, 0xf3d, 0xb16, 0x204, 0x0c1,
 0xcd4, 0x339, 0x39d, 0xfe3, 0x837, 0x8c7, 0x955, 0x69a,
 0x5f6, 0x4c6,    -1, 0x3d5, 0x000, 0x0e7, 0x4b1,    -1,
 0xa3e, 0xb03, 0x1ea, 0xac8,    -1, 0x000, 0xed8,    -1,
 0x4e0, 0x9f7, 0xc91, 0x6b3,    -1,    -1, 0xa53, 0x290,
 0xa64, 0x0e3, 0x3dc, 0xed3, 0xf2f, 0x000, 0xd7c, 0xf44,
    -1, 0x205, 0x900, 0x864,    -1,    -1, 0xed3, 0x7d2,
 0x000,    -1, 0xdd2, 0x79b, 0x000,    -1, 0xae6, 0x5cf,
 0xde8, 0x000, 0x1f2,    -1, 0x2f3, 0x000,    -1, 0x2ce,
 0xcf2, 0x8f4, 0xee8, 0x165, 0x309, 0x15f,    -1, 0x714,
 0xbfc, 0x532, 0xad0, 0x151, 0x2d5, 0x0a4, 0x391,    -1,
 0x0dc, 0x0c1, 0x451,    -1,    -1, 0x6a0, 0x250,    -1,
 0xab8, 0x977, 0xa86, 0x407, 0x72f,    -1, 0x05f, 0x000,
 0xefe, 0x950, 0x4f4, 0x957,    -1, 0xd68, 0x26c, 0xa30,
 0x4f1, 0x279, 0x584, 0xb34,    -1, 0x4d7, 0x258, 0x000,
 0x518, 0x685, 0x91c, 0x3ac, 0x0fa,    -1, 0x979, 0x40c,
 0x506, 0x000,    -1, 0x7bd, 0xb97, 0x87f, 0xc06, 0x050,
 0x7bf, 0xe3e, 0xc81, 0x000, 0x65e, 0x000,    -1, 0xb76,
 0xc37, 0x4c4, 0xfc9, 0x336, 0x9fa, 0xaa2, 0x32c, 0xb8b,
 0xaa9, 0xc95, 0x85a, 0xa9a, 0x260, 0x4cd, 0x8fe, 0xd3c,
 0x982, 0x0d7, 0xbc1, 0xdcf, 0xe62, 0xe0d, 0xf8f, 0xd7b,
 0x91a, 0x3e0, 0x33a, 0x1c5, 0xf00, 0xde5, 0xad1, 0xebc,
 0xebc, 0x942, 0xd86, 0x3bf, 0x8ce, 0xb8c, 0x000, 0x8d6,
 0x784, 0xb74,    -1, 0x818, 0x000, 0xfff, 0x07e, 0x029,
 0xf48, 0xb65, 0xd81, 0x220, 0x095, 0x21f, 0xac4, 0xb31,
    -1, 0x864, 0x000, 0x3bd, 0xf85, 0x237, 0x369, 0x2d9,
 0xfdf, 0x25a, 0x782, 0x7b8, 0xabd, 0x5e3, 0x438, 0x230,
 0xbc4, 0x7ad, 0x00a, 0x441, 0x6dc, 0x2c4, 0xf16, 0x0b3,
 0x04c, 0xfd2, 0x8aa, 0xad8, 0x3e4, 0x142, 0x585, 0xc8f,
 0x9bf, 0x29b, 0xac9, 0x743, 0xfb5, 0x7fc, 0x05e, 0xd38,
 0x002,    -1, 0xb4e, 0xd0c, 0x84c, 0xf93, 0x91f, 0xcd2,
 0x04f, 0x569, 0xd1b, 0xfc6, 0x630, 0x6f6, 0x1d8, 0x91a,
 0x4da, 0x9f5, 0x07a, 0xcf5, 0x634, 0x42f, 0xfff, 0x951,
 0x0f9, 0xc01, 0x491, 0xbd6, 0x730, 0xfea, 0x9f4, 0xbfc,
 0xf1a, 0x413, 0xa2a, 0xdc6, 0xc87, 0x9db, 0xc2c, 0x30f,
 0xdb5, 0x785, 0xbaa, 0x000, 0x000, 0xa49, 0x000, 0x61d,
 0xf6f,    -1, 0x031,    -1, 0x441, 0x7bf, 0x53e,    -1,
 0x6fd, 0x0f6,    -1, 0xadb,    -1, 0x000, 0x432, 0x187,
 0xd37, 0x154, 0x539, 0xc08, 0xe51, 0x219, 0x1e9, 0x897,
 0xa0e, 0x201, 0x447, 0x89f, 0x000, 0x463, 0x726, 0xa05,
 0xab9, 0xd01, 0x1e4, 0xfea, 0x895, 0x816, 0x313, 0xae3,
 0x3a4,    -1, 0x70f,    -1, 0xa42, 0x5e9, 0x78e,    -1,
 0x317, 0x6c8, 0x000, 0xbf7, 0xefd,    -1, 0xb17, 0x382,
 0xd26, 0x5ff, 0xf81, 0x20b, 0x373, 0x774, 0x081, 0xaae,
 0xfdb, 0xe5d,    -1,    -1, 0xcb7, 0x738, 0x919, 0x933,
 0x398, 0x000, 0x14e,    -1, 0xe14, 0xbf8, 0x11c, 0x94b,
 0x031,    -1, 0x000, 0x2d4, 0xd41, 0xdc6, 0x9f6, 0xea7,
 0x9e8, 0x2ec, 0x10a, 0x50d, 0xeae, 0xdb0, 0xef0, 0x9c8,
 0x000,    -1, 0x82e, 0x9d3, 0xdb7, 0x46d,    -1, 0x230,
 0x73b, 0x45b,    -1,    -1, 0x000, 0x4a7,    -1,    -1,
 0x47c, 0x10e, 0x4b4,    -1,    -1,    -1, 0x1d7, 0xa5d,
 0x233, 0x6b2, 0x6bd, 0x387, 0x7ca, 0xb1a, 0xf75, 0xea4,
 0xdc9, 0x98b, 0x80c, 0x702, 0xe22, 0xa6e, 0x6f8, 0x05b,
 0x17c,    -1, 0x000, 0xebe, 0xc8e, 0xaec,    -1, 0x42b,
 0xdce,    -1,    -1,    -1, 0xef3, 0xc52, 0x31b,    -1,
 0xdff, 0xbd0, 0x000, 0xa72, 0x525, 0x9cf, 0x2ff, 0xfc8,
 0x37c, 0xbce, 0xd8c, 0xd88, 0x3a6, 0xed8, 0x4ab, 0x000,
 0x449, 0x9d7,    -1,    -1, 0x9be, 0x59f, 0x000, 0x882,
    -1, 0x742, 0x000,    -1,    -1,    -1, 0xe8b, 0x0f3,
 0x771,    -1, 0x3ea, 0x8f9, 0xcbb, 0x548, 0x46d, 0x000,
    -1, 0xf74, 0xa23, 0x15b,    -1, 0xaeb, 0x7f8, 0xbe2,
 0x000,    -1, 0x023, 0x61e, 0x95d, 0x7ac, 0x024, 0x141,
 0x561, 0x9fe, 0xb10,    -1, 0x623, 0xc47, 0x413, 0x0e7,
 0x663, 0xcdf, 0xebe, 0x5c9, 0x573, 0x21d, 0xb28, 0x280,
 0xb9f, 0xd1a, 0xecf, 0xff0, 0x000, 0xfc0, 0x085, 0x9c4,
 0x48c, 0x000, 0xb0b, 0x43d,    -1, 0x73b, 0x802, 0x633,
 0x6ef,    -1,    -1, 0x5c1, 0xea6, 0x0a9, 0xab4, 0xacd,
 0xb81, 0xa32,    -1,    -1, 0xa26, 0x9d5, 0xf7c,    -1,
 0xf69, 0xdbb, 0x6d5, 0x405,    -1, 0xd0a, 0xfe0, 0xf5e,
 0xbd7,    -1, 0x89a, 0x868, 0xeb2, 0x792, 0x7fe, 0x115,
 0x000, 0x8bb, 0xdd1, 0xc40, 0x453, 0xbb3, 0x7cc, 0x3e6,
 0x071, 0x0f1, 0xbae, 0xf67, 0x896, 0x38e, 0x86e, 0xfaa,
 0xccc,    -1, 0x411, 0x8e5, 0x699, 0x2ef, 0x785, 0x9d4,
 0xe30, 0xb2e, 0x976, 0x203, 0x035, 0x75d, 0x8f1, 0x144,
 0x092, 0x1a5,    -1, 0x55f, 0x000, 0xa43, 0x5be, 0x68d,
 0x852, 0xb87, 0x9af, 0x0c0,    -1, 0xa50, 0x9ca, 0x15f,
 0xf06, 0x869, 0x0f3,    -1, 0x000,    -1, 0x9a9,    -1,
    -1,    -1,    -1, 0xf05, 0x000,    -1, 0x000, 0x4a9,
 0xf9d,    -1, 0x000, 0xab1, 0x04c,    -1, 0xd17, 0x893,
 0x763, 0x332,    -1, 0xc41, 0x5bd, 0xa72, 0x67c, 0xb78,
 0x973, 0x6c7, 0x569,    -1, 0x96a, 0xc68, 0x48c,    -1,
 0x6fa,    -1, 0xa2a, 0x44f,    -1, 0x73f, 0x28f, 0x536,
 0xd91, 0xc86, 0xef8, 0x1f5, 0xfb4, 0x060, 0x230, 0xe10,
    -1, 0x000, 0x305, 0x0e6, 0xb19, 0x1e2, 0x7fc, 0xf35,
    -1, 0x7d9, 0x000, 0x000, 0xd2f, 0xb3a, 0x0a2, 0x7c9,
 0xda6, 0x37c, 0xe43,    -1, 0x7da, 0x0d6, 0x000,    -1,
 0xd40,    -1, 0x156, 0xee9,    -1, 0x239, 0x10f, 0x60c,
 0x9d4, 0x663, 0x565, 0x603, 0x38b,    -1, 0x606, 0x13c,
 0x681, 0x436, 0xc29, 0x9c7, 0x1d9, 0x000, 0x0a6, 0x996,
 0x231, 0x055, 0x01f, 0x0a3, 0xd96, 0x7c8, 0x0f3, 0xaa7,
 0xd99,    -1, 0x3be, 0x476, 0x25f, 0x38c, 0xdf3, 0x6d5,
 0xcb5, 0xadd, 0x000, 0x136, 0x64d, 0xc0d, 0xe61, 0xd0b,
    -1, 0x000, 0x535, 0x9c3, 0x279, 0x00c, 0xa87, 0xa31,
 0xc4a, 0x167, 0x423, 0xec8,    -1, 0x926, 0xa4d, 0x5ba,
    -1,    -1, 0x9bf, 0x000, 0x47f, 0x8f3, 0xd5b, 0xc3b,
 0xa18,    -1, 0x548, 0x8f7, 0x8cf, 0x000, 0x9bd, 0xaa2,
 0x7ec, 0x000, 0xfb8, 0xafd, 0xe68,    -1, 0xfa7, 0x31c,
 0xef3, 0x288, 0xdf0, 0x1bc, 0xfe9, 0x1ea, 0xa9f, 0x000,
 0x53f, 0x000, 0xda6, 0x09c, 0x1bf, 0x09c, 0x31c, 0x0c8,
 0x31c,    -1, 0x689, 0x211,    -1, 0x77f, 0x723, 0xb8f,
 0x683, 0x351,    -1, 0xb33, 0xce0,    -1, 0x61c, 0x073,
 0x783, 0x6b2, 0x6a8, 0x729, 0x81b, 0xabf, 0xd15, 0x563,
 0x433,    -1, 0x823, 0xf99, 0x2c5,    -1, 0x367, 0xcc4,
 0x934, 0x6f2, 0xdf0, 0xa1f, 0x579, 0x012,    -1, 0x508,
 0x070,    -1, 0x018, 0x270, 0xa6f,    -1, 0x7a7,    -1,
 0x826, 0x4b5,    -1, 0x7d8, 0xb20,    -1, 0xc28, 0x463,
    -1, 0xdf9, 0x22c, 0xe17, 0x4f2, 0xe13, 0x4ff, 0x40a,
 0xdcb, 0x9ed, 0x34a, 0xeb8, 0xa0e, 0x5f2, 0x594, 0x60d,
 0x4b6, 0xd3c, 0x675, 0x1c4, 0xbb5, 0xc73, 0xfad, 0xead,
    -1, 0xfb6,    -1, 0x146, 0xd40, 0x02f, 0x000, 0x302,
    -1,    -1, 0x6e5, 0x000, 0xed7, 0xd8c, 0x7a3, 0x0fc,
 0x259, 0x34b, 0xa1b, 0x882,    -1, 0x211, 0x000, 0xd30,
 0xe02, 0x5cd, 0x53e, 0x11b, 0xa16,    -1, 0x24e,    -1,
 0xace, 0xe9a,    -1, 0x5c6, 0x9be, 0x000, 0x169, 0x982,
    -1, 0x3fd, 0x457, 0x06f, 0x7e7, 0xed1, 0x5ee, 0xcef,
 0x62b, 0x26c, 0xc9f, 0xe68, 0x59f, 0x0b5, 0x000, 0x0bc,
 0x086, 0x890, 0x005, 0xc42, 0x939, 0xaca, 0xdd9,    -1,
    -1, 0x6e5, 0x0dd, 0x434, 0x297, 0xe21, 0x0f5,    -1,
 0xa6c, 0x4ad, 0x978, 0x433, 0xa41, 0xd6f, 0x8bf, 0xfb8,
    -1, 0x928, 0x85e, 0xfb6, 0x5c7, 0x99a, 0x8ec, 0xebc,
 0x226, 0x7d4, 0xdcd, 0xc0b, 0x000, 0x7f4, 0xc6f, 0x000,
 0x3ad, 0x5b2,    -1, 0x67b,    -1, 0x568, 0x6e2,    -1,
    -1,    -1, 0x3f3, 0xaf5, 0x33f,    -1, 0x022, 0x1bd,
 0xae5,    -1, 0x9c3, 0x000, 0x92b, 0xee5, 0x29c, 0x000,
 0x15e, 0xe71, 0xacb, 0x9d2, 0x1a3, 0xb7f, 0xa5b, 0x095,
    -1, 0xb6e, 0x79f, 0x3d1, 0x7d0, 0x131, 0xcd7,    -1,
 0x2c3,    -1, 0x396, 0x4d2, 0x297, 0x405, 0x634,    -1,
    -1,    -1, 0x928, 0xbca, 0xb6c, 0x011, 0xfc0, 0xbaf,
 0xbd2,    -1, 0x585, 0x000, 0xb8a, 0x7f9, 0xd6b, 0x4eb,
 0x9a3, 0x000,    -1, 0xaeb, 0xa47, 0xcef, 0x9c6,    -1,
 0x000,    -1, 0x2a9, 0x371, 0xca6,    -1, 0xb7d, 0x15f,
 0x2a9,    -1, 0xe80, 0x7a7, 0x23a, 0x000, 0x000, 0xcc9,
 0x60e,    -1, 0x130, 0x9cd, 0x498, 0xe25, 0x366, 0x34b,
 0x899, 0xe49, 0x1a8, 0xc93, 0x94d, 0x05e,    -1, 0x0c2,
 0x757, 0xb9d, 0xaa3, 0x086, 0x395, 0x3c3, 0xa2e, 0xf77,
 0xcb1, 0x45e, 0x169, 0xbba, 0x367, 0x8cb, 0x260, 0x5a0,
 0x8cb, 0x737, 0xa1f, 0xaaf, 0xf92, 0x430, 0x97d, 0x542,
 0xb09,    -1, 0x774, 0x084, 0x4c0, 0x2b3, 0xaf6, 0x93c,
 0x32d, 0xee2,    -1, 0x605, 0xece, 0x8eb, 0xc62, 0x01d,
 0x000, 0xaba, 0xfc5, 0xb8e, 0xc07, 0xfb6, 0xbca, 0x8f0,
 0xd33,    -1, 0x283, 0x6d6, 0x6ad, 0x678, 0x51a, 0xc95,
 0xda4, 0x962, 0x9ed, 0x307, 0x94a, 0x052, 0xf4e, 0x3bd,
 0x210, 0x71a, 0x3c7, 0x5a4, 0x6e7, 0x23a, 0x523, 0x1dc,
 0x6b2, 0x5e0, 0xbb0, 0xae4, 0xdb1, 0xd40, 0xc0d, 0x59e,
 0x21b, 0x4e6, 0x8be, 0x3b1, 0xc71, 0x5e4, 0x4aa, 0xaf3,
 0xa27, 0x43c, 0x9ea, 0x2ee, 0x6b2, 0xd51, 0x59d, 0xd3a,
 0xd43, 0x59d, 0x405, 0x2d4, 0x05b, 0x1b9, 0x68b, 0xbfa,
 0xbb9, 0x77a, 0xf91, 0xfcb,    -1, 0x949, 0x177, 0x68d,
 0xcc3, 0xcf2, 0x000, 0xa87, 0x2e6, 0xd2f, 0x111, 0x168,
 0x94c, 0x54c, 0x000, 0x0c5, 0x829, 0xcbc, 0xc0b, 0x1ed,
 0x836, 0x9d8, 0xbdc, 0xc5e, 0x4e5, 0xb94, 0x6f2, 0x74f,
 0x878, 0x3b2, 0x48d, 0xc72, 0xcff, 0xccb, 0x8f9, 0x7ee,
 0x869, 0x228, 0x035, 0x81e, 0xcf9, 0x309, 0xdf2,    -1,
 0x047, 0xdd3, 0xcab, 0x11d, 0xe76, 0xb52, 0xbbd, 0x12d,
 0xf37, 0x552, 0x61d, 0xdd8, 0x2cd, 0x298, 0x9e2, 0xf2c,
 0x9f7, 0xf41, 0xcb4, 0x277, 0xfde, 0xe7e, 0x82a, 0x86b,
 0x9cc, 0x580, 0xfcc, 0x33a, 0x438, 0xd6e, 0x000, 0xc04,
 0xd50, 0x681, 0x1b3, 0x322, 0x86c, 0x4a6, 0x000, 0xa17,
 0xd53, 0xdc0, 0xb61, 0x323, 0x3d1, 0x3fb, 0x929, 0xa6e,
 0x919, 0xae0, 0x283, 0xe6a, 0xed3, 0x371, 0xd51, 0x309,
 0x510, 0xd50, 0x6f4, 0xc84, 0x566, 0xba7, 0x75b, 0xbeb,
 0x793, 0x58f, 0x974, 0xe77, 0x52c, 0xcef, 0x942, 0xa7c,
 0x56a, 0xaa0, 0x784, 0x0ec, 0xad3, 0xccf, 0xecf, 0xc3f,
 0x846, 0x1b2, 0x112, 0x4ee, 0x18b, 0xa76, 0xe14,    -1,
 0xfb1, 0xb4c,    -1, 0xd54, 0x0e0, 0x72d, 0xdf0, 0xf0c,
 0x881, 0xc60, 0xe1b, 0x000, 0x453, 0xe21, 0xb2a, 0x6df,
 0x84f, 0x000, 0x12a, 0x991, 0x587, 0xa4e, 0x522, 0x000,
 0xe17, 0xc64, 0x994, 0x4cc, 0x0e5, 0xc2b, 0x8cf, 0x458,
 0x992, 0xec0, 0xc80, 0xefb, 0x7b7, 0x863, 0xc0a, 0x250,
 0x338, 0xa44, 0x8ab, 0x873, 0x134, 0x23c, 0x4f6, 0x066,
 0xd0f, 0xdd6, 0x93d, 0xf20, 0x000, 0x9bb, 0x255, 0xe7b,
 0x916, 0x4f3, 0x68e, 0xb82, 0x2b4, 0x9d7, 0xfd1, 0x0fb,
 0x11e, 0x204, 0x241, 0x67f, 0x1c4, 0xe91, 0xf41, 0xb4a,
    -1, 0x6d2, 0xce6, 0xdfb,    -1, 0xdd0, 0xb8d, 0x8db,
 0x86c, 0x224, 0xdeb, 0x7bb, 0x50e, 0x000, 0xb79, 0x11e,
 0x486, 0xd4c, 0x000, 0xa54, 0x946, 0x83a, 0x537, 0x875,
 0x000, 0x8e4, 0x95a, 0xdd5, 0x9d5, 0x910, 0x95b, 0x8c8,
 0xd22, 0x07c, 0xac0, 0x000, 0x048, 0x170, 0x9b2, 0xcea,
 0xb0f, 0x958, 0x0f9, 0xa9e, 0x87a, 0x166, 0x69c, 0x112,
 0xde0, 0x487, 0xeca, 0x639, 0x4ee, 0x7fa, 0x2cc, 0x709,
 0x87a, 0x5bb, 0xf64, 0x173, 0xdc6, 0xaaf, 0xbff, 0xf2a,
 0x8fb, 0xd44, 0x0ca, 0xf34, 0xb3a, 0xeb3, 0xfc5, 0x61d,
 0x92f, 0x6fb, 0x1a1, 0xd85, 0x8fe, 0xb6a, 0x0a1, 0x44f,
 0x89a, 0xc5d, 0x13b, 0x5cc, 0x000, 0x9ac, 0x9e6, 0xf95,
 0x511, 0xf2e, 0xf3c, 0x707, 0xec5, 0xaec, 0xc72, 0xeb1,
 0x105, 0xda3, 0xbcb, 0x1d6, 0xf16, 0xd50, 0x306, 0x03f,
 0xe6a, 0x25c, 0x9fe, 0xd3f, 0x8a4, 0x7bc, 0x0bc, 0x532,
 0x62e, 0x111, 0x797, 0xc2c, 0x9d0, 0x338, 0xbc7, 0xd64,
 0xb09, 0x4d6, 0xcba, 0x62c, 0xef2, 0x32b, 0x9c5, 0xc06,
 0x38b, 0xbb2, 0x8b7, 0x6f2, 0x7ec, 0xa77,    -1, 0x7a3,
 0xc95, 0xa91, 0x5d3, 0xffc,    -1, 0xe27, 0xa0a, 0x071,
 0x9da, 0x000, 0xba3, 0x3fd, 0x120, 0x845, 0x151, 0xb5f,
 0x193, 0xe99, 0x0f6, 0x640, 0xef4, 0xe17, 0x46b, 0x432,
 0x8a4, 0x415, 0x252, 0x79c, 0xbe5, 0x3f0, 0xb64, 0x984,
 0x5a7, 0x1be, 0xedc,    -1, 0xd7e, 0x5b4,    -1, 0xd27,
 0x03e, 0x136, 0xb30, 0xfff, 0x1dc, 0xc32, 0x84a, 0x392,
 0xf4f, 0x911, 0x501, 0x836, 0x700, 0x9ac, 0x000, 0xdb5,
 0xfa3, 0x5d3, 0x1f8, 0x888,    -1, 0xfa4, 0xfe7, 0xd87,
 0x9fe, 0x6af, 0x9a5, 0xfd5, 0xf49, 0xded, 0x416, 0x2fc,
 0x078, 0xd2e, 0xbf1, 0xcd9, 0x733,    -1, 0xb50, 0xd57,
 0xaa1,    -1, 0x9b0, 0x874, 0x18f,    -1,    -1, 0x2f9,
 0xfbb, 0xf73, 0x646, 0x868, 0x000, 0x000, 0xba2, 0xd74,
 0xc8f, 0xe36, 0xcfd, 0x5b8, 0x850, 0x48a, 0x689, 0x7ad,
 0xefd, 0x7e1, 0xf45, 0xd9e, 0x0f0, 0x7c0, 0x675,    -1,
 0xf26, 0x3c0, 0xe2d, 0xb62, 0x276,    -1, 0xf90, 0x837,
 0xc7c, 0x8ed, 0x08d, 0x1d6, 0x506, 0xac9, 0xd81, 0x1be,
 0xf7e, 0x1a3, 0xb2a, 0x5e2, 0x217, 0x1df, 0x5a3, 0x498,
 0xfb1, 0x432, 0x2ca, 0x5a1, 0x5d5, 0x135, 0x6f0,    -1,
 0xf70, 0x000, 0xd4c, 0x634,    -1, 0x8ca, 0x198,    -1,
 0x7b9, 0x629, 0xc16, 0x68c, 0xea2,    -1, 0xba0, 0x6d9,
 0xce9, 0x7b2, 0x000, 0xf59, 0x252, 0x575, 0x0a8, 0x894,
 0x000, 0x824, 0xf63, 0xd70, 0xdd8, 0x856, 0xc77, 0x334,
 0xeb2, 0x2b8, 0x307, 0xc34, 0x2e7, 0xa74, 0x6b9, 0x0e1,
 0x20f, 0x3ee, 0xf80, 0xa1f, 0x6e6, 0xa03, 0x3c7, 0x72f,
 0xfff, 0x156, 0x273, 0x1b7, 0xd90, 0x998, 0x474, 0xf1b,
 0x80f, 0xe4c, 0x0ba, 0xfff, 0x85e, 0x3af, 0x58f, 0x000,
 0xf6b, 0x71c, 0x4ed, 0xec3, 0x4cb, 0x000, 0xa68, 0x6ca,
 0x086, 0x000, 0x6e4, 0xab3, 0xff5, 0x281, 0xf0a, 0xc92,
 0x8d5, 0x486, 0xdd1, 0x903, 0x8e3, 0x9df, 0x2ab, 0xd08,
 0x144, 0xdcd, 0x281, 0x046, 0x161, 0xe83, 0xf24, 0xce7,
 0x30a, 0xf89, 0xf01, 0x308, 0x142, 0x9df, 0x44d, 0x9dd,
 0x3ed, 0x81b, 0xd9d, 0x000, 0x8c2, 0xe01, 0xfe6, 0xa20,
 0x167, 0xedd, 0xdb1, 0x470, 0xe70, 0x3aa, 0x0ff, 0x4d1,
 0xd30, 0x67a, 0xc56, 0x3d8, 0xf2e, 0x86a, 0x18b, 0x3f5,
 0x1a7, 0xd97, 0x652, 0x000, 0x00d, 0xbc7, 0xed3, 0x39e,
 0xb0d, 0xd8d, 0xc49, 0x2db, 0x44e, 0x820, 0x189, 0xd51,
 0x523, 0x161, 0x2eb, 0x41c, 0x951,    -1, 0xbb7,    -1,
    -1, 0x0a7, 0x3ed, 0xfaa, 0x18e, 0xa34, 0x820, 0x2b4,
    -1, 0x8c2, 0x3ee, 0x59d, 0x97b, 0x209, 0x3a2, 0x102,
 0x351, 0x6bf, 0xd3f, 0x4fc, 0x55f, 0x4b5, 0xe22, 0xf13,
 0x53a, 0x08a, 0x38d, 0xf4b, 0x424, 0xea6, 0x48e, 0x11c,
 0x339, 0x5bd, 0xf7c, 0x3bd, 0x15a, 0x35c, 0x854, 0x71b,
 0x30f, 0x065, 0x97e, 0x354, 0x28e, 0x344, 0x926, 0xc0b,
 0xae0, 0x5db, 0xb3e, 0x661, 0x432, 0x3c8, 0xf5e, 0x368,
 0xc85, 0xfff, 0x7f5, 0x0b6, 0x98b,    -1, 0x28c, 0x784,
 0xb78, 0x50a, 0x696, 0x47c, 0x40d,    -1, 0xe4d, 0x5fc,
    -1,    -1, 0xadb, 0x1db, 0x830, 0xd48,    -1, 0xf3a,
 0xee4, 0xed4, 0xb1a, 0xa14, 0x36d, 0xf1c, 0x774, 0x000,
 0x942, 0x278, 0x7ee, 0x000, 0x550, 0x57c, 0x343, 0x22b,
 0x324, 0xa34, 0x0ea, 0x230, 0x51b, 0x2d1, 0x500, 0x59f,
 0xd56, 0x540, 0x2f4, 0x87d, 0x9e5, 0x9c5, 0x5ea, 0x771,
 0x491, 0x206, 0xa4b, 0x4bf, 0xdaf, 0x308, 0xb25, 0x261,
 0x991, 0x000, 0x88e, 0x7e8, 0x3d6, 0x15d, 0xebc, 0x6c2,
 0xd45, 0x000, 0x3c6, 0x48d, 0x622, 0x758, 0xfa9, 0x3cf,
 0x401, 0xcdb, 0xe3f, 0x544, 0xf1f, 0x000,    -1, 0x4d4,
 0x000, 0x7f1, 0xba4, 0x81c, 0x92f, 0x7d1, 0xa83, 0xa31,
 0xe74, 0xa20, 0x475, 0x489, 0xf20, 0x3d1, 0xac1, 0xb2d,
 0x6b2, 0x1b6, 0x063, 0xd00, 0xfeb, 0x5ca, 0xb2c, 0xcb2,
 0x1cb, 0x251, 0x82b, 0x8ba, 0x40b, 0xf1e, 0xa8a, 0xd24,
 0x880, 0x84e, 0x8cb, 0x0a3, 0x000, 0xaf7, 0xf99, 0x6a1,
 0x156, 0x382, 0x0a0, 0x000, 0xed1, 0xd07, 0xbf5, 0x000,
 0x295, 0xe48, 0x760, 0x019, 0x97f, 0xb46, 0xff5, 0x7c9,
 0x1cf, 0xba4, 0x630, 0xe58, 0xda6, 0xd4b, 0xc02, 0xf9f,
 0x11c, 0x000, 0xb99, 0x51f, 0x43e, 0x199, 0xdfb, 0x72f,
 0x913, 0x509, 0xac5, 0xa2e, 0xcdb, 0x348, 0xb15, 0x472,
 0x95d, 0x67f, 0x000, 0x4b9, 0xd78, 0xc87, 0x0f6, 0x281,
 0x0bd, 0x924, 0x35e, 0x129, 0xffd, 0xe24, 0x000, 0x640,
 0x09b, 0xd10, 0xa0d, 0x000, 0x474, 0x189, 0x49f, 0x62d,
 0xcba, 0x561, 0x000, 0x58a, 0xaa1, 0x603, 0x5ab, 0x0c7,
 0x00a, 0x784, 0x5e4, 0x7e4, 0xe4d,    -1, 0x276, 0x465,
 0xee9, 0xe51, 0xdae, 0xbb1, 0x51f, 0xcba, 0x1c3, 0xd70,
 0x000, 0x5be, 0x4ea, 0x3cc, 0xf13, 0x811, 0x813, 0x234,
 0x7e4, 0xbae, 0xd97, 0xb74, 0x000, 0x76a, 0xda1, 0x000,
 0xd8c, 0x53a, 0xc5a, 0x000, 0x000, 0x61b, 0xd87, 0x141,
 0x383, 0xe06, 0x6a3, 0x6c3, 0xbcc, 0xc44, 0xf63, 0xd8b,
 0x58d, 0x000, 0x839, 0x77a, 0x000, 0x8e4, 0x000, 0xdbe,
 0x483, 0xd5b, 0xa9d, 0xca5, 0x431, 0x491, 0x29a, 0x27d,
 0x2d2, 0x691, 0x000, 0x19a, 0xa0d, 0xb0b, 0xf32, 0xe49,
 0xfbf, 0x399, 0xd20, 0x000, 0x66a, 0x000, 0x447, 0xb20,
 0x894, 0x038, 0xc9c, 0xff0, 0x000, 0x0d4, 0xad4, 0x768,
 0x65c, 0x000, 0x27b, 0x6c6, 0x9be, 0xd35, 0xc6a, 0xdd3,
 0x000, 0x2a7, 0x158, 0x38d, 0x8ef, 0x7b6, 0xd49, 0x30c,
 0xec3, 0x211, 0x17c, 0xcd0, 0x61f, 0x000, 0xe6e, 0x1d4,
 0x6e9, 0x000, 0xc2d, 0x5c3, 0xcd4, 0x760, 0x532, 0xc94,
 0x590, 0x000, 0x4a3, 0xc33, 0x000, 0x426, 0x604, 0xa06,
 0xa99, 0x917, 0x0c4, 0xc8d, 0x9e5, 0xcc7, 0x415, 0xf79,
 0x000, 0xaf4, 0x622, 0x756, 0x9c2, 0xa51, 0xb0f, 0x4ef,
 0xbc4, 0xe15, 0x29e, 0x055, 0x6c9, 0x695, 0x94f, 0x9d6,
 0x000, 0xb9f, 0xd46, 0x1d4, 0x000, 0xcb2, 0x9e8, 0x000,
 0xa5e, 0xce0, 0x000, 0x098, 0xa98, 0x6d9, 0x5e2, 0x95f,
 0x791, 0xeb8, 0x5fa, 0x60a, 0xacc, 0x3d3, 0x4df, 0x0df,
 0x9ca, 0x972, 0x3cc, 0x583, 0xca5, 0xe1a, 0x000, 0x2d3,
 0x266, 0x000, 0x06c, 0xfff, 0x62d, 0x64e, 0x40c, 0x599,
 0x475, 0xaa9, 0xba6, 0x96f, 0xe32, 0x059, 0x342, 0x36d,
 0xfd1, 0x09b, 0x878, 0x9f8, 0x000, 0x3ad, 0xdba, 0x000,
 0x544, 0xc1a, 0x000, 0xee8, 0x492, 0xa6b, 0x447, 0xd2a,
 0xb4e, 0x02c, 0xadb, 0xde2, 0x904, 0x62d, 0xf01, 0xbb8,
 0x255, 0x382, 0xfff, 0x29e, 0x000, 0x000, 0x011, 0xfff,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "pdf417.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2008-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#define PDF417_STOP 0xbff

static inline signed short pdf417_decode8 (zbar_decoder_t *dcode)
{
    /* build edge signature of character
     * from similar edge measurements
     */
    unsigned s = dcode->pdf417.s8;
    dbprintf(2, " s=%d ", s);
    if(s < 8)
        return(-1);

    long sig = 0;
    signed char e;
    unsigned char i;
    for(i = 0; i < 7; i++) {
        if(get_color(dcode) == ZBAR_SPACE)
            e = decode_e(get_width(dcode, i) +
                         get_width(dcode, i + 1), s, 17);
        else
            e = decode_e(get_width(dcode, 7 - i) +
                         get_width(dcode, 6 - i), s, 17);
        dbprintf(4, "%x", e);
        if(e < 0 || e > 8)
            return(-1);
        sig = (sig << 3) ^ e;
    }
    dbprintf(2, " sig=%06lx", sig);

    /* determine cluster number */
    int clst = ((sig & 7) - ((sig >> 3) & 7) +
                ((sig >> 12) & 7) - ((sig >> 15) & 7));
    if(clst < 0)
        clst += 9;
    dbprintf(2, " k=%d", clst);
    zassert(clst >= 0 && clst < 9, -1, "dir=%x sig=%lx k=%x %s\n",
            dcode->pdf417.direction, sig, clst,
            _zbar_decoder_buf_dump(dcode->buf, dcode->pdf417.character));

    if(clst != 0 && clst != 3 && clst != 6) {
        if(get_color(dcode) && clst == 7 && sig == 0x080007)
            return(PDF417_STOP);
        return(-1);
    }

    signed short g[3];
    sig &= 0x3ffff;
    g[0] = pdf417_hash[(sig - (sig >> 10)) & PDF417_HASH_MASK];
    g[1] = pdf417_hash[((sig >> 8) - sig) & PDF417_HASH_MASK];
    g[2] = pdf417_hash[((sig >> 14) - (sig >> 1)) & PDF417_HASH_MASK];
    zassert(g[0] >= 0 && g[1] >= 0 && g[2] >= 0, -1,
            "dir=%x sig=%lx k=%x g0=%03x g1=%03x g2=%03x %s\n",
            dcode->pdf417.direction, sig, clst, g[0], g[1], g[2],
            _zbar_decoder_buf_dump(dcode->buf, dcode->pdf417.character));

    unsigned short c = (g[0] + g[1] + g[2]) & PDF417_HASH_MASK;
    dbprintf(2, " g0=%x g1=%x g2=%x c=%03d(%d)",
             g[0], g[1], g[2], c & 0x3ff, c >> 10);
    return(c);
}

static inline signed char pdf417_decode_start(zbar_decoder_t *dcode)
{
    unsigned s = dcode->pdf417.s8;
    if(s < 8)
        return(0);

    int ei = decode_e(get_width(dcode, 0) + get_width(dcode, 1), s, 17);
    int ex = (get_color(dcode) == ZBAR_SPACE) ? 2 : 6;
    if(ei != ex)
        return(0);

    ei = decode_e(get_width(dcode, 1) + get_width(dcode, 2), s, 17);
    if(ei)
        return(0);

    ei = decode_e(get_width(dcode, 2) + get_width(dcode, 3), s, 17);
    ex = (get_color(dcode) == ZBAR_SPACE) ? 0 : 2;
    if(ei != ex)
        return(0);

    ei = decode_e(get_width(dcode, 3) + get_width(dcode, 4), s, 17);
    ex = (get_color(dcode) == ZBAR_SPACE) ? 0 : 2;
    if(ei != ex)
        return(0);

    ei = decode_e(get_width(dcode, 4) + get_width(dcode, 5), s, 17);
    if(ei)
        return(0);

    ei = decode_e(get_width(dcode, 5) + get_width(dcode, 6), s, 17);
    if(ei)
        return(0);

    ei = decode_e(get_width(dcode, 6) + get_width(dcode, 7), s, 17);
    ex = (get_color(dcode) == ZBAR_SPACE) ? 7 : 1;
    if(ei != ex)
        return(0);

    ei = decode_e(get_width(dcode, 7) + get_width(dcode, 8), s, 17);
    ex = (get_color(dcode) == ZBAR_SPACE) ? 8 : 1;

    if(get_color(dcode) == ZBAR_BAR) {
        /* stop character has extra bar */
        if(ei != 1)
            return(0);
        ei = decode_e(get_width(dcode, 8) + get_width(dcode, 9), s, 17);
    }

    dbprintf(2, "      pdf417[%c]: s=%d",
             (get_color(dcode)) ? '<' : '>', s);

    /* check quiet zone */
    if(ei >= 0 && ei < ex) {
        dbprintf(2, " [invalid quiet]\n");
        return(0);
    }

    /* lock shared resources */
    if(acquire_lock(dcode, ZBAR_PDF417)) {
        dbprintf(2, " [locked %d]\n", dcode->lock);
        return(0);
    }

    pdf417_decoder_t *dcode417 = &dcode->pdf417;
    dcode417->direction = get_color(dcode);
    dcode417->element = 0;
    dcode417->character = 0;

    dbprintf(2, " [valid start]\n");
    return(ZBAR_PARTIAL);
}

zbar_symbol_type_t _zbar_decode_pdf417 (zbar_decoder_t *dcode)
{
    pdf417_decoder_t *dcode417 = &dcode->pdf417;

    /* update latest character width */
    dcode417->s8 -= get_width(dcode, 8);
    dcode417->s8 += get_width(dcode, 0);

    if(dcode417->character < 0) {
        pdf417_decode_start(dcode);
        dbprintf(4, "\n");
        return(0);
    }

    /* process every 8th element of active symbol */
    if(++dcode417->element)
        return(0);
    dcode417->element = 0;

    dbprintf(2, "      pdf417[%c%02d]:",
             (dcode417->direction) ? '<' : '>', dcode417->character);

    if(get_color(dcode) != dcode417->direction) {
        int c = dcode417->character;
        release_lock(dcode, ZBAR_PDF417);
        dcode417->character = -1;
        zassert(get_color(dcode) == dcode417->direction, ZBAR_NONE,
                "color=%x dir=%x char=%d elem=0 %s\n",
                get_color(dcode), dcode417->direction, c,
                _zbar_decoder_buf_dump(dcode->buf, c));
    }

    signed short c = pdf417_decode8(dcode);
    if(c < 0 || size_buf(dcode, dcode417->character + 1)) {
        dbprintf(1, (c < 0) ? " [aborted]\n" : " [overflow]\n");
        release_lock(dcode, ZBAR_PDF417);
        dcode417->character = -1;
        return(0);
    }

    /* FIXME TBD infer dimensions, save codewords */

    if(c == PDF417_STOP) {
        dbprintf(1, " [valid stop]");
        /* FIXME check trailing bar and qz */
        dcode->direction = 1 - 2 * dcode417->direction;
        dcode->modifiers = 0;
        release_lock(dcode, ZBAR_PDF417);
        dcode417->character = -1;
    }

    dbprintf(2, "\n");
    return(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "decoder.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

zbar_decoder_t *zbar_decoder_create ()
{
    zbar_decoder_t *dcode = calloc(1, sizeof(zbar_decoder_t));
    dcode->buf_alloc = BUFFER_MIN;
    dcode->buf = malloc(dcode->buf_alloc);

    /* initialize default configs */
#ifdef ENABLE_EAN
    dcode->ean.enable = 1;
    dcode->ean.ean13_config = ((1 << ZBAR_CFG_ENABLE) |
                               (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->ean.ean8_config = ((1 << ZBAR_CFG_ENABLE) |
                              (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->ean.upca_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.upce_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.isbn10_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.isbn13_config = 1 << ZBAR_CFG_EMIT_CHECK;
# ifdef FIXME_ADDON_SYNC
    dcode->ean.ean2_config = 1 << ZBAR_CFG_ENABLE;
    dcode->ean.ean5_config = 1 << ZBAR_CFG_ENABLE;
# endif
#endif
#ifdef ENABLE_I25
    dcode->i25.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->i25, ZBAR_CFG_MIN_LEN) = 6;
#endif
#ifdef ENABLE_DATABAR
    dcode->databar.config = ((1 << ZBAR_CFG_ENABLE) |
                             (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->databar.config_exp = ((1 << ZBAR_CFG_ENABLE) |
                                 (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->databar.csegs = 4;
    dcode->databar.segs = calloc(4, sizeof(*dcode->databar.segs));
#endif
#ifdef ENABLE_CODABAR
    dcode->codabar.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->codabar, ZBAR_CFG_MIN_LEN) = 4;
#endif
#ifdef ENABLE_CODE39
    dcode->code39.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->code39, ZBAR_CFG_MIN_LEN) = 1;
#endif
#ifdef ENABLE_CODE93
    dcode->code93.config = 1 << ZBAR_CFG_ENABLE;
#endif
#ifdef ENABLE_CODE128
    dcode->code128.config = 1 << ZBAR_CFG_ENABLE;
#endif
#ifdef ENABLE_PDF417
    dcode->pdf417.config = 1 << ZBAR_CFG_ENABLE;
#endif
#ifdef ENABLE_QRCODE
    dcode->qrf.config = 1 << ZBAR_CFG_ENABLE;
#endif

    zbar_decoder_reset(dcode);
    return(dcode);
}

void zbar_decoder_destroy (zbar_decoder_t *dcode)
{
#ifdef ENABLE_DATABAR
    if(dcode->databar.segs)
        free(dcode->databar.segs);
#endif
    if(dcode->buf)
        free(dcode->buf);
    free(dcode);
}

void zbar_decoder_reset (zbar_decoder_t *dcode)
{
    memset(dcode, 0, (long)&dcode->buf_alloc - (long)dcode);
#ifdef ENABLE_EAN
    ean_reset(&dcode->ean);
#endif
#ifdef ENABLE_I25
    i25_reset(&dcode->i25);
#endif
#ifdef ENABLE_DATABAR
    databar_reset(&dcode->databar);
#endif
#ifdef ENABLE_CODABAR
    codabar_reset(&dcode->codabar);
#endif
#ifdef ENABLE_CODE39
    code39_reset(&dcode->code39);
#endif
#ifdef ENABLE_CODE93
    code93_reset(&dcode->code93);
#endif
#ifdef ENABLE_CODE128
    code128_reset(&dcode->code128);
#endif
#ifdef ENABLE_PDF417
    pdf417_reset(&dcode->pdf417);
#endif
#ifdef ENABLE_QRCODE
    qr_finder_reset(&dcode->qrf);
#endif
}

void zbar_decoder_new_scan (zbar_decoder_t *dcode)
{
    /* soft reset decoder */
    memset(dcode->w, 0, sizeof(dcode->w));
    dcode->lock = 0;
    dcode->idx = 0;
    dcode->s6 = 0;
#ifdef ENABLE_EAN
    ean_new_scan(&dcode->ean);
#endif
#ifdef ENABLE_I25
    i25_reset(&dcode->i25);
#endif
#ifdef ENABLE_DATABAR
    databar_new_scan(&dcode->databar);
#endif
#ifdef ENABLE_CODABAR
    codabar_reset(&dcode->codabar);
#endif
#ifdef ENABLE_CODE39
    code39_reset(&dcode->code39);
#endif
#ifdef ENABLE_CODE93
    code93_reset(&dcode->code93);
#endif
#ifdef ENABLE_CODE128
    code128_reset(&dcode->code128);
#endif
#ifdef ENABLE_PDF417
    pdf417_reset(&dcode->pdf417);
#endif
#ifdef ENABLE_QRCODE
    qr_finder_reset(&dcode->qrf);
#endif
}


zbar_color_t zbar_decoder_get_color (const zbar_decoder_t *dcode)
{
    return(get_color(dcode));
}

const char *zbar_decoder_get_data (const zbar_decoder_t *dcode)
{
    return((char*)dcode->buf);
}

unsigned int zbar_decoder_get_data_length (const zbar_decoder_t *dcode)
{
    return(dcode->buflen);
}

int zbar_decoder_get_direction (const zbar_decoder_t *dcode)
{
    return(dcode->direction);
}

zbar_decoder_handler_t *
zbar_decoder_set_handler (zbar_decoder_t *dcode,
                          zbar_decoder_handler_t *handler)
{
    zbar_decoder_handler_t *result = dcode->handler;
    dcode->handler = handler;
    return(result);
}

void zbar_decoder_set_userdata (zbar_decoder_t *dcode,
                                void *userdata)
{
    dcode->userdata = userdata;
}

void *zbar_decoder_get_userdata (const zbar_decoder_t *dcode)
{
    return(dcode->userdata);
}

zbar_symbol_type_t zbar_decoder_get_type (const zbar_decoder_t *dcode)
{
    return(dcode->type);
}

unsigned int zbar_decoder_get_modifiers (const zbar_decoder_t *dcode)
{
    return(dcode->modifiers);
}

zbar_symbol_type_t zbar_decode_width (zbar_decoder_t *dcode,
                                      unsigned w)
{
    zbar_symbol_type_t tmp, sym = ZBAR_NONE;

    dcode->w[dcode->idx & (DECODE_WINDOW - 1)] = w;
    dbprintf(1, "    decode[%x]: w=%d (%g)\n", dcode->idx, w, (w / 32.));

    /* update shared character width */
    dcode->s6 -= get_width(dcode, 7);
    dcode->s6 += get_width(dcode, 1);

    /* each decoder processes width stream in parallel */
#ifdef ENABLE_QRCODE
    if(TEST_CFG(dcode->qrf.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_find_qr(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_EAN
    if((dcode->ean.enable) &&
       (tmp = _zbar_decode_ean(dcode)))
        sym = tmp;
#endif
#ifdef ENABLE_CODE39
    if(TEST_CFG(dcode->code39.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_code39(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_CODE93
    if(TEST_CFG(dcode->code93.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_code93(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_CODE128
    if(TEST_CFG(dcode->code128.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_code128(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_DATABAR
    if(TEST_CFG(dcode->databar.config | dcode->databar.config_exp,
                ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_databar(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_CODABAR
    if(TEST_CFG(dcode->codabar.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_codabar(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_I25
    if(TEST_CFG(dcode->i25.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_i25(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#ifdef ENABLE_PDF417
    if(TEST_CFG(dcode->pdf417.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_pdf417(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif

    dcode->idx++;
    dcode->type = sym;
    if(sym) {
        if(dcode->lock && sym > ZBAR_PARTIAL && sym != ZBAR_QRCODE)
            release_lock(dcode, sym);
        if(dcode->handler)
            dcode->handler(dcode);
    }
    return(sym);
}

static inline const unsigned int*
decoder_get_configp (const zbar_decoder_t *dcode,
                     zbar_symbol_type_t sym)
{
    const unsigned int *config;
    switch(sym) {
#ifdef ENABLE_EAN
    case ZBAR_EAN13:
        config = &dcode->ean.ean13_config;
        break;

    case ZBAR_EAN2:
        config = &dcode->ean.ean2_config;
        break;

    case ZBAR_EAN5:
        config = &dcode->ean.ean5_config;
        break;

    case ZBAR_EAN8:
        config = &dcode->ean.ean8_config;
        break;

    case ZBAR_UPCA:
        config = &dcode->ean.upca_config;
        break;

    case ZBAR_UPCE:
        config = &dcode->ean.upce_config;
        break;

    case ZBAR_ISBN10:
        config = &dcode->ean.isbn10_config;
        break;

    case ZBAR_ISBN13:
        config = &dcode->ean.isbn13_config;
        break;
#endif

#ifdef ENABLE_I25
    case ZBAR_I25:
        config = &dcode->i25.config;
        break;
#endif

#ifdef ENABLE_DATABAR
    case ZBAR_DATABAR:
        config = &dcode->databar.config;
        break;
    case ZBAR_DATABAR_EXP:
        config = &dcode->databar.config_exp;
        break;
#endif

#ifdef ENABLE_CODABAR
    case ZBAR_CODABAR:
        config = &dcode->codabar.config;
        break;
#endif

#ifdef ENABLE_CODE39
    case ZBAR_CODE39:
        config = &dcode->code39.config;
        break;
#endif

#ifdef ENABLE_CODE93
    case ZBAR_CODE93:
        config = &dcode->code93.config;
        break;
#endif

#ifdef ENABLE_CODE128
    case ZBAR_CODE128:
        config = &dcode->code128.config;
        break;
#endif

#ifdef ENABLE_PDF417
    case ZBAR_PDF417:
        config = &dcode->pdf417.config;
        break;
#endif

#ifdef ENABLE_QRCODE
    case ZBAR_QRCODE:
        config = &dcode->qrf.config;
        break;
#endif

    default:
        config = NULL;
    }
    return(config);
}

unsigned int zbar_decoder_get_configs (const zbar_decoder_t *dcode,
                                       zbar_symbol_type_t sym)
{
    const unsigned *config = decoder_get_configp(dcode, sym);
    if(!config)
        return(0);
    return(*config);
}

static inline int decoder_set_config_bool (zbar_decoder_t *dcode,
                                           zbar_symbol_type_t sym,
                                           zbar_config_t cfg,
                                           int val)
{
    unsigned *config = (void*)decoder_get_configp(dcode, sym);
    if(!config || cfg >= ZBAR_CFG_NUM)
        return(1);

    if(!val)
        *config &= ~(1 << cfg);
    else if(val == 1)
        *config |= (1 << cfg);
    else
        return(1);

#ifdef ENABLE_EAN
    dcode->ean.enable = TEST_CFG(dcode->ean.ean13_config |
                                 dcode->ean.ean2_config |
                                 dcode->ean.ean5_config |
                                 dcode->ean.ean8_config |
                                 dcode->ean.upca_config |
                                 dcode->ean.upce_config |
                                 dcode->ean.isbn10_config |
                                 dcode->ean.isbn13_config,
                                 ZBAR_CFG_ENABLE);
#endif

    return(0);
}

static inline int decoder_set_config_int (zbar_decoder_t *dcode,
                                          zbar_symbol_type_t sym,
                                          zbar_config_t cfg,
                                          int val)
{
    switch(sym) {

#ifdef ENABLE_I25
    case ZBAR_I25:
        CFG(dcode->i25, cfg) = val;
        break;
#endif
#ifdef ENABLE_CODABAR
    case ZBAR_CODABAR:
        CFG(dcode->codabar, cfg) = val;
        break;
#endif
#ifdef ENABLE_CODE39
    case ZBAR_CODE39:
        CFG(dcode->code39, cfg) = val;
        break;
#endif
#ifdef ENABLE_CODE93
    case ZBAR_CODE93:
        CFG(dcode->code93, cfg) = val;
        break;
#endif
#ifdef ENABLE_CODE128
    case ZBAR_CODE128:
        CFG(dcode->code128, cfg) = val;
        break;
#endif
#ifdef ENABLE_PDF417
    case ZBAR_PDF417:
        CFG(dcode->pdf417, cfg) = val;
        break;
#endif

    default:
        return(1);
    }
    return(0);
}

int zbar_decoder_set_config (zbar_decoder_t *dcode,
                             zbar_symbol_type_t sym,
                             zbar_config_t cfg,
                             int val)
{
    if(sym == ZBAR_NONE) {
        static const zbar_symbol_type_t all[] = {
        ZBAR_EAN13, ZBAR_EAN2, ZBAR_EAN5, ZBAR_EAN8,
            ZBAR_UPCA, ZBAR_UPCE, ZBAR_ISBN10, ZBAR_ISBN13,
            ZBAR_I25, ZBAR_DATABAR, ZBAR_DATABAR_EXP, ZBAR_CODABAR,
        ZBAR_CODE39, ZBAR_CODE93, ZBAR_CODE128, ZBAR_QRCODE,
        ZBAR_PDF417, 0
        };
        const zbar_symbol_type_t *symp;
        for(symp = all; *symp; symp++)
            zbar_decoder_set_config(dcode, *symp, cfg, val);
        return(0);
    }

    if(cfg >= 0 && cfg < ZBAR_CFG_NUM)
        return(decoder_set_config_bool(dcode, sym, cfg, val));
    else if(cfg >= ZBAR_CFG_MIN_LEN && cfg <= ZBAR_CFG_MAX_LEN)
        return(decoder_set_config_int(dcode, sym, cfg, val));
    else
        return(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "scanner.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#ifndef ZBAR_FIXED
# define ZBAR_FIXED 5
#endif
#define ROUND (1 << (ZBAR_FIXED - 1))

/* FIXME add runtime config API for these */
#ifndef ZBAR_SCANNER_THRESH_MIN
# define ZBAR_SCANNER_THRESH_MIN  4
#endif

#ifndef ZBAR_SCANNER_THRESH_INIT_WEIGHT
# define ZBAR_SCANNER_THRESH_INIT_WEIGHT .44
#endif
#define THRESH_INIT ((unsigned)((ZBAR_SCANNER_THRESH_INIT_WEIGHT       \
                                 * (1 << (ZBAR_FIXED + 1)) + 1) / 2))

#ifndef ZBAR_SCANNER_THRESH_FADE
# define ZBAR_SCANNER_THRESH_FADE 8
#endif

#ifndef ZBAR_SCANNER_EWMA_WEIGHT
# define ZBAR_SCANNER_EWMA_WEIGHT .78
#endif
#define EWMA_WEIGHT ((unsigned)((ZBAR_SCANNER_EWMA_WEIGHT              \
                                 * (1 << (ZBAR_FIXED + 1)) + 1) / 2))

/* scanner state */
struct zbar_scanner_s {
    zbar_decoder_t *decoder; /* associated bar width decoder */
    unsigned y1_min_thresh; /* minimum threshold */

    unsigned x;             /* relative scan position of next sample */
    int y0[4];              /* short circular buffer of average intensities */

    int y1_sign;            /* slope at last crossing */
    unsigned y1_thresh;     /* current slope threshold */

    unsigned cur_edge;      /* interpolated position of tracking edge */
    unsigned last_edge;     /* interpolated position of last located edge */
    unsigned width;         /* last element width */
};

zbar_scanner_t *zbar_scanner_create (zbar_decoder_t *dcode)
{
    zbar_scanner_t *scn = malloc(sizeof(zbar_scanner_t));
    scn->decoder = dcode;
    scn->y1_min_thresh = ZBAR_SCANNER_THRESH_MIN;
    zbar_scanner_reset(scn);
    return(scn);
}

void zbar_scanner_destroy (zbar_scanner_t *scn)
{
    free(scn);
}

zbar_symbol_type_t zbar_scanner_reset (zbar_scanner_t *scn)
{
    memset(&scn->x, 0, sizeof(zbar_scanner_t) - offsetof(zbar_scanner_t, x));
    scn->y1_thresh = scn->y1_min_thresh;
    if(scn->decoder)
        zbar_decoder_reset(scn->decoder);
    return(ZBAR_NONE);
}

unsigned zbar_scanner_get_width (const zbar_scanner_t *scn)
{
    return(scn->width);
}

unsigned zbar_scanner_get_edge (const zbar_scanner_t *scn,
                                unsigned offset,
                                int prec)
{
    unsigned edge = scn->last_edge - offset - (1 << ZBAR_FIXED) - ROUND;
    prec = ZBAR_FIXED - prec;
    if(prec > 0)
        return(edge >> prec);
    else if(!prec)
        return(edge);
    else
        return(edge << -prec);
}

zbar_color_t zbar_scanner_get_color (const zbar_scanner_t *scn)
{
    return((scn->y1_sign <= 0) ? ZBAR_SPACE : ZBAR_BAR);
}

static inline unsigned calc_thresh (zbar_scanner_t *scn)
{
    /* threshold 1st to improve noise rejection */
    unsigned dx, thresh = scn->y1_thresh;
    unsigned long t;
    if((thresh <= scn->y1_min_thresh) || !scn->width) {
        dbprintf(1, " tmin=%d", scn->y1_min_thresh);
        return(scn->y1_min_thresh);
    }
    /* slowly return threshold to min */
    dx = (scn->x << ZBAR_FIXED) - scn->last_edge;
    t = thresh * dx;
    t /= scn->width;
    t /= ZBAR_SCANNER_THRESH_FADE;
    dbprintf(1, " thr=%d t=%ld x=%d last=%d.%d (%d)",
             thresh, t, scn->x, scn->last_edge >> ZBAR_FIXED,
             scn->last_edge & ((1 << ZBAR_FIXED) - 1), dx);
    if(thresh > t) {
        thresh -= t;
        if(thresh > scn->y1_min_thresh)
            return(thresh);
    }
    scn->y1_thresh = scn->y1_min_thresh;
    return(scn->y1_min_thresh);
}

static inline zbar_symbol_type_t process_edge (zbar_scanner_t *scn,
                                               int y1)
{
    if(!scn->y1_sign)
        scn->last_edge = scn->cur_edge = (1 << ZBAR_FIXED) + ROUND;
    else if(!scn->last_edge)
        scn->last_edge = scn->cur_edge;

    scn->width = scn->cur_edge - scn->last_edge;
    dbprintf(1, " sgn=%d cur=%d.%d w=%d (%s)\n",
             scn->y1_sign, scn->cur_edge >> ZBAR_FIXED,
             scn->cur_edge & ((1 << ZBAR_FIXED) - 1), scn->width,
             ((y1 > 0) ? "SPACE" : "BAR"));
    scn->last_edge = scn->cur_edge;

#if DEBUG_SVG > 1
    svg_path_moveto(SVG_ABS, scn->last_edge - (1 << ZBAR_FIXED) - ROUND, 0);
#endif

    /* pass to decoder */
    if(scn->decoder)
        return(zbar_decode_width(scn->decoder, scn->width));
    return(ZBAR_PARTIAL);
}

inline zbar_symbol_type_t zbar_scanner_flush (zbar_scanner_t *scn)
{
    unsigned x;
    if(!scn->y1_sign)
        return(ZBAR_NONE);

    x = (scn->x << ZBAR_FIXED) + ROUND;

    if(scn->cur_edge != x || scn->y1_sign > 0) {
        zbar_symbol_type_t edge = process_edge(scn, -scn->y1_sign);
        dbprintf(1, "flush0:");
        scn->cur_edge = x;
        scn->y1_sign = -scn->y1_sign;
        return(edge);
    }

    scn->y1_sign = scn->width = 0;
    if(scn->decoder)
        return(zbar_decode_width(scn->decoder, 0));
    return(ZBAR_PARTIAL);
}

zbar_symbol_type_t zbar_scanner_new_scan (zbar_scanner_t *scn)
{
    zbar_symbol_type_t edge = ZBAR_NONE;
    while(scn->y1_sign) {
        zbar_symbol_type_t tmp = zbar_scanner_flush(scn);
        if(tmp < 0 || tmp > edge)
            edge = tmp;
    }

    /* reset scanner and associated decoder */
    memset(&scn->x, 0, sizeof(zbar_scanner_t) - offsetof(zbar_scanner_t, x));
    scn->y1_thresh = scn->y1_min_thresh;
    if(scn->decoder)
        zbar_decoder_new_scan(scn->decoder);
    return(edge);
}

zbar_symbol_type_t zbar_scan_y (zbar_scanner_t *scn,
                                int y)
{
    /* FIXME calc and clip to max y range... */
    /* retrieve short value history */
    register int x = scn->x;
    register int y0_1 = scn->y0[(x - 1) & 3];
    register int y0_0 = y0_1;
    register int y0_2, y0_3, y1_1, y2_1, y2_2;
    zbar_symbol_type_t edge;
    if(x) {
        /* update weighted moving average */
        y0_0 += ((int)((y - y0_1) * EWMA_WEIGHT)) >> ZBAR_FIXED;
        scn->y0[x & 3] = y0_0;
    }
    else
        y0_0 = y0_1 = scn->y0[0] = scn->y0[1] = scn->y0[2] = scn->y0[3] = y;
    y0_2 = scn->y0[(x - 2) & 3];
    y0_3 = scn->y0[(x - 3) & 3];
    /* 1st differential @ x-1 */
    y1_1 = y0_1 - y0_2;
    {
        register int y1_2 = y0_2 - y0_3;
        if((abs(y1_1) < abs(y1_2)) &&
           ((y1_1 >= 0) == (y1_2 >= 0)))
            y1_1 = y1_2;
    }

    /* 2nd differentials @ x-1 & x-2 */
    y2_1 = y0_0 - (y0_1 * 2) + y0_2;
    y2_2 = y0_1 - (y0_2 * 2) + y0_3;

    dbprintf(1, "scan: x=%d y=%d y0=%d y1=%d y2=%d",
             x, y, y0_1, y1_1, y2_1);

    edge = ZBAR_NONE;
    /* 2nd zero-crossing is 1st local min/max - could be edge */
    if((!y2_1 ||
        ((y2_1 > 0) ? y2_2 < 0 : y2_2 > 0)) &&
       (calc_thresh(scn) <= abs(y1_1)))
    {
        /* check for 1st sign change */
        char y1_rev = (scn->y1_sign > 0) ? y1_1 < 0 : y1_1 > 0;
        if(y1_rev)
            /* intensity change reversal - finalize previous edge */
            edge = process_edge(scn, y1_1);

        if(y1_rev || (abs(scn->y1_sign) < abs(y1_1))) {
            int d;
            scn->y1_sign = y1_1;

            /* adaptive thresholding */
            /* start at multiple of new min/max */
            scn->y1_thresh = (abs(y1_1) * THRESH_INIT + ROUND) >> ZBAR_FIXED;
            dbprintf(1, "\tthr=%d", scn->y1_thresh);
            if(scn->y1_thresh < scn->y1_min_thresh)
                scn->y1_thresh = scn->y1_min_thresh;

            /* update current edge */
            d = y2_1 - y2_2;
            scn->cur_edge = 1 << ZBAR_FIXED;
            if(!d)
                scn->cur_edge >>= 1;
            else if(y2_1)
                /* interpolate zero crossing */
                scn->cur_edge -= ((y2_1 << ZBAR_FIXED) + 1) / d;
            scn->cur_edge += x << ZBAR_FIXED;
            dbprintf(1, "\n");
        }
    }
    else
        dbprintf(1, "\n");
    /* FIXME add fall-thru pass to decoder after heuristic "idle" period
       (eg, 6-8 * last width) */
    scn->x = x + 1;
    return(edge);
}

/* undocumented API for drawing cutesy debug graphics */
void zbar_scanner_get_state (const zbar_scanner_t *scn,
                             unsigned *x,
                             unsigned *cur_edge,
                             unsigned *last_edge,
                             int *y0,
                             int *y1,
                             int *y2,
                             int *y1_thresh)
{
    register int y0_0 = scn->y0[(scn->x - 1) & 3];
    register int y0_1 = scn->y0[(scn->x - 2) & 3];
    register int y0_2 = scn->y0[(scn->x - 3) & 3];
    zbar_scanner_t *mut_scn;
    if(x) *x = scn->x - 1;
    if(last_edge) *last_edge = scn->last_edge;
    if(y0) *y0 = y0_1;
    if(y1) *y1 = y0_1 - y0_2;
    if(y2) *y2 = y0_0 - (y0_1 * 2) + y0_2;
    /* NB not quite accurate (uses updated x) */
    mut_scn = (zbar_scanner_t*)scn;
    if(y1_thresh) *y1_thresh = calc_thresh(mut_scn);
    dbprintf(1, "\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void imlib_find_barcodes(list_t *out, image_t *ptr, rectangle_t *roi)
{
    uint8_t *grayscale_image = (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->data : fb_alloc(roi->w * roi->h, FB_ALLOC_NO_HINT);
    umm_init_x(fb_avail());
    zbar_image_scanner_t *scanner = zbar_image_scanner_create();
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    zbar_image_t image;
    image.format = *((int *) "Y800");
    image.width = (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->w : roi->w;
    image.height = (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->h : roi->h;
    image.data = grayscale_image;
    image.datalen = ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->w : roi->w) * ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->h : roi->h);
    image.crop_x = (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? roi->x : 0;
    image.crop_y = (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? roi->y : 0;
    image.crop_w = roi->w;
    image.crop_h = roi->h;
    image.userdata = 0;
    image.seq = 0;
    image.syms = 0;

    switch (ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        default: {
            memset(grayscale_image, 0, roi->w * roi->h);
            break;
        }
    }

    list_init(out, sizeof(find_barcodes_list_lnk_data_t));

    if (zbar_scan_image(scanner, &image) > 0) {
        for (const zbar_symbol_t *symbol = (image.syms) ? image.syms->head : NULL; symbol; symbol = zbar_symbol_next(symbol)) {
            if (zbar_symbol_get_loc_size(symbol) > 0) {
                find_barcodes_list_lnk_data_t lnk_data;

                rectangle_init(&(lnk_data.rect),
                               zbar_symbol_get_loc_x(symbol, 0) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x),
                               zbar_symbol_get_loc_y(symbol, 0) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y),
                               (zbar_symbol_get_loc_size(symbol) == 1) ? 1 : 0,
                               (zbar_symbol_get_loc_size(symbol) == 1) ? 1 : 0);

                for (size_t k = 1, l = zbar_symbol_get_loc_size(symbol); k < l; k++) {
                    rectangle_t temp;
                    rectangle_init(&temp,
                                   zbar_symbol_get_loc_x(symbol, k) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x),
                                   zbar_symbol_get_loc_y(symbol, k) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y),
                                   0,
                                   0);
                    rectangle_united(&(lnk_data.rect), &temp);
                }

                // Add corners...
                lnk_data.corners[0].x = lnk_data.rect.x;                   // top-left
                lnk_data.corners[0].y = lnk_data.rect.y;                   // top-left
                lnk_data.corners[1].x = lnk_data.rect.x + lnk_data.rect.w; // top-right
                lnk_data.corners[1].y = lnk_data.rect.y;                   // top-right
                lnk_data.corners[2].x = lnk_data.rect.x + lnk_data.rect.w; // bottom-right
                lnk_data.corners[2].y = lnk_data.rect.y + lnk_data.rect.h; // bottom-right
                lnk_data.corners[3].x = lnk_data.rect.x;                   // bottom-left
                lnk_data.corners[3].y = lnk_data.rect.y + lnk_data.rect.h; // bottom-left

                // Payload is already null terminated.
                lnk_data.payload_len = zbar_symbol_get_data_length(symbol);
                lnk_data.payload = xalloc(zbar_symbol_get_data_length(symbol));
                memcpy(lnk_data.payload, zbar_symbol_get_data(symbol), zbar_symbol_get_data_length(symbol));

                switch (zbar_symbol_get_type(symbol)) {
                    case ZBAR_EAN2: lnk_data.type = BARCODE_EAN2; break;
                    case ZBAR_EAN5: lnk_data.type = BARCODE_EAN5; break;
                    case ZBAR_EAN8: lnk_data.type = BARCODE_EAN8; break;
                    case ZBAR_UPCE: lnk_data.type = BARCODE_UPCE; break;
                    case ZBAR_ISBN10: lnk_data.type = BARCODE_ISBN10; break;
                    case ZBAR_UPCA: lnk_data.type = BARCODE_UPCA; break;
                    case ZBAR_EAN13: lnk_data.type = BARCODE_EAN13; break;
                    case ZBAR_ISBN13: lnk_data.type = BARCODE_ISBN13; break;
                    case ZBAR_I25: lnk_data.type = BARCODE_I25; break;
                    case ZBAR_DATABAR: lnk_data.type = BARCODE_DATABAR; break;
                    case ZBAR_DATABAR_EXP: lnk_data.type = BARCODE_DATABAR_EXP; break;
                    case ZBAR_CODABAR: lnk_data.type = BARCODE_CODABAR; break;
                    case ZBAR_CODE39: lnk_data.type = BARCODE_CODE39; break;
                    case ZBAR_PDF417: lnk_data.type = BARCODE_PDF417; break;
                    case ZBAR_CODE93: lnk_data.type = BARCODE_CODE93; break;
                    case ZBAR_CODE128: lnk_data.type = BARCODE_CODE128; break;
                    default: continue;
                }

                switch (zbar_symbol_get_orientation(symbol)) {
                    case ZBAR_ORIENT_UP: lnk_data.rotation = 0; break;
                    case ZBAR_ORIENT_RIGHT: lnk_data.rotation = 270; break;
                    case ZBAR_ORIENT_DOWN: lnk_data.rotation = 180; break;
                    case ZBAR_ORIENT_LEFT: lnk_data.rotation = 90; break;
                    default: continue;
                }

                lnk_data.quality = zbar_symbol_get_quality(symbol);

                list_push_back(out, &lnk_data);
            }
        }
    }

    for (;;) { // Merge overlapping.
        bool merge_occured = false;

        list_t out_temp;
        list_init(&out_temp, sizeof(find_barcodes_list_lnk_data_t));

        while (list_size(out)) {
            find_barcodes_list_lnk_data_t lnk_code;
            list_pop_front(out, &lnk_code);

            for (size_t k = 0, l = list_size(out); k < l; k++) {
                find_barcodes_list_lnk_data_t tmp_code;
                list_pop_front(out, &tmp_code);

                if (rectangle_overlap(&(lnk_code.rect), &(tmp_code.rect))
                && (lnk_code.payload_len == tmp_code.payload_len)
                && (!strcmp(lnk_code.payload, tmp_code.payload))
                && (lnk_code.type == tmp_code.type)) {
                    lnk_code.rotation = ((lnk_code.rect.w * lnk_code.rect.h) > (tmp_code.rect.w * tmp_code.rect.h)) ? lnk_code.rotation : tmp_code.rotation;
                    lnk_code.quality += tmp_code.quality; // won't overflow
                    rectangle_united(&(lnk_code.rect), &(tmp_code.rect));
                    merge_occured = true;
                } else {
                    list_push_back(out, &tmp_code);
                }
            }

            list_push_back(&out_temp, &lnk_code);
        }

        list_copy(out, &out_temp);

        if (!merge_occured) {
            break;
        }
    }

    if (image.syms) {
        image.data = NULL;
        zbar_symbol_set_ref(image.syms, -1);
        image.syms = NULL;
    }

    zbar_image_scanner_destroy(scanner);
    fb_free(); // umm_init_x();
    if (ptr->bpp != IMAGE_BPP_GRAYSCALE) fb_free(); // grayscale_image;
}

#pragma GCC diagnostic pop
#endif //IMLIB_ENABLE_BARCODES
