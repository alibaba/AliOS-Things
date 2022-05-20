/*
 *    Copyright (c) 2016, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file contains definitions of spinel.
 */

#ifndef SPINEL_HEADER_INCLUDED
#define SPINEL_HEADER_INCLUDED 1

/*
 *   Spinel is a host-controller protocol designed to enable
 *   inter-operation over simple serial connections between general purpose
 *   device operating systems (OS) host and network co-processors (NCP) for
 *   the purpose of controlling and managing the NCP.
 *
 * ---------------------------------------------------------------------------
 *
 *   Frame Format
 *
 *   A frame is defined simply as the concatenation of
 *
 *   -  A header byte
 *   -  A command (up to three bytes)
 *   -  An optional command payload
 *
 *              +---------+--------+-----+-------------+
 *              | Octets: |   1    | 1-3 |      n      |
 *              +---------+--------+-----+-------------+
 *              | Fields: | HEADER | CMD | CMD_PAYLOAD |
 *              +---------+--------+-----+-------------+
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   Header Format
 *
 *   The header byte is broken down as follows:
 *
 *                    0   1   2   3   4   5   6   7
 *                  +---+---+---+---+---+---+---+---+
 *                  |  FLG  |  IID  |      TID      |
 *                  +---+---+---+---+---+---+---+---+
 *
 *
 *   The flag field of the header byte ("FLG") is always set to the value
 *   two (or "10" in binary).  Any frame received with these bits set to
 *   any other value else MUST NOT be considered a Spinel frame.
 *
 *   This convention allows Spinel to be line compatible with BTLE HCI.
 *   By defining the first two bit in this way we can disambiguate between
 *   Spinel frames and HCI frames (which always start with either "0x01"
 *   or "0x04") without any additional framing overhead.
 *
 *   The Interface Identifier (IID) is a number between 0 and 3, which
 *   is associated by the OS with a specific NCP. This allows the protocol
 *   to support up to 4 NCPs under same connection.
 *
 *   The least significant bits of the header represent the Transaction
 *   Identifier (TID). The TID is used for correlating responses to the
 *   commands which generated them.
 *
 *   When a command is sent from the host, any reply to that command sent
 *   by the NCP will use the same value for the TID.  When the host
 *   receives a frame that matches the TID of the command it sent, it can
 *   easily recognize that frame as the actual response to that command.
 *
 *   The TID value of zero (0) is used for commands to which a correlated
 *   response is not expected or needed, such as for unsolicited update
 *   commands sent to the host from the NCP.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   The command identifier is a 21-bit unsigned integer encoded in up to
 *   three bytes using the packed unsigned integer format described below.
 *   Depending on the semantics of the command in question, a payload MAY
 *   be included in the frame.  The exact composition and length of the
 *   payload is defined by the command identifier.
 *
 * ---------------------------------------------------------------------------
 *
 *   Data Packing
 *
 *   Data serialization for properties is performed using a light-weight
 *   data packing format which was loosely inspired by D-Bus.  The format
 *   of a serialization is defined by a specially formatted string.
 *
 *   This packing format is used for notational convenience.  While this
 *   string-based data-type format has been designed so that the strings
 *   may be directly used by a structured data parser, such a thing is not
 *   required to implement Spinel.
 *
 *   Goals:
 *
 *   -  Be lightweight and favor direct representation of values.
 *   -  Use an easily readable and memorable format string.
 *   -  Support lists and structures.
 *   -  Allow properties to be appended to structures while maintaining
 *      backward compatibility.
 *
 *   Each primitive data-type has an ASCII character associated with it.
 *   Structures can be represented as strings of these characters.  For
 *   example:
 *
 *   -  "C": A single unsigned byte.
 *   -  "C6U": A single unsigned byte, followed by a 128-bit IPv6 address,
 *      followed by a zero-terminated UTF8 string.
 *   -  "A(6)": An array of concatenated IPv6 addresses
 *
 *   In each case, the data is represented exactly as described.  For
 *   example, an array of 10 IPv6 address is stored as 160 bytes.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   Primitive Types
 *
 *   +----------+----------------------+---------------------------------+
 *   |   Char   | Name                 | Description                     |
 *   +----------+----------------------+---------------------------------+
 *   |   "."    | DATATYPE_VOID        | Empty data type. Used           |
 *   |          |                      | internally.                     |
 *   |   "b"    | DATATYPE_BOOL        | Boolean value. Encoded in       |
 *   |          |                      | 8-bits as either 0x00 or 0x01.  |
 *   |          |                      | All other values are illegal.   |
 *   |   "C"    | DATATYPE_UINT8       | Unsigned 8-bit integer.         |
 *   |   "c"    | DATATYPE_INT8        | Signed 8-bit integer.           |
 *   |   "S"    | DATATYPE_UINT16      | Unsigned 16-bit integer.        |
 *   |   "s"    | DATATYPE_INT16       | Signed 16-bit integer.          |
 *   |   "L"    | DATATYPE_UINT32      | Unsigned 32-bit integer.        |
 *   |   "l"    | DATATYPE_INT32       | Signed 32-bit integer.          |
 *   |   "i"    | DATATYPE_UINT_PACKED | Packed Unsigned Integer. See    |
 *   |          |                      | description below               |
 *   |   "6"    | DATATYPE_IPv6ADDR    | IPv6 Address. (Big-endian)      |
 *   |   "E"    | DATATYPE_EUI64       | EUI-64 Address. (Big-endian)    |
 *   |   "e"    | DATATYPE_EUI48       | EUI-48 Address. (Big-endian)    |
 *   |   "D"    | DATATYPE_DATA        | Arbitrary data. See related     |
 *   |          |                      | section below for details.      |
 *   |   "d"    | DATATYPE_DATA_WLEN   | Arbitrary data with prepended   |
 *   |          |                      | length. See below for details   |
 *   |   "U"    | DATATYPE_UTF8        | Zero-terminated UTF8-encoded    |
 *   |          |                      | string.                         |
 *   | "t(...)" | DATATYPE_STRUCT      | Structured datatype with        |
 *   |          |                      | prepended length.               |
 *   | "A(...)" | DATATYPE_ARRAY       | Array of datatypes. Compound    |
 *   |          |                      | type.                           |
 *   +----------+----------------------+---------------------------------+
 *
 *   All multi-byte values are little-endian unless explicitly stated
 *   otherwise.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   Packed Unsigned Integer
 *
 *   For certain types of integers, such command or property identifiers,
 *   usually have a value on the wire that is less than 127.  However, in
 *   order to not preclude the use of values larger than 255, we would
 *   need to add an extra byte.  Doing this would add an extra byte to the
 *   majority of instances, which can add up in terms of bandwidth.
 *
 *   The packed unsigned integer format is based on the unsigned integer
 *   format in EXI, except that we limit the maximum value to the
 *   largest value that can be encoded into three bytes (2,097,151).
 *
 *   For all values less than 127, the packed form of the number is simply
 *   a single byte which directly represents the number.  For values
 *   larger than 127, the following process is used to encode the value:
 *
 *   1.  The unsigned integer is broken up into _n_ 7-bit chunks and
 *       placed into _n_ octets, leaving the most significant bit of each
 *       octet unused.
 *   2.  Order the octets from least-significant to most-significant.
 *       (Little-endian)
 *   3.  Clear the most significant bit of the most significant octet.
 *       Set the least significant bit on all other octets.
 *
 *   Where `n` is the smallest number of 7-bit chunks you can use to
 *   represent the given value.
 *
 *   Take the value 1337, for example:
 *
 *                              1337 => 0x0539
 *                                   => [39 0A]
 *                                   => [B9 0A]
 *
 *   To decode the value, you collect the 7-bit chunks until you find an
 *   octet with the most significant bit clear.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   Data Blobs
 *
 *   There are two types for data blobs: "d" and "D".
 *
 *   -  "d" has the length of the data (in bytes) prepended to the data
 *      (with the length encoded as type "S").  The size of the length
 *      field is not included in the length.
 *   -  "D" does not have a prepended length: the length of the data is
 *      implied by the bytes remaining to be parsed.  It is an error for
 *      "D" to not be the last type in a type in a type signature.
 *
 *   This dichotomy allows for more efficient encoding by eliminating
 *   redundancy.  If the rest of the buffer is a data blob, encoding the
 *   length would be redundant because we already know how many bytes are
 *   in the rest of the buffer.
 *
 *   In some cases we use "d" even if it is the last field in a type
 *   signature.  We do this to allow for us to be able to append
 *   additional fields to the type signature if necessary in the future.
 *   This is usually the case with embedded structs, like in the scan
 *   results.
 *
 *   For example, let's say we have a buffer that is encoded with the
 *   datatype signature of "CLLD".  In this case, it is pretty easy to
 *   tell where the start and end of the data blob is: the start is 9
 *   bytes from the start of the buffer, and its length is the length of
 *   the buffer minus 9. (9 is the number of bytes taken up by a byte and
 *   two longs)
 *
 *   The datatype signature "CLLDU" is illegal because we can't determine
 *   where the last field (a zero-terminated UTF8 string) starts.  But the
 *   datatype "CLLdU" is legal, because the parser can determine the
 *   exact length of the data blob-- allowing it to know where the start
 *   of the next field would be.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   Structured Data
 *
 *   The structure data type ("t(...)") is a way of bundling together
 *   several fields into a single structure.  It can be thought of as a
 *   "d" type except that instead of being opaque, the fields in the
 *   content are known.  This is useful for things like scan results where
 *   you have substructures which are defined by different layers.
 *
 *   For example, consider the type signature "Lt(ES)t(6C)".  In this
 *   hypothetical case, the first struct is defined by the MAC layer, and
 *   the second struct is defined by the PHY layer.  Because of the use of
 *   structures, we know exactly what part comes from that layer.
 *   Additionally, we can add fields to each structure without introducing
 *   backward compatibility problems: Data encoded as "Lt(ESU)t(6C)"
 *   (Notice the extra "U") will decode just fine as "Lt(ES)t(6C)".
 *   Additionally, if we don't care about the MAC layer and only care
 *   about the network layer, we could parse as "Lt()t(6C)".
 *
 *   Note that data encoded as "Lt(ES)t(6C)" will also parse as "Ldd",
 *   with the structures from both layers now being opaque data blobs.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   Arrays
 *
 *   An array is simply a concatenated set of _n_ data encodings.  For
 *   example, the type "A(6)" is simply a list of IPv6 addresses---one
 *   after the other.  The type "A(6E)" likewise a concatenation of IPv6-
 *   address/EUI-64 pairs.
 *
 *   If an array contains many fields, the fields will often be surrounded
 *   by a structure ("t(...)").  This effectively prepends each item in
 *   the array with its length.  This is useful for improving parsing
 *   performance or to allow additional fields to be added in the future
 *   in a backward compatible way.  If there is a high certainty that
 *   additional fields will never be added, the struct may be omitted
 *   (saving two bytes per item).
 *
 *   This specification does not define a way to embed an array as a field
 *   alongside other fields.
 *
 * ---------------------------------------------------------------------------
 *
 *   Spinel definition compatibility guideline:
 *
 *   The compatibility policy for NCP versus RCP and host side are handled
 *   differently in spinel.
 *
 *   New NCP firmware should work with an older host driver, i.e., NCP
 *   implementation should remain backward compatible.
 *
 *    - Existing fields in the format of an already implemented spinel
 *      property or command cannot change.
 *
 *    - New fields may be appended at the end of the format (or the end of
 *      a struct) as long as the NCP implementation treats the new fields as
 *      optional (i.e., a driver not aware of and therefore not using the
 *      new fields should continue to function as before).
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *   For RCP and host, the "RCP API Version" numbers are used to check the
 *   compatibility between host implementation and RCP firmware. Generally,
 *   a newer host side implementation would work with a range of previous
 *   or older RCP firmware versions.
 *
 *   - SPINEL_RCP_API_VERSION specifies the current spinel RCP API version.
 *     This number MUST be incremented anytime there is a change in any of RCP
 *     specific spinel definitions.
 *
 *   - SPINEL_MIN_HOST_SUPPORTED_RCP_API_VERSION specifies the minimum spinel
 *     RCP API Version which is supported by the host-side implementation.
 *
 *   - On start, host implementation queries the RCP API version and accepts
 *     any version number from SPINEL_MIN_HOST_SUPPORTED_RCP_API_VERSION up to
 *     and including SPINEL_RCP_API_VERSION.
 *
 * ---------------------------------------------------------------------------
 */

#ifdef SPINEL_PLATFORM_HEADER
#include SPINEL_PLATFORM_HEADER
#else // ifdef SPINEL_PLATFORM_HEADER
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#endif // else SPINEL_PLATFORM_HEADER

// ----------------------------------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#if defined(__GNUC__)
#define SPINEL_API_EXTERN extern __attribute__((visibility("default")))
#define SPINEL_API_NONNULL_ALL __attribute__((nonnull))
#define SPINEL_API_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#endif // ifdef __GNUC__

#endif // ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef SPINEL_API_EXTERN
#define SPINEL_API_EXTERN extern
#endif

#ifndef SPINEL_API_NONNULL_ALL
#define SPINEL_API_NONNULL_ALL
#endif

#ifndef SPINEL_API_WARN_UNUSED_RESULT
#define SPINEL_API_WARN_UNUSED_RESULT
#endif

// ----------------------------------------------------------------------------

#define SPINEL_PROTOCOL_VERSION_THREAD_MAJOR 4
#define SPINEL_PROTOCOL_VERSION_THREAD_MINOR 3

/**
 * @def SPINEL_RCP_API_VERSION
 *
 * The RCP API version number.
 *
 * This number MUST increase by one each time any of the spinel definitions used by RCP change (independent of whether
 * the change is backward-compatible or not).
 *
 * Please see section "Spinel definition compatibility guideline" for more details.
 *
 */
#define SPINEL_RCP_API_VERSION 4

/**
 * @def SPINEL_MIN_HOST_SUPPORTED_RCP_API_VERSION
 *
 * The minimum RCP API version supported by the host implementation.
 *
 * This number MUST increase when there is a non-compatible RCP spinel related change on host implementation.
 *
 * Please see section "Spinel definition compatibility guideline" for more details.
 *
 */
#define SPINEL_MIN_HOST_SUPPORTED_RCP_API_VERSION 4

/**
 * @def SPINEL_FRAME_MAX_SIZE
 *
 *  The maximum size of SPINEL frame.
 *
 */
#define SPINEL_FRAME_MAX_SIZE 1300

/**
 * @def SPINEL_FRAME_MAX_COMMAND_HEADER_SIZE
 *
 *  The maximum size of SPINEL command header.
 *
 */
#define SPINEL_FRAME_MAX_COMMAND_HEADER_SIZE 4

/**
 * @def SPINEL_FRAME_MAX_PAYLOAD_SIZE
 *
 *  The maximum size of SPINEL command payload.
 *
 */
#define SPINEL_FRAME_MAX_COMMAND_PAYLOAD_SIZE (SPINEL_FRAME_MAX_SIZE - SPINEL_FRAME_MAX_COMMAND_HEADER_SIZE)

/**
 * @def SPINEL_ENCRYPTER_EXTRA_DATA_SIZE
 *
 *  The size of extra data to be allocated for spinel frame buffer,
 *  needed by Spinel Encrypter.
 *
 */
#define SPINEL_ENCRYPTER_EXTRA_DATA_SIZE 0

/**
 * @def SPINEL_FRAME_BUFFER_SIZE
 *
 *  The size of buffer large enough to fit one whole spinel frame with extra data
 *  needed by Spinel Encrypter.
 *
 */
#define SPINEL_FRAME_BUFFER_SIZE (SPINEL_FRAME_MAX_SIZE + SPINEL_ENCRYPTER_EXTRA_DATA_SIZE)

/// Macro for generating bit masks using bit index from the spec
#define SPINEL_BIT_MASK(bit_index, field_bit_count) ((1 << ((field_bit_count)-1)) >> (bit_index))

// ----------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {
#endif

enum
{
    SPINEL_STATUS_OK                       = 0,  ///< Operation has completed successfully.
    SPINEL_STATUS_FAILURE                  = 1,  ///< Operation has failed for some undefined reason.
    SPINEL_STATUS_UNIMPLEMENTED            = 2,  ///< Given operation has not been implemented.
    SPINEL_STATUS_INVALID_ARGUMENT         = 3,  ///< An argument to the operation is invalid.
    SPINEL_STATUS_INVALID_STATE            = 4,  ///< This operation is invalid for the current device state.
    SPINEL_STATUS_INVALID_COMMAND          = 5,  ///< This command is not recognized.
    SPINEL_STATUS_INVALID_INTERFACE        = 6,  ///< This interface is not supported.
    SPINEL_STATUS_INTERNAL_ERROR           = 7,  ///< An internal runtime error has occurred.
    SPINEL_STATUS_SECURITY_ERROR           = 8,  ///< A security/authentication error has occurred.
    SPINEL_STATUS_PARSE_ERROR              = 9,  ///< A error has occurred while parsing the command.
    SPINEL_STATUS_IN_PROGRESS              = 10, ///< This operation is in progress.
    SPINEL_STATUS_NOMEM                    = 11, ///< Operation prevented due to memory pressure.
    SPINEL_STATUS_BUSY                     = 12, ///< The device is currently performing a mutually exclusive operation
    SPINEL_STATUS_PROP_NOT_FOUND           = 13, ///< The given property is not recognized.
    SPINEL_STATUS_DROPPED                  = 14, ///< A/The packet was dropped.
    SPINEL_STATUS_EMPTY                    = 15, ///< The result of the operation is empty.
    SPINEL_STATUS_CMD_TOO_BIG              = 16, ///< The command was too large to fit in the internal buffer.
    SPINEL_STATUS_NO_ACK                   = 17, ///< The packet was not acknowledged.
    SPINEL_STATUS_CCA_FAILURE              = 18, ///< The packet was not sent due to a CCA failure.
    SPINEL_STATUS_ALREADY                  = 19, ///< The operation is already in progress.
    SPINEL_STATUS_ITEM_NOT_FOUND           = 20, ///< The given item could not be found.
    SPINEL_STATUS_INVALID_COMMAND_FOR_PROP = 21, ///< The given command cannot be performed on this property.
    SPINEL_STATUS_UNKNOWN_NEIGHBOR         = 22, ///< The neighbor is unknown.
    SPINEL_STATUS_NOT_CAPABLE              = 23, ///< The target is not capable of handling requested operation.
    SPINEL_STATUS_RESPONSE_TIMEOUT         = 24, ///< No response received from remote node

    SPINEL_STATUS_JOIN__BEGIN = 104,

    /// Generic failure to associate with other peers.
    /**
     *  This status error should not be used by implementors if
     *  enough information is available to determine that one of the
     *  later join failure status codes would be more accurate.
     *
     *  \sa SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING
     *  \sa SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING
     */
    SPINEL_STATUS_JOIN_FAILURE = SPINEL_STATUS_JOIN__BEGIN + 0,

    /// The node found other peers but was unable to decode their packets.
    /**
     *  Typically this error code indicates that the network
     *  key has been set incorrectly.
     *
     *  \sa SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING
     *  \sa SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING
     */
    SPINEL_STATUS_JOIN_SECURITY = SPINEL_STATUS_JOIN__BEGIN + 1,

    /// The node was unable to find any other peers on the network.
    /**
     *  \sa SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING
     *  \sa SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING
     */
    SPINEL_STATUS_JOIN_NO_PEERS = SPINEL_STATUS_JOIN__BEGIN + 2,

    /// The only potential peer nodes found are incompatible.
    /**
     *  \sa SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING
     */
    SPINEL_STATUS_JOIN_INCOMPATIBLE = SPINEL_STATUS_JOIN__BEGIN + 3,

    /// No response in expecting time.
    /**
     *  \sa SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING
     */
    SPINEL_STATUS_JOIN_RSP_TIMEOUT = SPINEL_STATUS_JOIN__BEGIN + 4,

    /// The node succeeds in commissioning and get the network credentials.
    /**
     *  \sa SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING
     */
    SPINEL_STATUS_JOIN_SUCCESS = SPINEL_STATUS_JOIN__BEGIN + 5,

    SPINEL_STATUS_JOIN__END = 112,

    SPINEL_STATUS_RESET__BEGIN   = 112,
    SPINEL_STATUS_RESET_POWER_ON = SPINEL_STATUS_RESET__BEGIN + 0,
    SPINEL_STATUS_RESET_EXTERNAL = SPINEL_STATUS_RESET__BEGIN + 1,
    SPINEL_STATUS_RESET_SOFTWARE = SPINEL_STATUS_RESET__BEGIN + 2,
    SPINEL_STATUS_RESET_FAULT    = SPINEL_STATUS_RESET__BEGIN + 3,
    SPINEL_STATUS_RESET_CRASH    = SPINEL_STATUS_RESET__BEGIN + 4,
    SPINEL_STATUS_RESET_ASSERT   = SPINEL_STATUS_RESET__BEGIN + 5,
    SPINEL_STATUS_RESET_OTHER    = SPINEL_STATUS_RESET__BEGIN + 6,
    SPINEL_STATUS_RESET_UNKNOWN  = SPINEL_STATUS_RESET__BEGIN + 7,
    SPINEL_STATUS_RESET_WATCHDOG = SPINEL_STATUS_RESET__BEGIN + 8,
    SPINEL_STATUS_RESET__END     = 128,

    SPINEL_STATUS_VENDOR__BEGIN = 15360,
    SPINEL_STATUS_VENDOR__END   = 16384,

    SPINEL_STATUS_STACK_NATIVE__BEGIN = 16384,
    SPINEL_STATUS_STACK_NATIVE__END   = 81920,

    SPINEL_STATUS_EXPERIMENTAL__BEGIN = 2000000,
    SPINEL_STATUS_EXPERIMENTAL__END   = 2097152,
};

typedef uint32_t spinel_status_t;

typedef enum
{
    SPINEL_NET_ROLE_DETACHED = 0,
    SPINEL_NET_ROLE_CHILD    = 1,
    SPINEL_NET_ROLE_ROUTER   = 2,
    SPINEL_NET_ROLE_LEADER   = 3,
} spinel_net_role_t;

typedef enum
{
    SPINEL_IPV6_ICMP_PING_OFFLOAD_DISABLED       = 0,
    SPINEL_IPV6_ICMP_PING_OFFLOAD_UNICAST_ONLY   = 1,
    SPINEL_IPV6_ICMP_PING_OFFLOAD_MULTICAST_ONLY = 2,
    SPINEL_IPV6_ICMP_PING_OFFLOAD_ALL            = 3,
} spinel_ipv6_icmp_ping_offload_mode_t;

typedef enum
{
    SPINEL_SCAN_STATE_IDLE     = 0,
    SPINEL_SCAN_STATE_BEACON   = 1,
    SPINEL_SCAN_STATE_ENERGY   = 2,
    SPINEL_SCAN_STATE_DISCOVER = 3,
} spinel_scan_state_t;

typedef enum
{
    SPINEL_MCU_POWER_STATE_ON        = 0,
    SPINEL_MCU_POWER_STATE_LOW_POWER = 1,
    SPINEL_MCU_POWER_STATE_OFF       = 2,
} spinel_mcu_power_state_t;

// The `spinel_power_state_t` enumeration and `POWER_STATE`
// property are deprecated. Please use `MCU_POWER_STATE`
// instead.
typedef enum
{
    SPINEL_POWER_STATE_OFFLINE    = 0,
    SPINEL_POWER_STATE_DEEP_SLEEP = 1,
    SPINEL_POWER_STATE_STANDBY    = 2,
    SPINEL_POWER_STATE_LOW_POWER  = 3,
    SPINEL_POWER_STATE_ONLINE     = 4,
} spinel_power_state_t;

typedef enum
{
    SPINEL_HOST_POWER_STATE_OFFLINE    = 0,
    SPINEL_HOST_POWER_STATE_DEEP_SLEEP = 1,
    SPINEL_HOST_POWER_STATE_RESERVED   = 2,
    SPINEL_HOST_POWER_STATE_LOW_POWER  = 3,
    SPINEL_HOST_POWER_STATE_ONLINE     = 4,
} spinel_host_power_state_t;

typedef enum
{
    SPINEL_MESHCOP_JOINER_STATE_IDLE       = 0,
    SPINEL_MESHCOP_JOINER_STATE_DISCOVER   = 1,
    SPINEL_MESHCOP_JOINER_STATE_CONNECTING = 2,
    SPINEL_MESHCOP_JOINER_STATE_CONNECTED  = 3,
    SPINEL_MESHCOP_JOINER_STATE_ENTRUST    = 4,
    SPINEL_MESHCOP_JOINER_STATE_JOINED     = 5,
} spinel_meshcop_joiner_state_t;

enum
{
    SPINEL_NET_FLAG_ON_MESH       = (1 << 0),
    SPINEL_NET_FLAG_DEFAULT_ROUTE = (1 << 1),
    SPINEL_NET_FLAG_CONFIGURE     = (1 << 2),
    SPINEL_NET_FLAG_DHCP          = (1 << 3),
    SPINEL_NET_FLAG_SLAAC         = (1 << 4),
    SPINEL_NET_FLAG_PREFERRED     = (1 << 5),

    SPINEL_NET_FLAG_PREFERENCE_OFFSET = 6,
    SPINEL_NET_FLAG_PREFERENCE_MASK   = (3 << SPINEL_NET_FLAG_PREFERENCE_OFFSET),
};

enum
{
    SPINEL_NET_FLAG_EXT_DP  = (1 << 6),
    SPINEL_NET_FLAG_EXT_DNS = (1 << 7),
};

