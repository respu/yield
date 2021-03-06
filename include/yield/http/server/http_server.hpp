// yield/http/server/http_server.hpp

// Copyright (c) 2013 Minor Gordon
// All rights reserved

// This source file is part of the Yield project.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Yield project nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _YIELD_HTTP_SERVER_HTTP_SERVER_HPP_
#define _YIELD_HTTP_SERVER_HTTP_SERVER_HPP_

#include "yield/stage/stage.hpp"
#include "yield/http/server/http_request_queue.hpp"

namespace yield {
namespace http {
namespace server {
/**
  An HTTP server stage.
  HttpServer is an "active" counterpart to HttpRequestQueue: rather than relying
    on a caller to drive it continuously, it continously polls an underlying
    HttpRequestQueue and dispatches HttpRequests and associated Events to
    an EventHandler.
*/
template <class AioQueueType = yield::sockets::aio::AioQueue>
class HttpServer : public yield::stage::Stage {
public:
  /**
    Construct an HttpServer that will listen to the given socket address and
      dispatch HttpRequests and related Events originating from the server
      to the given handler.
    @param http_request_handler handler for HttpRequests and related Events
      originating from the server
    @param sockname address to listen to
    @param log optional debug and error log
  */
  HttpServer(
    YO_NEW_REF EventHandler& http_request_handler,
    const yield::sockets::SocketAddress& sockname,
    YO_NEW_REF Log* log = NULL
  )
    : yield::stage::Stage(
      http_request_handler,
      *new HttpRequestQueue<AioQueueType>(sockname, log)
    ) {
  }
};
}
}
}

#endif
