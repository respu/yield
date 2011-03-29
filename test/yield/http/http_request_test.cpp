// http_request_test.cpp

// Copyright (c) 2011 Minor Gordon
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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yield/date_time.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPRequest);

namespace yield {
namespace http {
TEST(HTTPRequest, constructor) {
  HTTPRequest(HTTPRequest::METHOD_GET, "/");
  HTTPRequest(HTTPRequest::METHOD_GET, "/", &Buffer::copy("test"));
  HTTPRequest(HTTPRequest::METHOD_GET, "/", &Buffer::copy("test"), 0);
  HTTPRequest(HTTPRequest::METHOD_GET, "/", &Buffer::copy("test"), 0, 1.0f);
}

TEST(HTTPRequest, get_creation_date_time) {
  DateTime now = DateTime::now();
  throw_assert_ge(
    HTTPRequest(HTTPRequest::METHOD_GET, "/").get_creation_date_time(),
    now
  );  
}

TEST(HTTPRequest, get_method) {
  throw_assert_eq(
    HTTPRequest(HTTPRequest::METHOD_GET, "/").get_method(),
    HTTPRequest::METHOD_GET
  );

  throw_assert_eq(
    HTTPRequest(HTTPRequest::METHOD_PUT, "/").get_method(),
    HTTPRequest::METHOD_PUT
  );
}

TEST(HTTPRequest, get_uri) {
  yield::uri::URI uri("/test");
  throw_assert_eq(HTTPRequest(HTTPRequest::METHOD_GET, uri).get_uri(), uri);
}

TEST(HTTPRequest, respond) {
  auto_Object<HTTPRequest> http_request
    = new HTTPRequest(HTTPRequest::METHOD_GET, "/");

  http_request->respond(*new HTTPResponse(200));
  http_request->respond(404);
  http_request->respond(404, "test");
  http_request->respond(404, Buffer::copy("test"));
  http_request->respond(404, &Buffer::copy("test"));
  http_request->respond(*new Exception("exception"));
}
}
}