enum
{
    SPINEL_ROUTE_PREFERENCE_HIGH   = (1 << SPINEL_NET_FLAG_PREFERENCE_OFFSET),
    SPINEL_ROUTE_PREFERENCE_MEDIUM = (0 << SPINEL_NET_FLAG_PREFERENCE_OFFSET),
    SPINEL_ROUTE_PREFERENCE_LOW    = (3 << SPINEL_NET_FLAG_PREFERENCE_OFFSET),
};

enum
{
    SPINEL_ROUTE_FLAG_NAT64 = (1 << 5),
};

enum
{
    SPINEL_THREAD_MODE_FULL_NETWORK_DATA   = (1 << 0),
    SPINEL_THREAD_MODE_FULL_THREAD_DEV     = (1 << 1),
    SPINEL_THREAD_MODE_SECURE_DATA_REQUEST = (1 << 2),
    SPINEL_THREAD_MODE_RX_ON_WHEN_IDLE     = (1 << 3),
};

enum
{
    SPINEL_GPIO_FLAG_DIR_INPUT       = 0,
    SPINEL_GPIO_FLAG_DIR_OUTPUT      = SPINEL_BIT_MASK(0, 8),
    SPINEL_GPIO_FLAG_PULL_UP         = SPINEL_BIT_MASK(1, 8),
    SPINEL_GPIO_FLAG_PULL_DOWN       = SPINEL_BIT_MASK(2, 8),
    SPINEL_GPIO_FLAG_OPEN_DRAIN      = SPINEL_BIT_MASK(2, 8),
    SPINEL_GPIO_FLAG_TRIGGER_NONE    = 0,
    SPINEL_GPIO_FLAG_TRIGGER_RISING  = SPINEL_BIT_MASK(3, 8),
    SPINEL_GPIO_FLAG_TRIGGER_FALLING = SPINEL_BIT_MASK(4, 8),
    SPINEL_GPIO_FLAG_TRIGGER_ANY     = SPINEL_GPIO_FLAG_TRIGGER_RISING | SPINEL_GPIO_FLAG_TRIGGER_FALLING,
};

enum
{
    SPINEL_PROTOCOL_TYPE_BOOTLOADER = 0,
    SPINEL_PROTOCOL_TYPE_ZIGBEE_IP  = 2,
    SPINEL_PROTOCOL_TYPE_THREAD     = 3,
};

enum
{
    SPINEL_MAC_PROMISCUOUS_MODE_OFF     = 0, ///< Normal MAC filtering is in place.
    SPINEL_MAC_PROMISCUOUS_MODE_NETWORK = 1, ///< All MAC packets matching network are passed up the stack.
    SPINEL_MAC_PROMISCUOUS_MODE_FULL    = 2, ///< All decoded MAC packets are passed up the stack.
};

enum
{
    SPINEL_NCP_LOG_LEVEL_EMERG  = 0,
    SPINEL_NCP_LOG_LEVEL_ALERT  = 1,
    SPINEL_NCP_LOG_LEVEL_CRIT   = 2,
    SPINEL_NCP_LOG_LEVEL_ERR    = 3,
    SPINEL_NCP_LOG_LEVEL_WARN   = 4,
    SPINEL_NCP_LOG_LEVEL_NOTICE = 5,
    SPINEL_NCP_LOG_LEVEL_INFO   = 6,
    SPINEL_NCP_LOG_LEVEL_DEBUG  = 7,
};

enum
{
    SPINEL_NCP_LOG_REGION_NONE        = 0,
    SPINEL_NCP_LOG_REGION_OT_API      = 1,
    SPINEL_NCP_LOG_REGION_OT_MLE      = 2,
    SPINEL_NCP_LOG_REGION_OT_ARP      = 3,
    SPINEL_NCP_LOG_REGION_OT_NET_DATA = 4,
    SPINEL_NCP_LOG_REGION_OT_ICMP     = 5,
    SPINEL_NCP_LOG_REGION_OT_IP6      = 6,
    SPINEL_NCP_LOG_REGION_OT_TCP      = 7,
    SPINEL_NCP_LOG_REGION_OT_MAC      = 8,
    SPINEL_NCP_LOG_REGION_OT_MEM      = 9,
    SPINEL_NCP_LOG_REGION_OT_NCP      = 10,
    SPINEL_NCP_LOG_REGION_OT_MESH_COP = 11,
    SPINEL_NCP_LOG_REGION_OT_NET_DIAG = 12,
    SPINEL_NCP_LOG_REGION_OT_PLATFORM = 13,
    SPINEL_NCP_LOG_REGION_OT_COAP     = 14,
    SPINEL_NCP_LOG_REGION_OT_CLI      = 15,
    SPINEL_NCP_LOG_REGION_OT_CORE     = 16,
    SPINEL_NCP_LOG_REGION_OT_UTIL     = 17,
    SPINEL_NCP_LOG_REGION_OT_BBR      = 18,
    SPINEL_NCP_LOG_REGION_OT_MLR      = 19,
    SPINEL_NCP_LOG_REGION_OT_DUA      = 10,
    SPINEL_NCP_LOG_REGION_OT_BR       = 21,
    SPINEL_NCP_LOG_REGION_OT_SRP      = 22,
    SPINEL_NCP_LOG_REGION_OT_DNS      = 23,
};

enum
{
    SPINEL_MESHCOP_COMMISSIONER_STATE_DISABLED = 0,
    SPINEL_MESHCOP_COMMISSIONER_STATE_PETITION = 1,
    SPINEL_MESHCOP_COMMISSIONER_STATE_ACTIVE   = 2,
};

enum
{
    SPINEL_ADDRESS_CACHE_ENTRY_STATE_CACHED      = 0, // Entry is cached and in-use.
    SPINEL_ADDRESS_CACHE_ENTRY_STATE_SNOOPED     = 1, // Entry is created by snoop optimization.
    SPINEL_ADDRESS_CACHE_ENTRY_STATE_QUERY       = 2, // Entry represents an ongoing query for the EID.
    SPINEL_ADDRESS_CACHE_ENTRY_STATE_RETRY_QUERY = 3, // Entry is in retry mode (a prior query did not  a response).
};

enum
{
    SPINEL_RADIO_LINK_IEEE_802_15_4 = 0,
    SPINEL_RADIO_LINK_TREL_UDP6     = 1,
};

// Statuses that can be received as a result of:
// @ref SPINEL_PROP_THREAD_LINK_METRICS_QUERY
// @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK
// @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_FORWARD
enum
{
    SPINEL_LINK_METRICS_STATUS_SUCCESS                     = 0,
    SPINEL_LINK_METRICS_STATUS_CANNOT_SUPPORT_NEW_SERIES   = 1,
    SPINEL_LINK_METRICS_STATUS_SERIESID_ALREADY_REGISTERED = 2,
    SPINEL_LINK_METRICS_STATUS_SERIESID_NOT_RECOGNIZED     = 3,
    SPINEL_LINK_METRICS_STATUS_NO_MATCHING_FRAMES_RECEIVED = 4,
    SPINEL_LINK_METRICS_STATUS_OTHER_ERROR                 = 254
};

// Metric ids used for:
// @ref SPINEL_PROP_THREAD_LINK_METRICS_QUERY
// @ref SPINEL_PROP_THREAD_LINK_METRICS_QUERY_RESULT
// @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK
// @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_FORWARD
enum
{
    SPINEL_THREAD_LINK_METRIC_PDU_COUNT   = (1 << 0),
    SPINEL_THREAD_LINK_METRIC_LQI         = (1 << 1),
    SPINEL_THREAD_LINK_METRIC_LINK_MARGIN = (1 << 2),
    SPINEL_THREAD_LINK_METRIC_RSSI        = (1 << 3),
};

// Frame types used for:
// @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_FORWARD
enum
{
    SPINEL_THREAD_FRAME_TYPE_MLE_LINK_PROBE   = (1 << 0),
    SPINEL_THREAD_FRAME_TYPE_MAC_DATA         = (1 << 1),
    SPINEL_THREAD_FRAME_TYPE_MAC_DATA_REQUEST = (1 << 2),
    SPINEL_THREAD_FRAME_TYPE_MAC_ACK          = (1 << 3),
};

// Parameter ids used for:
// @ref SPINEL_PROP_THREAD_MLR_REQUEST
enum
{
    SPINEL_THREAD_MLR_PARAMID_TIMEOUT = 0
};

// Backbone Router states used for:
// @ref SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_STATE
enum
{
    SPINEL_THREAD_BACKBONE_ROUTER_STATE_DISABLED  = 0,
    SPINEL_THREAD_BACKBONE_ROUTER_STATE_SECONDARY = 1,
    SPINEL_THREAD_BACKBONE_ROUTER_STATE_PRIMARY   = 2,
};

typedef enum
{
    SPINEL_SRP_CLIENT_ITEM_STATE_TO_ADD     = 0, // Item to be added/registered.
    SPINEL_SRP_CLIENT_ITEM_STATE_ADDING     = 1, // Item is being added/registered.
    SPINEL_SRP_CLIENT_ITEM_STATE_TO_REFRESH = 2, // Item to be refreshed (re-register to renew lease).
    SPINEL_SRP_CLIENT_ITEM_STATE_REFRESHING = 3, // Item is being refreshed.
    SPINEL_SRP_CLIENT_ITEM_STATE_TO_REMOVE  = 4, // Item to be removed.
    SPINEL_SRP_CLIENT_ITEM_STATE_REMOVING   = 5, // Item is being removed.
    SPINEL_SRP_CLIENT_ITEM_STATE_REGISTERED = 6, // Item is registered with server.
    SPINEL_SRP_CLIENT_ITEM_STATE_REMOVED    = 7, // Item is removed.
} spinel_srp_client_item_state_t;

typedef enum
{
    SPINEL_SRP_CLIENT_ERROR_NONE             = 0, // No error.
    SPINEL_SRP_CLIENT_ERROR_PARSE            = 1, // Server unable to interpret due to format error.
    SPINEL_SRP_CLIENT_ERROR_FAILED           = 2, // Server encountered an internal failure.
    SPINEL_SRP_CLIENT_ERROR_NOT_FOUND        = 3, // Name that ought to exist, does not exists.
    SPINEL_SRP_CLIENT_ERROR_NOT_IMPLEMENTED  = 4, // Server does not support the query type.
    SPINEL_SRP_CLIENT_ERROR_SECURITY         = 5, // Service is not authoritative for zone.
    SPINEL_SRP_CLIENT_ERROR_DUPLICATED       = 6, // Some name that ought not to exist, does exist.
    SPINEL_SRP_CLIENT_ERROR_RESPONSE_TIMEOUT = 7, // Timed out waiting for response from server (client would retry).
    SPINEL_SRP_CLIENT_ERROR_INVALID_ARGS     = 8, // Invalid args (e.g., bad service name or TXT-DATA).
    SPINEL_SRP_CLIENT_ERROR_NO_BUFS          = 9, // No buffer to send the SRP update message.
} spinel_srp_client_error_t;

typedef struct
{
    uint8_t bytes[8];
} spinel_eui64_t;

typedef struct
{
    uint8_t bytes[8];
} spinel_net_xpanid_t;

typedef struct
{
    uint8_t bytes[16];
} spinel_net_pskc_t;

typedef struct
{
    uint8_t bytes[6];
} spinel_eui48_t;

typedef struct
{
    uint8_t bytes[16];
} spinel_ipv6addr_t;

typedef int          spinel_ssize_t;
typedef unsigned int spinel_size_t;
typedef uint8_t      spinel_tid_t;

enum
{
    SPINEL_MD_FLAG_TX        = 0x0001, //!< Packet was transmitted, not received.
    SPINEL_MD_FLAG_BAD_FCS   = 0x0004, //!< Packet was received with bad FCS
    SPINEL_MD_FLAG_DUPE      = 0x0008, //!< Packet seems to be a duplicate
    SPINEL_MD_FLAG_ACKED_FP  = 0x0010, //!< Packet was acknowledged with frame pending set
    SPINEL_MD_FLAG_ACKED_SEC = 0x0020, //!< Packet was acknowledged with secure enhance ACK
    SPINEL_MD_FLAG_RESERVED  = 0xFFC2, //!< Flags reserved for future use.
};

enum
{
    SPINEL_RESET_PLATFORM = 1,
    SPINEL_RESET_STACK    = 2,
};

enum
{
    /**
     * No-Operation command (Host -> NCP)
     *
     * Encoding: Empty
     *
     * Induces the NCP to send a success status back to the host. This is
     * primarily used for liveliness checks. The command payload for this
     * command SHOULD be empty.
     *
     * There is no error condition for this command.
     *
     */
    SPINEL_CMD_NOOP = 0,

    /**
     * Reset NCP command (Host -> NCP)
     *
     * Encoding: Empty or `C`
     *
     * Causes the NCP to perform a software reset. Due to the nature of
     * this command, the TID is ignored. The host should instead wait
     * for a `CMD_PROP_VALUE_IS` command from the NCP indicating
     * `PROP_LAST_STATUS` has been set to `STATUS_RESET_SOFTWARE`.
     *
     * The optional command payload specifies the reset type, can be
     * `SPINEL_RESET_PLATFORM` or `SPINEL_RESET_STACK`. Defaults to stack
     * reset if unspecified.
     *
     * If an error occurs, the value of `PROP_LAST_STATUS` will be emitted
     * instead with the value set to the generated status code for the error.
     *
     */
    SPINEL_CMD_RESET = 1,

    /**
     * Get property value command (Host -> NCP)
     *
     * Encoding: `i`
     *   `i` : Property Id
     *
     * Causes the NCP to emit a `CMD_PROP_VALUE_IS` command for the
     * given property identifier.
     *
     * The payload for this command is the property identifier encoded
     * in the packed unsigned integer format `i`.
     *
     * If an error occurs, the value of `PROP_LAST_STATUS` will be emitted
     * instead with the value set to the generated status code for the error.
     *
     */
    SPINEL_CMD_PROP_VALUE_GET = 2,

    /**
     * Set property value command (Host -> NCP)
     *
     * Encoding: `iD`
     *   `i` : Property Id
     *   `D` : Value (encoding depends on the property)
     *
     * Instructs the NCP to set the given property to the specific given
     * value, replacing any previous value.
     *
     * The payload for this command is the property identifier encoded in the
     * packed unsigned integer format, followed by the property value. The
     * exact format of the property value is defined by the property.
     *
     * On success a `CMD_PROP_VALUE_IS` command is emitted either for the
     * given property identifier with the set value, or for `PROP_LAST_STATUS`
     * with value `LAST_STATUS_OK`.
     *
     * If an error occurs, the value of `PROP_LAST_STATUS` will be emitted
     * with the value set to the generated status code for the error.
     *
     */
    SPINEL_CMD_PROP_VALUE_SET = 3,

    /**
     * Insert value into property command (Host -> NCP)
     *
     * Encoding: `iD`
     *   `i` : Property Id
     *   `D` : Value (encoding depends on the property)
     *
     * Instructs the NCP to insert the given value into a list-oriented
     * property without removing other items in the list. The resulting order
     * of items in the list is defined by the individual property being
     * operated on.
     *
     * The payload for this command is the property identifier encoded in the
     * packed unsigned integer format, followed by the value to be inserted.
     * The exact format of the value is defined by the property.
     *
     * If the type signature of the property consists of a single structure
     * enclosed by an array `A(t(...))`, then the contents of value MUST
     * contain the contents of the structure (`...`) rather than the
     * serialization of the whole item (`t(...)`).  Specifically, the length
     * of the structure MUST NOT be prepended to value. This helps to
     * eliminate redundant data.
     *
     * On success, either a `CMD_PROP_VALUE_INSERTED` command is emitted for
     * the given property, or a `CMD_PROP_VALUE_IS` command is emitted of
     * property `PROP_LAST_STATUS` with value `LAST_STATUS_OK`.
     *
     * If an error occurs, the value of `PROP_LAST_STATUS` will be emitted
     * with the value set to the generated status code for the error.
     *
     */
    SPINEL_CMD_PROP_VALUE_INSERT = 4,

    /**
     * Remove value from property command (Host -> NCP)
     *
     * Encoding: `iD`
     *   `i` : Property Id
     *   `D` : Value (encoding depends on the property)

     * Instructs the NCP to remove the given value from a list-oriented property,
     * without affecting other items in the list. The resulting order of items
     * in the list is defined by the individual property being operated on.
     *
     * Note that this command operates by value, not by index!
     *
     * The payload for this command is the property identifier encoded in the
     * packed unsigned integer format, followed by the value to be removed. The
     * exact format of the value is defined by the property.
     *
     * If the type signature of the property consists of a single structure
     * enclosed by an array `A(t(...))`, then the contents of value MUST contain
     * the contents of the structure (`...`) rather than the serialization of the
     * whole item (`t(...)`).  Specifically, the length of the structure MUST NOT
     * be prepended to `VALUE`. This helps to eliminate redundant data.
     *
     * On success, either a `CMD_PROP_VALUE_REMOVED` command is emitted for the
     * given property, or a `CMD_PROP_VALUE_IS` command is emitted of property
     * `PROP_LAST_STATUS` with value `LAST_STATUS_OK`.
     *
     * If an error occurs, the value of `PROP_LAST_STATUS` will be emitted
     * with the value set to the generated status code for the error.
     *
     */
    SPINEL_CMD_PROP_VALUE_REMOVE = 5,

    /**
     * Property value notification command (NCP -> Host)
     *
     * Encoding: `iD`
     *   `i` : Property Id
     *   `D` : Value (encoding depends on the property)
     *
     * This command can be sent by the NCP in response to a previous command
     * from the host, or it can be sent by the NCP in an unsolicited fashion
     * to notify the host of various state changes asynchronously.
     *
     * The payload for this command is the property identifier encoded in the
     * packed unsigned integer format, followed by the current value of the
     * given property.
     *
     */
    SPINEL_CMD_PROP_VALUE_IS = 6,

    /**
     * Property value insertion notification command (NCP -> Host)
     *
     * Encoding:`iD`
     *   `i` : Property Id
     *   `D` : Value (encoding depends on the property)
     *
     * This command can be sent by the NCP in response to the
     * `CMD_PROP_VALUE_INSERT` command, or it can be sent by the NCP in an
     * unsolicited fashion to notify the host of various state changes
     * asynchronously.
     *
     * The payload for this command is the property identifier encoded in the
     * packed unsigned integer format, followed by the value that was inserted
     * into the given property.
     *
     * If the type signature of the property specified by property id consists
     * of a single structure enclosed by an array (`A(t(...))`), then the
     * contents of value MUST contain the contents of the structure (`...`)
     * rather than the serialization of the whole item (`t(...)`). Specifically,
     * the length of the structure MUST NOT be prepended to `VALUE`. This
     * helps to eliminate redundant data.
     *
     * The resulting order of items in the list is defined by the given
     * property.
     *
     */
    SPINEL_CMD_PROP_VALUE_INSERTED = 7,

    /**
     * Property value removal notification command (NCP -> Host)
     *
     * Encoding: `iD`
     *   `i` : Property Id
     *   `D` : Value (encoding depends on the property)
     *
     * This command can be sent by the NCP in response to the
     * `CMD_PROP_VALUE_REMOVE` command, or it can be sent by the NCP in an
     * unsolicited fashion to notify the host of various state changes
     * asynchronously.
     *
     * Note that this command operates by value, not by index!
     *
     * The payload for this command is the property identifier encoded in the
     * packed unsigned integer format described in followed by the value that
     * was removed from the given property.
     *
     * If the type signature of the property specified by property id consists
     * of a single structure enclosed by an array (`A(t(...))`), then the
     * contents of value MUST contain the contents of the structure (`...`)
     * rather than the serialization of the whole item (`t(...)`).  Specifically,
     * the length of the structure MUST NOT be prepended to `VALUE`. This
     * helps to eliminate redundant data.
     *
     * The resulting order of items in the list is defined by the given
     * property.
     *
     */
    SPINEL_CMD_PROP_VALUE_REMOVED = 8,

    SPINEL_CMD_NET_SAVE = 9, // Deprecated

    /**
     * Clear saved network settings command (Host -> NCP)
     *
     * Encoding: Empty
     *
     * Erases all network credentials and state from non-volatile memory.
     *
     * This operation affects non-volatile memory only. The current network
     * information stored in volatile memory is unaffected.
     *
     * The response to this command is always a `CMD_PROP_VALUE_IS` for
     * `PROP_LAST_STATUS`, indicating the result of the operation.
     *
     */
    SPINEL_CMD_NET_CLEAR = 10,

    SPINEL_CMD_NET_RECALL = 11, // Deprecated

    /**
     * Host buffer offload is an optional NCP capability that, when
     * present, allows the NCP to store data buffers on the host processor
     * that can be recalled at a later time.
     *
     * The presence of this feature can be detected by the host by
     * checking for the presence of the `CAP_HBO`
     * capability in `PROP_CAPS`.
     *
     * This feature is not currently supported on OpenThread.
     *
     */

    SPINEL_CMD_HBO_OFFLOAD   = 12,
    SPINEL_CMD_HBO_RECLAIM   = 13,
    SPINEL_CMD_HBO_DROP      = 14,
    SPINEL_CMD_HBO_OFFLOADED = 15,
    SPINEL_CMD_HBO_RECLAIMED = 16,
    SPINEL_CMD_HBO_DROPPED   = 17,

    /**
     * Peek command (Host -> NCP)
     *
     * Encoding: `LU`
     *   `L` : The address to peek
     *   `U` : Number of bytes to read
     *
     * This command allows the NCP to fetch values from the RAM of the NCP
     * for debugging purposes. Upon success, `CMD_PEEK_RET` is sent from the
     * NCP to the host. Upon failure, `PROP_LAST_STATUS` is emitted with
     * the appropriate error indication.
     *
     * The NCP MAY prevent certain regions of memory from being accessed.
     *
     * This command requires the capability `CAP_PEEK_POKE` to be present.
     *
     */
    SPINEL_CMD_PEEK = 18,

    /**
     * Peek return command (NCP -> Host)
     *
     * Encoding: `LUD`
     *   `L` : The address peeked
     *   `U` : Number of bytes read
     *   `D` : Memory content
     *
     * This command contains the contents of memory that was requested by
     * a previous call to `CMD_PEEK`.
     *
     * This command requires the capability `CAP_PEEK_POKE` to be present.
     *
     */
    SPINEL_CMD_PEEK_RET = 19,

    /**
     * Poke command (Host -> NCP)
     *
     * Encoding: `LUD`
     *   `L` : The address to be poked
     *   `U` : Number of bytes to write
     *   `D` : Content to write
     *
     * This command writes the bytes to the specified memory address
     * for debugging purposes.
     *
     * This command requires the capability `CAP_PEEK_POKE` to be present.
     *
     */
    SPINEL_CMD_POKE = 20,

    SPINEL_CMD_PROP_VALUE_MULTI_GET = 21,
    SPINEL_CMD_PROP_VALUE_MULTI_SET = 22,
    SPINEL_CMD_PROP_VALUES_ARE      = 23,

    SPINEL_CMD_NEST__BEGIN = 15296,
    SPINEL_CMD_NEST__END   = 15360,

    SPINEL_CMD_VENDOR__BEGIN = 15360,
    SPINEL_CMD_VENDOR__END   = 16384,

    SPINEL_CMD_EXPERIMENTAL__BEGIN = 2000000,
    SPINEL_CMD_EXPERIMENTAL__END   = 2097152,
};

typedef uint32_t spinel_command_t;

enum
{
    SPINEL_CAP_LOCK       = 1,
    SPINEL_CAP_NET_SAVE   = 2,
    SPINEL_CAP_HBO        = 3,
    SPINEL_CAP_POWER_SAVE = 4,

    SPINEL_CAP_COUNTERS   = 5,
    SPINEL_CAP_JAM_DETECT = 6,

    SPINEL_CAP_PEEK_POKE = 7,

    SPINEL_CAP_WRITABLE_RAW_STREAM = 8,
    SPINEL_CAP_GPIO                = 9,
    SPINEL_CAP_TRNG                = 10,
    SPINEL_CAP_CMD_MULTI           = 11,
    SPINEL_CAP_UNSOL_UPDATE_FILTER = 12,
    SPINEL_CAP_MCU_POWER_STATE     = 13,
    SPINEL_CAP_PCAP                = 14,

    SPINEL_CAP_802_15_4__BEGIN        = 16,
    SPINEL_CAP_802_15_4_2003          = (SPINEL_CAP_802_15_4__BEGIN + 0),
    SPINEL_CAP_802_15_4_2006          = (SPINEL_CAP_802_15_4__BEGIN + 1),
    SPINEL_CAP_802_15_4_2011          = (SPINEL_CAP_802_15_4__BEGIN + 2),
    SPINEL_CAP_802_15_4_PIB           = (SPINEL_CAP_802_15_4__BEGIN + 5),
    SPINEL_CAP_802_15_4_2450MHZ_OQPSK = (SPINEL_CAP_802_15_4__BEGIN + 8),
    SPINEL_CAP_802_15_4_915MHZ_OQPSK  = (SPINEL_CAP_802_15_4__BEGIN + 9),
    SPINEL_CAP_802_15_4_868MHZ_OQPSK  = (SPINEL_CAP_802_15_4__BEGIN + 10),
    SPINEL_CAP_802_15_4_915MHZ_BPSK   = (SPINEL_CAP_802_15_4__BEGIN + 11),
    SPINEL_CAP_802_15_4_868MHZ_BPSK   = (SPINEL_CAP_802_15_4__BEGIN + 12),
    SPINEL_CAP_802_15_4_915MHZ_ASK    = (SPINEL_CAP_802_15_4__BEGIN + 13),
    SPINEL_CAP_802_15_4_868MHZ_ASK    = (SPINEL_CAP_802_15_4__BEGIN + 14),
    SPINEL_CAP_802_15_4__END          = 32,

