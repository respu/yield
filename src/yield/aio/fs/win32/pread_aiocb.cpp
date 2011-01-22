// yield/aio/fs/win32/pread_aiocb.cpp

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


#include "yield/page.hpp"
#include "yield/aio/fs/pread_aiocb.hpp"
#include "yield/fs/file.hpp"

#include <Windows.h>


namespace yield {
namespace aio {
namespace fs {
bool preadAIOCB::issue( EventHandler& completion_handler ) {
  if ( page.get_next_page() == NULL ) {
    set_completion_handler( completion_handler );

    return ReadFileEx
           (
             get_file(),
             page,
             get_nbytes(),
             *this,
             CompletionRoutine
           ) == TRUE
           ||
           GetLastError() == ERROR_IO_PENDING;
  } else
    return AIOCB::issue( completion_handler );
}

bool preadAIOCB::issue( yield::aio::win32::AIOQueue& ) {
  if ( page.get_next_page() != NULL ) {
    vector<FILE_SEGMENT_ELEMENT> aSegmentArray;
    Page* next_page = &page;
    do {
      FILE_SEGMENT_ELEMENT file_segment_element;
      file_segment_element.Buffer = *next_page;
      aSegmentArray.push_back( file_segment_element );
      next_page = next_page->get_next_page();
    } while ( next_page != NULL );

    return ReadFileScatter
           (
             get_file(),
             &aSegmentArray[0],
             get_nbytes(),
             NULL,
             *this
           ) == TRUE
           ||
           GetLastError() == ERROR_IO_PENDING;
  } else {
    return ReadFile
           (
             get_file(),
             page,
             get_nbytes(),
             NULL,
             *this
           ) == TRUE
           ||
           GetLastError() == ERROR_IO_PENDING;
  }
}
}
}
}
