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
 *   This file includes definitions for generating and processing CoAP messages.
 */

#ifndef COAP_HEADER_HPP_
#define COAP_HEADER_HPP_

#include "openthread-core-config.h"

#include <openthread/coap.h>

#include "common/clearable.hpp"
#include "common/code_utils.hpp"
#include "common/const_cast.hpp"
#include "common/encoding.hpp"
#include "common/message.hpp"
#include "net/ip6.hpp"
#include "net/ip6_address.hpp"
#include "net/udp6.hpp"

namespace ot {

/**
 * @namespace ot::Coap
 * @brief
 *   This namespace includes definitions for CoAP.
 *
 */
namespace Coap {

using ot::Encoding::BigEndian::HostSwap16;

/**
 * @addtogroup core-coap
 *
 * @brief
 *   This module includes definitions for CoAP.
 *
 * @{
 *
 */

class Option;

/**
 * CoAP Type values.
 *
 */
enum Type : uint8_t
{
    kTypeConfirmable    = OT_COAP_TYPE_CONFIRMABLE,     ///< Confirmable type.
    kTypeNonConfirmable = OT_COAP_TYPE_NON_CONFIRMABLE, ///< Non-confirmable type.
    kTypeAck            = OT_COAP_TYPE_ACKNOWLEDGMENT,  ///< Acknowledgment type.
    kTypeReset          = OT_COAP_TYPE_RESET,           ///< Reset type.
};

/**
 * CoAP Code values.
 *
 */
enum Code : uint8_t
{
    // Request Codes:

    kCodeEmpty  = OT_COAP_CODE_EMPTY,  ///< Empty message code
    kCodeGet    = OT_COAP_CODE_GET,    ///< Get
    kCodePost   = OT_COAP_CODE_POST,   ///< Post
    kCodePut    = OT_COAP_CODE_PUT,    ///< Put
    kCodeDelete = OT_COAP_CODE_DELETE, ///< Delete

    // Response Codes:

    kCodeResponseMin = OT_COAP_CODE_RESPONSE_MIN, ///< 2.00
    kCodeCreated     = OT_COAP_CODE_CREATED,      ///< Created
    kCodeDeleted     = OT_COAP_CODE_DELETED,      ///< Deleted
    kCodeValid       = OT_COAP_CODE_VALID,        ///< Valid
    kCodeChanged     = OT_COAP_CODE_CHANGED,      ///< Changed
    kCodeContent     = OT_COAP_CODE_CONTENT,      ///< Content
    kCodeContinue    = OT_COAP_CODE_CONTINUE,     ///< RFC7959 Continue

    // Client Error Codes:

    kCodeBadRequest         = OT_COAP_CODE_BAD_REQUEST,         ///< Bad Request
    kCodeUnauthorized       = OT_COAP_CODE_UNAUTHORIZED,        ///< Unauthorized
    kCodeBadOption          = OT_COAP_CODE_BAD_OPTION,          ///< Bad Option
    kCodeForbidden          = OT_COAP_CODE_FORBIDDEN,           ///< Forbidden
    kCodeNotFound           = OT_COAP_CODE_NOT_FOUND,           ///< Not Found
    kCodeMethodNotAllowed   = OT_COAP_CODE_METHOD_NOT_ALLOWED,  ///< Method Not Allowed
    kCodeNotAcceptable      = OT_COAP_CODE_NOT_ACCEPTABLE,      ///< Not Acceptable
    kCodeRequestIncomplete  = OT_COAP_CODE_REQUEST_INCOMPLETE,  ///< RFC7959 Request Entity Incomplete
    kCodePreconditionFailed = OT_COAP_CODE_PRECONDITION_FAILED, ///< Precondition Failed
    kCodeRequestTooLarge    = OT_COAP_CODE_REQUEST_TOO_LARGE,   ///< Request Entity Too Large
    kCodeUnsupportedFormat  = OT_COAP_CODE_UNSUPPORTED_FORMAT,  ///< Unsupported Content-Format

    // Server Error Codes:

    kCodeInternalError      = OT_COAP_CODE_INTERNAL_ERROR,      ///< Internal Server Error
    kCodeNotImplemented     = OT_COAP_CODE_NOT_IMPLEMENTED,     ///< Not Implemented
    kCodeBadGateway         = OT_COAP_CODE_BAD_GATEWAY,         ///< Bad Gateway
    kCodeServiceUnavailable = OT_COAP_CODE_SERVICE_UNAVAILABLE, ///< Service Unavailable
    kCodeGatewayTimeout     = OT_COAP_CODE_GATEWAY_TIMEOUT,     ///< Gateway Timeout
    kCodeProxyNotSupported  = OT_COAP_CODE_PROXY_NOT_SUPPORTED, ///< Proxying Not Supported
};

/**
 * CoAP Option Numbers.
 *
 */
enum OptionNumber : uint16_t
{
    kOptionIfMatch       = OT_COAP_OPTION_IF_MATCH,       ///< If-Match
    kOptionUriHost       = OT_COAP_OPTION_URI_HOST,       ///< Uri-Host
    kOptionETag          = OT_COAP_OPTION_E_TAG,          ///< ETag
    kOptionIfNoneMatch   = OT_COAP_OPTION_IF_NONE_MATCH,  ///< If-None-Match
    kOptionObserve       = OT_COAP_OPTION_OBSERVE,        ///< Observe [RFC7641]
    kOptionUriPort       = OT_COAP_OPTION_URI_PORT,       ///< Uri-Port
    kOptionLocationPath  = OT_COAP_OPTION_LOCATION_PATH,  ///< Location-Path
    kOptionUriPath       = OT_COAP_OPTION_URI_PATH,       ///< Uri-Path
    kOptionContentFormat = OT_COAP_OPTION_CONTENT_FORMAT, ///< Content-Format
    kOptionMaxAge        = OT_COAP_OPTION_MAX_AGE,        ///< Max-Age
    kOptionUriQuery      = OT_COAP_OPTION_URI_QUERY,      ///< Uri-Query
    kOptionAccept        = OT_COAP_OPTION_ACCEPT,         ///< Accept
    kOptionLocationQuery = OT_COAP_OPTION_LOCATION_QUERY, ///< Location-Query
    kOptionBlock2        = OT_COAP_OPTION_BLOCK2,         ///< Block2 (RFC7959)
    kOptionBlock1        = OT_COAP_OPTION_BLOCK1,         ///< Block1 (RFC7959)
    kOptionSize2         = OT_COAP_OPTION_SIZE2,          ///< Size2 (RFC7959)
    kOptionProxyUri      = OT_COAP_OPTION_PROXY_URI,      ///< Proxy-Uri
    kOptionProxyScheme   = OT_COAP_OPTION_PROXY_SCHEME,   ///< Proxy-Scheme
    kOptionSize1         = OT_COAP_OPTION_SIZE1,          ///< Size1
};

/**
 * This class implements CoAP message generation and parsing.
 *
 */
class Message : public ot::Message
{
    friend class Option;

public:
    static constexpr uint8_t kDefaultTokenLength = OT_COAP_DEFAULT_TOKEN_LENGTH; ///< Default token length.
    static constexpr uint8_t kMaxReceivedUriPath = 32;                           ///< Max URI path length on rx msgs.
    static constexpr uint8_t kMaxTokenLength     = OT_COAP_MAX_TOKEN_LENGTH;     ///< Maximum token length.

    typedef ot::Coap::Type Type; ///< CoAP Type.
    typedef ot::Coap::Code Code; ///< CoAP Code.

    /**
     * CoAP Block1/Block2 Types
     *
     */
    enum BlockType : uint8_t
    {
        kBlockType1 = 1,
        kBlockType2 = 2,
    };

    static constexpr uint8_t kBlockSzxBase = 4;

    /**
     * This method initializes the CoAP header.
     *
     */
    void Init(void);

    /**
     * This method initializes the CoAP header with specific Type and Code.
     *
     * @param[in]  aType  The Type value.
     * @param[in]  aCode  The Code value.
     *
     */
    void Init(Type aType, Code aCode);

    /**
     * This method initializes the CoAP header as `kTypeConfirmable` and `kCodePost`.
     *
     */
    void InitAsConfirmablePost(void);

    /**
     * This method initializes the CoAP header as `kTypeNonConfirmable` and `kCodePost`.
     *
     */
    void InitAsNonConfirmablePost(void);

    /**
     * This method initializes the CoAP header with specific Type and Code.
     *
     * @param[in]  aType              The Type value.
     * @param[in]  aCode              The Code value.
     * @param[in]  aUriPath           A pointer to a null-terminated string.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error Init(Type aType, Code aCode, const char *aUriPath);

    /**
     * This method initializes the CoAP header as `kTypeConfirmable` and `kCodePost` with a given URI Path.
     *
     * @param[in]  aUriPath           A pointer to a null-terminated string.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error InitAsConfirmablePost(const char *aUriPath);

    /**
     * This method initializes the CoAP header as `kTypeNonConfirmable` and `kCodePost` with a given URI Path.
     *
     * @param[in]  aUriPath           A pointer to a null-terminated string.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error InitAsNonConfirmablePost(const char *aUriPath);

    /**
     * This method initializes the CoAP header as `kCodePost` with a given URI Path with its type determined from a
     * given destination IPv6 address.
     *
     * @param[in]  aDestination       The message destination IPv6 address used to determine the CoAP type,
     *                                `kTypeNonConfirmable` if multicast address, `kTypeConfirmable` otherwise.
     * @param[in]  aUriPath           A pointer to a null-terminated string.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error InitAsPost(const Ip6::Address &aDestination, const char *aUriPath);

    /**
     * This method writes header to the message. This must be called before sending the message.
     *
     * This method also checks whether the payload marker is set (`SetPayloadMarker()`) but the message contains no
     * payload, and if so it removes the payload marker from the message.
     *
     */
    void Finish(void);

    /**
     * This method returns the Version value.
     *
     * @returns The Version value.
     *
     */
    uint8_t GetVersion(void) const
    {
        return (GetHelpData().mHeader.mVersionTypeToken & kVersionMask) >> kVersionOffset;
    }