    SPINEL_CAP_CONFIG__BEGIN = 32,
    SPINEL_CAP_CONFIG_FTD    = (SPINEL_CAP_CONFIG__BEGIN + 0),
    SPINEL_CAP_CONFIG_MTD    = (SPINEL_CAP_CONFIG__BEGIN + 1),
    SPINEL_CAP_CONFIG_RADIO  = (SPINEL_CAP_CONFIG__BEGIN + 2),
    SPINEL_CAP_CONFIG__END   = 40,

    SPINEL_CAP_ROLE__BEGIN = 48,
    SPINEL_CAP_ROLE_ROUTER = (SPINEL_CAP_ROLE__BEGIN + 0),
    SPINEL_CAP_ROLE_SLEEPY = (SPINEL_CAP_ROLE__BEGIN + 1),
    SPINEL_CAP_ROLE__END   = 52,

    SPINEL_CAP_NET__BEGIN     = 52,
    SPINEL_CAP_NET_THREAD_1_0 = (SPINEL_CAP_NET__BEGIN + 0),
    SPINEL_CAP_NET_THREAD_1_1 = (SPINEL_CAP_NET__BEGIN + 1),
    SPINEL_CAP_NET_THREAD_1_2 = (SPINEL_CAP_NET__BEGIN + 2),
    SPINEL_CAP_NET__END       = 64,

    SPINEL_CAP_RCP__BEGIN      = 64,
    SPINEL_CAP_RCP_API_VERSION = (SPINEL_CAP_RCP__BEGIN + 0),
    SPINEL_CAP_RCP__END        = 80,

    SPINEL_CAP_OPENTHREAD__BEGIN       = 512,
    SPINEL_CAP_MAC_ALLOWLIST           = (SPINEL_CAP_OPENTHREAD__BEGIN + 0),
    SPINEL_CAP_MAC_RAW                 = (SPINEL_CAP_OPENTHREAD__BEGIN + 1),
    SPINEL_CAP_OOB_STEERING_DATA       = (SPINEL_CAP_OPENTHREAD__BEGIN + 2),
    SPINEL_CAP_CHANNEL_MONITOR         = (SPINEL_CAP_OPENTHREAD__BEGIN + 3),
    SPINEL_CAP_ERROR_RATE_TRACKING     = (SPINEL_CAP_OPENTHREAD__BEGIN + 4),
    SPINEL_CAP_CHANNEL_MANAGER         = (SPINEL_CAP_OPENTHREAD__BEGIN + 5),
    SPINEL_CAP_OPENTHREAD_LOG_METADATA = (SPINEL_CAP_OPENTHREAD__BEGIN + 6),
    SPINEL_CAP_TIME_SYNC               = (SPINEL_CAP_OPENTHREAD__BEGIN + 7),
    SPINEL_CAP_CHILD_SUPERVISION       = (SPINEL_CAP_OPENTHREAD__BEGIN + 8),
    SPINEL_CAP_POSIX                   = (SPINEL_CAP_OPENTHREAD__BEGIN + 9),
    SPINEL_CAP_SLAAC                   = (SPINEL_CAP_OPENTHREAD__BEGIN + 10),
    SPINEL_CAP_RADIO_COEX              = (SPINEL_CAP_OPENTHREAD__BEGIN + 11),
    SPINEL_CAP_MAC_RETRY_HISTOGRAM     = (SPINEL_CAP_OPENTHREAD__BEGIN + 12),
    SPINEL_CAP_MULTI_RADIO             = (SPINEL_CAP_OPENTHREAD__BEGIN + 13),
    SPINEL_CAP_SRP_CLIENT              = (SPINEL_CAP_OPENTHREAD__BEGIN + 14),
    SPINEL_CAP_DUA                     = (SPINEL_CAP_OPENTHREAD__BEGIN + 15),
    SPINEL_CAP_REFERENCE_DEVICE        = (SPINEL_CAP_OPENTHREAD__BEGIN + 16),
    SPINEL_CAP_OPENTHREAD__END         = 640,

    SPINEL_CAP_THREAD__BEGIN          = 1024,
    SPINEL_CAP_THREAD_COMMISSIONER    = (SPINEL_CAP_THREAD__BEGIN + 0),
    SPINEL_CAP_THREAD_TMF_PROXY       = (SPINEL_CAP_THREAD__BEGIN + 1),
    SPINEL_CAP_THREAD_UDP_FORWARD     = (SPINEL_CAP_THREAD__BEGIN + 2),
    SPINEL_CAP_THREAD_JOINER          = (SPINEL_CAP_THREAD__BEGIN + 3),
    SPINEL_CAP_THREAD_BORDER_ROUTER   = (SPINEL_CAP_THREAD__BEGIN + 4),
    SPINEL_CAP_THREAD_SERVICE         = (SPINEL_CAP_THREAD__BEGIN + 5),
    SPINEL_CAP_THREAD_CSL_RECEIVER    = (SPINEL_CAP_THREAD__BEGIN + 6),
    SPINEL_CAP_THREAD_LINK_METRICS    = (SPINEL_CAP_THREAD__BEGIN + 7),
    SPINEL_CAP_THREAD_BACKBONE_ROUTER = (SPINEL_CAP_THREAD__BEGIN + 8),
    SPINEL_CAP_THREAD__END            = 1152,

    SPINEL_CAP_NEST__BEGIN           = 15296,
    SPINEL_CAP_NEST_LEGACY_INTERFACE = (SPINEL_CAP_NEST__BEGIN + 0),
    SPINEL_CAP_NEST_LEGACY_NET_WAKE  = (SPINEL_CAP_NEST__BEGIN + 1),
    SPINEL_CAP_NEST_TRANSMIT_HOOK    = (SPINEL_CAP_NEST__BEGIN + 2),
    SPINEL_CAP_NEST__END             = 15360,

    SPINEL_CAP_VENDOR__BEGIN = 15360,
    SPINEL_CAP_VENDOR__END   = 16384,

    SPINEL_CAP_EXPERIMENTAL__BEGIN = 2000000,
    SPINEL_CAP_EXPERIMENTAL__END   = 2097152,
};

typedef uint32_t spinel_capability_t;

/**
 * Property Keys
 *
 * The properties are broken up into several sections, each with a
 * reserved ranges of property identifiers:
 *
 *    Name         | Range (Inclusive)              | Description
 *    -------------|--------------------------------|------------------------
 *    Core         | 0x000 - 0x01F, 0x1000 - 0x11FF | Spinel core
 *    PHY          | 0x020 - 0x02F, 0x1200 - 0x12FF | Radio PHY layer
 *    MAC          | 0x030 - 0x03F, 0x1300 - 0x13FF | MAC layer
 *    NET          | 0x040 - 0x04F, 0x1400 - 0x14FF | Network
 *    Thread       | 0x050 - 0x05F, 0x1500 - 0x15FF | Thread
 *    IPv6         | 0x060 - 0x06F, 0x1600 - 0x16FF | IPv6
 *    Stream       | 0x070 - 0x07F, 0x1700 - 0x17FF | Stream
 *    MeshCop      | 0x080 - 0x08F, 0x1800 - 0x18FF | Thread Mesh Commissioning
 *    OpenThread   |                0x1900 - 0x19FF | OpenThread specific
 *    Server       | 0x0A0 - 0x0AF                  | ALOC Service Server
 *    RCP          | 0x0B0 - 0x0FF                  | RCP specific
 *    Interface    | 0x100 - 0x1FF                  | Interface (e.g., UART)
 *    PIB          | 0x400 - 0x4FF                  | 802.15.4 PIB
 *    Counter      | 0x500 - 0x7FF                  | Counters (MAC, IP, etc).
 *    RCP          | 0x800 - 0x8FF                  | RCP specific property (extended)
 *    Nest         |                0x3BC0 - 0x3BFF | Nest (legacy)
 *    Vendor       |                0x3C00 - 0x3FFF | Vendor specific
 *    Debug        |                0x4000 - 0x43FF | Debug related
 *    Experimental |          2,000,000 - 2,097,151 | Experimental use only
 *
 */
enum
{
    /// Last Operation Status
    /** Format: `i` - Read-only
     *
     * Describes the status of the last operation. Encoded as a packed
     * unsigned integer (see `SPINEL_STATUS_*` for list of values).
     *
     * This property is emitted often to indicate the result status of
     * pretty much any Host-to-NCP operation.
     *
     * It is emitted automatically at NCP startup with a value indicating
     * the reset reason. It is also emitted asynchronously on an error (
     * e.g., NCP running out of buffer).
     *
     */
    SPINEL_PROP_LAST_STATUS = 0,

    /// Protocol Version
    /** Format: `ii` - Read-only
     *
     * Describes the protocol version information. This property contains
     * two fields, each encoded as a packed unsigned integer:
     *   `i`: Major Version Number
     *   `i`: Minor Version Number
     *
     * The version number is defined by `SPINEL_PROTOCOL_VERSION_THREAD_MAJOR`
     * and `SPINEL_PROTOCOL_VERSION_THREAD_MINOR`.
     *
     * This specification describes major version 4, minor version 3.
     *
     */
    SPINEL_PROP_PROTOCOL_VERSION = 1,

    /// NCP Version
    /** Format: `U` - Read-only
     *
     * Contains a string which describes the firmware currently running on
     * the NCP. Encoded as a zero-terminated UTF-8 string.
     *
     */
    SPINEL_PROP_NCP_VERSION = 2,

    /// NCP Network Protocol Type
    /** Format: 'i' - Read-only
     *
     * This value identifies what the network protocol for this NCP.
     * The valid protocol type values are defined by enumeration
     * `SPINEL_PROTOCOL_TYPE_*`:
     *
     *   `SPINEL_PROTOCOL_TYPE_BOOTLOADER` = 0
     *   `SPINEL_PROTOCOL_TYPE_ZIGBEE_IP`  = 2,
     *   `SPINEL_PROTOCOL_TYPE_THREAD`     = 3,
     *
     * OpenThread NCP supports only `SPINEL_PROTOCOL_TYPE_THREAD`
     *
     */
    SPINEL_PROP_INTERFACE_TYPE = 3,

    /// NCP Vendor ID
    /** Format: 'i` - Read-only
     *
     * Vendor ID. Zero for unknown.
     *
     */
    SPINEL_PROP_VENDOR_ID = 4,

    /// NCP Capability List
    /** Format: 'A(i)` - Read-only
     *
     * Describes the supported capabilities of this NCP. Encoded as a list of
     * packed unsigned integers.
     *
     * The capability values are specified by SPINEL_CAP_* enumeration.
     *
     */
    SPINEL_PROP_CAPS = 5,

    /// NCP Interface Count
    /** Format: 'C` - Read-only
     *
     * Provides number of interfaces.
     *
     * Currently always reads as 1.
     *
     */
    SPINEL_PROP_INTERFACE_COUNT = 6,

    SPINEL_PROP_POWER_STATE = 7, ///< PowerState [C] (deprecated, use `MCU_POWER_STATE` instead).

    /// NCP Hardware Address
    /** Format: 'E` - Read-only
     *
     * The static EUI64 address of the device, used as a serial number.
     *
     */
    SPINEL_PROP_HWADDR = 8,

    SPINEL_PROP_LOCK          = 9,  ///< PropLock [b] (not supported)
    SPINEL_PROP_HBO_MEM_MAX   = 10, ///< Max offload mem [S] (not supported)
    SPINEL_PROP_HBO_BLOCK_MAX = 11, ///< Max offload block [S] (not supported)

    /// Host Power State
    /** Format: 'C`
     *
     * Describes the current power state of the host. This property is used
     * by the host to inform the NCP when it has changed power states. The
     * NCP can then use this state to determine which properties need
     * asynchronous updates. Enumeration `spinel_host_power_state_t` defines
     * the valid values (`SPINEL_HOST_POWER_STATE_*`):
     *
     *   `HOST_POWER_STATE_OFFLINE`: Host is physically powered off and
     *   cannot be woken by the NCP. All asynchronous commands are
     *   squelched.
     *
     *   `HOST_POWER_STATE_DEEP_SLEEP`: The host is in a low power state
     *   where it can be woken by the NCP but will potentially require more
     *   than two seconds to become fully responsive. The NCP MUST
     *   avoid sending unnecessary property updates, such as child table
     *   updates or non-critical messages on the debug stream. If the NCP
     *   needs to wake the host for traffic, the NCP MUST first take
     *   action to wake the host. Once the NCP signals to the host that it
     *   should wake up, the NCP MUST wait for some activity from the
     *   host (indicating that it is fully awake) before sending frames.
     *
     *   `HOST_POWER_STATE_RESERVED`:  This value MUST NOT be set by the host. If
     *   received by the NCP, the NCP SHOULD consider this as a synonym
     *   of `HOST_POWER_STATE_DEEP_SLEEP`.
     *
     *   `HOST_POWER_STATE_LOW_POWER`: The host is in a low power state
     *   where it can be immediately woken by the NCP. The NCP SHOULD
     *   avoid sending unnecessary property updates, such as child table
     *   updates or non-critical messages on the debug stream.
     *
     *   `HOST_POWER_STATE_ONLINE`: The host is awake and responsive. No
     *   special filtering is performed by the NCP on asynchronous updates.
     *
     *   All other values are RESERVED. They MUST NOT be set by the
     *   host. If received by the NCP, the NCP SHOULD consider the value as
     *   a synonym of `HOST_POWER_STATE_LOW_POWER`.
     *
     * After setting this power state, any further commands from the host to
     * the NCP will cause `HOST_POWER_STATE` to automatically revert to
     * `HOST_POWER_STATE_ONLINE`.
     *
     * When the host is entering a low-power state, it should wait for the
     * response from the NCP acknowledging the command (with `CMD_VALUE_IS`).
     * Once that acknowledgment is received the host may enter the low-power
     * state.
     *
     * If the NCP has the `CAP_UNSOL_UPDATE_FILTER` capability, any unsolicited
     * property updates masked by `PROP_UNSOL_UPDATE_FILTER` should be honored
     * while the host indicates it is in a low-power state. After resuming to the
     * `HOST_POWER_STATE_ONLINE` state, the value of `PROP_UNSOL_UPDATE_FILTER`
     * MUST be unchanged from the value assigned prior to the host indicating
     * it was entering a low-power state.
     *
     */
    SPINEL_PROP_HOST_POWER_STATE = 12,

    /// NCP's MCU Power State
    /** Format: 'C`
     *  Required capability: CAP_MCU_POWER_SAVE
     *
     * This property specifies the desired power state of NCP's micro-controller
     * (MCU) when the underlying platform's operating system enters idle mode (i.e.,
     * all active tasks/events are processed and the MCU can potentially enter a
     * energy-saving power state).
     *
     * The power state primarily determines how the host should interact with the NCP
     * and whether the host needs an external trigger (a "poke") to NCP before it can
     * communicate with the NCP or not. After a reset, the MCU power state MUST be
     * SPINEL_MCU_POWER_STATE_ON.
     *
     * Enumeration `spinel_mcu_power_state_t` defines the valid values
     * (`SPINEL_MCU_POWER_STATE_*` constants):
     *
     *   `SPINEL_MCU_POWER_STATE_ON`: NCP's MCU stays on and active all the time.
     *   When the NCP's desired power state is set to this value, host can send
     *   messages to NCP without requiring any "poke" or external triggers. MCU is
     *   expected to stay on and active. Note that the `ON` power state only
     *   determines the MCU's power mode and is not related to radio's state.
     *
     *   `SPINEL_MCU_POWER_STATE_LOW_POWER`: NCP's MCU can enter low-power
     *   (energy-saving) state. When the NCP's desired power state is set to
     *   `LOW_POWER`, host is expected to "poke" the NCP (e.g., an external trigger
     *   like an interrupt) before it can communicate with the NCP (send a message
     *   to the NCP). The "poke" mechanism is determined by the platform code (based
     *   on NCP's interface to the host).
     *   While power state is set to `LOW_POWER`, NCP can still (at any time) send
     *   messages to host. Note that receiving a message from the NCP does NOT
     *   indicate that the NCP's power state has changed, i.e., host is expected to
     *   continue to "poke" NCP when it wants to talk to the NCP until the power
     *   state is explicitly changed (by setting this property to `ON`).
     *   Note that the `LOW_POWER` power state only determines the MCU's power mode
     *   and is not related to radio's state.
     *
     *   `SPINEL_MCU_POWER_STATE_OFF`: NCP is fully powered off.
     *   An NCP hardware reset (via a RESET pin) is required to bring the NCP back
     *   to `SPINEL_MCU_POWER_STATE_ON`. RAM is not retained after reset.
     *
     */
    SPINEL_PROP_MCU_POWER_STATE = 13,

    SPINEL_PROP_BASE_EXT__BEGIN = 0x1000,

    /// GPIO Configuration
    /** Format: `A(CCU)`
     *  Type: Read-Only (Optionally Read-write using `CMD_PROP_VALUE_INSERT`)
     *
     * An array of structures which contain the following fields:
     *
     * *   `C`: GPIO Number
     * *   `C`: GPIO Configuration Flags
     * *   `U`: Human-readable GPIO name
     *
     * GPIOs which do not have a corresponding entry are not supported.
     *
     * The configuration parameter contains the configuration flags for the
     * GPIO:
     *
     *       0   1   2   3   4   5   6   7
     *     +---+---+---+---+---+---+---+---+
     *     |DIR|PUP|PDN|TRIGGER|  RESERVED |
     *     +---+---+---+---+---+---+---+---+
     *             |O/D|
     *             +---+
     *
     * *   `DIR`: Pin direction. Clear (0) for input, set (1) for output.
     * *   `PUP`: Pull-up enabled flag.
     * *   `PDN`/`O/D`: Flag meaning depends on pin direction:
     *     *   Input: Pull-down enabled.
     *     *   Output: Output is an open-drain.
     * *   `TRIGGER`: Enumeration describing how pin changes generate
     *     asynchronous notification commands (TBD) from the NCP to the host.
     *     *   0: Feature disabled for this pin
     *     *   1: Trigger on falling edge
     *     *   2: Trigger on rising edge
     *     *   3: Trigger on level change
     * *   `RESERVED`: Bits reserved for future use. Always cleared to zero
     *     and ignored when read.
     *
     * As an optional feature, the configuration of individual pins may be
     * modified using the `CMD_PROP_VALUE_INSERT` command. Only the GPIO
     * number and flags fields MUST be present, the GPIO name (if present)
     * would be ignored. This command can only be used to modify the
     * configuration of GPIOs which are already exposed---it cannot be used
     * by the host to add additional GPIOs.
     */
    SPINEL_PROP_GPIO_CONFIG = SPINEL_PROP_BASE_EXT__BEGIN + 0,

    /// GPIO State Bitmask
    /** Format: `D`
     *  Type: Read-Write
     *
     * Contains a bit field identifying the state of the GPIOs. The length of
     * the data associated with these properties depends on the number of
     * GPIOs. If you have 10 GPIOs, you'd have two bytes. GPIOs are numbered
     * from most significant bit to least significant bit, so 0x80 is GPIO 0,
     * 0x40 is GPIO 1, etc.
     *
     * For GPIOs configured as inputs:
     *
     * *   `CMD_PROP_VAUE_GET`: The value of the associated bit describes the
     *     logic level read from the pin.
     * *   `CMD_PROP_VALUE_SET`: The value of the associated bit is ignored
     *     for these pins.
     *
     * For GPIOs configured as outputs:
     *
     * *   `CMD_PROP_VAUE_GET`: The value of the associated bit is
     *     implementation specific.
     * *   `CMD_PROP_VALUE_SET`: The value of the associated bit determines
     *     the new logic level of the output. If this pin is configured as an
     *     open-drain, setting the associated bit to 1 will cause the pin to
     *     enter a Hi-Z state.
     *
     * For GPIOs which are not specified in `PROP_GPIO_CONFIG`:
     *
     * *   `CMD_PROP_VAUE_GET`: The value of the associated bit is
     *     implementation specific.
     * *   `CMD_PROP_VALUE_SET`: The value of the associated bit MUST be
     *     ignored by the NCP.
     *
     * When writing, unspecified bits are assumed to be zero.
     */
    SPINEL_PROP_GPIO_STATE = SPINEL_PROP_BASE_EXT__BEGIN + 2,

    /// GPIO State Set-Only Bitmask
    /** Format: `D`
     *  Type: Write-Only
     *
     * Allows for the state of various output GPIOs to be set without affecting
     * other GPIO states. Contains a bit field identifying the output GPIOs that
     * should have their state set to 1.
     *
     * When writing, unspecified bits are assumed to be zero. The value of
     * any bits for GPIOs which are not specified in `PROP_GPIO_CONFIG` MUST
     * be ignored.
     */
    SPINEL_PROP_GPIO_STATE_SET = SPINEL_PROP_BASE_EXT__BEGIN + 3,

    /// GPIO State Clear-Only Bitmask
    /** Format: `D`
     *  Type: Write-Only
     *
     * Allows for the state of various output GPIOs to be cleared without affecting
     * other GPIO states. Contains a bit field identifying the output GPIOs that
     * should have their state cleared to 0.
     *
     * When writing, unspecified bits are assumed to be zero. The value of
     * any bits for GPIOs which are not specified in `PROP_GPIO_CONFIG` MUST
     * be ignored.
     */
    SPINEL_PROP_GPIO_STATE_CLEAR = SPINEL_PROP_BASE_EXT__BEGIN + 4,

    /// 32-bit random number from TRNG, ready-to-use.
    SPINEL_PROP_TRNG_32 = SPINEL_PROP_BASE_EXT__BEGIN + 5,

    /// 16 random bytes from TRNG, ready-to-use.
    SPINEL_PROP_TRNG_128 = SPINEL_PROP_BASE_EXT__BEGIN + 6,

    /// Raw samples from TRNG entropy source representing 32 bits of entropy.
    SPINEL_PROP_TRNG_RAW_32 = SPINEL_PROP_BASE_EXT__BEGIN + 7,

    /// NCP Unsolicited update filter
    /** Format: `A(I)`
     *  Type: Read-Write (optional Insert-Remove)
     *  Required capability: `CAP_UNSOL_UPDATE_FILTER`
     *
     * Contains a list of properties which are excluded from generating
     * unsolicited value updates. This property is empty after reset.
     * In other words, the host may opt-out of unsolicited property updates
     * for a specific property by adding that property id to this list.
     * Hosts SHOULD NOT add properties to this list which are not
     * present in `PROP_UNSOL_UPDATE_LIST`. If such properties are added,
     * the NCP ignores the unsupported properties.
     *
     */
    SPINEL_PROP_UNSOL_UPDATE_FILTER = SPINEL_PROP_BASE_EXT__BEGIN + 8,

    /// List of properties capable of generating unsolicited value update.
    /** Format: `A(I)`
     *  Type: Read-Only
     *  Required capability: `CAP_UNSOL_UPDATE_FILTER`
     *
     * Contains a list of properties which are capable of generating
     * unsolicited value updates. This list can be used when populating
     * `PROP_UNSOL_UPDATE_FILTER` to disable all unsolicited property
     * updates.
     *
     * This property is intended to effectively behave as a constant
     * for a given NCP firmware.
     */
    SPINEL_PROP_UNSOL_UPDATE_LIST = SPINEL_PROP_BASE_EXT__BEGIN + 9,

    SPINEL_PROP_BASE_EXT__END = 0x1100,

    SPINEL_PROP_PHY__BEGIN         = 0x20,
    SPINEL_PROP_PHY_ENABLED        = SPINEL_PROP_PHY__BEGIN + 0,  ///< [b]
    SPINEL_PROP_PHY_CHAN           = SPINEL_PROP_PHY__BEGIN + 1,  ///< [C]
    SPINEL_PROP_PHY_CHAN_SUPPORTED = SPINEL_PROP_PHY__BEGIN + 2,  ///< [A(C)]
    SPINEL_PROP_PHY_FREQ           = SPINEL_PROP_PHY__BEGIN + 3,  ///< kHz [L]
    SPINEL_PROP_PHY_CCA_THRESHOLD  = SPINEL_PROP_PHY__BEGIN + 4,  ///< dBm [c]
    SPINEL_PROP_PHY_TX_POWER       = SPINEL_PROP_PHY__BEGIN + 5,  ///< [c]
    SPINEL_PROP_PHY_RSSI           = SPINEL_PROP_PHY__BEGIN + 6,  ///< dBm [c]
    SPINEL_PROP_PHY_RX_SENSITIVITY = SPINEL_PROP_PHY__BEGIN + 7,  ///< dBm [c]
    SPINEL_PROP_PHY_PCAP_ENABLED   = SPINEL_PROP_PHY__BEGIN + 8,  ///< [b]
    SPINEL_PROP_PHY_CHAN_PREFERRED = SPINEL_PROP_PHY__BEGIN + 9,  ///< [A(C)]
    SPINEL_PROP_PHY_FEM_LNA_GAIN   = SPINEL_PROP_PHY__BEGIN + 10, ///< dBm [c]

    /// Signal the max power for a channel
    /** Format: `Cc`
     *
     * First byte is the channel then the max transmit power, write-only.
     */
    SPINEL_PROP_PHY_CHAN_MAX_POWER = SPINEL_PROP_PHY__BEGIN + 11,
    /// Region code
    /** Format: `S`
     *
     * The ascii representation of the ISO 3166 alpha-2 code.
     *
     */
    SPINEL_PROP_PHY_REGION_CODE = SPINEL_PROP_PHY__BEGIN + 12,

    SPINEL_PROP_PHY__END = 0x30,

    SPINEL_PROP_PHY_EXT__BEGIN = 0x1200,

    /// Signal Jamming Detection Enable
    /** Format: `b`
     *
     * Indicates if jamming detection is enabled or disabled. Set to true
     * to enable jamming detection.
     */
    SPINEL_PROP_JAM_DETECT_ENABLE = SPINEL_PROP_PHY_EXT__BEGIN + 0,

