/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *  This file defines the top-level functions for the OpenThread CoAP implementation.
 */

#ifndef OPENTHREAD_COAP_H_
#define OPENTHREAD_COAP_H_

#include <stdint.h>

#include <openthread/ip6.h>
#include <openthread/message.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-coap
 *
 * @brief
 *   This module includes functions that control CoAP communication.
 *
 *   The functions in this module are available when CoAP API feature (`OPENTHREAD_CONFIG_COAP_API_ENABLE`) is enabled.
 *
 * @{
 *
 */

#define OT_DEFAULT_COAP_PORT 5683 ///< Default CoAP port, as specified in RFC 7252

#define OT_COAP_DEFAULT_TOKEN_LENGTH 2 ///< Default token length.

#define OT_COAP_MAX_TOKEN_LENGTH 8 ///< Max token length as specified (RFC 7252).

#define OT_COAP_MAX_RETRANSMIT 20 ///< Max retransmit supported by OpenThread.

#define OT_COAP_MIN_ACK_TIMEOUT 1000 ///< Minimal ACK timeout in milliseconds supported by OpenThread.

/**
 * CoAP Type values (2 bit unsigned integer).
 *
 */
typedef enum otCoapType
{
    OT_COAP_TYPE_CONFIRMABLE     = 0, ///< Confirmable
    OT_COAP_TYPE_NON_CONFIRMABLE = 1, ///< Non-confirmable
    OT_COAP_TYPE_ACKNOWLEDGMENT  = 2, ///< Acknowledgment
    OT_COAP_TYPE_RESET           = 3, ///< Reset
} otCoapType;

/**
 * Helper macro to define CoAP Code values.
 *
 */
#define OT_COAP_CODE(c, d) ((((c)&0x7) << 5) | ((d)&0x1f))

/**
 * CoAP Code values.
 *
 */
typedef enum otCoapCode
{
    OT_COAP_CODE_EMPTY  = OT_COAP_CODE(0, 0), ///< Empty message code
    OT_COAP_CODE_GET    = OT_COAP_CODE(0, 1), ///< Get
    OT_COAP_CODE_POST   = OT_COAP_CODE(0, 2), ///< Post
    OT_COAP_CODE_PUT    = OT_COAP_CODE(0, 3), ///< Put
    OT_COAP_CODE_DELETE = OT_COAP_CODE(0, 4), ///< Delete

    OT_COAP_CODE_RESPONSE_MIN = OT_COAP_CODE(2, 0),  ///< 2.00
    OT_COAP_CODE_CREATED      = OT_COAP_CODE(2, 1),  ///< Created
    OT_COAP_CODE_DELETED      = OT_COAP_CODE(2, 2),  ///< Deleted
    OT_COAP_CODE_VALID        = OT_COAP_CODE(2, 3),  ///< Valid
    OT_COAP_CODE_CHANGED      = OT_COAP_CODE(2, 4),  ///< Changed
    OT_COAP_CODE_CONTENT      = OT_COAP_CODE(2, 5),  ///< Content
    OT_COAP_CODE_CONTINUE     = OT_COAP_CODE(2, 31), ///< RFC7959 Continue

    OT_COAP_CODE_BAD_REQUEST         = OT_COAP_CODE(4, 0),  ///< Bad Request
    OT_COAP_CODE_UNAUTHORIZED        = OT_COAP_CODE(4, 1),  ///< Unauthorized
    OT_COAP_CODE_BAD_OPTION          = OT_COAP_CODE(4, 2),  ///< Bad Option
    OT_COAP_CODE_FORBIDDEN           = OT_COAP_CODE(4, 3),  ///< Forbidden
    OT_COAP_CODE_NOT_FOUND           = OT_COAP_CODE(4, 4),  ///< Not Found
    OT_COAP_CODE_METHOD_NOT_ALLOWED  = OT_COAP_CODE(4, 5),  ///< Method Not Allowed
    OT_COAP_CODE_NOT_ACCEPTABLE      = OT_COAP_CODE(4, 6),  ///< Not Acceptable
    OT_COAP_CODE_REQUEST_INCOMPLETE  = OT_COAP_CODE(4, 8),  ///< RFC7959 Request Entity Incomplete
    OT_COAP_CODE_PRECONDITION_FAILED = OT_COAP_CODE(4, 12), ///< Precondition Failed
    OT_COAP_CODE_REQUEST_TOO_LARGE   = OT_COAP_CODE(4, 13), ///< Request Entity Too Large
    OT_COAP_CODE_UNSUPPORTED_FORMAT  = OT_COAP_CODE(4, 15), ///< Unsupported Content-Format

    OT_COAP_CODE_INTERNAL_ERROR      = OT_COAP_CODE(5, 0), ///< Internal Server Error
    OT_COAP_CODE_NOT_IMPLEMENTED     = OT_COAP_CODE(5, 1), ///< Not Implemented
    OT_COAP_CODE_BAD_GATEWAY         = OT_COAP_CODE(5, 2), ///< Bad Gateway
    OT_COAP_CODE_SERVICE_UNAVAILABLE = OT_COAP_CODE(5, 3), ///< Service Unavailable
    OT_COAP_CODE_GATEWAY_TIMEOUT     = OT_COAP_CODE(5, 4), ///< Gateway Timeout
    OT_COAP_CODE_PROXY_NOT_SUPPORTED = OT_COAP_CODE(5, 5), ///< Proxying Not Supported
} otCoapCode;

/**
 * CoAP Option Numbers
 */
typedef enum otCoapOptionType
{
    OT_COAP_OPTION_IF_MATCH       = 1,  ///< If-Match
    OT_COAP_OPTION_URI_HOST       = 3,  ///< Uri-Host
    OT_COAP_OPTION_E_TAG          = 4,  ///< ETag
    OT_COAP_OPTION_IF_NONE_MATCH  = 5,  ///< If-None-Match
    OT_COAP_OPTION_OBSERVE        = 6,  ///< Observe [RFC7641]
    OT_COAP_OPTION_URI_PORT       = 7,  ///< Uri-Port
    OT_COAP_OPTION_LOCATION_PATH  = 8,  ///< Location-Path
    OT_COAP_OPTION_URI_PATH       = 11, ///< Uri-Path
    OT_COAP_OPTION_CONTENT_FORMAT = 12, ///< Content-Format
    OT_COAP_OPTION_MAX_AGE        = 14, ///< Max-Age
    OT_COAP_OPTION_URI_QUERY      = 15, ///< Uri-Query
    OT_COAP_OPTION_ACCEPT         = 17, ///< Accept
    OT_COAP_OPTION_LOCATION_QUERY = 20, ///< Location-Query
    OT_COAP_OPTION_BLOCK2         = 23, ///< Block2 (RFC7959)
    OT_COAP_OPTION_BLOCK1         = 27, ///< Block1 (RFC7959)
    OT_COAP_OPTION_SIZE2          = 28, ///< Size2 (RFC7959)
    OT_COAP_OPTION_PROXY_URI      = 35, ///< Proxy-Uri
    OT_COAP_OPTION_PROXY_SCHEME   = 39, ///< Proxy-Scheme
    OT_COAP_OPTION_SIZE1          = 60, ///< Size1
} otCoapOptionType;

/**
 * This structure represents a CoAP option.
 *
 */
typedef struct otCoapOption
{
    uint16_t mNumber; ///< Option Number
    uint16_t mLength; ///< Option Length
} otCoapOption;

/**
 * This structure acts as an iterator for CoAP options
 *
 */
typedef struct otCoapOptionIterator
{
    const otMessage *mMessage;          ///< CoAP message
    otCoapOption     mOption;           ///< CoAP message option
    uint16_t         mNextOptionOffset; ///< Byte offset of next option
} otCoapOptionIterator;

/**
 * CoAP Content Format codes.  The full list is documented at
 * https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
 */
typedef enum otCoapOptionContentFormat
{
    /**
     * text/plain; charset=utf-8: [RFC2046][RFC3676][RFC5147]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_TEXT_PLAIN = 0,

    /**
     * application/cose; cose-type="cose-encrypt0": [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_ENCRYPT0 = 16,

    /**
     * application/cose; cose-type="cose-mac0": [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_MAC0 = 17,

    /**
     * application/cose; cose-type="cose-sign1": [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_SIGN1 = 18,

    /**
     * application/link-format: [RFC6690]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_LINK_FORMAT = 40,

    /**
     * application/xml: [RFC3023]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_XML = 41,

    /**
     * application/octet-stream: [RFC2045][RFC2046]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_OCTET_STREAM = 42,

    /**
     * application/exi:
     * ["Efficient XML Interchange (EXI) Format 1.0 (Second Edition)", February 2014]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_EXI = 47,

    /**
     * application/json: [RFC7159]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_JSON = 50,

    /**
     * application/json-patch+json: [RFC6902]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_JSON_PATCH_JSON = 51,

    /**
     * application/merge-patch+json: [RFC7396]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_MERGE_PATCH_JSON = 52,

    /**
     * application/cbor: [RFC7049]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_CBOR = 60,

    /**
     * application/cwt: [RFC8392]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_CWT = 61,

    /**
     * application/cose; cose-type="cose-encrypt": [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_ENCRYPT = 96,

    /**
     * application/cose; cose-type="cose-mac": [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_MAC = 97,

    /**
     * application/cose; cose-type="cose-sign": [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_SIGN = 98,

    /**
     * application/cose-key: [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_KEY = 101,

    /**
     * application/cose-key-set: [RFC8152]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COSE_KEY_SET = 102,

    /**
     * application/senml+json: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENML_JSON = 110,

    /**
     * application/sensml+json: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENSML_JSON = 111,

    /**
     * application/senml+cbor: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENML_CBOR = 112,

    /**
     * application/sensml+cbor: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENSML_CBOR = 113,

    /**
     * application/senml-exi: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENML_EXI = 114,

    /**
     * application/sensml-exi: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENSML_EXI = 115,

    /**
     * application/coap-group+json: [RFC7390]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_COAP_GROUP_JSON = 256,

    /**
     * application/senml+xml: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENML_XML = 310,

    /**
     * application/sensml+xml: [RFC8428]
     */
    OT_COAP_OPTION_CONTENT_FORMAT_SENSML_XML = 311
} otCoapOptionContentFormat;

/**
 * CoAP Block Size Exponents
 */
typedef enum otCoapBlockSzx
{
    OT_COAP_OPTION_BLOCK_SZX_16   = 0,
    OT_COAP_OPTION_BLOCK_SZX_32   = 1,
    OT_COAP_OPTION_BLOCK_SZX_64   = 2,
    OT_COAP_OPTION_BLOCK_SZX_128  = 3,
    OT_COAP_OPTION_BLOCK_SZX_256  = 4,
    OT_COAP_OPTION_BLOCK_SZX_512  = 5,
    OT_COAP_OPTION_BLOCK_SZX_1024 = 6
} otCoapBlockSzx;

/**
 * This function pointer is called when a CoAP response is received or on the request timeout.
 *
 * @param[in]  aContext      A pointer to application-specific context.
 * @param[in]  aMessage      A pointer to the message buffer containing the response. NULL if no response was received.
 * @param[in]  aMessageInfo  A pointer to the message info for @p aMessage. NULL if no response was received.
 * @param[in]  aResult       A result of the CoAP transaction.
 *
 * @retval  OT_ERROR_NONE              A response was received successfully.
 * @retval  OT_ERROR_ABORT             A CoAP transaction was reset by peer.
 * @retval  OT_ERROR_RESPONSE_TIMEOUT  No response or acknowledgment received during timeout period.
 *
 */
typedef void (*otCoapResponseHandler)(void *               aContext,
                                      otMessage *          aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError              aResult);

/**
 * This function pointer is called when a CoAP request with a given Uri-Path is received.
 *
 * @param[in]  aContext      A pointer to arbitrary context information.
 * @param[in]  aMessage      A pointer to the message.
 * @param[in]  aMessageInfo  A pointer to the message info for @p aMessage.
 *
 */
typedef void (*otCoapRequestHandler)(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * This function pointer is called when a CoAP message with an block-wise transfer option is received.
 *
 * This function is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE configuration
 * is enabled.
 *
 * @param[in]  aContext     A pointer to application-specific context.
 * @param[in]  aBlock       A pointer to the block segment.
 * @param[in]  aPosition    The position of @p aBlock in a sequence in bytes.
 * @param[in]  aBlockLength The length of the block segment in bytes.
 * @param[in]  aMore        Flag if more block segments are following.
 * @param[in]  aTotalLength The total length in bytes of the transferred information (indicated by a Size1 or Size2
 *                          option).
 *
 * @retval  OT_ERROR_NONE               Block segment was stored successfully.
 * @retval  OT_ERROR_NO_BUFS            No more memory to store blocks.
 * @retval  OT_ERROR_NO_FRAME_RECEIVED  Block segment missing.
 *
 */
typedef otError (*otCoapBlockwiseReceiveHook)(void *         aContext,
                                              const uint8_t *aBlock,
                                              uint32_t       aPosition,
                                              uint16_t       aBlockLength,
                                              bool           aMore,
                                              uint32_t       aTotalLength);

/**
 * This function pointer is called before the next block in a block-wise transfer is sent.
 *
 * This function is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE configuration
 * is enabled.
 *
 * @param[in]       aContext     A pointer to application-specific context.
 * @param[inout]    aBlock       A pointer to where the block segment can be written to.
 * @param[in]       aPosition    The position in a sequence from which to obtain the block segment.
 * @param[inout]    aBlockLength On entry, the maximum block segment length in bytes.
 * @param[out]      aMore        A pointer to the flag if more block segments will follow.
 *
 * @warning By changing the value of aBlockLength, the block size of the whole exchange is
 *          renegotiated. It is recommended to do this after the first block has been received as
 *          later changes could cause problems with other CoAP implementations.
 *
 * @retval  OT_ERROR_NONE           No error occurred.
 * @retval  OT_ERROR_INVALID_ARGS   Block at @p aPosition does not exist.
 *
 */
typedef otError (*otCoapBlockwiseTransmitHook)(void *    aContext,
                                               uint8_t * aBlock,
                                               uint32_t  aPosition,
                                               uint16_t *aBlockLength,
                                               bool *    aMore);

/**
 * This structure represents a CoAP resource.
 *
 */
typedef struct otCoapResource
{
    const char *           mUriPath; ///< The URI Path string
    otCoapRequestHandler   mHandler; ///< The callback for handling a received request
    void *                 mContext; ///< Application-specific context
    struct otCoapResource *mNext;    ///< The next CoAP resource in the list
} otCoapResource;

/**
 * This structure represents a CoAP resource with block-wise transfer.
 *
 */
typedef struct otCoapBlockwiseResource
{
    const char *         mUriPath; ///< The URI Path string
    otCoapRequestHandler mHandler; ///< The callback for handling a received request

    /** The callback for handling incoming block-wise transfer.
     *  This callback is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
     *  configuration is enabled.
     */
    otCoapBlockwiseReceiveHook mReceiveHook;

    /** The callback for handling outgoing block-wise transfer.
     *  This callback is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
     *  configuration is enabled.
     */
    otCoapBlockwiseTransmitHook     mTransmitHook;
    void *                          mContext; ///< Application-specific context
    struct otCoapBlockwiseResource *mNext;    ///< The next CoAP resource in the list
} otCoapBlockwiseResource;

/**
 * This structure represents the CoAP transmission parameters.
 *
 * @note mAckTimeout * ((2 ** (mMaxRetransmit + 1)) - 1) * (mAckRandomFactorNumerator / mAckRandomFactorDenominator)
 *       must not exceed what can be represented by a uint32_t (0xffffffff). This limitation allows OpenThread to
 *       avoid 64-bit arithmetic.
 *
 */
typedef struct otCoapTxParameters
{
    /**
     * Minimum spacing before first retransmission when ACK is not received, in milliseconds (RFC7252 default value is
     * 2000ms).
     *
     */
    uint32_t mAckTimeout;

    /**
     * Numerator of ACK_RANDOM_FACTOR used to calculate maximum spacing before first retransmission when ACK is not
     * received (RFC7252 default value of ACK_RANDOM_FACTOR is 1.5; must not be decreased below 1).
     *
     */
    uint8_t mAckRandomFactorNumerator;

    /**
     * Denominator of ACK_RANDOM_FACTOR used to calculate maximum spacing before first retransmission when ACK is not
     * received (RFC7252 default value of ACK_RANDOM_FACTOR is 1.5; must not be decreased below 1).
     *
     */
    uint8_t mAckRandomFactorDenominator;

    /**
     * Maximum number of retransmissions for CoAP Confirmable messages (RFC7252 default value is 4).
     *
     */
    uint8_t mMaxRetransmit;
} otCoapTxParameters;

/**
 * This function initializes the CoAP header.
 *
 * @param[inout] aMessage   A pointer to the CoAP message to initialize.
 * @param[in]    aType      CoAP message type.
 * @param[in]    aCode      CoAP message code.
 *
 */
void otCoapMessageInit(otMessage *aMessage, otCoapType aType, otCoapCode aCode);

/**
 * This function initializes a response message.
 *
 * @note Both message ID and token are set according to @p aRequest.
 *
 * @param[inout] aResponse  A pointer to the CoAP response message.
 * @param[in]    aRequest   A pointer to the CoAP request message.
 * @param[in]    aType      CoAP message type.
 * @param[in]    aCode      CoAP message code.
 *
 * @retval OT_ERROR_NONE     Successfully initialized the response message.
 * @retval OT_ERROR_NO_BUFS  Insufficient message buffers available to initialize the response message.
 *
 */
otError otCoapMessageInitResponse(otMessage *aResponse, const otMessage *aRequest, otCoapType aType, otCoapCode aCode);

/**
 * This function sets the Token value and length in a header.
 *
 * @param[inout]  aMessage          A pointer to the CoAP message.
 * @param[in]     aToken            A pointer to the Token value.
 * @param[in]     aTokenLength      The Length of @p aToken.
 *
 * @retval OT_ERROR_NONE     Successfully set the Token value.
 * @retval OT_ERROR_NO_BUFS  Insufficient buffers to set the Token value.
 *
 */
otError otCoapMessageSetToken(otMessage *aMessage, const uint8_t *aToken, uint8_t aTokenLength);

/**
 * This function sets the Token length and randomizes its value.
 *
 * @param[inout]  aMessage      A pointer to the CoAP message.
 * @param[in]     aTokenLength  The Length of a Token to set.
 *
 */
void otCoapMessageGenerateToken(otMessage *aMessage, uint8_t aTokenLength);

/**
 * This function appends the Content Format CoAP option as specified in
 * https://tools.ietf.org/html/rfc7252#page-92.  This *must* be called before
 * setting otCoapMessageSetPayloadMarker if a payload is to be included in the
 * message.
 *
 * The function is a convenience wrapper around otCoapMessageAppendUintOption,
 * and if the desired format type code isn't listed in otCoapOptionContentFormat,
 * this base function should be used instead.
 *
 * @param[inout]  aMessage          A pointer to the CoAP message.
 * @param[in]     aContentFormat    One of the content formats listed in
 *                                  otCoapOptionContentFormat above.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendContentFormatOption(otMessage *aMessage, otCoapOptionContentFormat aContentFormat);

/**
 * This function appends a CoAP option in a header.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aNumber   The CoAP Option number.
 * @param[in]     aLength   The CoAP Option length.
 * @param[in]     aValue    A pointer to the CoAP value.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendOption(otMessage *aMessage, uint16_t aNumber, uint16_t aLength, const void *aValue);

/**
 * This function appends an unsigned integer CoAP option as specified in
 * https://tools.ietf.org/html/rfc7252#section-3.2
 *
 * @param[inout]  aMessage A pointer to the CoAP message.
 * @param[in]     aNumber  The CoAP Option number.
 * @param[in]     aValue   The CoAP Option unsigned integer value.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 * @see otCoapMessageGetOptionUintValue
 */
otError otCoapMessageAppendUintOption(otMessage *aMessage, uint16_t aNumber, uint32_t aValue);

/**
 * This function appends an Observe option.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aObserve  Observe field value.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendObserveOption(otMessage *aMessage, uint32_t aObserve);

/**
 * This function appends a Uri-Path option.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aUriPath  A pointer to a NULL-terminated string.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendUriPathOptions(otMessage *aMessage, const char *aUriPath);

/**
 * This function converts a CoAP Block option SZX field to the actual block size
 *
 * @param[in]     aSize     Block size exponent.
 *
 * @returns The actual size exponent value.
 *
 */
uint16_t otCoapBlockSizeFromExponent(otCoapBlockSzx aSize);

/**
 * This function appends a Block2 option
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aNum      Current block number.
 * @param[in]     aMore     Boolean to indicate more blocks are to be sent.
 * @param[in]     aSize     Block Size Exponent.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendBlock2Option(otMessage *aMessage, uint32_t aNum, bool aMore, otCoapBlockSzx aSize);

/**
 * This function appends a Block1 option
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aNum      Current block number.
 * @param[in]     aMore     Boolean to indicate more blocks are to be sent.
 * @param[in]     aSize     Block Size Exponent.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendBlock1Option(otMessage *aMessage, uint32_t aNum, bool aMore, otCoapBlockSzx aSize);

/**
 * This function appends a Proxy-Uri option.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aUriPath  A pointer to a NULL-terminated string.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendProxyUriOption(otMessage *aMessage, const char *aUriPath);

/**
 * This function appends a Max-Age option.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aMaxAge   The Max-Age value.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 *
 */
otError otCoapMessageAppendMaxAgeOption(otMessage *aMessage, uint32_t aMaxAge);

/**
 * This function appends a single Uri-Query option.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 * @param[in]     aUriQuery A pointer to NULL-terminated string, which should contain a single key=value pair.
 *
 * @retval OT_ERROR_NONE          Successfully appended the option.
 * @retval OT_ERROR_INVALID_ARGS  The option type is not equal or greater than the last option type.
 * @retval OT_ERROR_NO_BUFS       The option length exceeds the buffer size.
 */
otError otCoapMessageAppendUriQueryOption(otMessage *aMessage, const char *aUriQuery);

/**
 * This function adds Payload Marker indicating beginning of the payload to the CoAP header.
 *
 * @param[inout]  aMessage  A pointer to the CoAP message.
 *
 * @retval OT_ERROR_NONE     Payload Marker successfully added.
 * @retval OT_ERROR_NO_BUFS  Header Payload Marker exceeds the buffer size.
 *
 */
otError otCoapMessageSetPayloadMarker(otMessage *aMessage);

/**
 * This function returns the Type value.
 *
 * @param[in]  aMessage  A pointer to the CoAP message.
 *
 * @returns The Type value.
 *
 */
otCoapType otCoapMessageGetType(const otMessage *aMessage);

/**
 * This function returns the Code value.
 *
 * @param[in]  aMessage  A pointer to the CoAP message.
 *
 * @returns The Code value.
 *
 */
otCoapCode otCoapMessageGetCode(const otMessage *aMessage);

/**
 * This method returns the CoAP Code as human readable string.
 *
 * @param[in]   aMessage    A pointer to the CoAP message.
 *
 * @ returns The CoAP Code as string.
 *
 */
const char *otCoapMessageCodeToString(const otMessage *aMessage);

/**
 * This function returns the Message ID value.
 *
 * @param[in]  aMessage  A pointer to the CoAP message.
 *
 * @returns The Message ID value.
 *
 */
uint16_t otCoapMessageGetMessageId(const otMessage *aMessage);

/**
 * This function returns the Token length.
 *
 * @param[in]  aMessage  A pointer to the CoAP message.
 *
 * @returns The Token length.
 *
 */
uint8_t otCoapMessageGetTokenLength(const otMessage *aMessage);

/**
 * This function returns a pointer to the Token value.
 *
 * @param[in]  aMessage  A pointer to the CoAP message.
 *
 * @returns A pointer to the Token value.
 *
 */
const uint8_t *otCoapMessageGetToken(const otMessage *aMessage);

/**
 * This function initialises an iterator for the options in the given message.
 *
 * @param[inout]  aIterator A pointer to the CoAP message option iterator.
 * @param[in]     aMessage  A pointer to the CoAP message.
 *
 * @retval  OT_ERROR_NONE   Successfully initialised.
 * @retval  OT_ERROR_PARSE  Message state is inconsistent.
 *
 */
otError otCoapOptionIteratorInit(otCoapOptionIterator *aIterator, const otMessage *aMessage);

/**
 * This function returns a pointer to the first option matching the specified option number.
 *
 * @param[in]  aIterator A pointer to the CoAP message option iterator.
 * @param[in]  aOption   The option number sought.
 *
 * @returns A pointer to the first matching option. If no matching option is present NULL pointer is returned.
 *
 */
const otCoapOption *otCoapOptionIteratorGetFirstOptionMatching(otCoapOptionIterator *aIterator, uint16_t aOption);

/**
 * This function returns a pointer to the first option.
 *
 * @param[inout]  aIterator A pointer to the CoAP message option iterator.
 *
 * @returns A pointer to the first option. If no option is present NULL pointer is returned.
 *
 */
const otCoapOption *otCoapOptionIteratorGetFirstOption(otCoapOptionIterator *aIterator);

/**
 * This function returns a pointer to the next option matching the specified option number.
 *
 * @param[in]  aIterator A pointer to the CoAP message option iterator.
 * @param[in]  aOption   The option number sought.
 *
 * @returns A pointer to the next matching option. If no further matching option is present NULL pointer is returned.
 *
 */
const otCoapOption *otCoapOptionIteratorGetNextOptionMatching(otCoapOptionIterator *aIterator, uint16_t aOption);

/**
 * This function returns a pointer to the next option.
 *
 * @param[inout]  aIterator A pointer to the CoAP message option iterator.
 *
 * @returns A pointer to the next option. If no more options are present NULL pointer is returned.
 *
 */
const otCoapOption *otCoapOptionIteratorGetNextOption(otCoapOptionIterator *aIterator);

/**
 * This function fills current option value into @p aValue assuming the current value is an unsigned integer encoded
 * according to https://tools.ietf.org/html/rfc7252#section-3.2
 *
 * @param[inout]    aIterator   A pointer to the CoAP message option iterator.
 * @param[out]      aValue      A pointer to an unsigned integer to receive the option value.
 *
 * @retval  OT_ERROR_NONE       Successfully filled value.
 * @retval  OT_ERROR_NOT_FOUND  No current option.
 * @retval  OT_ERROR_NO_BUFS    Value is too long to fit in a uint64_t.
 *
 * @see otCoapMessageAppendUintOption
 */
otError otCoapOptionIteratorGetOptionUintValue(otCoapOptionIterator *aIterator, uint64_t *aValue);

/**
 * This function fills current option value into @p aValue.
 *
 * @param[inout]  aIterator A pointer to the CoAP message option iterator.
 * @param[out]    aValue    A pointer to a buffer to receive the option value.
 *
 * @retval  OT_ERROR_NONE       Successfully filled value.
 * @retval  OT_ERROR_NOT_FOUND  No current option.
 *
 */
otError otCoapOptionIteratorGetOptionValue(otCoapOptionIterator *aIterator, void *aValue);

/**
 * This function creates a new CoAP message.
 *
 * @note If @p aSettings is 'NULL', the link layer security is enabled and the message priority is set to
 * OT_MESSAGE_PRIORITY_NORMAL by default.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSettings  A pointer to the message settings or NULL to set default settings.
 *
 * @returns A pointer to the message buffer or NULL if no message buffers are available or parameters are invalid.
 *
 */
otMessage *otCoapNewMessage(otInstance *aInstance, const otMessageSettings *aSettings);

/**
 * This function sends a CoAP request with custom transmission parameters.
 *
 * If a response for a request is expected, respective function and context information should be provided.
 * If no response is expected, these arguments should be NULL pointers.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aMessage         A pointer to the message to send.
 * @param[in]  aMessageInfo     A pointer to the message info associated with @p aMessage.
 * @param[in]  aHandler         A function pointer that shall be called on response reception or timeout.
 * @param[in]  aContext         A pointer to arbitrary context information. May be NULL if not used.
 * @param[in]  aTxParameters    A pointer to transmission parameters for this request. Use NULL for defaults.
 *                              Otherwise, parameters given must meet the following conditions:
 *                              1. mMaxRetransmit is no more than OT_COAP_MAX_RETRANSMIT.
 *                              2. mAckRandomFactorNumerator / mAckRandomFactorDenominator must not be below 1.0.
 *                              3. The calculated exchange life time must not overflow uint32_t.
 *
 * @retval OT_ERROR_INVALID_ARGS    @p aTxParameters is invalid.
 * @retval OT_ERROR_NONE            Successfully sent CoAP message.
 * @retval OT_ERROR_NO_BUFS         Failed to allocate retransmission data.
 *
 */
otError otCoapSendRequestWithParameters(otInstance *              aInstance,
                                        otMessage *               aMessage,
                                        const otMessageInfo *     aMessageInfo,
                                        otCoapResponseHandler     aHandler,
                                        void *                    aContext,
                                        const otCoapTxParameters *aTxParameters);

/**
 * This function sends a CoAP request block-wise with custom transmission parameters.
 *
 * This function is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE configuration
 * is enabled.
 *
 * If a response for a request is expected, respective function and context information should be provided.
 * If the response is expected to be block-wise, a respective hook function should be provided.
 * If no response is expected, these arguments should be NULL pointers.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aMessage         A pointer to the message to send.
 * @param[in]  aMessageInfo     A pointer to the message info associated with @p aMessage.
 * @param[in]  aHandler         A function pointer that shall be called on response reception or timeout.
 * @param[in]  aContext         A pointer to arbitrary context information. May be NULL if not used.
 * @param[in]  aTxParameters    A pointer to transmission parameters for this request. Use NULL for defaults.
 * @param[in]  aTransmitHook    A pointer to a hook function for outgoing block-wise transfer.
 * @param[in]  aReceiveHook     A pointer to a hook function for incoming block-wise transfer.
 *
 * @retval OT_ERROR_NONE    Successfully sent CoAP message.
 * @retval OT_ERROR_NO_BUFS Failed to allocate retransmission data.
 *
 */
otError otCoapSendRequestBlockWiseWithParameters(otInstance *                aInstance,
                                                 otMessage *                 aMessage,
                                                 const otMessageInfo *       aMessageInfo,
                                                 otCoapResponseHandler       aHandler,
                                                 void *                      aContext,
                                                 const otCoapTxParameters *  aTxParameters,
                                                 otCoapBlockwiseTransmitHook aTransmitHook,
                                                 otCoapBlockwiseReceiveHook  aReceiveHook);

/**
 * This function sends a CoAP request block-wise.
 *
 * This function is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE configuration
 * is enabled.
 *
 * If a response for a request is expected, respective function and context information should be provided.
 * If the response is expected to be block-wise, a respective hook function should be provided.
 * If no response is expected, these arguments should be NULL pointers.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMessage      A pointer to the message to send.
 * @param[in]  aMessageInfo  A pointer to the message info associated with @p aMessage.
 * @param[in]  aHandler      A function pointer that shall be called on response reception or timeout.
 * @param[in]  aContext      A pointer to arbitrary context information. May be NULL if not used.
 * @param[in]  aTransmitHook A pointer to a hook function for outgoing block-wise transfer.
 * @param[in]  aReceiveHook  A pointer to a hook function for incoming block-wise transfer.
 *
 * @retval OT_ERROR_NONE    Successfully sent CoAP message.
 * @retval OT_ERROR_NO_BUFS Failed to allocate retransmission data.
 *
 */
static inline otError otCoapSendRequestBlockWise(otInstance *                aInstance,
                                                 otMessage *                 aMessage,
                                                 const otMessageInfo *       aMessageInfo,
                                                 otCoapResponseHandler       aHandler,
                                                 void *                      aContext,
                                                 otCoapBlockwiseTransmitHook aTransmitHook,
                                                 otCoapBlockwiseReceiveHook  aReceiveHook)
{
    // NOLINTNEXTLINE(modernize-use-nullptr)
    return otCoapSendRequestBlockWiseWithParameters(aInstance, aMessage, aMessageInfo, aHandler, aContext, NULL,
                                                    aTransmitHook, aReceiveHook);
}

/**
 * This function sends a CoAP request.
 *
 * If a response for a request is expected, respective function and context information should be provided.
 * If no response is expected, these arguments should be NULL pointers.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMessage      A pointer to the message to send.
 * @param[in]  aMessageInfo  A pointer to the message info associated with @p aMessage.
 * @param[in]  aHandler      A function pointer that shall be called on response reception or timeout.
 * @param[in]  aContext      A pointer to arbitrary context information. May be NULL if not used.
 *
 * @retval OT_ERROR_NONE    Successfully sent CoAP message.
 * @retval OT_ERROR_NO_BUFS Failed to allocate retransmission data.
 *
 */
static inline otError otCoapSendRequest(otInstance *          aInstance,
                                        otMessage *           aMessage,
                                        const otMessageInfo * aMessageInfo,
                                        otCoapResponseHandler aHandler,
                                        void *                aContext)
{
    // NOLINTNEXTLINE(modernize-use-nullptr)
    return otCoapSendRequestWithParameters(aInstance, aMessage, aMessageInfo, aHandler, aContext, NULL);
}

/**
 * This function starts the CoAP server.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aPort      The local UDP port to bind to.
 *
 * @retval OT_ERROR_NONE    Successfully started the CoAP server.
 * @retval OT_ERROR_FAILED  Failed to start the CoAP server.
 *
 */
otError otCoapStart(otInstance *aInstance, uint16_t aPort);

/**
 * This function stops the CoAP server.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE  Successfully stopped the CoAP server.
 *
 */
otError otCoapStop(otInstance *aInstance);

/**
 * This function adds a resource to the CoAP server.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aResource  A pointer to the resource.
 *
 */
void otCoapAddResource(otInstance *aInstance, otCoapResource *aResource);

/**
 * This function removes a resource from the CoAP server.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aResource  A pointer to the resource.
 *
 */
void otCoapRemoveResource(otInstance *aInstance, otCoapResource *aResource);

/**
 * This function adds a block-wise resource to the CoAP server.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aResource  A pointer to the resource.
 *
 */
void otCoapAddBlockWiseResource(otInstance *aInstance, otCoapBlockwiseResource *aResource);

/**
 * This function removes a block-wise resource from the CoAP server.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aResource  A pointer to the resource.
 *
 */
void otCoapRemoveBlockWiseResource(otInstance *aInstance, otCoapBlockwiseResource *aResource);

/**
 * This function sets the default handler for unhandled CoAP requests.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aHandler   A function pointer that shall be called when an unhandled request arrives.
 * @param[in]  aContext   A pointer to arbitrary context information. May be NULL if not used.
 *
 */
void otCoapSetDefaultHandler(otInstance *aInstance, otCoapRequestHandler aHandler, void *aContext);

/**
 * This function sends a CoAP response from the server with custom transmission parameters.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aMessage         A pointer to the CoAP response to send.
 * @param[in]  aMessageInfo     A pointer to the message info associated with @p aMessage.
 * @param[in]  aTxParameters    A pointer to transmission parameters for this response. Use NULL for defaults.
 *
 * @retval OT_ERROR_NONE     Successfully enqueued the CoAP response message.
 * @retval OT_ERROR_NO_BUFS  Insufficient buffers available to send the CoAP response.
 *
 */
otError otCoapSendResponseWithParameters(otInstance *              aInstance,
                                         otMessage *               aMessage,
                                         const otMessageInfo *     aMessageInfo,
                                         const otCoapTxParameters *aTxParameters);

/**
 * This function sends a CoAP response block-wise from the server with custom transmission parameters.
 *
 * This function is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE configuration
 * is enabled.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aMessage         A pointer to the CoAP response to send.
 * @param[in]  aMessageInfo     A pointer to the message info associated with @p aMessage.
 * @param[in]  aTxParameters    A pointer to transmission parameters for this response. Use NULL for defaults.
 * @param[in]  aContext         A pointer to arbitrary context information. May be NULL if not used.
 * @param[in]  aTransmitHook    A pointer to a hook function for outgoing block-wise transfer.
 *
 * @retval OT_ERROR_NONE     Successfully enqueued the CoAP response message.
 * @retval OT_ERROR_NO_BUFS  Insufficient buffers available to send the CoAP response.
 *
 */
otError otCoapSendResponseBlockWiseWithParameters(otInstance *                aInstance,
                                                  otMessage *                 aMessage,
                                                  const otMessageInfo *       aMessageInfo,
                                                  const otCoapTxParameters *  aTxParameters,
                                                  void *                      aContext,
                                                  otCoapBlockwiseTransmitHook aTransmitHook);

/**
 * This function sends a CoAP response block-wise from the server.
 *
 * This function is available when OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE configuration
 * is enabled.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMessage      A pointer to the CoAP response to send.
 * @param[in]  aMessageInfo  A pointer to the message info associated with @p aMessage.
 * @param[in]  aContext      A pointer to arbitrary context information. May be NULL if not used.
 * @param[in]  aTransmitHook A pointer to a hook function for outgoing block-wise transfer.
 *
 * @retval OT_ERROR_NONE     Successfully enqueued the CoAP response message.
 * @retval OT_ERROR_NO_BUFS  Insufficient buffers available to send the CoAP response.
 *
 */
static inline otError otCoapSendResponseBlockWise(otInstance *                aInstance,
                                                  otMessage *                 aMessage,
                                                  const otMessageInfo *       aMessageInfo,
                                                  void *                      aContext,
                                                  otCoapBlockwiseTransmitHook aTransmitHook)
{
    // NOLINTNEXTLINE(modernize-use-nullptr)
    return otCoapSendResponseBlockWiseWithParameters(aInstance, aMessage, aMessageInfo, NULL, aContext, aTransmitHook);
}

/**
 * This function sends a CoAP response from the server.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMessage      A pointer to the CoAP response to send.
 * @param[in]  aMessageInfo  A pointer to the message info associated with @p aMessage.
 *
 * @retval OT_ERROR_NONE     Successfully enqueued the CoAP response message.
 * @retval OT_ERROR_NO_BUFS  Insufficient buffers available to send the CoAP response.
 *
 */
static inline otError otCoapSendResponse(otInstance *aInstance, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    // NOLINTNEXTLINE(modernize-use-nullptr)
    return otCoapSendResponseWithParameters(aInstance, aMessage, aMessageInfo, NULL);
}

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* OPENTHREAD_COAP_H_ */