    /**
     * This method sets the Version value.
     *
     * @param[in]  aVersion  The Version value.
     *
     */
    void SetVersion(uint8_t aVersion)
    {
        GetHelpData().mHeader.mVersionTypeToken &= ~kVersionMask;
        GetHelpData().mHeader.mVersionTypeToken |= aVersion << kVersionOffset;
    }

    /**
     * This method returns the Type value.
     *
     * @returns The Type value.
     *
     */
    uint8_t GetType(void) const { return (GetHelpData().mHeader.mVersionTypeToken & kTypeMask) >> kTypeOffset; }

    /**
     * This method sets the Type value.
     *
     * @param[in]  aType  The Type value.
     *
     */
    void SetType(Type aType)
    {
        GetHelpData().mHeader.mVersionTypeToken &= ~kTypeMask;
        GetHelpData().mHeader.mVersionTypeToken |= (static_cast<uint8_t>(aType) << kTypeOffset);
    }

    /**
     * This method returns the Code value.
     *
     * @returns The Code value.
     *
     */
    uint8_t GetCode(void) const { return static_cast<Code>(GetHelpData().mHeader.mCode); }

    /**
     * This method sets the Code value.
     *
     * @param[in]  aCode  The Code value.
     *
     */
    void SetCode(Code aCode) { GetHelpData().mHeader.mCode = static_cast<uint8_t>(aCode); }

#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    /**
     * This method returns the CoAP Code as human readable string.
     *
     * @ returns The CoAP Code as string.
     *
     */
    const char *CodeToString(void) const;
#endif // OPENTHREAD_CONFIG_COAP_API_ENABLE

    /**
     * This method returns the Message ID value.
     *
     * @returns The Message ID value.
     *
     */
    uint16_t GetMessageId(void) const { return HostSwap16(GetHelpData().mHeader.mMessageId); }

    /**
     * This method sets the Message ID value.
     *
     * @param[in]  aMessageId  The Message ID value.
     *
     */
    void SetMessageId(uint16_t aMessageId) { GetHelpData().mHeader.mMessageId = HostSwap16(aMessageId); }

    /**
     * This method returns the Token length.
     *
     * @returns The Token length.
     *
     */
    uint8_t GetTokenLength(void) const
    {
        return (GetHelpData().mHeader.mVersionTypeToken & kTokenLengthMask) >> kTokenLengthOffset;
    }

    /**
     * This method returns a pointer to the Token value.
     *
     * @returns A pointer to the Token value.
     *
     */
    const uint8_t *GetToken(void) const { return GetHelpData().mHeader.mToken; }

    /**
     * This method sets the Token value and length.
     *
     * @param[in]  aToken        A pointer to the Token value.
     * @param[in]  aTokenLength  The Length of @p aToken.
     *
     * @retval kErrorNone    Successfully set the token value.
     * @retval kErrorNoBufs  Insufficient message buffers available to set the token value.
     *
     */
    Error SetToken(const uint8_t *aToken, uint8_t aTokenLength);

    /**
     * This method sets the Token value and length by copying it from another given message.
     *
     * @param[in] aMessage       The message to copy the Token from.
     *
     * @retval kErrorNone    Successfully set the token value.
     * @retval kErrorNoBufs  Insufficient message buffers available to set the token value.
     *
     */
    Error SetTokenFromMessage(const Message &aMessage);

    /**
     * This method sets the Token length and randomizes its value.
     *
     * @param[in]  aTokenLength  The Length of a Token to set.
     *
     * @retval kErrorNone    Successfully set the token value.
     * @retval kErrorNoBufs  Insufficient message buffers available to set the token value.
     *
     */
    Error GenerateRandomToken(uint8_t aTokenLength);

    /**
     * This method checks if Tokens in two CoAP headers are equal.
     *
     * @param[in]  aMessage  A header to compare.
     *
     * @retval TRUE   If two Tokens are equal.
     * @retval FALSE  If Tokens differ in length or value.
     *
     */
    bool IsTokenEqual(const Message &aMessage) const;

    /**
     * This method appends a CoAP option.
     *
     * @param[in] aNumber   The CoAP Option number.
     * @param[in] aLength   The CoAP Option length.
     * @param[in] aValue    A pointer to the CoAP Option value (@p aLength bytes are used as Option value).
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendOption(uint16_t aNumber, uint16_t aLength, const void *aValue);

    /**
     * This method appends an unsigned integer CoAP option as specified in RFC-7252 section-3.2
     *
     * @param[in]  aNumber  The CoAP Option number.
     * @param[in]  aValue   The CoAP Option unsigned integer value.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendUintOption(uint16_t aNumber, uint32_t aValue);

    /**
     * This method appends a string CoAP option.
     *
     * @param[in]  aNumber  The CoAP Option number.
     * @param[in]  aValue   The CoAP Option string value.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendStringOption(uint16_t aNumber, const char *aValue);

    /**
     * This method appends an Observe option.
     *
     * @param[in]  aObserve  Observe field value.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     */
    Error AppendObserveOption(uint32_t aObserve) { return AppendUintOption(kOptionObserve, aObserve & kObserveMask); }