    /// Signal Jamming Detected Indicator
    /** Format: `b` (Read-Only)
     *
     * Set to true if radio jamming is detected. Set to false otherwise.
     *
     * When jamming detection is enabled, changes to the value of this
     * property are emitted asynchronously via `CMD_PROP_VALUE_IS`.
     */
    SPINEL_PROP_JAM_DETECTED = SPINEL_PROP_PHY_EXT__BEGIN + 1,

    /// Jamming detection RSSI threshold
    /** Format: `c`
     *  Units: dBm
     *
     * This parameter describes the threshold RSSI level (measured in
     * dBm) above which the jamming detection will consider the
     * channel blocked.
     */
    SPINEL_PROP_JAM_DETECT_RSSI_THRESHOLD = SPINEL_PROP_PHY_EXT__BEGIN + 2,

    /// Jamming detection window size
    /** Format: `C`
     *  Units: Seconds (1-63)
     *
     * This parameter describes the window period for signal jamming
     * detection.
     */
    SPINEL_PROP_JAM_DETECT_WINDOW = SPINEL_PROP_PHY_EXT__BEGIN + 3,

    /// Jamming detection busy period
    /** Format: `C`
     *  Units: Seconds (1-63)
     *
     * This parameter describes the number of aggregate seconds within
     * the detection window where the RSSI must be above
     * `PROP_JAM_DETECT_RSSI_THRESHOLD` to trigger detection.
     *
     * The behavior of the jamming detection feature when `PROP_JAM_DETECT_BUSY`
     * is larger than `PROP_JAM_DETECT_WINDOW` is undefined.
     */
    SPINEL_PROP_JAM_DETECT_BUSY = SPINEL_PROP_PHY_EXT__BEGIN + 4,

    /// Jamming detection history bitmap (for debugging)
    /** Format: `X` (read-only)
     *
     * This value provides information about current state of jamming detection
     * module for monitoring/debugging purpose. It returns a 64-bit value where
     * each bit corresponds to one second interval starting with bit 0 for the
     * most recent interval and bit 63 for the oldest intervals (63 sec earlier).
     * The bit is set to 1 if the jamming detection module observed/detected
     * high signal level during the corresponding one second interval.
     *
     */
    SPINEL_PROP_JAM_DETECT_HISTORY_BITMAP = SPINEL_PROP_PHY_EXT__BEGIN + 5,

    /// Channel monitoring sample interval
    /** Format: `L` (read-only)
     *  Units: Milliseconds
     *
     * Required capability: SPINEL_CAP_CHANNEL_MONITOR
     *
     * If channel monitoring is enabled and active, every sample interval, a
     * zero-duration Energy Scan is performed, collecting a single RSSI sample
     * per channel. The RSSI samples are compared with a pre-specified RSSI
     * threshold.
     *
     */
    SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_INTERVAL = SPINEL_PROP_PHY_EXT__BEGIN + 6,

    /// Channel monitoring RSSI threshold
    /** Format: `c` (read-only)
     *  Units: dBm
     *
     * Required capability: SPINEL_CAP_CHANNEL_MONITOR
     *
     * This value specifies the threshold used by channel monitoring module.
     * Channel monitoring maintains the average rate of RSSI samples that
     * are above the threshold within (approximately) a pre-specified number
     * of samples (sample window).
     *
     */
    SPINEL_PROP_CHANNEL_MONITOR_RSSI_THRESHOLD = SPINEL_PROP_PHY_EXT__BEGIN + 7,

    /// Channel monitoring sample window
    /** Format: `L` (read-only)
     *  Units: Number of samples
     *
     * Required capability: SPINEL_CAP_CHANNEL_MONITOR
     *
     * The averaging sample window length (in units of number of channel
     * samples) used by channel monitoring module. Channel monitoring will
     * sample all channels every sample interval. It maintains the average rate
     * of RSSI samples that are above the RSSI threshold within (approximately)
     * the sample window.
     *
     */
    SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_WINDOW = SPINEL_PROP_PHY_EXT__BEGIN + 8,

    /// Channel monitoring sample count
    /** Format: `L` (read-only)
     *  Units: Number of samples
     *
     * Required capability: SPINEL_CAP_CHANNEL_MONITOR
     *
     * Total number of RSSI samples (per channel) taken by the channel
     * monitoring module since its start (since Thread network interface
     * was enabled).
     *
     */
    SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_COUNT = SPINEL_PROP_PHY_EXT__BEGIN + 9,

    /// Channel monitoring channel occupancy
    /** Format: `A(t(CU))` (read-only)
     *
     * Required capability: SPINEL_CAP_CHANNEL_MONITOR
     *
     * Data per item is:
     *
     *  `C`: Channel
     *  `U`: Channel occupancy indicator
     *
     * The channel occupancy value represents the average rate/percentage of
     * RSSI samples that were above RSSI threshold ("bad" RSSI samples) within
     * (approximately) sample window latest RSSI samples.
     *
     * Max value of `0xffff` indicates all RSSI samples were above RSSI
     * threshold (i.e. 100% of samples were "bad").
     *
     */
    SPINEL_PROP_CHANNEL_MONITOR_CHANNEL_OCCUPANCY = SPINEL_PROP_PHY_EXT__BEGIN + 10,

    /// Radio caps
    /** Format: `i` (read-only)
     *
     * Data per item is:
     *
     *  `i`: Radio Capabilities.
     *
     */
    SPINEL_PROP_RADIO_CAPS = SPINEL_PROP_PHY_EXT__BEGIN + 11,

    /// All coex metrics related counters.
    /** Format: t(LLLLLLLL)t(LLLLLLLLL)bL  (Read-only)
     *
     * Required capability: SPINEL_CAP_RADIO_COEX
     *
     * The contents include two structures and two common variables, first structure corresponds to
     * all transmit related coex counters, second structure provides the receive related counters.
     *
     * The transmit structure includes:
     *   'L': NumTxRequest                       (The number of tx requests).
     *   'L': NumTxGrantImmediate                (The number of tx requests while grant was active).
     *   'L': NumTxGrantWait                     (The number of tx requests while grant was inactive).
     *   'L': NumTxGrantWaitActivated            (The number of tx requests while grant was inactive that were
     *                                            ultimately granted).
     *   'L': NumTxGrantWaitTimeout              (The number of tx requests while grant was inactive that timed out).
     *   'L': NumTxGrantDeactivatedDuringRequest (The number of tx requests that were in progress when grant was
     *                                            deactivated).
     *   'L': NumTxDelayedGrant                  (The number of tx requests that were not granted within 50us).
     *   'L': AvgTxRequestToGrantTime            (The average time in usec from tx request to grant).
     *
     * The receive structure includes:
     *   'L': NumRxRequest                       (The number of rx requests).
     *   'L': NumRxGrantImmediate                (The number of rx requests while grant was active).
     *   'L': NumRxGrantWait                     (The number of rx requests while grant was inactive).
     *   'L': NumRxGrantWaitActivated            (The number of rx requests while grant was inactive that were
     *                                            ultimately granted).
     *   'L': NumRxGrantWaitTimeout              (The number of rx requests while grant was inactive that timed out).
     *   'L': NumRxGrantDeactivatedDuringRequest (The number of rx requests that were in progress when grant was
     *                                            deactivated).
     *   'L': NumRxDelayedGrant                  (The number of rx requests that were not granted within 50us).
     *   'L': AvgRxRequestToGrantTime            (The average time in usec from rx request to grant).
     *   'L': NumRxGrantNone                     (The number of rx requests that completed without receiving grant).
     *
     * Two common variables:
     *   'b': Stopped        (Stats collection stopped due to saturation).
     *   'L': NumGrantGlitch (The number of of grant glitches).
     */
    SPINEL_PROP_RADIO_COEX_METRICS = SPINEL_PROP_PHY_EXT__BEGIN + 12,

    /// Radio Coex Enable
    /** Format: `b`
     *
     * Required capability: SPINEL_CAP_RADIO_COEX
     *
     * Indicates if radio coex is enabled or disabled. Set to true to enable radio coex.
     */
    SPINEL_PROP_RADIO_COEX_ENABLE = SPINEL_PROP_PHY_EXT__BEGIN + 13,

    SPINEL_PROP_PHY_EXT__END = 0x1300,

    SPINEL_PROP_MAC__BEGIN = 0x30,

    /// MAC Scan State
    /** Format: `C`
     *
     * Possible values are from enumeration `spinel_scan_state_t`.
     *
     *   SCAN_STATE_IDLE
     *   SCAN_STATE_BEACON
     *   SCAN_STATE_ENERGY
     *   SCAN_STATE_DISCOVER
     *
     * Set to `SCAN_STATE_BEACON` to start an active scan.
     * Beacons will be emitted from `PROP_MAC_SCAN_BEACON`.
     *
     * Set to `SCAN_STATE_ENERGY` to start an energy scan.
     * Channel energy result will be reported by emissions
     * of `PROP_MAC_ENERGY_SCAN_RESULT` (per channel).
     *
     * Set to `SCAN_STATE_DISOVER` to start a Thread MLE discovery
     * scan operation. Discovery scan result will be emitted from
     * `PROP_MAC_SCAN_BEACON`.
     *
     * Value switches to `SCAN_STATE_IDLE` when scan is complete.
     *
     */
    SPINEL_PROP_MAC_SCAN_STATE = SPINEL_PROP_MAC__BEGIN + 0,

    /// MAC Scan Channel Mask
    /** Format: `A(C)`
     *
     * List of channels to scan.
     *
     */
    SPINEL_PROP_MAC_SCAN_MASK = SPINEL_PROP_MAC__BEGIN + 1,

    /// MAC Scan Channel Period
    /** Format: `S`
     *  Unit: milliseconds per channel
     *
     */
    SPINEL_PROP_MAC_SCAN_PERIOD = SPINEL_PROP_MAC__BEGIN + 2,

    /// MAC Scan Beacon
    /** Format `Cct(ESSc)t(iCUdd)` - Asynchronous event only
     *
     * Scan beacons have two embedded structures which contain
     * information about the MAC layer and the NET layer. Their
     * format depends on the MAC and NET layer currently in use.
     * The format below is for an 802.15.4 MAC with Thread:
     *
     *  `C`: Channel
     *  `c`: RSSI of the beacon
     *  `t`: MAC layer properties (802.15.4 layer)
     *    `E`: Long address
     *    `S`: Short address
     *    `S`: PAN-ID
     *    `c`: LQI
     *  NET layer properties
     *    `i`: Protocol Number (SPINEL_PROTOCOL_TYPE_* values)
     *    `C`: Flags (SPINEL_BEACON_THREAD_FLAG_* values)
     *    `U`: Network Name
     *    `d`: XPANID
     *    `d`: Steering data
     *
     * Extra parameters may be added to each of the structures
     * in the future, so care should be taken to read the length
     * that prepends each structure.
     *
     */
    SPINEL_PROP_MAC_SCAN_BEACON = SPINEL_PROP_MAC__BEGIN + 3,

    /// MAC Long Address
    /** Format: `E`
     *
     * The 802.15.4 long address of this node.
     *
     */
    SPINEL_PROP_MAC_15_4_LADDR = SPINEL_PROP_MAC__BEGIN + 4,

    /// MAC Short Address
    /** Format: `S`
     *
     * The 802.15.4 short address of this node.
     *
     */
    SPINEL_PROP_MAC_15_4_SADDR = SPINEL_PROP_MAC__BEGIN + 5,

    /// MAC PAN ID
    /** Format: `S`
     *
     * The 802.15.4 PANID this node is associated with.
     *
     */
    SPINEL_PROP_MAC_15_4_PANID = SPINEL_PROP_MAC__BEGIN + 6,

    /// MAC Stream Raw Enabled
    /** Format: `b`
     *
     * Set to true to enable raw MAC frames to be emitted from
     * `PROP_STREAM_RAW`.
     *
     */
    SPINEL_PROP_MAC_RAW_STREAM_ENABLED = SPINEL_PROP_MAC__BEGIN + 7,

    /// MAC Promiscuous Mode
    /** Format: `C`
     *
     * Possible values are from enumeration
     * `SPINEL_MAC_PROMISCUOUS_MODE_*`:
     *
     *   `SPINEL_MAC_PROMISCUOUS_MODE_OFF`
     *        Normal MAC filtering is in place.
     *
     *   `SPINEL_MAC_PROMISCUOUS_MODE_NETWORK`
     *        All MAC packets matching network are passed up
     *        the stack.
     *
     *   `SPINEL_MAC_PROMISCUOUS_MODE_FULL`
     *        All decoded MAC packets are passed up the stack.
     *
     */
    SPINEL_PROP_MAC_PROMISCUOUS_MODE = SPINEL_PROP_MAC__BEGIN + 8,

    /// MAC Energy Scan Result
    /** Format: `Cc` - Asynchronous event only
     *
     * This property is emitted during energy scan operation
     * per scanned channel with following format:
     *
     *   `C`: Channel
     *   `c`: RSSI (in dBm)
     *
     */
    SPINEL_PROP_MAC_ENERGY_SCAN_RESULT = SPINEL_PROP_MAC__BEGIN + 9,

    /// MAC Data Poll Period
    /** Format: `L`
     *  Unit: millisecond
     * The (user-specified) data poll (802.15.4 MAC Data Request) period
     * in milliseconds. Value zero means there is no user-specified
     * poll period, and the network stack determines the maximum period
     * based on the MLE Child Timeout.
     *
     * If the value is non-zero, it specifies the maximum period between
     * data poll transmissions. Note that the network stack may send data
     * request transmissions more frequently when expecting a control-message
     * (e.g., when waiting for an MLE Child ID Response).
     *
     */
    SPINEL_PROP_MAC_DATA_POLL_PERIOD = SPINEL_PROP_MAC__BEGIN + 10,

    SPINEL_PROP_MAC__END = 0x40,

    SPINEL_PROP_MAC_EXT__BEGIN = 0x1300,

    /// MAC Allowlist
    /** Format: `A(t(Ec))`
     * Required capability: `CAP_MAC_ALLOWLIST`
     *
     * Structure Parameters:
     *
     *  `E`: EUI64 address of node
     *  `c`: Optional RSSI-override value. The value 127 indicates
     *       that the RSSI-override feature is not enabled for this
     *       address. If this value is omitted when setting or
     *       inserting, it is assumed to be 127. This parameter is
     *       ignored when removing.
     */
    SPINEL_PROP_MAC_ALLOWLIST = SPINEL_PROP_MAC_EXT__BEGIN + 0,

    /// MAC Allowlist Enabled Flag
    /** Format: `b`
     * Required capability: `CAP_MAC_ALLOWLIST`
     *
     */
    SPINEL_PROP_MAC_ALLOWLIST_ENABLED = SPINEL_PROP_MAC_EXT__BEGIN + 1,

    /// MAC Extended Address
    /** Format: `E`
     *
     *  Specified by Thread. Randomly-chosen, but non-volatile EUI-64.
     */
    SPINEL_PROP_MAC_EXTENDED_ADDR = SPINEL_PROP_MAC_EXT__BEGIN + 2,

    /// MAC Source Match Enabled Flag
    /** Format: `b`
     * Required Capability: SPINEL_CAP_MAC_RAW or SPINEL_CAP_CONFIG_RADIO
     *
     * Set to true to enable radio source matching or false to disable it.
     * The source match functionality is used by radios when generating
     * ACKs. The short and extended address lists are used for setting
     * the Frame Pending bit in the ACKs.
     *
     */
    SPINEL_PROP_MAC_SRC_MATCH_ENABLED = SPINEL_PROP_MAC_EXT__BEGIN + 3,

    /// MAC Source Match Short Address List
    /** Format: `A(S)`
     * Required Capability: SPINEL_CAP_MAC_RAW or SPINEL_CAP_CONFIG_RADIO
     *
     */
    SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES = SPINEL_PROP_MAC_EXT__BEGIN + 4,

    /// MAC Source Match Extended Address List
    /** Format: `A(E)`
     *  Required Capability: SPINEL_CAP_MAC_RAW or SPINEL_CAP_CONFIG_RADIO
     *
     */
    SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES = SPINEL_PROP_MAC_EXT__BEGIN + 5,

    /// MAC Denylist
    /** Format: `A(t(E))`
     * Required capability: `CAP_MAC_ALLOWLIST`
     *
     * Structure Parameters:
     *
     *  `E`: EUI64 address of node
     *
     */
    SPINEL_PROP_MAC_DENYLIST = SPINEL_PROP_MAC_EXT__BEGIN + 6,

    /// MAC Denylist Enabled Flag
    /** Format: `b`
     *  Required capability: `CAP_MAC_ALLOWLIST`
     */
    SPINEL_PROP_MAC_DENYLIST_ENABLED = SPINEL_PROP_MAC_EXT__BEGIN + 7,

    /// MAC Received Signal Strength Filter
    /** Format: `A(t(Ec))`
     * Required capability: `CAP_MAC_ALLOWLIST`
     *
     * Structure Parameters:
     *
     * * `E`: Optional EUI64 address of node. Set default RSS if not included.
     * * `c`: Fixed RSS. 127 means not set.
     */
    SPINEL_PROP_MAC_FIXED_RSS = SPINEL_PROP_MAC_EXT__BEGIN + 8,

    /// The CCA failure rate
    /** Format: `S`
     *
     * This property provides the current CCA (Clear Channel Assessment) failure rate.
     *
     * Maximum value `0xffff` corresponding to 100% failure rate.
     *
     */
    SPINEL_PROP_MAC_CCA_FAILURE_RATE = SPINEL_PROP_MAC_EXT__BEGIN + 9,

    /// MAC Max direct retry number
    /** Format: `C`
     *
     * The maximum (user-specified) number of direct frame transmission retries.
     *
     */
    SPINEL_PROP_MAC_MAX_RETRY_NUMBER_DIRECT = SPINEL_PROP_MAC_EXT__BEGIN + 10,

    /// MAC Max indirect retry number
    /** Format: `C`
     * Required capability: `SPINEL_CAP_CONFIG_FTD`
     *
     * The maximum (user-specified) number of indirect frame transmission retries.
     *
     */
    SPINEL_PROP_MAC_MAX_RETRY_NUMBER_INDIRECT = SPINEL_PROP_MAC_EXT__BEGIN + 11,

    SPINEL_PROP_MAC_EXT__END = 0x1400,

    SPINEL_PROP_NET__BEGIN = 0x40,

    /// Network Is Saved (Is Commissioned)
    /** Format: `b` - Read only
     *
     * Returns true if there is a network state stored/saved.
     *
     */
    SPINEL_PROP_NET_SAVED = SPINEL_PROP_NET__BEGIN + 0,

    /// Network Interface Status
    /** Format `b` - Read-write
     *
     * Network interface up/down status. Write true to bring
     * interface up and false to bring interface down.
     *
     */
    SPINEL_PROP_NET_IF_UP = SPINEL_PROP_NET__BEGIN + 1,

    /// Thread Stack Operational Status
    /** Format `b` - Read-write
     *
     * Thread stack operational status. Write true to start
     * Thread stack and false to stop it.
     *
     */
    SPINEL_PROP_NET_STACK_UP = SPINEL_PROP_NET__BEGIN + 2,

    /// Thread Device Role
    /** Format `C` - Read-write
     *
     * Possible values are from enumeration `spinel_net_role_t`
     *
     *  SPINEL_NET_ROLE_DETACHED = 0,
     *  SPINEL_NET_ROLE_CHILD    = 1,
     *  SPINEL_NET_ROLE_ROUTER   = 2,
     *  SPINEL_NET_ROLE_LEADER   = 3,
     *
     */
    SPINEL_PROP_NET_ROLE = SPINEL_PROP_NET__BEGIN + 3,

    /// Thread Network Name
    /** Format `U` - Read-write
     *
     */
    SPINEL_PROP_NET_NETWORK_NAME = SPINEL_PROP_NET__BEGIN + 4,

    /// Thread Network Extended PAN ID
    /** Format `D` - Read-write
     *
     */
    SPINEL_PROP_NET_XPANID = SPINEL_PROP_NET__BEGIN + 5,

    /// Thread Network Key
    /** Format `D` - Read-write
     *
     */
    SPINEL_PROP_NET_NETWORK_KEY = SPINEL_PROP_NET__BEGIN + 6,

    /// Thread Network Key Sequence Counter
    /** Format `L` - Read-write
     *
     */
    SPINEL_PROP_NET_KEY_SEQUENCE_COUNTER = SPINEL_PROP_NET__BEGIN + 7,

    /// Thread Network Partition Id
    /** Format `L` - Read-write
     *
     * The partition ID of the partition that this node is a
     * member of.
     *
     */
    SPINEL_PROP_NET_PARTITION_ID = SPINEL_PROP_NET__BEGIN + 8,

    /// Require Join Existing
    /** Format: `b`
     *  Default Value: `false`
     *
     * This flag is typically used for nodes that are associating with an
     * existing network for the first time. If this is set to `true` before
     * `PROP_NET_STACK_UP` is set to `true`, the
     * creation of a new partition at association is prevented. If the node
     * cannot associate with an existing partition, `PROP_LAST_STATUS` will
     * emit a status that indicates why the association failed and
     * `PROP_NET_STACK_UP` will automatically revert to `false`.
     *
     * Once associated with an existing partition, this flag automatically
     * reverts to `false`.
     *
     * The behavior of this property being set to `true` when
     * `PROP_NET_STACK_UP` is already set to `true` is undefined.
     *
     */
    SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING = SPINEL_PROP_NET__BEGIN + 9,

    /// Thread Network Key Switch Guard Time
    /** Format `L` - Read-write
     *
     */
    SPINEL_PROP_NET_KEY_SWITCH_GUARDTIME = SPINEL_PROP_NET__BEGIN + 10,

    /// Thread Network PSKc
    /** Format `D` - Read-write
     *
     */
    SPINEL_PROP_NET_PSKC = SPINEL_PROP_NET__BEGIN + 11,

    SPINEL_PROP_NET__END = 0x50,

    SPINEL_PROP_NET_EXT__BEGIN = 0x1400,
    SPINEL_PROP_NET_EXT__END   = 0x1500,

    SPINEL_PROP_THREAD__BEGIN = 0x50,

    /// Thread Leader IPv6 Address
    /** Format `6` - Read only
     *
     */
    SPINEL_PROP_THREAD_LEADER_ADDR = SPINEL_PROP_THREAD__BEGIN + 0,

    /// Thread Parent Info
    /** Format: `ESLccCC` - Read only
     *
     *  `E`: Extended address
     *  `S`: RLOC16
     *  `L`: Age (seconds since last heard from)
     *  `c`: Average RSS (in dBm)
     *  `c`: Last RSSI (in dBm)
     *  `C`: Link Quality In
     *  `C`: Link Quality Out
     *
     */
    SPINEL_PROP_THREAD_PARENT = SPINEL_PROP_THREAD__BEGIN + 1,

    /// Thread Child Table
    /** Format: [A(t(ESLLCCcCc)] - Read only
     *
     * Data per item is:
     *
     *  `E`: Extended address
     *  `S`: RLOC16
     *  `L`: Timeout (in seconds)
     *  `L`: Age (in seconds)
     *  `L`: Network Data version
     *  `C`: Link Quality In
     *  `c`: Average RSS (in dBm)
     *  `C`: Mode (bit-flags)
     *  `c`: Last RSSI (in dBm)
     *
     */
    SPINEL_PROP_THREAD_CHILD_TABLE = SPINEL_PROP_THREAD__BEGIN + 2,

    /// Thread Leader Router Id
    /** Format `C` - Read only
     *
     * The router-id of the current leader.
     *
     */
    SPINEL_PROP_THREAD_LEADER_RID = SPINEL_PROP_THREAD__BEGIN + 3,

    /// Thread Leader Weight
    /** Format `C` - Read only
     *
     * The leader weight of the current leader.
     *
     */
    SPINEL_PROP_THREAD_LEADER_WEIGHT = SPINEL_PROP_THREAD__BEGIN + 4,

    /// Thread Local Leader Weight
    /** Format `C` - Read only
     *
     * The leader weight of this node.
     *
     */
    SPINEL_PROP_THREAD_LOCAL_LEADER_WEIGHT = SPINEL_PROP_THREAD__BEGIN + 5,

    /// Thread Local Network Data
    /** Format `D` - Read only
     *
     */
    SPINEL_PROP_THREAD_NETWORK_DATA = SPINEL_PROP_THREAD__BEGIN + 6,

    /// Thread Local Network Data Version
    /** Format `C` - Read only
     *
     */
    SPINEL_PROP_THREAD_NETWORK_DATA_VERSION = SPINEL_PROP_THREAD__BEGIN + 7,

    /// Thread Local Stable Network Data
    /** Format `D` - Read only
     *
     */
    SPINEL_PROP_THREAD_STABLE_NETWORK_DATA = SPINEL_PROP_THREAD__BEGIN + 8,

    /// Thread Local Stable Network Data Version
    /** Format `C` - Read only
     *
     */
    SPINEL_PROP_THREAD_STABLE_NETWORK_DATA_VERSION = SPINEL_PROP_THREAD__BEGIN + 9,

    /// On-Mesh Prefixes
    /** Format: `A(t(6CbCbSC))`
     *
     * Data per item is:
     *
     *  `6`: IPv6 Prefix
     *  `C`: Prefix length in bits
     *  `b`: Stable flag
     *  `C`: TLV flags (SPINEL_NET_FLAG_* definition)
     *  `b`: "Is defined locally" flag. Set if this network was locally
     *       defined. Assumed to be true for set, insert and replace. Clear if
     *       the on mesh network was defined by another node.
     *       This field is ignored for INSERT and REMOVE commands.
     *  `S`: The RLOC16 of the device that registered this on-mesh prefix entry.
     *       This value is not used and ignored when adding an on-mesh prefix.
     *       This field is ignored for INSERT and REMOVE commands.
     *  `C`: TLV flags extended (additional field for Thread 1.2 features).
     *
     */
    SPINEL_PROP_THREAD_ON_MESH_NETS = SPINEL_PROP_THREAD__BEGIN + 10,

