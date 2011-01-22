// yield/net/sockets/socket_test.hpp

// Copyright (c) 2010 Minor Gordon
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


#ifndef _YIELD_NET_SOCKETS_SOCKET_TEST_HPP_
#define _YIELD_NET_SOCKETS_SOCKET_TEST_HPP_


#include "../../channel_test.hpp"
#include "socket_pair_test.hpp"
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/net/sockets/socket_address.hpp"


namespace yield {
namespace net {
namespace sockets {
template <class SocketType>
class SocketBindTest : public yunit::Test {
public:
  // Test
  void run() {
    auto_Object<SocketType> socket_ = SocketType::create();
    if ( !socket_->bind( SocketAddress( SocketAddress::IN_ANY, 31000 ) ) )
      throw Exception();
  }
};


class SocketGetFQDNTest : public yunit::Test {
public:
  // Test
  void run() {
    string hostname = Socket::gethostname();
    string fqdn = Socket::getfqdn();
    throw_assert_false( fqdn.empty() );
    throw_assert_eq( fqdn.find( hostname ), 0 );
    throw_assert_ge( fqdn.size(), hostname.size() );
  }
};


class SocketGetHostNameTest : public yunit::Test {
public:
  // Test
  void run() {
    string hostname = Socket::gethostname();
    throw_assert_false( hostname.empty() );
    throw_assert_ne( hostname, "localhost" );
  }
};


template <class SocketType>
class SocketShutdownTest : public yunit::Test {
public:
  // Test
  void run() {
    {
      auto_Object<SocketType> socket_ = SocketType::create();

      if ( !socket_->shutdown( true, false ) )
        throw Exception();

      if ( !socket_->shutdown( false, true ) )
        throw Exception();
    }

    {
      auto_Object<SocketType> socket_ = SocketType::create();

      if ( !socket_->shutdown( true, true ) )
        throw Exception();
    }
  }
};


template <class SocketType>
class SocketTestSuite : public ChannelTestSuite {
public:
  SocketTestSuite()
    : ChannelTestSuite
    (
      *new SocketPairFactory
      (
        SocketType::DOMAIN_DEFAULT,
        SocketType::TYPE,
        SocketType::PROTOCOL
      )
    ) {
    add( "Socket::bind", new SocketBindTest<SocketType> );
    add( "Socket::getfqdn()", new SocketGetFQDNTest );
    add( "Socket::gethostname", new SocketGetHostNameTest );
    add( "Socket::shutdown", new SocketShutdownTest<SocketType> );
  }
};
}
}
}


#endif