    /**
     * This method appends a Uri-Path option.
     *
     * @param[in]  aUriPath           A pointer to a null-terminated string.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendUriPathOptions(const char *aUriPath);

    /**
     * This method reads the Uri-Path options and constructs the URI path in the buffer referenced by @p `aUriPath`.
     *
     * @param[in] aUriPath  A reference to the buffer for storing URI path.
     *                      NOTE: The buffer size must be `kMaxReceivedUriPath + 1`.
     *
     * @retval  kErrorNone   Successfully read the Uri-Path options.
     * @retval  kErrorParse  CoAP Option header not well-formed.
     *
     */
    Error ReadUriPathOptions(char (&aUriPath)[kMaxReceivedUriPath + 1]) const;

    /**
     * This method appends a Block option
     *
     * @param[in]  aType              Type of block option, 1 or 2.
     * @param[in]  aNum               Current block number.
     * @param[in]  aMore              Boolean to indicate more blocks are to be sent.
     * @param[in]  aSize              Maximum block size.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendBlockOption(BlockType aType, uint32_t aNum, bool aMore, otCoapBlockSzx aSize);

    /**
     * This method appends a Proxy-Uri option.
     *
     * @param[in]  aProxyUri          A pointer to a null-terminated string.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendProxyUriOption(const char *aProxyUri) { return AppendStringOption(kOptionProxyUri, aProxyUri); }

    /**
     * This method appends a Content-Format option.
     *
     * @param[in]  aContentFormat  The Content Format value.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     *
     */
    Error AppendContentFormatOption(otCoapOptionContentFormat aContentFormat)
    {
        return AppendUintOption(kOptionContentFormat, static_cast<uint32_t>(aContentFormat));
    }

    /**
     * This method appends a Max-Age option.
     *
     * @param[in]  aMaxAge  The Max-Age value.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     */
    Error AppendMaxAgeOption(uint32_t aMaxAge) { return AppendUintOption(kOptionMaxAge, aMaxAge); }

    /**
     * This method appends a single Uri-Query option.
     *
     * @param[in]  aUriQuery  A pointer to null-terminated string, which should contain a single key=value pair.
     *
     * @retval kErrorNone         Successfully appended the option.
     * @retval kErrorInvalidArgs  The option type is not equal or greater than the last option type.
     * @retval kErrorNoBufs       The option length exceeds the buffer size.
     */
    Error AppendUriQueryOption(const char *aUriQuery) { return AppendStringOption(kOptionUriQuery, aUriQuery); }

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    /**
     * This function reads the information contained in a Block1 or Block2 option and set it in
     * the HelpData of the message.
     *
     * @param[in]   aBlockType  Block1 or Block2 option value.
     *
     * @retval  kErrorNone          The option has been found and is valid.
     * @retval  kErrorNotFound      The option has not been found.
     * @retval  kErrorInvalidArgs   The option is invalid.
     */
    Error ReadBlockOptionValues(uint16_t aBlockType);

    /**
     * This method returns the current header length of a message.
     *
     * @returns The length of the message header.
     *
     */
    uint16_t GetHeaderLength(void) const { return GetHelpData().mHeaderLength; }

    /**
     * This method returns the block number of a CoAP block-wise transfer message.
     *
     * @returns The block number.
     *
     */
    uint32_t GetBlockWiseBlockNumber(void) const { return GetHelpData().mBlockWiseData.mBlockNumber; }

    /**
     * This method checks if the More Blocks flag is set.
     *
     * @retval TRUE   More Blocks flag is set.
     * @retval FALSE  More Blocks flag is not set.
     *
     */
    bool IsMoreBlocksFlagSet(void) const { return GetHelpData().mBlockWiseData.mMoreBlocks; }

    /**
     * This method returns the block size of a CoAP block-wise transfer message.
     *
     * @returns The block size.
     *
     */
    otCoapBlockSzx GetBlockWiseBlockSize(void) const { return GetHelpData().mBlockWiseData.mBlockSize; }
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

    /**
     * This function reads and reassembles the URI path string and fills it into @p aUriPath.
     *
     * @retval  kErrorNone      URI path string has been reassembled.
     * @retval  kErrorNoBufs    URI path string is too long.
     *
     */
    Error GetUriPath(char *aUriPath) const;

    /**
     * This method adds Payload Marker indicating beginning of the payload to the CoAP header.
     *
     * It also set offset to the start of payload.
     *
     * @retval kErrorNone    Payload Marker successfully added.
     * @retval kErrorNoBufs  Message Payload Marker exceeds the buffer size.
     *
     */
    Error SetPayloadMarker(void);

    /**
     * This method returns the offset of the first CoAP option.
     *
     * @returns The offset of the first CoAP option.
     *
     */
    uint16_t GetOptionStart(void) const { return kMinHeaderLength + GetTokenLength(); }