    /// Off-mesh routes
    /** Format: [A(t(6CbCbb))]
     *
     * Data per item is:
     *
     *  `6`: Route Prefix
     *  `C`: Prefix length in bits
     *  `b`: Stable flag
     *  `C`: Route flags (SPINEL_ROUTE_FLAG_* and SPINEL_ROUTE_PREFERNCE_* definitions)
     *  `b`: "Is defined locally" flag. Set if this route info was locally
     *       defined as part of local network data. Assumed to be true for set,
     *       insert and replace. Clear if the route is part of partition's network
     *       data.
     *  `b`: "Next hop is this device" flag. Set if the next hop for the
     *       route is this device itself (i.e., route was added by this device)
     *       This value is ignored when adding an external route. For any added
     *       route the next hop is this device.
     *  `S`: The RLOC16 of the device that registered this route entry.
     *       This value is not used and ignored when adding a route.
     *
     */
    SPINEL_PROP_THREAD_OFF_MESH_ROUTES = SPINEL_PROP_THREAD__BEGIN + 11,

    /// Thread Assisting Ports
    /** Format `A(S)`
     *
     * Array of port numbers.
     */
    SPINEL_PROP_THREAD_ASSISTING_PORTS = SPINEL_PROP_THREAD__BEGIN + 12,

    /// Thread Allow Local Network Data Change
    /** Format `b` - Read-write
     *
     * Set to true before changing local net data. Set to false when finished.
     * This allows changes to be aggregated into a single event.
     *
     */
    SPINEL_PROP_THREAD_ALLOW_LOCAL_NET_DATA_CHANGE = SPINEL_PROP_THREAD__BEGIN + 13,

    /// Thread Mode
    /** Format: `C`
     *
     *  This property contains the value of the mode
     *  TLV for this node. The meaning of the bits in this
     *  bit-field are defined by section 4.5.2 of the Thread
     *  specification.
     *
     * The values `SPINEL_THREAD_MODE_*` defines the bit-fields
     *
     */
    SPINEL_PROP_THREAD_MODE = SPINEL_PROP_THREAD__BEGIN + 14,

    SPINEL_PROP_THREAD__END = 0x60,

    SPINEL_PROP_THREAD_EXT__BEGIN = 0x1500,

    /// Thread Child Timeout
    /** Format: `L`
     *  Unit: Seconds
     *
     *  Used when operating in the Child role.
     */
    SPINEL_PROP_THREAD_CHILD_TIMEOUT = SPINEL_PROP_THREAD_EXT__BEGIN + 0,

    /// Thread RLOC16
    /** Format: `S`
     *
     */
    SPINEL_PROP_THREAD_RLOC16 = SPINEL_PROP_THREAD_EXT__BEGIN + 1,

    /// Thread Router Upgrade Threshold
    /** Format: `C`
     *
     */
    SPINEL_PROP_THREAD_ROUTER_UPGRADE_THRESHOLD = SPINEL_PROP_THREAD_EXT__BEGIN + 2,

    /// Thread Context Reuse Delay
    /** Format: `L`
     *
     */
    SPINEL_PROP_THREAD_CONTEXT_REUSE_DELAY = SPINEL_PROP_THREAD_EXT__BEGIN + 3,

    /// Thread Network ID Timeout
    /** Format: `C`
     *
     */
    SPINEL_PROP_THREAD_NETWORK_ID_TIMEOUT = SPINEL_PROP_THREAD_EXT__BEGIN + 4,

    /// List of active thread router ids
    /** Format: `A(C)`
     *
     * Note that some implementations may not support CMD_GET_VALUE
     * router ids, but may support CMD_REMOVE_VALUE when the node is
     * a leader.
     *
     */
    SPINEL_PROP_THREAD_ACTIVE_ROUTER_IDS = SPINEL_PROP_THREAD_EXT__BEGIN + 5,

    /// Forward IPv6 packets that use RLOC16 addresses to HOST.
    /** Format: `b`
     *
     * Allow host to directly observe all IPv6 packets received by the NCP,
     * including ones sent to the RLOC16 address.
     *
     * Default is false.
     *
     */
    SPINEL_PROP_THREAD_RLOC16_DEBUG_PASSTHRU = SPINEL_PROP_THREAD_EXT__BEGIN + 6,

    /// Router Role Enabled
    /** Format `b`
     *
     * Allows host to indicate whether or not the router role is enabled.
     * If current role is a router, setting this property to `false` starts
     * a re-attach process as an end-device.
     *
     */
    SPINEL_PROP_THREAD_ROUTER_ROLE_ENABLED = SPINEL_PROP_THREAD_EXT__BEGIN + 7,

    /// Thread Router Downgrade Threshold
    /** Format: `C`
     *
     */
    SPINEL_PROP_THREAD_ROUTER_DOWNGRADE_THRESHOLD = SPINEL_PROP_THREAD_EXT__BEGIN + 8,

    /// Thread Router Selection Jitter
    /** Format: `C`
     *
     */
    SPINEL_PROP_THREAD_ROUTER_SELECTION_JITTER = SPINEL_PROP_THREAD_EXT__BEGIN + 9,

    /// Thread Preferred Router Id
    /** Format: `C` - Write only
     *
     * Specifies the preferred Router Id. Upon becoming a router/leader the node
     * attempts to use this Router Id. If the preferred Router Id is not set or
     * if it can not be used, a randomly generated router id is picked. This
     * property can be set only when the device role is either detached or
     * disabled.
     *
     */
    SPINEL_PROP_THREAD_PREFERRED_ROUTER_ID = SPINEL_PROP_THREAD_EXT__BEGIN + 10,

    /// Thread Neighbor Table
    /** Format: `A(t(ESLCcCbLLc))` - Read only
     *
     * Data per item is:
     *
     *  `E`: Extended address
     *  `S`: RLOC16
     *  `L`: Age (in seconds)
     *  `C`: Link Quality In
     *  `c`: Average RSS (in dBm)
     *  `C`: Mode (bit-flags)
     *  `b`: `true` if neighbor is a child, `false` otherwise.
     *  `L`: Link Frame Counter
     *  `L`: MLE Frame Counter
     *  `c`: The last RSSI (in dBm)
     *
     */
    SPINEL_PROP_THREAD_NEIGHBOR_TABLE = SPINEL_PROP_THREAD_EXT__BEGIN + 11,

    /// Thread Max Child Count
    /** Format: `C`
     *
     * Specifies the maximum number of children currently allowed.
     * This parameter can only be set when Thread protocol operation
     * has been stopped.
     *
     */
    SPINEL_PROP_THREAD_CHILD_COUNT_MAX = SPINEL_PROP_THREAD_EXT__BEGIN + 12,

    /// Leader Network Data
    /** Format: `D` - Read only
     *
     */
    SPINEL_PROP_THREAD_LEADER_NETWORK_DATA = SPINEL_PROP_THREAD_EXT__BEGIN + 13,

    /// Stable Leader Network Data
    /** Format: `D` - Read only
     *
     */
    SPINEL_PROP_THREAD_STABLE_LEADER_NETWORK_DATA = SPINEL_PROP_THREAD_EXT__BEGIN + 14,

    /// Thread Joiner Data
    /** Format `A(T(ULE))`
     *  PSKd, joiner timeout, eui64 (optional)
     *
     * This property is being deprecated by SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS.
     *
     */
    SPINEL_PROP_THREAD_JOINERS = SPINEL_PROP_THREAD_EXT__BEGIN + 15,

    /// Thread Commissioner Enable
    /** Format `b`
     *
     * Default value is `false`.
     *
     * This property is being deprecated by SPINEL_PROP_MESHCOP_COMMISSIONER_STATE.
     *
     */
    SPINEL_PROP_THREAD_COMMISSIONER_ENABLED = SPINEL_PROP_THREAD_EXT__BEGIN + 16,

    /// Thread TMF proxy enable
    /** Format `b`
     * Required capability: `SPINEL_CAP_THREAD_TMF_PROXY`
     *
     * This property is deprecated.
     *
     */
    SPINEL_PROP_THREAD_TMF_PROXY_ENABLED = SPINEL_PROP_THREAD_EXT__BEGIN + 17,

    /// Thread TMF proxy stream
    /** Format `dSS`
     * Required capability: `SPINEL_CAP_THREAD_TMF_PROXY`
     *
     * This property is deprecated. Please see `SPINEL_PROP_THREAD_UDP_FORWARD_STREAM`.
     *
     */
    SPINEL_PROP_THREAD_TMF_PROXY_STREAM = SPINEL_PROP_THREAD_EXT__BEGIN + 18,

    /// Thread "joiner" flag used during discovery scan operation
    /** Format `b`
     *
     * This property defines the Joiner Flag value in the Discovery Request TLV.
     *
     * Default value is `false`.
     *
     */
    SPINEL_PROP_THREAD_DISCOVERY_SCAN_JOINER_FLAG = SPINEL_PROP_THREAD_EXT__BEGIN + 19,

    /// Enable EUI64 filtering for discovery scan operation.
    /** Format `b`
     *
     * Default value is `false`
     *
     */
    SPINEL_PROP_THREAD_DISCOVERY_SCAN_ENABLE_FILTERING = SPINEL_PROP_THREAD_EXT__BEGIN + 20,

    /// PANID used for Discovery scan operation (used for PANID filtering).
    /** Format: `S`
     *
     * Default value is 0xffff (Broadcast PAN) to disable PANID filtering
     *
     */
    SPINEL_PROP_THREAD_DISCOVERY_SCAN_PANID = SPINEL_PROP_THREAD_EXT__BEGIN + 21,

    /// Thread (out of band) steering data for MLE Discovery Response.
    /** Format `E` - Write only
     *
     * Required capability: SPINEL_CAP_OOB_STEERING_DATA.
     *
     * Writing to this property allows to set/update the MLE
     * Discovery Response steering data out of band.
     *
     *  - All zeros to clear the steering data (indicating that
     *    there is no steering data).
     *  - All 0xFFs to set steering data/bloom filter to
     *    accept/allow all.
     *  - A specific EUI64 which is then added to current steering
     *    data/bloom filter.
     *
     */
    SPINEL_PROP_THREAD_STEERING_DATA = SPINEL_PROP_THREAD_EXT__BEGIN + 22,

    /// Thread Router Table.
    /** Format: `A(t(ESCCCCCCb)` - Read only
     *
     * Data per item is:
     *
     *  `E`: IEEE 802.15.4 Extended Address
     *  `S`: RLOC16
     *  `C`: Router ID
     *  `C`: Next hop to router
     *  `C`: Path cost to router
     *  `C`: Link Quality In
     *  `C`: Link Quality Out
     *  `C`: Age (seconds since last heard)
     *  `b`: Link established with Router ID or not.
     *
     */
    SPINEL_PROP_THREAD_ROUTER_TABLE = SPINEL_PROP_THREAD_EXT__BEGIN + 23,

    /// Thread Active Operational Dataset
    /** Format: `A(t(iD))` - Read-Write
     *
     * This property provides access to current Thread Active Operational Dataset. A Thread device maintains the
     * Operational Dataset that it has stored locally and the one currently in use by the partition to which it is
     * attached. This property corresponds to the locally stored Dataset on the device.
     *
     * Operational Dataset consists of a set of supported properties (e.g., channel, network key, network name, PAN id,
     * etc). Note that not all supported properties may be present (have a value) in a Dataset.
     *
     * The Dataset value is encoded as an array of structs containing pairs of property key (as `i`) followed by the
     * property value (as `D`). The property value must follow the format associated with the corresponding property.
     *
     * On write, any unknown/unsupported property keys must be ignored.
     *
     * The following properties can be included in a Dataset list:
     *
     *   SPINEL_PROP_DATASET_ACTIVE_TIMESTAMP
     *   SPINEL_PROP_PHY_CHAN
     *   SPINEL_PROP_PHY_CHAN_SUPPORTED (Channel Mask Page 0)
     *   SPINEL_PROP_NET_NETWORK_KEY
     *   SPINEL_PROP_NET_NETWORK_NAME
     *   SPINEL_PROP_NET_XPANID
     *   SPINEL_PROP_MAC_15_4_PANID
     *   SPINEL_PROP_IPV6_ML_PREFIX
     *   SPINEL_PROP_NET_PSKC
     *   SPINEL_PROP_DATASET_SECURITY_POLICY
     *
     */
    SPINEL_PROP_THREAD_ACTIVE_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 24,

    /// Thread Pending Operational Dataset
    /** Format: `A(t(iD))` - Read-Write
     *
     * This property provide access to current locally stored Pending Operational Dataset.
     *
     * The formatting of this property follows the same rules as in SPINEL_PROP_THREAD_ACTIVE_DATASET.
     *
     * In addition supported properties in SPINEL_PROP_THREAD_ACTIVE_DATASET, the following properties can also
     * be included in the Pending Dataset:
     *
     *   SPINEL_PROP_DATASET_PENDING_TIMESTAMP
     *   SPINEL_PROP_DATASET_DELAY_TIMER
     *
     */
    SPINEL_PROP_THREAD_PENDING_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 25,

    /// Send MGMT_SET Thread Active Operational Dataset
    /** Format: `A(t(iD))` - Write only
     *
     * The formatting of this property follows the same rules as in SPINEL_PROP_THREAD_ACTIVE_DATASET.
     *
     * This is write-only property. When written, it triggers a MGMT_ACTIVE_SET meshcop command to be sent to leader
     * with the given Dataset. The spinel frame response should be a `LAST_STATUS` with the status of the transmission
     * of MGMT_ACTIVE_SET command.
     *
     * In addition to supported properties in SPINEL_PROP_THREAD_ACTIVE_DATASET, the following property can be
     * included in the Dataset (to allow for custom raw TLVs):
     *
     *    SPINEL_PROP_DATASET_RAW_TLVS
     *
     */
    SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 26,

    /// Send MGMT_SET Thread Pending Operational Dataset
    /** Format: `A(t(iD))` - Write only
     *
     * This property is similar to SPINEL_PROP_THREAD_PENDING_DATASET and follows the same format and rules.
     *
     * In addition to supported properties in SPINEL_PROP_THREAD_PENDING_DATASET, the following property can be
     * included the Dataset (to allow for custom raw TLVs to be provided).
     *
     *    SPINEL_PROP_DATASET_RAW_TLVS
     *
     */
    SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 27,

    /// Operational Dataset Active Timestamp
    /** Format: `X` - No direct read or write
     *
     * It can only be included in one of the Dataset related properties below:
     *
     *   SPINEL_PROP_THREAD_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET
     *
     */
    SPINEL_PROP_DATASET_ACTIVE_TIMESTAMP = SPINEL_PROP_THREAD_EXT__BEGIN + 28,

    /// Operational Dataset Pending Timestamp
    /** Format: `X` - No direct read or write
     *
     * It can only be included in one of the Pending Dataset properties:
     *
     *   SPINEL_PROP_THREAD_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET
     *
     */
    SPINEL_PROP_DATASET_PENDING_TIMESTAMP = SPINEL_PROP_THREAD_EXT__BEGIN + 29,

    /// Operational Dataset Delay Timer
    /** Format: `L` - No direct read or write
     *
     * Delay timer (in ms) specifies the time renaming until Thread devices overwrite the value in the Active
     * Operational Dataset with the corresponding values in the Pending Operational Dataset.
     *
     * It can only be included in one of the Pending Dataset properties:
     *
     *   SPINEL_PROP_THREAD_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET
     *
     */
    SPINEL_PROP_DATASET_DELAY_TIMER = SPINEL_PROP_THREAD_EXT__BEGIN + 30,

    /// Operational Dataset Security Policy
    /** Format: `SD` - No direct read or write
     *
     * It can only be included in one of the Dataset related properties below:
     *
     *   SPINEL_PROP_THREAD_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET
     *
     * Content is
     *   `S` : Key Rotation Time (in units of hour)
     *   `C` : Security Policy Flags (as specified in Thread 1.1 Section 8.10.1.15)
     *   `C` : Optional Security Policy Flags extension (as specified in Thread 1.2 Section 8.10.1.15).
     *         0xf8 is used if this field is missing.
     *
     */
    SPINEL_PROP_DATASET_SECURITY_POLICY = SPINEL_PROP_THREAD_EXT__BEGIN + 31,

    /// Operational Dataset Additional Raw TLVs
    /** Format: `D` - No direct read or write
     *
     * This property defines extra raw TLVs that can be added to an Operational DataSet.
     *
     * It can only be included in one of the following Dataset properties:
     *
     *   SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET
     *
     */
    SPINEL_PROP_DATASET_RAW_TLVS = SPINEL_PROP_THREAD_EXT__BEGIN + 32,

    /// Child table addresses
    /** Format: `A(t(ESA(6)))` - Read only
     *
     * This property provides the list of all addresses associated with every child
     * including any registered IPv6 addresses.
     *
     * Data per item is:
     *
     *  `E`: Extended address of the child
     *  `S`: RLOC16 of the child
     *  `A(6)`: List of IPv6 addresses registered by the child (if any)
     *
     */
    SPINEL_PROP_THREAD_CHILD_TABLE_ADDRESSES = SPINEL_PROP_THREAD_EXT__BEGIN + 33,

    /// Neighbor Table Frame and Message Error Rates
    /** Format: `A(t(ESSScc))`
     *  Required capability: `CAP_ERROR_RATE_TRACKING`
     *
     * This property provides link quality related info including
     * frame and (IPv6) message error rates for all neighbors.
     *
     * With regards to message error rate, note that a larger (IPv6)
     * message can be fragmented and sent as multiple MAC frames. The
     * message transmission is considered a failure, if any of its
     * fragments fail after all MAC retry attempts.
     *
     * Data per item is:
     *
     *  `E`: Extended address of the neighbor
     *  `S`: RLOC16 of the neighbor
     *  `S`: Frame error rate (0 -> 0%, 0xffff -> 100%)
     *  `S`: Message error rate (0 -> 0%, 0xffff -> 100%)
     *  `c`: Average RSSI (in dBm)
     *  `c`: Last RSSI (in dBm)
     *
     */
    SPINEL_PROP_THREAD_NEIGHBOR_TABLE_ERROR_RATES = SPINEL_PROP_THREAD_EXT__BEGIN + 34,

    /// EID (Endpoint Identifier) IPv6 Address Cache Table
    /** Format `A(t(6SCCt(bL6)t(bSS)))
     *
     * This property provides Thread EID address cache table.
     *
     * Data per item is:
     *
     *  `6` : Target IPv6 address
     *  `S` : RLOC16 of target
     *  `C` : Age (order of use, 0 indicates most recently used entry)
     *  `C` : Entry state (values are defined by enumeration `SPINEL_ADDRESS_CACHE_ENTRY_STATE_*`).
     *
     *  `t` : Info when state is `SPINEL_ADDRESS_CACHE_ENTRY_STATE_CACHED`
     *    `b` : Indicates whether last transaction time and ML-EID are valid.
     *    `L` : Last transaction time
     *    `6` : Mesh-local EID
     *
     *  `t` : Info when state is other than `SPINEL_ADDRESS_CACHE_ENTRY_STATE_CACHED`
     *    `b` : Indicates whether the entry can be evicted.
     *    `S` : Timeout in seconds
     *    `S` : Retry delay (applicable if in query-retry state).
     *
     */
    SPINEL_PROP_THREAD_ADDRESS_CACHE_TABLE = SPINEL_PROP_THREAD_EXT__BEGIN + 35,

    /// Thread UDP forward stream
    /** Format `dS6S`
     * Required capability: `SPINEL_CAP_THREAD_UDP_FORWARD`
     *
     * This property helps exchange UDP packets with host.
     *
     *  `d`: UDP payload
     *  `S`: Remote UDP port
     *  `6`: Remote IPv6 address
     *  `S`: Local UDP port
     *
     */
    SPINEL_PROP_THREAD_UDP_FORWARD_STREAM = SPINEL_PROP_THREAD_EXT__BEGIN + 36,

    /// Send MGMT_GET Thread Active Operational Dataset
    /** Format: `A(t(iD))` - Write only
     *
     * The formatting of this property follows the same rules as in SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET. This
     * property further allows the sender to not include a value associated with properties in formating of `t(iD)`,
     * i.e., it should accept either a `t(iD)` or a `t(i)` encoding (in both cases indicating that the associated
     * Dataset property should be requested as part of MGMT_GET command).
     *
     * This is write-only property. When written, it triggers a MGMT_ACTIVE_GET meshcop command to be sent to leader
     * requesting the Dataset related properties from the format. The spinel frame response should be a `LAST_STATUS`
     * with the status of the transmission of MGMT_ACTIVE_GET command.
     *
     * In addition to supported properties in SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET, the following property can be
     * optionally included in the Dataset:
     *
     *    SPINEL_PROP_DATASET_DEST_ADDRESS
     *
     */
    SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 37,

    /// Send MGMT_GET Thread Pending Operational Dataset
    /** Format: `A(t(iD))` - Write only
     *
     * The formatting of this property follows the same rules as in SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET.
     *
     * This is write-only property. When written, it triggers a MGMT_PENDING_GET meshcop command to be sent to leader
     * with the given Dataset. The spinel frame response should be a `LAST_STATUS` with the status of the transmission
     * of MGMT_PENDING_GET command.
     *
     */
    SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 38,

    /// Operational Dataset (MGMT_GET) Destination IPv6 Address
    /** Format: `6` - No direct read or write
     *
     * This property specifies the IPv6 destination when sending MGMT_GET command for either Active or Pending Dataset
     * if not provided, Leader ALOC address is used as default.
     *
     * It can only be included in one of the MGMT_GET Dataset properties:
     *
     *   SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET
     *   SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET
     *
     */
    SPINEL_PROP_DATASET_DEST_ADDRESS = SPINEL_PROP_THREAD_EXT__BEGIN + 39,

    /// Thread New Operational Dataset
    /** Format: `A(t(iD))` - Read only - FTD build only
     *
     * This property allows host to request NCP to create and return a new Operation Dataset to use when forming a new
     * network.
     *
     * Operational Dataset consists of a set of supported properties (e.g., channel, network key, network name, PAN id,
     * etc). Note that not all supported properties may be present (have a value) in a Dataset.
     *
     * The Dataset value is encoded as an array of structs containing pairs of property key (as `i`) followed by the
     * property value (as `D`). The property value must follow the format associated with the corresponding property.
     *
     * The following properties can be included in a Dataset list:
     *
     *   SPINEL_PROP_DATASET_ACTIVE_TIMESTAMP
     *   SPINEL_PROP_PHY_CHAN
     *   SPINEL_PROP_PHY_CHAN_SUPPORTED (Channel Mask Page 0)
     *   SPINEL_PROP_NET_NETWORK_KEY
     *   SPINEL_PROP_NET_NETWORK_NAME
     *   SPINEL_PROP_NET_XPANID
     *   SPINEL_PROP_MAC_15_4_PANID
     *   SPINEL_PROP_IPV6_ML_PREFIX
     *   SPINEL_PROP_NET_PSKC
     *   SPINEL_PROP_DATASET_SECURITY_POLICY
     *
     */
    SPINEL_PROP_THREAD_NEW_DATASET = SPINEL_PROP_THREAD_EXT__BEGIN + 40,

    /// MAC CSL Period
    /** Format: `S`
     * Required capability: `SPINEL_CAP_THREAD_CSL_RECEIVER`
     *
     * The CSL period in units of 10 symbols. Value of 0 indicates that CSL should be disabled.
     */
    SPINEL_PROP_THREAD_CSL_PERIOD = SPINEL_PROP_THREAD_EXT__BEGIN + 41,

    /// MAC CSL Timeout
    /** Format: `L`
     * Required capability: `SPINEL_CAP_THREAD_CSL_RECEIVER`
     *
     * The CSL timeout in seconds.
     */
    SPINEL_PROP_THREAD_CSL_TIMEOUT = SPINEL_PROP_THREAD_EXT__BEGIN + 42,

    /// MAC CSL Channel
    /** Format: `C`
     * Required capability: `SPINEL_CAP_THREAD_CSL_RECEIVER`
     *
     * The CSL channel as described in chapter 4.6.5.1.2 of the Thread v1.2.0 Specification.
     * Value of 0 means that CSL reception (if enabled) occurs on the Thread Network channel.
     * Value from range [11,26] is an alternative channel on which a CSL reception occurs.
     */
    SPINEL_PROP_THREAD_CSL_CHANNEL = SPINEL_PROP_THREAD_EXT__BEGIN + 43,

    /// Thread Domain Name
    /** Format `U` - Read-write
     * Required capability: `SPINEL_CAP_NET_THREAD_1_2`
     *
     * This property is available since Thread 1.2.0.
     * Write to this property succeeds only when Thread protocols are disabled.
     *
     */
    SPINEL_PROP_THREAD_DOMAIN_NAME = SPINEL_PROP_THREAD_EXT__BEGIN + 44,

