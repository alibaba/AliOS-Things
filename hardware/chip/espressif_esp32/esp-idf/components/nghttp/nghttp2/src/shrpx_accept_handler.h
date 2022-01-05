/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2014 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef SHRPX_ACCEPT_HANDLER_H
#define SHRPX_ACCEPT_HANDLER_H

#include "shrpx.h"

#include <ev.h>

namespace shrpx {

class ConnectionHandler;
struct UpstreamAddr;

class AcceptHandler {
public:
  AcceptHandler(const UpstreamAddr *faddr, ConnectionHandler *h);
  ~AcceptHandler();
  void accept_connection();
  void enable();
  void disable();
  int get_fd() const;

private:
  ev_io wev_;
  ConnectionHandler *conn_hnr_;
  const UpstreamAddr *faddr_;
};

} // namespace shrpx

#endif // SHRPX_ACCEPT_HANDLER_H
