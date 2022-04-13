/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CryptoStreamBuf.h"
#include <algorithm>
#include <cstring>

using namespace AlibabaCloud::OSS;


CryptoStreamBuf::CryptoStreamBuf(std::iostream& stream, 
    const std::shared_ptr<SymmetricCipher>& cipher,
    const ByteBuffer& key, const ByteBuffer& iv,
    const int skipCnt) :
    StreamBufProxy(stream),
    cipher_(cipher),
    encBufferCnt_(0),
    encBufferOff_(0),
    decBufferCnt_(0),
    decBufferOff_(0),
    key_(key),
    iv_(iv),
    initEncrypt(false),
    initDecrypt(false),
    skipCnt_(skipCnt)
{
    StartPosForIV_ = StreamBufProxy::seekoff(std::streamoff(0), std::ios_base::cur, std::ios_base::in);
}

CryptoStreamBuf::~CryptoStreamBuf()
{
    //flush decBuffer when its size is BLK_SIZE
    if (decBufferCnt_ > 0) {
        unsigned char block[BLK_SIZE];
        auto ret = cipher_->Decrypt(block, static_cast<int>(decBufferCnt_), decBuffer_, static_cast<int>(decBufferCnt_));
        decBufferCnt_ = 0;
        if (ret < 0) {
            return;
        }
        xsputn_with_skip(reinterpret_cast<char *>(block), ret);
    }
}

std::streamsize CryptoStreamBuf::xsgetn(char * _Ptr, std::streamsize _Count)
{
    const std::streamsize startCount = _Count;
    std::streamsize readCnt;
    unsigned char block[BLK_SIZE];

    //update iv base the pos
    if (!initEncrypt) {
        auto currPos = StreamBufProxy::seekoff(std::streamoff(0), std::ios_base::cur, std::ios_base::in);
        currPos -= StartPosForIV_;
        auto blkOff = currPos / BLK_SIZE;
        auto blkIdx = currPos % BLK_SIZE;
        auto iv = SymmetricCipher::IncCTRCounter(iv_, blkOff);
        cipher_->EncryptInit(key_, iv);
        encBufferCnt_ = 0;
        encBufferOff_ = 0;
        if (blkIdx > 0) {
            StreamBufProxy::seekpos(blkOff * BLK_SIZE, std::ios_base::in);
            readCnt = StreamBufProxy::xsgetn(reinterpret_cast<char *>(block), BLK_SIZE);
            auto ret = cipher_->Encrypt(encBuffer_, static_cast<int>(readCnt), block, static_cast<int>(readCnt));
            if (ret < 0) {
                return -1;
            }
            encBufferCnt_ = ret - blkIdx;
            encBufferOff_ = blkIdx;
        }
        initEncrypt = true;
    }

    //read from inner encBuffer_ first
    readCnt = read_from_encrypted_buffer(_Ptr, _Count);
    if (readCnt > 0) {
        _Count -= readCnt;
        _Ptr += readCnt;
    }

    //read from streambuf by BLK_SIZE
    while (_Count > 0) {
        readCnt = StreamBufProxy::xsgetn(reinterpret_cast<char *>(block), BLK_SIZE);
        if (readCnt <= 0)
            break;

        if (_Count < readCnt) {
            auto ret = cipher_->Encrypt(encBuffer_, static_cast<int>(readCnt), block, static_cast<int>(readCnt));
            if (ret < 0) {
                return -1;
            }
            encBufferCnt_ = ret;
            encBufferOff_ = 0;
            break;
        }
        else {
            auto ret = cipher_->Encrypt(reinterpret_cast<unsigned char *>(_Ptr), static_cast<int>(readCnt), block, static_cast<int>(readCnt));
            if (ret < 0) {
                return -1;
            }
            _Count -= ret;
            _Ptr += ret;
        }
    }

    //read from inner encBuffer_ again
    readCnt = read_from_encrypted_buffer(_Ptr, _Count);
    if (readCnt > 0) {
        _Count -= readCnt;
        _Ptr += readCnt;
    }

    return startCount - _Count;
}