    /**
     * This method parses CoAP header and moves offset end of CoAP header.
     *
     * @retval  kErrorNone   Successfully parsed CoAP header from the message.
     * @retval  kErrorParse  Failed to parse the CoAP header.
     *
     */
    Error ParseHeader(void);

    /**
     * This method sets a default response header based on request header.
     *
     * @param[in]  aRequest  The request message.
     *
     * @retval kErrorNone    Successfully set the default response header.
     * @retval kErrorNoBufs  Insufficient message buffers available to set the default response header.
     *
     */
    Error SetDefaultResponseHeader(const Message &aRequest);

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

    /**
     * This method sets the block number value in the message HelpData.
     *
     * @param[in]   aBlockNumber    Block number value to set.
     *
     */
    void SetBlockWiseBlockNumber(uint32_t aBlockNumber) { GetHelpData().mBlockWiseData.mBlockNumber = aBlockNumber; }

    /**
     * This method sets the More Blocks falg in the message HelpData.
     *
     * @param[in]   aMoreBlocks    TRUE or FALSE.
     *
     */
    void SetMoreBlocksFlag(bool aMoreBlocks) { GetHelpData().mBlockWiseData.mMoreBlocks = aMoreBlocks; }

    /**
     * This method sets the block size value in the message HelpData.
     *
     * @param[in]   aBlockSize    Block size value to set.
     *
     */
    void SetBlockWiseBlockSize(otCoapBlockSzx aBlockSize) { GetHelpData().mBlockWiseData.mBlockSize = aBlockSize; }
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

    /**
     * This method checks if a header is an empty message header.
     *
     * @retval TRUE   Message is an empty message header.
     * @retval FALSE  Message is not an empty message header.
     *
     */
    bool IsEmpty(void) const { return (GetCode() == kCodeEmpty); }

    /**
     * This method checks if a header is a request header.
     *
     * @retval TRUE   Message is a request header.
     * @retval FALSE  Message is not a request header.
     *
     */
    bool IsRequest(void) const { return (GetCode() >= kCodeGet) && (GetCode() <= kCodeDelete); }

    /**
     * This method indicates whether or not the CoAP code in header is "Get" request.
     *
     * @retval TRUE   Message is a Get request.
     * @retval FALSE  Message is not a Get request.
     *
     */
    bool IsGetRequest(void) const { return GetCode() == kCodeGet; }

    /**
     * This method indicates whether or not the CoAP code in header is "Post" request.
     *
     * @retval TRUE   Message is a Post request.
     * @retval FALSE  Message is not a Post request.
     *
     */
    bool IsPostRequest(void) const { return GetCode() == kCodePost; }

    /**
     * This method indicates whether or not the CoAP code in header is "Put" request.
     *
     * @retval TRUE   Message is a Put request.
     * @retval FALSE  Message is not a Put request.
     *
     */
    bool IsPutRequest(void) const { return GetCode() == kCodePut; }

    /**
     * This method indicates whether or not the CoAP code in header is "Delete" request.
     *
     * @retval TRUE   Message is a Delete request.
     * @retval FALSE  Message is not a Delete request.
     *
     */
    bool IsDeleteRequest(void) const { return GetCode() == kCodeDelete; }

    /**
     * This method checks if a header is a response header.
     *
     * @retval TRUE   Message is a response header.
     * @retval FALSE  Message is not a response header.
     *
     */
    bool IsResponse(void) const { return GetCode() >= OT_COAP_CODE_RESPONSE_MIN; }

    /**
     * This method checks if a header is a CON message header.
     *
     * @retval TRUE   Message is a CON message header.
     * @retval FALSE  Message is not is a CON message header.
     *
     */
    bool IsConfirmable(void) const { return (GetType() == kTypeConfirmable); }

    /**
     * This method checks if a header is a NON message header.
     *
     * @retval TRUE   Message is a NON message header.
     * @retval FALSE  Message is not is a NON message header.
     *
     */
    bool IsNonConfirmable(void) const { return (GetType() == kTypeNonConfirmable); }

    /**
     * This method checks if a header is a ACK message header.
     *
     * @retval TRUE   Message is a ACK message header.
     * @retval FALSE  Message is not is a ACK message header.
     *
     */
    bool IsAck(void) const { return (GetType() == kTypeAck); }

    /**
     * This method checks if a header is a RST message header.
     *
     * @retval TRUE   Message is a RST message header.
     * @retval FALSE  Message is not is a RST message header.
     *
     */
    bool IsReset(void) const { return (GetType() == kTypeReset); }

    /**
     * This method indicates whether or not the header is a confirmable Put request (i.e, `kTypeConfirmable` with
     *  `kCodePost`).
     *
     * @retval TRUE   Message is a confirmable Post request.
     * @retval FALSE  Message is not a confirmable Post request.
     *
     */
    bool IsConfirmablePostRequest(void) const;

    /**
     * This method indicates whether or not the header is a non-confirmable Put request (i.e, `kTypeNonConfirmable` with
     *  `kCodePost`).
     *
     * @retval TRUE   Message is a non-confirmable Post request.
     * @retval FALSE  Message is not a non-confirmable Post request.
     *
     */
    bool IsNonConfirmablePostRequest(void) const;