    /// Link metrics query
    /** Format: `6CC` - Write-Only
     *
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * `6` : IPv6 destination address
     * `C` : Series id (0 for Single Probe)
     * `C` : List of requested metric ids encoded as bit fields in single byte
     *
     *   +---------------+----+
     *   |    Metric     | Id |
     *   +---------------+----+
     *   | Received PDUs |  0 |
     *   | LQI           |  1 |
     *   | Link margin   |  2 |
     *   | RSSI          |  3 |
     *   +---------------+----+
     *
     * If the query succeeds, the NCP will send a result to the Host using
     * @ref SPINEL_PROP_THREAD_LINK_METRICS_QUERY_RESULT.
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_QUERY = SPINEL_PROP_THREAD_EXT__BEGIN + 45,

    /// Link metrics query result
    /** Format: `6Ct(A(t(CD)))` - Unsolicited notifications only
     *
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * `6` : IPv6 destination address
     * `C` : Status
     * `t(A(t(CD)))` : Array of structs encoded as following:
     *   `C` : Metric id
     *   `D` : Metric value
     *
     *   +---------------+----+----------------+
     *   |    Metric     | Id |  Value format  |
     *   +---------------+----+----------------+
     *   | Received PDUs |  0 | `L` (uint32_t) |
     *   | LQI           |  1 | `C` (uint8_t)  |
     *   | Link margin   |  2 | `C` (uint8_t)  |
     *   | RSSI          |  3 | `c` (int8_t)   |
     *   +---------------+----+----------------+
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_QUERY_RESULT = SPINEL_PROP_THREAD_EXT__BEGIN + 46,

    /// Link metrics probe
    /** Format `6CC` - Write only
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * Send a MLE Link Probe message to the peer.
     *
     * `6` : IPv6 destination address
     * `C` : The Series ID for which this Probe message targets at
     * `C` : The length of the Probe message, valid range: [0, 64]
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_PROBE = SPINEL_PROP_THREAD_EXT__BEGIN + 47,

    /// Link metrics Enhanced-ACK Based Probing management
    /** Format: 6Cd - Write only
     *
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * `6` : IPv6 destination address
     * `C` : Indicate whether to register or clear the probing. `0` - clear, `1` - register
     * `C` : List of requested metric ids encoded as bit fields in single byte
     *
     *   +---------------+----+
     *   |    Metric     | Id |
     *   +---------------+----+
     *   | LQI           |  1 |
     *   | Link margin   |  2 |
     *   | RSSI          |  3 |
     *   +---------------+----+
     *
     * Result of configuration is reported asynchronously to the Host using the
     * @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_RESPONSE.
     *
     * Whenever Enh-ACK IE report is received it is passed to the Host using the
     * @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK_IE property.
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK = SPINEL_PROP_THREAD_EXT__BEGIN + 48,

    /// Link metrics Enhanced-ACK Based Probing IE report
    /** Format: SEA(t(CD)) - Unsolicited notifications only
     *
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * `S` : Short address of the Probing Subject
     * `E` : Extended address of the Probing Subject
     * `t(A(t(CD)))` : Struct that contains array of structs encoded as following:
     *   `C` : Metric id
     *   `D` : Metric value
     *
     *   +---------------+----+----------------+
     *   |    Metric     | Id |  Value format  |
     *   +---------------+----+----------------+
     *   | LQI           |  1 | `C` (uint8_t)  |
     *   | Link margin   |  2 | `C` (uint8_t)  |
     *   | RSSI          |  3 | `c` (int8_t)   |
     *   +---------------+----+----------------+
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK_IE = SPINEL_PROP_THREAD_EXT__BEGIN + 49,

    /// Link metrics Forward Tracking Series management
    /** Format: 6CCC - Write only
     *
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * `6` : IPv6 destination address
     * `C` : Series id
     * `C` : Tracked frame types encoded as bit fields in single byte, if equal to zero,
     *       accounting is stopped and a series is removed
     * `C` : Requested metric ids encoded as bit fields in single byte
     *
     *   +------------------+----+
     *   |    Frame type    | Id |
     *   +------------------+----+
     *   | MLE Link Probe   |  0 |
     *   | MAC Data         |  1 |
     *   | MAC Data Request |  2 |
     *   | MAC ACK          |  3 |
     *   +------------------+----+
     *
     *   +---------------+----+
     *   |    Metric     | Id |
     *   +---------------+----+
     *   | Received PDUs |  0 |
     *   | LQI           |  1 |
     *   | Link margin   |  2 |
     *   | RSSI          |  3 |
     *   +---------------+----+
     *
     * Result of configuration is reported asynchronously to the Host using the
     * @ref SPINEL_PROP_THREAD_LINK_METRICS_MGMT_RESPONSE.
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_MGMT_FORWARD = SPINEL_PROP_THREAD_EXT__BEGIN + 50,

    /// Link metrics management response
    /** Format: 6C - Unsolicited notifications only
     *
     * Required capability: `SPINEL_CAP_THREAD_LINK_METRICS`
     *
     * `6` : IPv6 source address
     * `C` : Received status
     *
     */
    SPINEL_PROP_THREAD_LINK_METRICS_MGMT_RESPONSE = SPINEL_PROP_THREAD_EXT__BEGIN + 51,

    /// Multicast Listeners Register Request
    /** Format `t(A(6))A(t(CD))` - Write-only
     * Required capability: `SPINEL_CAP_NET_THREAD_1_2`
     *
     * `t(A(6))`: Array of IPv6 multicast addresses
     * `A(t(CD))`: Array of structs holding optional parameters as follows
     *   `C`: Parameter id
     *   `D`: Parameter value
     *
     *   +----------------------------------------------------------------+
     *   | Id:   SPINEL_THREAD_MLR_PARAMID_TIMEOUT                        |
     *   | Type: `L`                                                      |
     *   | Description: Timeout in seconds. If this optional parameter is |
     *   |   omitted, the default value of the BBR will be used.          |
     *   | Special values:                                                |
     *   |   0 causes given addresses to be removed                       |
     *   |   0xFFFFFFFF is permanent and persistent registration          |
     *   +----------------------------------------------------------------+
     *
     * Write to this property initiates update of Multicast Listeners Table on the primary BBR.
     * If the write succeeded, the result of network operation will be notified later by the
     * SPINEL_PROP_THREAD_MLR_RESPONSE property. If the write fails, no MLR.req is issued and
     * notifiaction through the SPINEL_PROP_THREAD_MLR_RESPONSE property will not occur.
     *
     */
    SPINEL_PROP_THREAD_MLR_REQUEST = SPINEL_PROP_THREAD_EXT__BEGIN + 52,

    /// Multicast Listeners Register Response
    /** Format `CCt(A(6))` - Unsolicited notifications only
     * Required capability: `SPINEL_CAP_NET_THREAD_1_2`
     *
     * `C`: Status
     * `C`: MlrStatus (The Multicast Listener Registration Status)
     * `A(6)`: Array of IPv6 addresses that failed to be updated on the primary BBR
     *
     * This property is notified asynchronously when the NCP receives MLR.rsp following
     * previous write to the SPINEL_PROP_THREAD_MLR_REQUEST property.
     */
    SPINEL_PROP_THREAD_MLR_RESPONSE = SPINEL_PROP_THREAD_EXT__BEGIN + 53,

    /// Interface Identifier specified for Thread Domain Unicast Address.
    /** Format: `A(C)` - Read-write
     *
     *   `A(C)`: Interface Identifier (8 bytes).
     *
     * Required capability: SPINEL_CAP_DUA
     *
     * If write to this property is performed without specified parameter
     * the Interface Identifier of the Thread Domain Unicast Address will be cleared.
     * If the DUA Interface Identifier is cleared on the NCP device,
     * the get spinel property command will be returned successfully without specified parameter.
     *
     */
    SPINEL_PROP_THREAD_DUA_ID = SPINEL_PROP_THREAD_EXT__BEGIN + 54,

    /// Thread 1.2 Primary Backbone Router information in the Thread Network.
    /** Format: `SSLC` - Read-Only
     *
     * Required capability: `SPINEL_CAP_NET_THREAD_1_2`
     *
     * `S`: Server.
     * `S`: Reregistration Delay (in seconds).
     * `L`: Multicast Listener Registration Timeout (in seconds).
     * `C`: Sequence Number.
     *
     */
    SPINEL_PROP_THREAD_BACKBONE_ROUTER_PRIMARY = SPINEL_PROP_THREAD_EXT__BEGIN + 55,

    /// Thread 1.2 Backbone Router local state.
    /** Format: `C` - Read-Write
     *
     * Required capability: `SPINEL_CAP_THREAD_BACKBONE_ROUTER`
     *
     * The valid values are specified by SPINEL_THREAD_BACKBONE_ROUTER_STATE_<state> enumeration.
     * Backbone functionality will be disabled if SPINEL_THREAD_BACKBONE_ROUTER_STATE_DISABLED
     * is writted to this property, enabled otherwise.
     *
     */
    SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_STATE = SPINEL_PROP_THREAD_EXT__BEGIN + 56,

    /// Local Thread 1.2 Backbone Router configuration.
    /** Format: SLC - Read-Write
     *
     * Required capability: `SPINEL_CAP_THREAD_BACKBONE_ROUTER`
     *
     * `S`: Reregistration Delay (in seconds).
     * `L`: Multicast Listener Registration Timeout (in seconds).
     * `C`: Sequence Number.
     *
     */
    SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_CONFIG = SPINEL_PROP_THREAD_EXT__BEGIN + 57,

    /// Register local Thread 1.2 Backbone Router configuration.
    /** Format: Empty (Write only).
     *
     * Required capability: `SPINEL_CAP_THREAD_BACKBONE_ROUTER`
     *
     * Writing to this property (with any value) will register local Backbone Router configuration.
     *
     */
    SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTER = SPINEL_PROP_THREAD_EXT__BEGIN + 58,

    /// Thread 1.2 Backbone Router registration jitter.
    /** Format: `C` - Read-Write
     *
     * Required capability: `SPINEL_CAP_THREAD_BACKBONE_ROUTER`
     *
     * `C`: Backbone Router registration jitter.
     *
     */
    SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTRATION_JITTER = SPINEL_PROP_THREAD_EXT__BEGIN + 59,

    SPINEL_PROP_THREAD_EXT__END = 0x1600,

    SPINEL_PROP_IPV6__BEGIN = 0x60,

    /// Link-Local IPv6 Address
    /** Format: `6` - Read only
     *
     */
    SPINEL_PROP_IPV6_LL_ADDR = SPINEL_PROP_IPV6__BEGIN + 0, ///< [6]

    /// Mesh Local IPv6 Address
    /** Format: `6` - Read only
     *
     */
    SPINEL_PROP_IPV6_ML_ADDR = SPINEL_PROP_IPV6__BEGIN + 1,

    /// Mesh Local Prefix
    /** Format: `6C` - Read-write
     *
     * Provides Mesh Local Prefix
     *
     *   `6`: Mesh local prefix
     *   `C` : Prefix length (64 bit for Thread).
     *
     */
    SPINEL_PROP_IPV6_ML_PREFIX = SPINEL_PROP_IPV6__BEGIN + 2,

    /// IPv6 (Unicast) Address Table
    /** Format: `A(t(6CLLC))`
     *
     * This property provides all unicast addresses.
     *
     * Array of structures containing:
     *
     *  `6`: IPv6 Address
     *  `C`: Network Prefix Length (in bits)
     *  `L`: Valid Lifetime
     *  `L`: Preferred Lifetime
     *
     */
    SPINEL_PROP_IPV6_ADDRESS_TABLE = SPINEL_PROP_IPV6__BEGIN + 3,

    /// IPv6 Route Table - Deprecated
    SPINEL_PROP_IPV6_ROUTE_TABLE = SPINEL_PROP_IPV6__BEGIN + 4,

    /// IPv6 ICMP Ping Offload
    /** Format: `b`
     *
     * Allow the NCP to directly respond to ICMP ping requests. If this is
     * turned on, ping request ICMP packets will not be passed to the host.
     *
     * Default value is `false`.
     */
    SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD = SPINEL_PROP_IPV6__BEGIN + 5,

    /// IPv6 Multicast Address Table
    /** Format: `A(t(6))`
     *
     * This property provides all multicast addresses.
     *
     */
    SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE = SPINEL_PROP_IPV6__BEGIN + 6,

    /// IPv6 ICMP Ping Offload
    /** Format: `C`
     *
     * Allow the NCP to directly respond to ICMP ping requests. If this is
     * turned on, ping request ICMP packets will not be passed to the host.
     *
     * This property allows enabling responses sent to unicast only, multicast
     * only, or both. The valid value are defined by enumeration
     * `spinel_ipv6_icmp_ping_offload_mode_t`.
     *
     *   SPINEL_IPV6_ICMP_PING_OFFLOAD_DISABLED       = 0
     *   SPINEL_IPV6_ICMP_PING_OFFLOAD_UNICAST_ONLY   = 1
     *   SPINEL_IPV6_ICMP_PING_OFFLOAD_MULTICAST_ONLY = 2
     *   SPINEL_IPV6_ICMP_PING_OFFLOAD_ALL            = 3
     *
     * Default value is `NET_IPV6_ICMP_PING_OFFLOAD_DISABLED`.
     *
     */
    SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD_MODE = SPINEL_PROP_IPV6__BEGIN + 7, ///< [b]

    SPINEL_PROP_IPV6__END = 0x70,

    SPINEL_PROP_IPV6_EXT__BEGIN = 0x1600,
    SPINEL_PROP_IPV6_EXT__END   = 0x1700,

    SPINEL_PROP_STREAM__BEGIN = 0x70,

    /// Debug Stream
    /** Format: `U` (stream, read only)
     *
     * This property is a streaming property, meaning that you cannot explicitly
     * fetch the value of this property. The stream provides human-readable debugging
     * output which may be displayed in the host logs.
     *
     * The location of newline characters is not assumed by the host: it is
     * the NCP's responsibility to insert newline characters where needed,
     * just like with any other text stream.
     *
     * To receive the debugging stream, you wait for `CMD_PROP_VALUE_IS`
     * commands for this property from the NCP.
     *
     */
    SPINEL_PROP_STREAM_DEBUG = SPINEL_PROP_STREAM__BEGIN + 0,

    /// Raw Stream
    /** Format: `dD` (stream, read only)
     *  Required Capability: SPINEL_CAP_MAC_RAW or SPINEL_CAP_CONFIG_RADIO
     *
     * This stream provides the capability of sending and receiving raw 15.4 frames
     * to and from the radio. The exact format of the frame metadata and data is
     * dependent on the MAC and PHY being used.
     *
     * This property is a streaming property, meaning that you cannot explicitly
     * fetch the value of this property. To receive traffic, you wait for
     * `CMD_PROP_VALUE_IS` commands with this property id from the NCP.
     *
     * The general format of this property is:
     *
     *    `d` : frame data
     *    `D` : frame meta data
     *
     * The frame meta data is optional. Frame metadata MAY be empty or partially
     * specified. Partially specified metadata MUST be accepted. Default values
     * are used for all unspecified fields.
     *
     * The frame metadata field consists of the following fields:
     *
     *   `c` : Received Signal Strength (RSSI) in dBm - default is -128
     *   `c` : Noise floor in dBm - default is -128
     *   `S` : Flags (see below).
     *   `d` : PHY-specific data/struct
     *   `d` : Vendor-specific data/struct
     *
     * Flags fields are defined by the following enumeration bitfields:
     *
     *   SPINEL_MD_FLAG_TX       = 0x0001 :  Packet was transmitted, not received.
     *   SPINEL_MD_FLAG_BAD_FCS  = 0x0004 :  Packet was received with bad FCS
     *   SPINEL_MD_FLAG_DUPE     = 0x0008 :  Packet seems to be a duplicate
     *   SPINEL_MD_FLAG_RESERVED = 0xFFF2 :  Flags reserved for future use.
     *
     * The format of PHY-specific data for a Thread device contains the following
     * optional fields:

     *   `C` : 802.15.4 channel (Receive channel)
     *   `C` : IEEE 802.15.4 LQI
     *   `L` : The timestamp milliseconds
     *   `S` : The timestamp microseconds, offset to mMsec
     *
     * Frames written to this stream with `CMD_PROP_VALUE_SET` will be sent out
     * over the radio. This allows the caller to use the radio directly.
     *
     * The frame meta data for the `CMD_PROP_VALUE_SET` contains the following
     * optional fields.  Default values are used for all unspecified fields.
     *
     *  `C` : Channel (for frame tx)
     *  `C` : Maximum number of backoffs attempts before declaring CCA failure
     *        (use Thread stack default if not specified)
     *  `C` : Maximum number of retries allowed after a transmission failure
     *        (use Thread stack default if not specified)
     *  `b` : Set to true to enable CSMA-CA for this packet, false otherwise.
     *        (default true).
     *  `b` : Set to true to indicate it is a retransmission packet, false otherwise.
     *        (default false).
     *  `b` : Set to true to indicate that SubMac should skip AES processing, false otherwise.
     *        (default false).
     *
     */
    SPINEL_PROP_STREAM_RAW = SPINEL_PROP_STREAM__BEGIN + 1,

    /// (IPv6) Network Stream
    /** Format: `dD` (stream, read only)
     *
     * This stream provides the capability of sending and receiving (IPv6)
     * data packets to and from the currently attached network. The packets
     * are sent or received securely (encryption and authentication).
     *
     * This property is a streaming property, meaning that you cannot explicitly
     * fetch the value of this property. To receive traffic, you wait for
     * `CMD_PROP_VALUE_IS` commands with this property id from the NCP.
     *
     * To send network packets, you call `CMD_PROP_VALUE_SET` on this property with
     * the value of the packet.
     *
     * The general format of this property is:
     *
     *    `d` : packet data
     *    `D` : packet meta data
     *
     * The packet metadata is optional. Packet meta data MAY be empty or partially
     * specified. Partially specified metadata MUST be accepted. Default values
     * are used for all unspecified fields.
     *
     * For OpenThread the meta data is currently empty.
     *
     */
    SPINEL_PROP_STREAM_NET = SPINEL_PROP_STREAM__BEGIN + 2,

    /// (IPv6) Network Stream Insecure
    /** Format: `dD` (stream, read only)
     *
     * This stream provides the capability of sending and receiving unencrypted
     * and unauthenticated data packets to and from nearby devices for the
     * purposes of device commissioning.
     *
     * This property is a streaming property, meaning that you cannot explicitly
     * fetch the value of this property. To receive traffic, you wait for
     * `CMD_PROP_VALUE_IS` commands with this property id from the NCP.
     *
     * To send network packets, you call `CMD_PROP_VALUE_SET` on this property with
     * the value of the packet.
     *
     * The general format of this property is:
     *
     *    `d` : packet data
     *    `D` : packet meta data
     *
     * The packet metadata is optional. Packet meta data MAY be empty or partially
     * specified. Partially specified metadata MUST be accepted. Default values
     * are used for all unspecified fields.
     *
     * For OpenThread the meta data is currently empty.
     *
     */
    SPINEL_PROP_STREAM_NET_INSECURE = SPINEL_PROP_STREAM__BEGIN + 3,

    /// Log Stream
    /** Format: `UD` (stream, read only)
     *
     * This property is a read-only streaming property which provides
     * formatted log string from NCP. This property provides asynchronous
     * `CMD_PROP_VALUE_IS` updates with a new log string and includes
     * optional meta data.
     *
     *   `U`: The log string
     *   `D`: Log metadata (optional).
     *
     * Any data after the log string is considered metadata and is OPTIONAL.
     * Presence of `SPINEL_CAP_OPENTHREAD_LOG_METADATA` capability
     * indicates that OpenThread log metadata format is used as defined
     * below:
     *
     *    `C`: Log level (as per definition in enumeration
     *         `SPINEL_NCP_LOG_LEVEL_<level>`)
     *    `i`: OpenThread Log region (as per definition in enumeration
     *         `SPINEL_NCP_LOG_REGION_<region>).
     *    `X`: Log timestamp = <timestamp_base> + <current_time_ms>
     *
     */
    SPINEL_PROP_STREAM_LOG = SPINEL_PROP_STREAM__BEGIN + 4,

    SPINEL_PROP_STREAM__END = 0x80,

    SPINEL_PROP_STREAM_EXT__BEGIN = 0x1700,
    SPINEL_PROP_STREAM_EXT__END   = 0x1800,

    SPINEL_PROP_MESHCOP__BEGIN = 0x80,

    // Thread Joiner State
    /** Format `C` - Read Only
     *
     * Required capability: SPINEL_CAP_THREAD_JOINER
     *
     * The valid values are specified by `spinel_meshcop_joiner_state_t` (`SPINEL_MESHCOP_JOINER_STATE_<state>`)
     * enumeration.
     *
     */
    SPINEL_PROP_MESHCOP_JOINER_STATE = SPINEL_PROP_MESHCOP__BEGIN + 0, ///<[C]

    /// Thread Joiner Commissioning command and the parameters
    /** Format `b` or `bU(UUUUU)` (fields in parenthesis are optional) - Write Only
     *
     * This property starts or stops Joiner's commissioning process
     *
     * Required capability: SPINEL_CAP_THREAD_JOINER
     *
     * Writing to this property starts/stops the Joiner commissioning process.
     * The immediate `VALUE_IS` response indicates success/failure of the starting/stopping
     * the Joiner commissioning process.
     *
     * After a successful start operation, the join process outcome is reported through an
     * asynchronous `VALUE_IS(LAST_STATUS)` update with one of the following error status values:
     *
     *     - SPINEL_STATUS_JOIN_SUCCESS     the join process succeeded.
     *     - SPINEL_STATUS_JOIN_SECURITY    the join process failed due to security credentials.
     *     - SPINEL_STATUS_JOIN_NO_PEERS    no joinable network was discovered.
     *     - SPINEL_STATUS_JOIN_RSP_TIMEOUT if a response timed out.
     *     - SPINEL_STATUS_JOIN_FAILURE     join failure.
     *
     * Frame format:
     *
     *  `b` : Start or stop commissioning process (true to start).
     *
     * Only if the start commissioning.
     *
     *  `U` : Joiner's PSKd.
     *
     * The next fields are all optional. If not provided, OpenThread default values would be used.
     *
     *  `U` : Provisioning URL (use empty string if not required).
     *  `U` : Vendor Name. If not specified or empty string, use OpenThread default (PACKAGE_NAME).
     *  `U` : Vendor Model. If not specified or empty string, use OpenThread default (OPENTHREAD_CONFIG_PLATFORM_INFO).
     *  `U` : Vendor Sw Version. If not specified or empty string, use OpenThread default (PACKAGE_VERSION).
     *  `U` : Vendor Data String. Will not be appended if not specified.
     *
     */
    SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING = SPINEL_PROP_MESHCOP__BEGIN + 1,

    // Thread Commissioner State
    /** Format `C`
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * The valid values are specified by SPINEL_MESHCOP_COMMISSIONER_STATE_<state> enumeration.
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_STATE = SPINEL_PROP_MESHCOP__BEGIN + 2,

    // Thread Commissioner Joiners
    /** Format `A(t(t(E|CX)UL))` - get, insert or remove.
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Data per array entry is:
     *
     *  `t()` | `t(E)` | `t(CX)` : Joiner info struct (formatting varies).
     *
     *   -  `t()` or empty struct indicates any joiner.
     *   -  `t(E)` specifies the Joiner EUI-64.
     *   -  `t(CX) specifies Joiner Discerner, `C` is Discerner length (in bits), and `X` is Discerner value.
     *
     * The struct is followed by:
     *
     *  `L` : Timeout after which to remove Joiner (when written should be in seconds, when read is in milliseconds)
     *  `U` : PSKd
     *
     * For CMD_PROP_VALUE_REMOVE the timeout and PSKd are optional.
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS = SPINEL_PROP_MESHCOP__BEGIN + 3,

    // Thread Commissioner Provisioning URL
    /** Format `U`
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_PROVISIONING_URL = SPINEL_PROP_MESHCOP__BEGIN + 4,

    // Thread Commissioner Session ID
    /** Format `S` - Read only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_SESSION_ID = SPINEL_PROP_MESHCOP__BEGIN + 5,

    /// Thread Joiner Discerner
    /** Format `CX`  - Read-write
     *
     * Required capability: SPINEL_CAP_THREAD_JOINER
     *
     * This property represents a Joiner Discerner.
     *
     * The Joiner Discerner is used to calculate the Joiner ID used during commissioning/joining process.
     *
     * By default (when a discerner is not provided or cleared), Joiner ID is derived as first 64 bits of the result
     * of computing SHA-256 over factory-assigned IEEE EUI-64. Note that this is the main behavior expected by Thread
     * specification.
     *
     * Format:
     *
     *   'C' : The Joiner Discerner bit length (number of bits).
     *   `X` : The Joiner Discerner value (64-bit unsigned)  - Only present/applicable when length is non-zero.
     *
     * When writing to this property, the length can be set to zero to clear any previously set Joiner Discerner value.
     *
     * When reading this property if there is no currently set Joiner Discerner, zero is returned as the length (with
     * no value field).
     *
     */
    SPINEL_PROP_MESHCOP_JOINER_DISCERNER = SPINEL_PROP_MESHCOP__BEGIN + 6,

    SPINEL_PROP_MESHCOP__END = 0x90,

    SPINEL_PROP_MESHCOP_EXT__BEGIN = 0x1800,

