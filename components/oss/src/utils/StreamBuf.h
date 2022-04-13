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

#pragma once
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{

template<class _Elem, class _Traits>
class basic_streambuf_proxy : public std::basic_streambuf <_Elem, _Traits> 
{
public:
    virtual ~basic_streambuf_proxy() {
        std::ios_base::iostate state = _Stream->rdstate();
        _Stream->rdbuf(_Sbuf);
        _Stream->setstate(state);
    }
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;

protected:
    basic_streambuf_proxy(std::basic_iostream<_Elem, _Traits>& stream)
        : _Stream(&stream)
        , _Sbuf(_Stream->rdbuf(this)) {
    }

    int_type overflow(int_type _Meta = _Traits::eof())
    {   // put a character to stream
        return _Sbuf->sputc(_Meta);
    }

    int_type pbackfail(int_type _Meta = _Traits::eof())
    {   // put a character back to stream
        return _Sbuf->sputbackc(_Meta);
    }

    std::streamsize showmanyc()
    {   // return count of input characters
        return _Sbuf->in_avail();;
    }

    int_type underflow()
    {   // get a character from stream, but don't point past it
        return _Sbuf->sgetc();
    }

    int_type uflow()
    {   // get a character from stream, point past it
        return _Sbuf->sbumpc();
    }

    std::streamsize xsgetn(_Elem * _Ptr, std::streamsize _Count)
    {   // get _Count characters from stream
        return _Sbuf->sgetn(_Ptr, _Count);
    }

    std::streamsize xsputn(const _Elem *_Ptr, std::streamsize _Count)
    {   // put _Count characters to stream
        return _Sbuf->sputn(_Ptr, _Count);
    }

    pos_type seekoff(off_type _Off, std::ios_base::seekdir _Way, std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
    {   // change position by offset, according to way and mode
        return _Sbuf->pubseekoff(_Off, _Way, _Mode);
    }

    pos_type seekpos(pos_type _Pos, std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
    {   // change to specified position, according to mode
        return _Sbuf->pubseekpos(_Pos, _Mode);
    }

    std::basic_streambuf<_Elem, _Traits>* setbuf(_Elem *_Buffer, std::streamsize _Count)
    {   // offer buffer to external agent
        return _Sbuf->pubsetbuf(_Buffer, _Count);
    }

    int sync()
    {   // synchronize with external agent
        return _Sbuf->pubsync();
    }

    void imbue(const std::locale& _Newlocale)
    {   // set locale to argument
        _Sbuf->pubimbue(_Newlocale);
    }
private:
    std::basic_iostream<_Elem, _Traits>*  _Stream;
    std::basic_streambuf<_Elem, _Traits>* _Sbuf;
};

using StreamBufProxy = basic_streambuf_proxy<char, std::char_traits<char>>;

}
}