    /**
     * This method creates a copy of this CoAP message.
     *
     * It allocates the new message from the same message pool as the original one and copies @p aLength octets
     * of the payload. The `Type`, `SubType`, `LinkSecurity`, `Offset`, `InterfaceId`, and `Priority` fields on the
     * cloned message are also copied from the original one.
     *
     * @param[in] aLength  Number of payload bytes to copy.
     *
     * @returns A pointer to the message or nullptr if insufficient message buffers are available.
     *
     */
    Message *Clone(uint16_t aLength) const;

    /**
     * This method creates a copy of the message.
     *
     * It allocates the new message from the same message pool as the original one and copies the entire payload. The
     * `Type`, `SubType`, `LinkSecurity`, `Offset`, `InterfaceId`, and `Priority` fields on the cloned message are also
     * copied from the original one.
     *
     * @returns A pointer to the message or nullptr if insufficient message buffers are available.
     *
     */
    Message *Clone(void) const { return Clone(GetLength()); }

    /**
     * This method returns the minimal reserved bytes required for CoAP message.
     *
     */
    static uint16_t GetHelpDataReserved(void) { return sizeof(HelpData) + kHelpDataAlignment; }

    /**
     * This method returns a pointer to the next message after this as a `Coap::Message`.
     *
     * This method should be used when the message is in a `Coap::MessageQueue` (i.e., a queue containing only CoAP
     * messages).
     *
     * @returns A pointer to the next message in the queue or nullptr if at the end of the queue.
     *
     */
    Message *GetNextCoapMessage(void) { return static_cast<Message *>(GetNext()); }

    /**
     * This method returns a pointer to the next message after this as a `Coap::Message`.
     *
     * This method should be used when the message is in a `Coap::MessageQueue` (i.e., a queue containing only CoAP
     * messages).
     *
     * @returns A pointer to the next message in the queue or nullptr if at the end of the queue.
     *
     */
    const Message *GetNextCoapMessage(void) const { return static_cast<const Message *>(GetNext()); }

private:
    /*
     * Header field first byte (RFC 7252).
     *
     *    7 6 5 4 3 2 1 0
     *   +-+-+-+-+-+-+-+-+
     *   |Ver| T |  TKL  |  (Version, Type and Token Length).
     *   +-+-+-+-+-+-+-+-+
     */
    static constexpr uint8_t kVersionOffset     = 6;
    static constexpr uint8_t kVersionMask       = 0x3 << kVersionOffset;
    static constexpr uint8_t kVersion1          = 1;
    static constexpr uint8_t kTypeOffset        = 4;
    static constexpr uint8_t kTypeMask          = 0x3 << kTypeOffset;
    static constexpr uint8_t kTokenLengthOffset = 0;
    static constexpr uint8_t kTokenLengthMask   = 0xf << kTokenLengthOffset;

    /*
     *
     * Option Format (RFC 7252).
     *
     *      7   6   5   4   3   2   1   0
     *    +---------------+---------------+
     *    |  Option Delta | Option Length |   1 byte
     *    +---------------+---------------+
     *    /         Option Delta          /   0-2 bytes
     *    \          (extended)           \
     *    +-------------------------------+
     *    /         Option Length         /   0-2 bytes
     *    \          (extended)           \
     *    +-------------------------------+
     *    /         Option Value          /   0 or more bytes
     *    +-------------------------------+
     *
     */

    static constexpr uint8_t kOptionDeltaOffset  = 4;
    static constexpr uint8_t kOptionDeltaMask    = 0xf << kOptionDeltaOffset;
    static constexpr uint8_t kOptionLengthOffset = 0;
    static constexpr uint8_t kOptionLengthMask   = 0xf << kOptionLengthOffset;

    static constexpr uint8_t kMaxOptionHeaderSize = 5;

    static constexpr uint8_t kOption1ByteExtension = 13; // Indicates a one-byte extension.
    static constexpr uint8_t kOption2ByteExtension = 14; // Indicates a two-byte extension.

    static constexpr uint8_t kPayloadMarker = 0xff;

    static constexpr uint8_t kHelpDataAlignment = sizeof(uint16_t); // Alignment of help data.

    static constexpr uint16_t kMinHeaderLength = 4;
    static constexpr uint16_t kMaxHeaderLength = 512;

    static constexpr uint16_t kOption1ByteExtensionOffset = 13;  // Delta/Length offset as specified (RFC 7252).
    static constexpr uint16_t kOption2ByteExtensionOffset = 269; // Delta/Length offset as specified (RFC 7252).

    static constexpr uint8_t kBlockSzxOffset = 0;
    static constexpr uint8_t kBlockMOffset   = 3;
    static constexpr uint8_t kBlockNumOffset = 4;

    static constexpr uint32_t kObserveMask = 0xffffff;
    static constexpr uint32_t kBlockNumMax = 0xffff;

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    struct BlockWiseData
    {
        uint32_t       mBlockNumber;
        bool           mMoreBlocks;
        otCoapBlockSzx mBlockSize;
    };
#endif