    // Thread Commissioner Announce Begin
    /** Format `LCS6` - Write only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Writing to this property sends an Announce Begin message with the specified parameters. Response is a
     * `LAST_STATUS` update with status of operation.
     *
     *   `L` : Channel mask
     *   `C` : Number of messages per channel
     *   `S` : The time between two successive MLE Announce transmissions (milliseconds)
     *   `6` : IPv6 destination
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_ANNOUNCE_BEGIN = SPINEL_PROP_MESHCOP_EXT__BEGIN + 0,

    // Thread Commissioner Energy Scan Query
    /** Format `LCSS6` - Write only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Writing to this property sends an Energy Scan Query message with the specified parameters. Response is a
     * `LAST_STATUS` with status of operation. The energy scan results are emitted asynchronously through
     * `SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN_RESULT` updates.
     *
     * Format is:
     *
     *   `L` : Channel mask
     *   `C` : The number of energy measurements per channel
     *   `S` : The time between energy measurements (milliseconds)
     *   `S` : The scan duration for each energy measurement (milliseconds)
     *   `6` : IPv6 destination.
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN = SPINEL_PROP_MESHCOP_EXT__BEGIN + 1,

    // Thread Commissioner Energy Scan Result
    /** Format `Ld` - Asynchronous event only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * This property provides asynchronous `CMD_PROP_VALUE_INSERTED` updates to report energy scan results for a
     * previously sent Energy Scan Query message (please see `SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN`).
     *
     * Format is:
     *
     *   `L` : Channel mask
     *   `d` : Energy measurement data (note that `d` encoding includes the length)
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN_RESULT = SPINEL_PROP_MESHCOP_EXT__BEGIN + 2,

    // Thread Commissioner PAN ID Query
    /** Format `SL6` - Write only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Writing to this property sends a PAN ID Query message with the specified parameters. Response is a
     * `LAST_STATUS` with status of operation. The PAN ID Conflict results are emitted asynchronously through
     * `SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_CONFLICT_RESULT` updates.
     *
     * Format is:
     *
     *   `S` : PAN ID to query
     *   `L` : Channel mask
     *   `6` : IPv6 destination
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_QUERY = SPINEL_PROP_MESHCOP_EXT__BEGIN + 3,

    // Thread Commissioner PAN ID Conflict Result
    /** Format `SL` - Asynchronous event only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * This property provides asynchronous `CMD_PROP_VALUE_INSERTED` updates to report PAN ID conflict results for a
     * previously sent PAN ID Query message (please see `SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_QUERY`).
     *
     * Format is:
     *
     *   `S` : The PAN ID
     *   `L` : Channel mask
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_CONFLICT_RESULT = SPINEL_PROP_MESHCOP_EXT__BEGIN + 4,

    // Thread Commissioner Send MGMT_COMMISSIONER_GET
    /** Format `d` - Write only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Writing to this property sends a MGMT_COMMISSIONER_GET message with the specified parameters. Response is a
     * `LAST_STATUS` with status of operation.
     *
     * Format is:
     *
     *   `d` : List of TLV types to get
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_MGMT_GET = SPINEL_PROP_MESHCOP_EXT__BEGIN + 5,

    // Thread Commissioner Send MGMT_COMMISSIONER_SET
    /** Format `d` - Write only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Writing to this property sends a MGMT_COMMISSIONER_SET message with the specified parameters. Response is a
     * `LAST_STATUS` with status of operation.
     *
     * Format is:
     *
     *   `d` : TLV encoded data
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_MGMT_SET = SPINEL_PROP_MESHCOP_EXT__BEGIN + 6,

    // Thread Commissioner Generate PSKc
    /** Format: `UUd` - Write only
     *
     * Required capability: SPINEL_CAP_THREAD_COMMISSIONER
     *
     * Writing to this property allows user to generate PSKc from a given commissioning pass-phrase, network name,
     * extended PAN Id.
     *
     * Written value format is:
     *
     *   `U` : The commissioning pass-phrase.
     *   `U` : Network Name.
     *   `d` : Extended PAN ID.
     *
     * The response on success would be a `VALUE_IS` command with the PSKc with format below:
     *
     *   `D` : The PSKc
     *
     * On a failure a `LAST_STATUS` is emitted with the error status.
     *
     */
    SPINEL_PROP_MESHCOP_COMMISSIONER_GENERATE_PSKC = SPINEL_PROP_MESHCOP_EXT__BEGIN + 7,

    SPINEL_PROP_MESHCOP_EXT__END = 0x1900,

    SPINEL_PROP_OPENTHREAD__BEGIN = 0x1900,

    /// Channel Manager - Channel Change New Channel
    /** Format: `C` (read-write)
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * Setting this property triggers the Channel Manager to start
     * a channel change process. The network switches to the given
     * channel after the specified delay (see `CHANNEL_MANAGER_DELAY`).
     *
     * A subsequent write to this property will cancel an ongoing
     * (previously requested) channel change.
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_NEW_CHANNEL = SPINEL_PROP_OPENTHREAD__BEGIN + 0,

    /// Channel Manager - Channel Change Delay
    /** Format 'S'
     *  Units: seconds
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * This property specifies the delay (in seconds) to be used for
     * a channel change request.
     *
     * The delay should preferably be longer than maximum data poll
     * interval used by all sleepy-end-devices within the Thread
     * network.
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_DELAY = SPINEL_PROP_OPENTHREAD__BEGIN + 1,

    /// Channel Manager Supported Channels
    /** Format 'A(C)'
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * This property specifies the list of supported channels.
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_SUPPORTED_CHANNELS = SPINEL_PROP_OPENTHREAD__BEGIN + 2,

    /// Channel Manager Favored Channels
    /** Format 'A(C)'
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * This property specifies the list of favored channels (when `ChannelManager` is asked to select channel)
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_FAVORED_CHANNELS = SPINEL_PROP_OPENTHREAD__BEGIN + 3,

    /// Channel Manager Channel Select Trigger
    /** Format 'b'
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * Writing to this property triggers a request on `ChannelManager` to select a new channel.
     *
     * Once a Channel Select is triggered, the Channel Manager will perform the following 3 steps:
     *
     * 1) `ChannelManager` decides if the channel change would be helpful. This check can be skipped if in the input
     *    boolean to this property is set to `true` (skipping the quality check).
     *    This step uses the collected link quality metrics on the device such as CCA failure rate, frame and message
     *    error rates per neighbor, etc. to determine if the current channel quality is at the level that justifies
     *    a channel change.
     *
     * 2) If first step passes, then `ChannelManager` selects a potentially better channel. It uses the collected
     *    channel quality data by `ChannelMonitor` module. The supported and favored channels are used at this step.
     *
     * 3) If the newly selected channel is different from the current channel, `ChannelManager` requests/starts the
     *    channel change process.
     *
     * Reading this property always yields `false`.
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_CHANNEL_SELECT = SPINEL_PROP_OPENTHREAD__BEGIN + 4,

    /// Channel Manager Auto Channel Selection Enabled
    /** Format 'b'
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * This property indicates if auto-channel-selection functionality is enabled/disabled on `ChannelManager`.
     *
     * When enabled, `ChannelManager` will periodically checks and attempts to select a new channel. The period interval
     * is specified by `SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL`.
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_ENABLED = SPINEL_PROP_OPENTHREAD__BEGIN + 5,

    /// Channel Manager Auto Channel Selection Interval
    /** Format 'L'
     *  units: seconds
     *
     * Required capability: SPINEL_CAP_CHANNEL_MANAGER
     *
     * This property specifies the auto-channel-selection check interval (in seconds).
     *
     */
    SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL = SPINEL_PROP_OPENTHREAD__BEGIN + 6,

    /// Thread network time.
    /** Format: `Xc` - Read only
     *
     * Data per item is:
     *
     *  `X`: The Thread network time, in microseconds.
     *  `c`: Time synchronization status.
     *
     */
    SPINEL_PROP_THREAD_NETWORK_TIME = SPINEL_PROP_OPENTHREAD__BEGIN + 7,

    /// Thread time synchronization period
    /** Format: `S` - Read-Write
     *
     * Data per item is:
     *
     *  `S`: Time synchronization period, in seconds.
     *
     */
    SPINEL_PROP_TIME_SYNC_PERIOD = SPINEL_PROP_OPENTHREAD__BEGIN + 8,

    /// Thread Time synchronization XTAL accuracy threshold for Router
    /** Format: `S` - Read-Write
     *
     * Data per item is:
     *
     *  `S`: The XTAL accuracy threshold for Router, in PPM.
     *
     */
    SPINEL_PROP_TIME_SYNC_XTAL_THRESHOLD = SPINEL_PROP_OPENTHREAD__BEGIN + 9,

    /// Child Supervision Interval
    /** Format: `S` - Read-Write
     *  Units: Seconds
     *
     * Required capability: `SPINEL_CAP_CHILD_SUPERVISION`
     *
     * The child supervision interval (in seconds). Zero indicates that child supervision is disabled.
     *
     * When enabled, Child supervision feature ensures that at least one message is sent to every sleepy child within
     * the given supervision interval. If there is no other message, a supervision message (a data message with empty
     * payload) is enqueued and sent to the child.
     *
     * This property is available for FTD build only.
     *
     */
    SPINEL_PROP_CHILD_SUPERVISION_INTERVAL = SPINEL_PROP_OPENTHREAD__BEGIN + 10,

    /// Child Supervision Check Timeout
    /** Format: `S` - Read-Write
     *  Units: Seconds
     *
     * Required capability: `SPINEL_CAP_CHILD_SUPERVISION`
     *
     * The child supervision check timeout interval (in seconds). Zero indicates supervision check on the child is
     * disabled.
     *
     * Supervision check is only applicable on a sleepy child. When enabled, if the child does not hear from its parent
     * within the specified check timeout, it initiates a re-attach process by starting an MLE Child Update
     * Request/Response exchange with the parent.
     *
     * This property is available for FTD and MTD builds.
     *
     */
    SPINEL_PROP_CHILD_SUPERVISION_CHECK_TIMEOUT = SPINEL_PROP_OPENTHREAD__BEGIN + 11,

    // RCP (NCP in radio only mode) version
    /** Format `U` - Read only
     *
     * Required capability: SPINEL_CAP_POSIX
     *
     * This property gives the version string of RCP (NCP in radio mode) which is being controlled by a POSIX
     * application. It is available only in "POSIX" platform (i.e., `OPENTHREAD_PLATFORM_POSIX` is enabled).
     *
     */
    SPINEL_PROP_RCP_VERSION = SPINEL_PROP_OPENTHREAD__BEGIN + 12,

    /// Thread Parent Response info
    /** Format: `ESccCCCb` - Asynchronous event only
     *
     *  `E`: Extended address
     *  `S`: RLOC16
     *  `c`: Instant RSSI
     *  'c': Parent Priority
     *  `C`: Link Quality3
     *  `C`: Link Quality2
     *  `C`: Link Quality1
     *  'b': Is the node receiving parent response frame attached
     *
     * This property sends Parent Response frame information to the Host.
     * This property is available for FTD build only.
     *
     */
    SPINEL_PROP_PARENT_RESPONSE_INFO = SPINEL_PROP_OPENTHREAD__BEGIN + 13,

    /// SLAAC enabled
    /** Format `b` - Read-Write
     *  Required capability: `SPINEL_CAP_SLAAC`
     *
     * This property allows the host to enable/disable SLAAC module on NCP at run-time. When SLAAC module is enabled,
     * SLAAC addresses (based on on-mesh prefixes in Network Data) are added to the interface. When SLAAC module is
     * disabled any previously added SLAAC address is removed.
     *
     */
    SPINEL_PROP_SLAAC_ENABLED = SPINEL_PROP_OPENTHREAD__BEGIN + 14,

    // Supported Radio Links (by device)
    /**
     * Format `A(i)` - Read only
     *
     * This property returns list of supported radio links by the device itself. Enumeration `SPINEL_RADIO_LINK_{TYPE}`
     * values indicate different radio link types.
     *
     */
    SPINEL_PROP_SUPPORTED_RADIO_LINKS = SPINEL_PROP_OPENTHREAD__BEGIN + 15,

    /// Neighbor Table Multi Radio Link Info
    /** Format: `A(t(ESA(t(iC))))` - Read only
     * Required capability: `SPINEL_CAP_MULTI_RADIO`.
     *
     * Each item represents info about a neighbor:
     *
     *  `E`: Neighbor's Extended Address
     *  `S`: Neighbor's RLOC16
     *
     *  This is then followed by an array of radio link info structures indicating which radio links are supported by
     *  the neighbor:
     *
     *    `i` : Radio link type (enumeration `SPINEL_RADIO_LINK_{TYPE}`).
     *    `C` : Preference value associated with radio link.
     *
     */
    SPINEL_PROP_NEIGHBOR_TABLE_MULTI_RADIO_INFO = SPINEL_PROP_OPENTHREAD__BEGIN + 16,

    /// SRP Client Start
    /** Format: `b(6Sb)` - Write only
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * Writing to this property allows user to start or stop the SRP client operation with a given SRP server.
     *
     * Written value format is:
     *
     *   `b` : TRUE to start the client, FALSE to stop the client.
     *
     * When used to start the SRP client, the following fields should also be included:
     *
     *   `6` : SRP server IPv6 address.
     *   `U` : SRP server port number.
     *   `b` : Boolean to indicate whether or not to emit SRP client events (using `SPINEL_PROP_SRP_CLIENT_EVENT`).
     *
     */
    SPINEL_PROP_SRP_CLIENT_START = SPINEL_PROP_OPENTHREAD__BEGIN + 17,

    /// SRP Client Lease Interval
    /** Format: `L` - Read/Write
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * The lease interval used in SRP update requests (in seconds).
     *
     */
    SPINEL_PROP_SRP_CLIENT_LEASE_INTERVAL = SPINEL_PROP_OPENTHREAD__BEGIN + 18,

    /// SRP Client Key Lease Interval
    /** Format: `L` - Read/Write
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * The key lease interval used in SRP update requests (in seconds).
     *
     */
    SPINEL_PROP_SRP_CLIENT_KEY_LEASE_INTERVAL = SPINEL_PROP_OPENTHREAD__BEGIN + 19,

    /// SRP Client Host Info
    /** Format: `UCt(A(6))` - Read only
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * Format is:
     *
     *   `U`       : The host name.
     *   `C`       : The host state (values from `spinel_srp_client_item_state_t`).
     *   `t(A(6))` : Structure containing array of host IPv6 addresses.
     *
     */
    SPINEL_PROP_SRP_CLIENT_HOST_INFO = SPINEL_PROP_OPENTHREAD__BEGIN + 20,

    /// SRP Client Host Name (label).
    /** Format: `U` - Read/Write
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     */
    SPINEL_PROP_SRP_CLIENT_HOST_NAME = SPINEL_PROP_OPENTHREAD__BEGIN + 21,

    /// SRP Client Host Addresses
    /** Format: `A(6)` - Read/Write
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     */
    SPINEL_PROP_SRP_CLIENT_HOST_ADDRESSES = SPINEL_PROP_OPENTHREAD__BEGIN + 22,

    /// SRP Client Services
    /** Format: `A(t(UUSSSd))` - Read/Insert/Remove
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * This property provides a list/array of services.
     *
     * Data per item for `SPINEL_CMD_PROP_VALUE_GET` and/or `SPINEL_CMD_PROP_VALUE_INSERT` operation is as follows:
     *
     *   `U` : The service name labels (e.g., "_chip._udp", not the full domain name.
     *   `U` : The service instance name label (not the full name).
     *   `S` : The service port number.
     *   `S` : The service priority.
     *   `S` : The service weight.
     *
     * For `SPINEL_CMD_PROP_VALUE_REMOVE` command, the following format is used:
     *
     *   `U` : The service name labels (e.g., "_chip._udp", not the full domain name.
     *   `U` : The service instance name label (not the full name).
     *   `b` : Indicates whether to clear the service entry (optional).
     *
     * The last boolean (`b`) field is optional. When included it indicates on `true` to clear the service (clear it
     * on client immediately with no interaction to server) and on `false` to remove the service (inform server and
     * wait for the service entry to be removed on server). If it is not included, the value is `false`.
     *
     */
    SPINEL_PROP_SRP_CLIENT_SERVICES = SPINEL_PROP_OPENTHREAD__BEGIN + 23,

    /// SRP Client Host And Services Remove
    /** Format: `bb` : Write only
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * Writing to this property with starts the remove process of the host info and all services.
     * Please see `otSrpClientRemoveHostAndServices()` for more details.
     *
     * Format is:
     *
     *    `b` : A boolean indicating whether or not the host key lease should also be cleared.
     *    `b` : A boolean indicating whether or not to send update to server when host info is not registered.
     *
     */
    SPINEL_PROP_SRP_CLIENT_HOST_SERVICES_REMOVE = SPINEL_PROP_OPENTHREAD__BEGIN + 24,

    /// SRP Client Host And Services Clear
    /** Format: Empty : Write only
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * Writing to this property clears all host info and all the services.
     * Please see `otSrpClientClearHostAndServices()` for more details.
     *
     */
    SPINEL_PROP_SRP_CLIENT_HOST_SERVICES_CLEAR = SPINEL_PROP_OPENTHREAD__BEGIN + 25,

    /// SRP Client Event
    /** Format: t() : Asynchronous event only
     * Required capability: `SPINEL_CAP_SRP_CLIENT`.
     *
     * This property is asynchronously emitted when there is an event from SRP client notifying some state changes or
     * errors.
     *
     * The general format of this property is as follows:
     *
     *    `S` : Error code (see `spinel_srp_client_error_t` enumeration).
     *    `d` : Host info data.
     *    `d` : Active services.
     *    `d` : Removed services.
     *
     * The host info data contains:
     *
     *   `U`       : The host name.
     *   `C`       : The host state (values from `spinel_srp_client_item_state_t`).
     *   `t(A(6))` : Structure containing array of host IPv6 addresses.
     *
     * The active or removed services data is an array of services `A(t(UUSSSd))` with each service format:
     *
     *   `U` : The service name labels (e.g., "_chip._udp", not the full domain name.
     *   `U` : The service instance name label (not the full name).
     *   `S` : The service port number.
     *   `S` : The service priority.
     *   `S` : The service weight.
     *   `d` : The encoded TXT-DATA.
     *
     */
    SPINEL_PROP_SRP_CLIENT_EVENT = SPINEL_PROP_OPENTHREAD__BEGIN + 26,

    /// SRP Client Service Key Inclusion Enabled
    /** Format `b` : Read-Write
     * Required capability: `SPINEL_CAP_SRP_CLIENT` & `SPINEL_CAP_REFERENCE_DEVICE`.
     *
     * This boolean property indicates whether the "service key record inclusion" mode is enabled or not.
     *
     * When enabled, SRP client will include KEY record in Service Description Instructions in the SRP update messages
     * that it sends.
     *
     * KEY record is optional in Service Description Instruction (it is required and always included in the Host
     * Description Instruction). The default behavior of SRP client is to not include it. This function is intended to
     * override the default behavior for testing only.
     *
     */
    SPINEL_PROP_SRP_CLIENT_SERVICE_KEY_ENABLED = SPINEL_PROP_OPENTHREAD__BEGIN + 27,

    SPINEL_PROP_OPENTHREAD__END = 0x2000,

    SPINEL_PROP_SERVER__BEGIN = 0xA0,

    /// Server Allow Local Network Data Change
    /** Format `b` - Read-write
     *
     * Required capability: SPINEL_CAP_THREAD_SERVICE
     *
     * Set to true before changing local server net data. Set to false when finished.
     * This allows changes to be aggregated into a single event.
     *
     */
    SPINEL_PROP_SERVER_ALLOW_LOCAL_DATA_CHANGE = SPINEL_PROP_SERVER__BEGIN + 0,

    // Server Services
    /** Format: `A(t(LdbdS))`
     *
     * This property provides all services registered on the device
     *
     * Required capability: SPINEL_CAP_THREAD_SERVICE
     *
     * Array of structures containing:
     *
     *  `L`: Enterprise Number
     *  `d`: Service Data
     *  `b`: Stable
     *  `d`: Server Data
     *  `S`: RLOC
     *
     */
    SPINEL_PROP_SERVER_SERVICES = SPINEL_PROP_SERVER__BEGIN + 1,

    // Server Leader Services
    /** Format: `A(t(CLdbdS))`
     *
     * This property provides all services registered on the leader
     *
     * Array of structures containing:
     *
     *  `C`: Service ID
     *  `L`: Enterprise Number
     *  `d`: Service Data
     *  `b`: Stable
     *  `d`: Server Data
     *  `S`: RLOC
     *
     */
    SPINEL_PROP_SERVER_LEADER_SERVICES = SPINEL_PROP_SERVER__BEGIN + 2,

    SPINEL_PROP_SERVER__END = 0xB0,

    SPINEL_PROP_RCP__BEGIN = 0xB0,

    /// RCP API Version number
    /** Format: `i` (read-only)
     *
     * Required capability: SPINEL_CAP_RADIO and SPINEL_CAP_RCP_API_VERSION.
     *
     * This property gives the RCP API Version number.
     *
     * Please see "Spinel definition compatibility guideline" section.
     *
     */
    SPINEL_PROP_RCP_API_VERSION = SPINEL_PROP_RCP__BEGIN + 0,

    SPINEL_PROP_RCP__END = 0xFF,

    SPINEL_PROP_INTERFACE__BEGIN = 0x100,

    /// UART Bitrate
    /** Format: `L`
     *
     *  If the NCP is using a UART to communicate with the host,
     *  this property allows the host to change the bitrate
     *  of the serial connection. The value encoding is `L`,
     *  which is a little-endian 32-bit unsigned integer.
     *  The host should not assume that all possible numeric values
     *  are supported.
     *
     *  If implemented by the NCP, this property should be persistent
     *  across software resets and forgotten upon hardware resets.
     *
     *  This property is only implemented when a UART is being
     *  used for Spinel. This property is optional.
     *
     *  When changing the bitrate, all frames will be received
     *  at the previous bitrate until the response frame to this command
     *  is received. Once a successful response frame is received by
     *  the host, all further frames will be transmitted at the new
     *  bitrate.
     */
    SPINEL_PROP_UART_BITRATE = SPINEL_PROP_INTERFACE__BEGIN + 0,

    /// UART Software Flow Control
    /** Format: `b`
     *
     *  If the NCP is using a UART to communicate with the host,
     *  this property allows the host to determine if software flow
     *  control (XON/XOFF style) should be used and (optionally) to
     *  turn it on or off.
     *
     *  This property is only implemented when a UART is being
     *  used for Spinel. This property is optional.
     */
    SPINEL_PROP_UART_XON_XOFF = SPINEL_PROP_INTERFACE__BEGIN + 1,

    SPINEL_PROP_INTERFACE__END = 0x200,

    SPINEL_PROP_15_4_PIB__BEGIN = 0x400,
    // For direct access to the 802.15.4 PID.
    // Individual registers are fetched using
    // `SPINEL_PROP_15_4_PIB__BEGIN+[PIB_IDENTIFIER]`
    // Only supported if SPINEL_CAP_15_4_PIB is set.
    //
    // For brevity, the entire 802.15.4 PIB space is
    // not defined here, but a few choice attributes
    // are defined for illustration and convenience.
    SPINEL_PROP_15_4_PIB_PHY_CHANNELS_SUPPORTED = SPINEL_PROP_15_4_PIB__BEGIN + 0x01, ///< [A(L)]
    SPINEL_PROP_15_4_PIB_MAC_PROMISCUOUS_MODE   = SPINEL_PROP_15_4_PIB__BEGIN + 0x51, ///< [b]
    SPINEL_PROP_15_4_PIB_MAC_SECURITY_ENABLED   = SPINEL_PROP_15_4_PIB__BEGIN + 0x5d, ///< [b]
    SPINEL_PROP_15_4_PIB__END                   = 0x500,

    SPINEL_PROP_CNTR__BEGIN = 0x500,

    /// Counter reset
    /** Format: Empty (Write only).
     *
     * Writing to this property (with any value) will reset all MAC, MLE, IP, and NCP counters to zero.
     *
     */
    SPINEL_PROP_CNTR_RESET = SPINEL_PROP_CNTR__BEGIN + 0,

    /// The total number of transmissions.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_TOTAL = SPINEL_PROP_CNTR__BEGIN + 1,

    /// The number of transmissions with ack request.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_ACK_REQ = SPINEL_PROP_CNTR__BEGIN + 2,

    /// The number of transmissions that were acked.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_ACKED = SPINEL_PROP_CNTR__BEGIN + 3,

    /// The number of transmissions without ack request.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_NO_ACK_REQ = SPINEL_PROP_CNTR__BEGIN + 4,

    /// The number of transmitted data.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_DATA = SPINEL_PROP_CNTR__BEGIN + 5,

    /// The number of transmitted data poll.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_DATA_POLL = SPINEL_PROP_CNTR__BEGIN + 6,

    /// The number of transmitted beacon.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_BEACON = SPINEL_PROP_CNTR__BEGIN + 7,

    /// The number of transmitted beacon request.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_BEACON_REQ = SPINEL_PROP_CNTR__BEGIN + 8,

    /// The number of transmitted other types of frames.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_OTHER = SPINEL_PROP_CNTR__BEGIN + 9,

    /// The number of retransmission times.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_RETRY = SPINEL_PROP_CNTR__BEGIN + 10,

    /// The number of CCA failure times.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_ERR_CCA = SPINEL_PROP_CNTR__BEGIN + 11,

    /// The number of unicast packets transmitted.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_UNICAST = SPINEL_PROP_CNTR__BEGIN + 12,

    /// The number of broadcast packets transmitted.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_PKT_BROADCAST = SPINEL_PROP_CNTR__BEGIN + 13,

    /// The number of frame transmission failures due to abort error.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_ERR_ABORT = SPINEL_PROP_CNTR__BEGIN + 14,

    /// The total number of received packets.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_TOTAL = SPINEL_PROP_CNTR__BEGIN + 100,

    /// The number of received data.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_DATA = SPINEL_PROP_CNTR__BEGIN + 101,

    /// The number of received data poll.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_DATA_POLL = SPINEL_PROP_CNTR__BEGIN + 102,

    /// The number of received beacon.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_BEACON = SPINEL_PROP_CNTR__BEGIN + 103,

    /// The number of received beacon request.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_BEACON_REQ = SPINEL_PROP_CNTR__BEGIN + 104,

    /// The number of received other types of frames.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_OTHER = SPINEL_PROP_CNTR__BEGIN + 105,

    /// The number of received packets filtered by allowlist.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_FILT_WL = SPINEL_PROP_CNTR__BEGIN + 106,

    /// The number of received packets filtered by destination check.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_FILT_DA = SPINEL_PROP_CNTR__BEGIN + 107,

