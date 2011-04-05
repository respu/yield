// yield/sockets/aio/accept_aiocb.hpp

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

#ifndef _YIELD_SOCKETS_AIO_ACCEPT_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_ACCEPT_AIOCB_HPP_

#include "yield/buffer.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
class acceptAIOCB : public AIOCB {
public:
  const static uint32_t TYPE_ID = 3895043741UL;

public:
  acceptAIOCB(
    StreamSocket& socket_,
    YO_NEW_REF Buffer* recv_buffer = NULL
  );

  ~acceptAIOCB() {
    StreamSocket::dec_ref(accepted_socket);
    SocketAddress::dec_ref(peername);
    Buffer::dec_ref(recv_buffer);
  }

public:
  StreamSocket* get_accepted_socket() {
    return accepted_socket;
  }

  SocketAddress& get_peername() {
    return peername;
  }

  Buffer* get_recv_buffer() {
    return recv_buffer;
  }

  StreamSocket& get_socket() {
    return static_cast<StreamSocket&>(AIOCB::get_socket());
  }

public:
  void set_accepted_socket(YO_NEW_REF StreamSocket& accepted_socket) {
    StreamSocket::dec_ref(this->accepted_socket);
    this->accepted_socket = &accepted_socket;
  }

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::sockets::aio::acceptAIOCB";
  }

public:
  // yield::aio::AIOCB
  void set_return(ssize_t return_);

#ifdef _WIN32
  void* get_output_buffer();
  uint32_t get_peername_length();
  uint32_t get_recv_data_length();
  uint32_t get_sockname_length();
#endif

private:
  StreamSocket* accepted_socket;
  SocketAddress& peername;
  Buffer* recv_buffer;
};
}
}
}

#endif