    /**
     * This structure represents a CoAP header excluding CoAP options.
     *
     */
    OT_TOOL_PACKED_BEGIN
    struct Header
    {
        uint8_t  mVersionTypeToken;       ///< The CoAP Version, Type, and Token Length
        uint8_t  mCode;                   ///< The CoAP Code
        uint16_t mMessageId;              ///< The CoAP Message ID
        uint8_t  mToken[kMaxTokenLength]; ///< The CoAP Token
    } OT_TOOL_PACKED_END;

    /**
     * This structure represents a HelpData used by this CoAP message.
     *
     */
    struct HelpData : public Clearable<HelpData>
    {
        Header   mHeader;
        uint16_t mOptionLast;
        uint16_t mHeaderOffset; ///< The byte offset for the CoAP Header
        uint16_t mHeaderLength;
        bool     mPayloadMarkerSet;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        BlockWiseData mBlockWiseData;
#endif
    };

    static_assert(sizeof(HelpData) <= sizeof(Ip6::Header) + sizeof(Ip6::HopByHopHeader) + sizeof(Ip6::OptionMpl) +
                                          sizeof(Ip6::Udp::Header),
                  "HelpData size exceeds the size of the reserved region in the message");

    const HelpData &GetHelpData(void) const
    {
        static_assert(sizeof(HelpData) + kHelpDataAlignment <= kHeadBufferDataSize,
                      "Insufficient buffer size for CoAP processing!");

        return *static_cast<const HelpData *>(OT_ALIGN(GetFirstData(), kHelpDataAlignment));
    }

    HelpData &GetHelpData(void) { return AsNonConst(AsConst(this)->GetHelpData()); }

    uint8_t *GetToken(void) { return GetHelpData().mHeader.mToken; }

    void SetTokenLength(uint8_t aTokenLength)
    {
        GetHelpData().mHeader.mVersionTypeToken &= ~kTokenLengthMask;
        GetHelpData().mHeader.mVersionTypeToken |= ((aTokenLength << kTokenLengthOffset) & kTokenLengthMask);
    }

    uint8_t WriteExtendedOptionField(uint16_t aValue, uint8_t *&aBuffer);
};

/**
 * This class implements a CoAP message queue.
 *
 */
class MessageQueue : public ot::MessageQueue
{
public:
    /**
     * This constructor initializes the message queue.
     *
     */
    MessageQueue(void) = default;

    /**
     * This method returns a pointer to the first message.
     *
     * @returns A pointer to the first message.
     *
     */
    Message *GetHead(void) const { return static_cast<Message *>(ot::MessageQueue::GetHead()); }

    /**
     * This method adds a message to the end of the queue.
     *
     * @param[in]  aMessage  The message to add.
     *
     */
    void Enqueue(Message &aMessage) { Enqueue(aMessage, kQueuePositionTail); }

    /**
     * This method adds a message at a given position (head/tail) of the queue.
     *
     * @param[in]  aMessage  The message to add.
     * @param[in]  aPosition The position (head or tail) where to add the message.
     *
     */
    void Enqueue(Message &aMessage, QueuePosition aPosition) { ot::MessageQueue::Enqueue(aMessage, aPosition); }

    /**
     * This method removes a message from the queue.
     *
     * @param[in]  aMessage  The message to remove.
     *
     */
    void Dequeue(Message &aMessage) { ot::MessageQueue::Dequeue(aMessage); }

    /**
     * This method removes a message from the queue and frees it.
     *
     * @param[in]  aMessage  The message to remove and free.
     *
     */
    void DequeueAndFree(Message &aMessage) { ot::MessageQueue::DequeueAndFree(aMessage); }
};

/**
 * This class represents a CoAP option.
 *
 */
class Option : public otCoapOption
{
public:
    /**
     * This class represents an iterator for CoAP options.
     *
     */
    class Iterator : public otCoapOptionIterator
    {
    public:
        /**
         * This method initializes the iterator to iterate over CoAP Options in a CoAP message.
         *
         * The iterator MUST be initialized before any other methods are used, otherwise its behavior is undefined.
         *
         * After initialization, the iterator is either updated to point to the first option, or it is marked as done
         * (i.e., `IsDone()` returns `true`) when there is no option or if there is a parse error.
         *
         * @param[in] aMessage  The CoAP message.
         *
         * @retval kErrorNone   Successfully initialized. Iterator is either at the first option or done.
         * @retval kErrorParse  CoAP Option header in @p aMessage is not well-formed.
         *
         */
        Error Init(const Message &aMessage);

        /**
         * This method initializes the iterator to iterate over CoAP Options in a CoAP message matching a given Option
         * Number value.
         *
         * The iterator MUST be initialized before any other methods are used, otherwise its behavior is undefined.
         *
         * After initialization, the iterator is either updated to point to the first option matching the given Option
         * Number value, or it is marked as done (i.e., `IsDone()` returns `true`) when there is no matching option or
         * if there is a parse error.
         *
         * @param[in] aMessage  The CoAP message.
         * @param[in] aNumber   The CoAP Option Number.
         *
         * @retval  kErrorNone   Successfully initialized. Iterator is either at the first matching option or done.
         * @retval  kErrorParse  CoAP Option header in @p aMessage is not well-formed.
         *
         */
        Error Init(const Message &aMessage, uint16_t aNumber) { return InitOrAdvance(&aMessage, aNumber); }