    /// The number of received packets that are empty.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_ERR_EMPTY = SPINEL_PROP_CNTR__BEGIN + 108,

    /// The number of received packets from an unknown neighbor.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_ERR_UKWN_NBR = SPINEL_PROP_CNTR__BEGIN + 109,

    /// The number of received packets whose source address is invalid.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_ERR_NVLD_SADDR = SPINEL_PROP_CNTR__BEGIN + 110,

    /// The number of received packets with a security error.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_ERR_SECURITY = SPINEL_PROP_CNTR__BEGIN + 111,

    /// The number of received packets with a checksum error.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_ERR_BAD_FCS = SPINEL_PROP_CNTR__BEGIN + 112,

    /// The number of received packets with other errors.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_ERR_OTHER = SPINEL_PROP_CNTR__BEGIN + 113,

    /// The number of received duplicated.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_DUP = SPINEL_PROP_CNTR__BEGIN + 114,

    /// The number of unicast packets received.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_UNICAST = SPINEL_PROP_CNTR__BEGIN + 115,

    /// The number of broadcast packets received.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_PKT_BROADCAST = SPINEL_PROP_CNTR__BEGIN + 116,

    /// The total number of secure transmitted IP messages.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_IP_SEC_TOTAL = SPINEL_PROP_CNTR__BEGIN + 200,

    /// The total number of insecure transmitted IP messages.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_IP_INSEC_TOTAL = SPINEL_PROP_CNTR__BEGIN + 201,

    /// The number of dropped (not transmitted) IP messages.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_IP_DROPPED = SPINEL_PROP_CNTR__BEGIN + 202,

    /// The total number of secure received IP message.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_IP_SEC_TOTAL = SPINEL_PROP_CNTR__BEGIN + 203,

    /// The total number of insecure received IP message.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_IP_INSEC_TOTAL = SPINEL_PROP_CNTR__BEGIN + 204,

    /// The number of dropped received IP messages.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_IP_DROPPED = SPINEL_PROP_CNTR__BEGIN + 205,

    /// The number of transmitted spinel frames.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_TX_SPINEL_TOTAL = SPINEL_PROP_CNTR__BEGIN + 300,

    /// The number of received spinel frames.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_SPINEL_TOTAL = SPINEL_PROP_CNTR__BEGIN + 301,

    /// The number of received spinel frames with error.
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_SPINEL_ERR = SPINEL_PROP_CNTR__BEGIN + 302,

    /// Number of out of order received spinel frames (tid increase by more than 1).
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_RX_SPINEL_OUT_OF_ORDER_TID = SPINEL_PROP_CNTR__BEGIN + 303,

    /// The number of successful Tx IP packets
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_IP_TX_SUCCESS = SPINEL_PROP_CNTR__BEGIN + 304,

    /// The number of successful Rx IP packets
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_IP_RX_SUCCESS = SPINEL_PROP_CNTR__BEGIN + 305,

    /// The number of failed Tx IP packets
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_IP_TX_FAILURE = SPINEL_PROP_CNTR__BEGIN + 306,

    /// The number of failed Rx IP packets
    /** Format: `L` (Read-only) */
    SPINEL_PROP_CNTR_IP_RX_FAILURE = SPINEL_PROP_CNTR__BEGIN + 307,

    /// The message buffer counter info
    /** Format: `SSSSSSSSSSSSSSSS` (Read-only)
     *      `S`, (TotalBuffers)           The number of buffers in the pool.
     *      `S`, (FreeBuffers)            The number of free message buffers.
     *      `S`, (6loSendMessages)        The number of messages in the 6lo send queue.
     *      `S`, (6loSendBuffers)         The number of buffers in the 6lo send queue.
     *      `S`, (6loReassemblyMessages)  The number of messages in the 6LoWPAN reassembly queue.
     *      `S`, (6loReassemblyBuffers)   The number of buffers in the 6LoWPAN reassembly queue.
     *      `S`, (Ip6Messages)            The number of messages in the IPv6 send queue.
     *      `S`, (Ip6Buffers)             The number of buffers in the IPv6 send queue.
     *      `S`, (MplMessages)            The number of messages in the MPL send queue.
     *      `S`, (MplBuffers)             The number of buffers in the MPL send queue.
     *      `S`, (MleMessages)            The number of messages in the MLE send queue.
     *      `S`, (MleBuffers)             The number of buffers in the MLE send queue.
     *      `S`, (ArpMessages)            The number of messages in the ARP send queue.
     *      `S`, (ArpBuffers)             The number of buffers in the ARP send queue.
     *      `S`, (CoapMessages)           The number of messages in the CoAP send queue.
     *      `S`, (CoapBuffers)            The number of buffers in the CoAP send queue.
     */
    SPINEL_PROP_MSG_BUFFER_COUNTERS = SPINEL_PROP_CNTR__BEGIN + 400,

    /// All MAC related counters.
    /** Format: t(A(L))t(A(L))
     *
     * The contents include two structs, first one corresponds to
     * all transmit related MAC counters, second one provides the
     * receive related counters.
     *
     * The transmit structure includes:
     *
     *   'L': TxTotal                  (The total number of transmissions).
     *   'L': TxUnicast                (The total number of unicast transmissions).
     *   'L': TxBroadcast              (The total number of broadcast transmissions).
     *   'L': TxAckRequested           (The number of transmissions with ack request).
     *   'L': TxAcked                  (The number of transmissions that were acked).
     *   'L': TxNoAckRequested         (The number of transmissions without ack request).
     *   'L': TxData                   (The number of transmitted data).
     *   'L': TxDataPoll               (The number of transmitted data poll).
     *   'L': TxBeacon                 (The number of transmitted beacon).
     *   'L': TxBeaconRequest          (The number of transmitted beacon request).
     *   'L': TxOther                  (The number of transmitted other types of frames).
     *   'L': TxRetry                  (The number of retransmission times).
     *   'L': TxErrCca                 (The number of CCA failure times).
     *   'L': TxErrAbort               (The number of frame transmission failures due to abort error).
     *   'L': TxErrBusyChannel         (The number of frames that were dropped due to a busy channel).
     *   'L': TxDirectMaxRetryExpiry   (The number of expired retransmission retries for direct message).
     *   'L': TxIndirectMaxRetryExpiry (The number of expired retransmission retries for indirect message).
     *
     * The receive structure includes:
     *
     *   'L': RxTotal                  (The total number of received packets).
     *   'L': RxUnicast                (The total number of unicast packets received).
     *   'L': RxBroadcast              (The total number of broadcast packets received).
     *   'L': RxData                   (The number of received data).
     *   'L': RxDataPoll               (The number of received data poll).
     *   'L': RxBeacon                 (The number of received beacon).
     *   'L': RxBeaconRequest          (The number of received beacon request).
     *   'L': RxOther                  (The number of received other types of frames).
     *   'L': RxAddressFiltered        (The number of received packets filtered by address filter
     *                                  (allowlist or denylist)).
     *   'L': RxDestAddrFiltered       (The number of received packets filtered by destination check).
     *   'L': RxDuplicated             (The number of received duplicated packets).
     *   'L': RxErrNoFrame             (The number of received packets with no or malformed content).
     *   'L': RxErrUnknownNeighbor     (The number of received packets from unknown neighbor).
     *   'L': RxErrInvalidSrcAddr      (The number of received packets whose source address is invalid).
     *   'L': RxErrSec                 (The number of received packets with security error).
     *   'L': RxErrFcs                 (The number of received packets with FCS error).
     *   'L': RxErrOther               (The number of received packets with other error).
     *
     * Writing to this property with any value would reset all MAC counters to zero.
     *
     */
    SPINEL_PROP_CNTR_ALL_MAC_COUNTERS = SPINEL_PROP_CNTR__BEGIN + 401,

    /// Thread MLE counters.
    /** Format: `SSSSSSSSS`
     *
     *   'S': DisabledRole                  (The number of times device entered OT_DEVICE_ROLE_DISABLED role).
     *   'S': DetachedRole                  (The number of times device entered OT_DEVICE_ROLE_DETACHED role).
     *   'S': ChildRole                     (The number of times device entered OT_DEVICE_ROLE_CHILD role).
     *   'S': RouterRole                    (The number of times device entered OT_DEVICE_ROLE_ROUTER role).
     *   'S': LeaderRole                    (The number of times device entered OT_DEVICE_ROLE_LEADER role).
     *   'S': AttachAttempts                (The number of attach attempts while device was detached).
     *   'S': PartitionIdChanges            (The number of changes to partition ID).
     *   'S': BetterPartitionAttachAttempts (The number of attempts to attach to a better partition).
     *   'S': ParentChanges                 (The number of times device changed its parents).
     *
     * Writing to this property with any value would reset all MLE counters to zero.
     *
     */
    SPINEL_PROP_CNTR_MLE_COUNTERS = SPINEL_PROP_CNTR__BEGIN + 402,

    /// Thread IPv6 counters.
    /** Format: `t(LL)t(LL)`
     *
     * The contents include two structs, first one corresponds to
     * all transmit related MAC counters, second one provides the
     * receive related counters.
     *
     * The transmit structure includes:
     *   'L': TxSuccess (The number of IPv6 packets successfully transmitted).
     *   'L': TxFailure (The number of IPv6 packets failed to transmit).
     *
     * The receive structure includes:
     *   'L': RxSuccess (The number of IPv6 packets successfully received).
     *   'L': RxFailure (The number of IPv6 packets failed to receive).
     *
     * Writing to this property with any value would reset all IPv6 counters to zero.
     *
     */
    SPINEL_PROP_CNTR_ALL_IP_COUNTERS = SPINEL_PROP_CNTR__BEGIN + 403,

    /// MAC retry histogram.
    /** Format: t(A(L))t(A(L))
     *
     * Required capability: SPINEL_CAP_MAC_RETRY_HISTOGRAM
     *
     * The contents include two structs, first one is histogram which corresponds to retransmissions number of direct
     * messages, second one provides the histogram of retransmissions for indirect messages.
     *
     * The first structure includes:
     *   'L': DirectRetry[0]                   (The number of packets after 0 retry).
     *   'L': DirectRetry[1]                   (The number of packets after 1 retry).
     *    ...
     *   'L': DirectRetry[n]                   (The number of packets after n retry).
     *
     * The size of the array is OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_DIRECT.
     *
     * The second structure includes:
     *   'L': IndirectRetry[0]                   (The number of packets after 0 retry).
     *   'L': IndirectRetry[1]                   (The number of packets after 1 retry).
     *    ...
     *   'L': IndirectRetry[m]                   (The number of packets after m retry).
     *
     * The size of the array is OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_INDIRECT.
     *
     * Writing to this property with any value would reset MAC retry histogram.
     *
     */
    SPINEL_PROP_CNTR_MAC_RETRY_HISTOGRAM = SPINEL_PROP_CNTR__BEGIN + 404,

    SPINEL_PROP_CNTR__END = 0x800,

    SPINEL_PROP_RCP_EXT__BEGIN = 0x800,

    /// MAC Key
    /** Format: `CCddd`.
     *
     *  `C`: MAC key ID mode
     *  `C`: MAC key ID
     *  `d`: previous MAC key material data
     *  `d`: current MAC key material data
     *  `d`: next MAC key material data
     *
     * The Spinel property is used to set/get MAC key materials to and from RCP.
     *
     */
    SPINEL_PROP_RCP_MAC_KEY = SPINEL_PROP_RCP_EXT__BEGIN + 0,

    /// MAC Frame Counter
    /** Format: `L`.
     *
     *  `L`: MAC frame counter
     *
     * The Spinel property is used to set MAC frame counter to RCP.
     *
     */
    SPINEL_PROP_RCP_MAC_FRAME_COUNTER = SPINEL_PROP_RCP_EXT__BEGIN + 1,

    /// Timestamps when Spinel frame is received and transmitted
    /** Format: `X`.
     *
     *  `X`: Spinel frame transmit timestamp
     *
     * The Spinel property is used to get timestamp from RCP to calculate host and RCP timer difference.
     *
     */
    SPINEL_PROP_RCP_TIMESTAMP = SPINEL_PROP_RCP_EXT__BEGIN + 2,

    SPINEL_PROP_RCP_EXT__END = 0x900,

    SPINEL_PROP_NEST__BEGIN = 0x3BC0,

    SPINEL_PROP_NEST_STREAM_MFG = SPINEL_PROP_NEST__BEGIN + 0,

    /// The legacy network ULA prefix (8 bytes)
    /** Format: 'D' */
    SPINEL_PROP_NEST_LEGACY_ULA_PREFIX = SPINEL_PROP_NEST__BEGIN + 1,

    /// The EUI64 of last node joined using legacy protocol (if none, all zero EUI64 is returned).
    /** Format: 'E' */
    SPINEL_PROP_NEST_LEGACY_LAST_NODE_JOINED = SPINEL_PROP_NEST__BEGIN + 2,

    SPINEL_PROP_NEST__END = 0x3C00,

    SPINEL_PROP_VENDOR__BEGIN = 0x3C00,
    SPINEL_PROP_VENDOR__END   = 0x4000,

    SPINEL_PROP_DEBUG__BEGIN = 0x4000,

    /// Testing platform assert
    /** Format: 'b' (read-only)
     *
     * Reading this property will cause an assert on the NCP. This is intended for testing the assert functionality of
     * underlying platform/NCP. Assert should ideally cause the NCP to reset, but if this is not supported a `false`
     * boolean is returned in response.
     *
     */
    SPINEL_PROP_DEBUG_TEST_ASSERT = SPINEL_PROP_DEBUG__BEGIN + 0,

    /// The NCP log level.
    /** Format: `C` */
    SPINEL_PROP_DEBUG_NCP_LOG_LEVEL = SPINEL_PROP_DEBUG__BEGIN + 1,

    /// Testing platform watchdog
    /** Format: Empty  (read-only)
     *
     * Reading this property will causes NCP to start a `while(true) ;` loop and thus triggering a watchdog.
     *
     * This is intended for testing the watchdog functionality on the underlying platform/NCP.
     *
     */
    SPINEL_PROP_DEBUG_TEST_WATCHDOG = SPINEL_PROP_DEBUG__BEGIN + 2,

    /// The NCP timestamp base
    /** Format: X (write-only)
     *
     * This property controls the time base value that is used for logs timestamp field calculation.
     *
     */
    SPINEL_PROP_DEBUG_LOG_TIMESTAMP_BASE = SPINEL_PROP_DEBUG__BEGIN + 3,

    /// TREL Radio Link - test mode enable
    /** Format `b` (read-write)
     *
     * This property is intended for testing TREL (Thread Radio Encapsulation Link) radio type only (during simulation).
     * It allows the TREL interface to be temporarily disabled and (re)enabled.  While disabled all traffic through
     * TREL interface is dropped silently (to emulate a radio/interface down scenario).
     *
     * This property is only available when the TREL radio link type is supported.
     *
     */
    SPINEL_PROP_DEBUG_TREL_TEST_MODE_ENABLE = SPINEL_PROP_DEBUG__BEGIN + 4,

    SPINEL_PROP_DEBUG__END = 0x4400,

    SPINEL_PROP_EXPERIMENTAL__BEGIN = 2000000,
    SPINEL_PROP_EXPERIMENTAL__END   = 2097152,
};

typedef uint32_t spinel_prop_key_t;

// ----------------------------------------------------------------------------

#define SPINEL_HEADER_FLAG 0x80

#define SPINEL_HEADER_TID_SHIFT 0
#define SPINEL_HEADER_TID_MASK (15 << SPINEL_HEADER_TID_SHIFT)

#define SPINEL_HEADER_IID_SHIFT 4
#define SPINEL_HEADER_IID_MASK (3 << SPINEL_HEADER_IID_SHIFT)

#define SPINEL_HEADER_IID_0 (0 << SPINEL_HEADER_IID_SHIFT)
#define SPINEL_HEADER_IID_1 (1 << SPINEL_HEADER_IID_SHIFT)
#define SPINEL_HEADER_IID_2 (2 << SPINEL_HEADER_IID_SHIFT)
#define SPINEL_HEADER_IID_3 (3 << SPINEL_HEADER_IID_SHIFT)

#define SPINEL_HEADER_GET_IID(x) (((x)&SPINEL_HEADER_IID_MASK) >> SPINEL_HEADER_IID_SHIFT)
#define SPINEL_HEADER_GET_TID(x) (spinel_tid_t)(((x)&SPINEL_HEADER_TID_MASK) >> SPINEL_HEADER_TID_SHIFT)

#define SPINEL_GET_NEXT_TID(x) (spinel_tid_t)((x) >= 0xF ? 1 : (x) + 1)

#define SPINEL_BEACON_THREAD_FLAG_VERSION_SHIFT 4

#define SPINEL_BEACON_THREAD_FLAG_VERSION_MASK (0xf << SPINEL_BEACON_THREAD_FLAG_VERSION_SHIFT)

#define SPINEL_BEACON_THREAD_FLAG_JOINABLE (1 << 0)

#define SPINEL_BEACON_THREAD_FLAG_NATIVE (1 << 3)

// ----------------------------------------------------------------------------

enum
{
    SPINEL_DATATYPE_NULL_C        = 0,
    SPINEL_DATATYPE_VOID_C        = '.',
    SPINEL_DATATYPE_BOOL_C        = 'b',
    SPINEL_DATATYPE_UINT8_C       = 'C',
    SPINEL_DATATYPE_INT8_C        = 'c',
    SPINEL_DATATYPE_UINT16_C      = 'S',
    SPINEL_DATATYPE_INT16_C       = 's',
    SPINEL_DATATYPE_UINT32_C      = 'L',
    SPINEL_DATATYPE_INT32_C       = 'l',
    SPINEL_DATATYPE_UINT64_C      = 'X',
    SPINEL_DATATYPE_INT64_C       = 'x',
    SPINEL_DATATYPE_UINT_PACKED_C = 'i',
    SPINEL_DATATYPE_IPv6ADDR_C    = '6',
    SPINEL_DATATYPE_EUI64_C       = 'E',
    SPINEL_DATATYPE_EUI48_C       = 'e',
    SPINEL_DATATYPE_DATA_WLEN_C   = 'd',
    SPINEL_DATATYPE_DATA_C        = 'D',
    SPINEL_DATATYPE_UTF8_C        = 'U', //!< Zero-Terminated UTF8-Encoded String
    SPINEL_DATATYPE_STRUCT_C      = 't',
    SPINEL_DATATYPE_ARRAY_C       = 'A',
};

typedef char spinel_datatype_t;

#define SPINEL_DATATYPE_NULL_S ""
#define SPINEL_DATATYPE_VOID_S "."
#define SPINEL_DATATYPE_BOOL_S "b"
#define SPINEL_DATATYPE_UINT8_S "C"
#define SPINEL_DATATYPE_INT8_S "c"
#define SPINEL_DATATYPE_UINT16_S "S"
#define SPINEL_DATATYPE_INT16_S "s"
#define SPINEL_DATATYPE_UINT32_S "L"
#define SPINEL_DATATYPE_INT32_S "l"
#define SPINEL_DATATYPE_UINT64_S "X"
#define SPINEL_DATATYPE_INT64_S "x"
#define SPINEL_DATATYPE_UINT_PACKED_S "i"
#define SPINEL_DATATYPE_IPv6ADDR_S "6"
#define SPINEL_DATATYPE_EUI64_S "E"
#define SPINEL_DATATYPE_EUI48_S "e"
#define SPINEL_DATATYPE_DATA_WLEN_S "d"
#define SPINEL_DATATYPE_DATA_S "D"
#define SPINEL_DATATYPE_UTF8_S "U" //!< Zero-Terminated UTF8-Encoded String

#define SPINEL_DATATYPE_ARRAY_S(x) "A(" x ")"
#define SPINEL_DATATYPE_STRUCT_S(x) "t(" x ")"

#define SPINEL_DATATYPE_ARRAY_STRUCT_S(x) SPINEL_DATATYPE_ARRAY_S(SPINEL_DATATYPE_STRUCT_WLEN_S(x))

#define SPINEL_DATATYPE_COMMAND_S                   \
    SPINEL_DATATYPE_UINT8_S           /* header  */ \
        SPINEL_DATATYPE_UINT_PACKED_S /* command */

#define SPINEL_DATATYPE_COMMAND_PROP_S                    \
    SPINEL_DATATYPE_COMMAND_S         /* prop command  */ \
        SPINEL_DATATYPE_UINT_PACKED_S /* property id */

#define SPINEL_MAX_UINT_PACKED 2097151

SPINEL_API_EXTERN spinel_ssize_t spinel_datatype_pack(uint8_t *     data_out,
                                                      spinel_size_t data_len_max,
                                                      const char *  pack_format,
                                                      ...);
SPINEL_API_EXTERN spinel_ssize_t spinel_datatype_vpack(uint8_t *     data_out,
                                                       spinel_size_t data_len_max,
                                                       const char *  pack_format,
                                                       va_list       args);
SPINEL_API_EXTERN spinel_ssize_t spinel_datatype_unpack(const uint8_t *data_in,
                                                        spinel_size_t  data_len,
                                                        const char *   pack_format,
                                                        ...);
/**
 * This function parses spinel data similar to sscanf().
 *
 * This function actually calls spinel_datatype_vunpack_in_place() to parse data.
 *
 * @param[in]   data_in     A pointer to the data to parse.
 * @param[in]   data_len    The length of @p data_in in bytes.
 * @param[in]   pack_format C string that contains a format string follows the same specification of spinel.
 * @param[in]   ...         Additional arguments depending on the format string @p pack_format.
 *
 * @returns The parsed length in bytes.
 *
 * @note This function behaves different from `spinel_datatype_unpack()`:
 *       - This function expects composite data arguments of pointer to data type, while `spinel_datatype_unpack()`
 *         expects them of pointer to data type pointer. For example, if `SPINEL_DATATYPE_EUI64_C` is present in
 *         @p pack_format, this function expects a `spinel_eui64_t *` is included in variable arguments, while
 *         `spinel_datatype_unpack()` expects a `spinel_eui64_t **` is included.
 *       - For `SPINEL_DATATYPE_UTF8_C`, this function expects two arguments, the first of type `char *` and the
 *         second is of type `size_t` to indicate length of the provided buffer in the first argument just like
 *         `strncpy()`, while `spinel_datatype_unpack()` only expects a `const char **`.
 *
 * @sa spinel_datatype_vunpack_in_place()
 *
 */
SPINEL_API_EXTERN spinel_ssize_t spinel_datatype_unpack_in_place(const uint8_t *data_in,
                                                                 spinel_size_t  data_len,
                                                                 const char *   pack_format,
                                                                 ...);
SPINEL_API_EXTERN spinel_ssize_t spinel_datatype_vunpack(const uint8_t *data_in,
                                                         spinel_size_t  data_len,
                                                         const char *   pack_format,
                                                         va_list        args);
/**
 * This function parses spinel data similar to vsscanf().
 *
 * @param[in]   data_in     A pointer to the data to parse.
 * @param[in]   data_len    The length of @p data_in in bytes.
 * @param[in]   pack_format C string that contains a format string follows the same specification of spinel.
 * @param[in]   args        A value identifying a variable arguments list.
 *
 * @returns The parsed length in bytes.
 *
 * @note This function behaves different from `spinel_datatype_vunpack()`:
 *       - This function expects composite data arguments of pointer to data type, while `spinel_datatype_vunpack()`
 *         expects them of pointer to data type pointer. For example, if `SPINEL_DATATYPE_EUI64_C` is present in
 *         @p pack_format, this function expects a `spinel_eui64_t *` is included in variable arguments, while
 *         `spinel_datatype_vunpack()` expects a `spinel_eui64_t **` is included.
 *       - For `SPINEL_DATATYPE_UTF8_C`, this function expects two arguments, the first of type `char *` and the
 *         second is of type `size_t` to indicate length of the provided buffer in the first argument just like
 *         `strncpy()`, while `spinel_datatype_vunpack()` only expects a `const char **`.
 *
 * @sa spinel_datatype_unpack_in_place()
 *
 */
SPINEL_API_EXTERN spinel_ssize_t spinel_datatype_vunpack_in_place(const uint8_t *data_in,
                                                                  spinel_size_t  data_len,
                                                                  const char *   pack_format,
                                                                  va_list        args);

SPINEL_API_EXTERN spinel_ssize_t spinel_packed_uint_decode(const uint8_t *bytes,
                                                           spinel_size_t  len,
                                                           unsigned int * value_ptr);
SPINEL_API_EXTERN spinel_ssize_t spinel_packed_uint_encode(uint8_t *bytes, spinel_size_t len, unsigned int value);
SPINEL_API_EXTERN spinel_ssize_t spinel_packed_uint_size(unsigned int value);

SPINEL_API_EXTERN const char *spinel_next_packed_datatype(const char *pack_format);

// ----------------------------------------------------------------------------

SPINEL_API_EXTERN const char *spinel_command_to_cstr(spinel_command_t command);

SPINEL_API_EXTERN const char *spinel_prop_key_to_cstr(spinel_prop_key_t prop_key);

SPINEL_API_EXTERN const char *spinel_net_role_to_cstr(uint8_t net_role);

SPINEL_API_EXTERN const char *spinel_mcu_power_state_to_cstr(uint8_t mcu_power_state);

SPINEL_API_EXTERN const char *spinel_status_to_cstr(spinel_status_t status);

SPINEL_API_EXTERN const char *spinel_capability_to_cstr(spinel_capability_t capability);

SPINEL_API_EXTERN const char *spinel_radio_link_to_cstr(uint32_t radio);

SPINEL_API_EXTERN const char *spinel_link_metrics_status_to_cstr(uint8_t status);

// ----------------------------------------------------------------------------

#if defined(__cplusplus)
}
#endif

#endif /* defined(SPINEL_HEADER_INCLUDED) */