std::streamsize CryptoStreamBuf::read_from_encrypted_buffer(char * _Ptr, std::streamsize _Count)
{
    const std::streamsize startCount = _Count;
    if (_Count > 0 && encBufferCnt_ > 0) {
        auto cnt = std::min(_Count, encBufferCnt_);
        memcpy(_Ptr, encBuffer_ + encBufferOff_, static_cast<size_t>(cnt));
        _Ptr += cnt;
        _Count -= cnt;
        encBufferCnt_ -= cnt;
        encBufferOff_ += cnt;
    }
    return startCount - _Count;
}

std::streampos CryptoStreamBuf::seekoff(off_type _Off, std::ios_base::seekdir _Way, std::ios_base::openmode _Mode)
{
    if (_Mode & std::ios_base::in) {
        initEncrypt = false;
    }
    if (_Mode & std::ios_base::out) {
        initDecrypt = false;
    }
    return StreamBufProxy::seekoff(_Off, _Way, _Mode);
}

std::streampos CryptoStreamBuf::seekpos(pos_type _Pos, std::ios_base::openmode _Mode)
{
    if (_Mode & std::ios_base::in) {
        initEncrypt = false;
    }
    if (_Mode & std::ios_base::out) {
        initDecrypt = false;
    }
    return StreamBufProxy::seekpos(_Pos, _Mode);
}

std::streamsize CryptoStreamBuf::xsputn(const char *_Ptr, std::streamsize _Count)
{
    const std::streamsize startCount = _Count;
    unsigned char block[BLK_SIZE * 2];
    std::streamsize writeCnt;
    //update iv
    if (!initDecrypt) {
        cipher_->DecryptInit(key_, iv_);
        decBufferCnt_ = 0;
        decBufferOff_ = 0;
        initDecrypt = true;
    }

    //append to decBuffer first
    if (decBufferCnt_ > 0) {
        writeCnt = std::min(_Count, (BLK_SIZE - decBufferCnt_));
        memcpy(decBuffer_ + decBufferOff_, _Ptr, static_cast<int>(writeCnt));
        decBufferOff_ += writeCnt;
        decBufferCnt_ += writeCnt;
        _Ptr += writeCnt;
        _Count -= writeCnt;
    }

    //flush decBuffer when its size is BLK_SIZE
    if (decBufferCnt_ == BLK_SIZE) {
        auto ret = cipher_->Decrypt(block, static_cast<int>(BLK_SIZE), decBuffer_, static_cast<int>(BLK_SIZE));
        if (ret < 0) {
            return -1;
        }
        decBufferCnt_ = 0;
        decBufferOff_ = 0;
        writeCnt = xsputn_with_skip(reinterpret_cast<char *>(block), BLK_SIZE);
        if (writeCnt != BLK_SIZE) {
            //Todo Save decrypted data
            return startCount - _Count;
        }
    }

    auto blkOff = _Count / BLK_SIZE;
    auto blkIdx = _Count % BLK_SIZE;

    //decrypt by BLK_SIZE
    for (auto i = std::streamsize(0); i < blkOff; i++) {
        auto ret = cipher_->Decrypt(block, static_cast<int>(BLK_SIZE), reinterpret_cast<const unsigned char *>(_Ptr), static_cast<int>(BLK_SIZE));
        if (ret < 0) {
            return -1;
        }
        _Ptr += BLK_SIZE;
        _Count -= BLK_SIZE;
        writeCnt = xsputn_with_skip(reinterpret_cast<char *>(block), BLK_SIZE);
        if (writeCnt != BLK_SIZE) {
            //Todo Save decrypted data
            return startCount - _Count;
        }
    }

    //save to decBuffer and decrypt next time
    if (blkIdx > 0) {
        memcpy(decBuffer_, _Ptr, static_cast<int>(blkIdx));
        _Ptr += blkIdx;
        _Count -= blkIdx;
        decBufferCnt_ = blkIdx;
        decBufferOff_ = blkIdx;
    }

    return startCount - _Count;
}

std::streamsize CryptoStreamBuf::xsputn_with_skip(const char *_Ptr, std::streamsize _Count)
{
    const std::streamsize startCount = _Count;
    if (skipCnt_ > 0) {
        auto min = std::min(skipCnt_, _Count);
        skipCnt_ -= min;
        _Count -= min;
        _Ptr += min;
    }

    if (_Count > 0) {
        _Count -= StreamBufProxy::xsputn(_Ptr, _Count);
    }
    return startCount - _Count;
}