        /**
         * This method indicates whether or not the iterator is done (i.e., has reached the end of CoAP Option Header).
         *
         * @retval TRUE   Iterator is done (reached end of Option header).
         * @retval FALSE  Iterator is not done and currently pointing to a CoAP Option.
         *
         */
        bool IsDone(void) const { return mOption.mLength == kIteratorDoneLength; }

        /**
         * This method indicates whether or not there was a earlier parse error (i.e., whether the iterator is valid).
         *
         * After a parse errors, iterator would also be marked as done.
         *
         * @retval TRUE   There was an earlier parse error and the iterator is not valid.
         * @retval FALSE  There was no earlier parse error and the iterator is valid.
         *
         */
        bool HasParseErrored(void) const { return mNextOptionOffset == kNextOptionOffsetParseError; }

        /**
         * This method advances the iterator to the next CoAP Option in the header.
         *
         * The iterator is updated to point to the next option or marked as done when there are no more options.
         *
         * @retval  kErrorNone   Successfully advanced the iterator.
         * @retval  kErrorParse  CoAP Option header is not well-formed.
         *
         */
        Error Advance(void);

        /**
         * This method advances the iterator to the next CoAP Option in the header matching a given Option Number value.
         *
         * The iterator is updated to point to the next matching option or marked as done when there are no more
         * matching options.
         *
         * @param[in] aNumber   The CoAP Option Number.
         *
         * @retval  kErrorNone   Successfully advanced the iterator.
         * @retval  kErrorParse  CoAP Option header is not well-formed.
         *
         */
        Error Advance(uint16_t aNumber) { return InitOrAdvance(nullptr, aNumber); }

        /**
         * This method gets the CoAP message associated with the iterator.
         *
         * @returns A reference to the CoAP message.
         *
         */
        const Message &GetMessage(void) const { return *static_cast<const Message *>(mMessage); }

        /**
         * This methods gets a pointer to the current CoAP Option to which the iterator is currently pointing.
         *
         * @returns A pointer to the current CoAP Option, or nullptr if iterator is done (or there was an earlier
         *          parse error).
         *
         */
        const Option *GetOption(void) const { return IsDone() ? nullptr : static_cast<const Option *>(&mOption); }

        /**
         * This method reads the current Option Value into a given buffer.
         *
         * @param[out]  aValue   The pointer to a buffer to copy the Option Value. The buffer is assumed to be
         *                       sufficiently large (i.e. at least `GetOption()->GetLength()` bytes).
         *
         * @retval kErrorNone       Successfully read and copied the Option Value into given buffer.
         * @retval kErrorNotFound   Iterator is done (not pointing to any option).
         *
         */
        Error ReadOptionValue(void *aValue) const;

        /**
         * This method read the current Option Value which is assumed to be an unsigned integer.
         *
         * @param[out]  aUintValue      A reference to `uint64_t` to output the read Option Value.
         *
         * @retval kErrorNone       Successfully read the Option value.
         * @retval kErrorNoBufs     Value is too long to fit in an `uint64_t`.
         * @retval kErrorNotFound   Iterator is done (not pointing to any option).
         *
         */
        Error ReadOptionValue(uint64_t &aUintValue) const;

        /**
         * This method gets the offset of beginning of the CoAP message payload (after the CoAP header).
         *
         * This method MUST be used after the iterator is done (i.e. iterated through all options).
         *
         * @returns The offset of beginning of the CoAP message payload
         *
         */
        uint16_t GetPayloadMessageOffset(void) const { return mNextOptionOffset; }

    private:
        // `mOption.mLength` value to indicate iterator is done.
        static constexpr uint16_t kIteratorDoneLength = 0xffff;

        // Special `mNextOptionOffset` value to indicate a parse error.
        static constexpr uint16_t kNextOptionOffsetParseError = 0;

        void MarkAsDone(void) { mOption.mLength = kIteratorDoneLength; }
        void MarkAsParseErrored(void) { MarkAsDone(), mNextOptionOffset = kNextOptionOffsetParseError; }

        Error Read(uint16_t aLength, void *aBuffer);
        Error ReadExtendedOptionField(uint16_t &aValue);
        Error InitOrAdvance(const Message *aMessage, uint16_t aNumber);
    };

    /**
     * This method gets the CoAP Option Number.
     *
     * @returns The CoAP Option Number.
     *
     */
    uint16_t GetNumber(void) const { return mNumber; }

    /**
     * This method gets the CoAP Option Length (length of Option Value in bytes).
     *
     * @returns The CoAP Option Length (in bytes).
     *
     */
    uint16_t GetLength(void) const { return mLength; }
};

/**
 * @}
 *
 */

} // namespace Coap
} // namespace ot

#endif // COAP_HEADER_HPP_
