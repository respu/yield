// yield/fs/win32/file_system_test.cpp

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

#include "../file_system_test.hpp"

TEST_SUITE_EX(FileSystem, yield::fs::FileSystemTestSuite);

namespace yield {
namespace fs {
namespace win32 {
TEST_EX(FileSystem, Win32FileSystemUTime, FileSystemTest) {
  DateTime atime = DateTime::now();
  DateTime mtime = DateTime::now();
  DateTime ctime = DateTime::now();

  if (FileSystem().utime(get_test_file_name(), atime, mtime, ctime)) {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
    throw_assert_le(stbuf->get_atime() - atime, Time::NS_IN_S);
    throw_assert_le(stbuf->get_mtime() - mtime, Time::NS_IN_S);
    throw_assert_le(stbuf->get_ctime() - ctime, Time::NS_IN_S);
  } else if (Exception::get_last_error_code() != ENOTSUP)
    throw Exception();
}
}
}
}
