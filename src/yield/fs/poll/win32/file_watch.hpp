// yield/fs/poll/win32/file_watch.hpp

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

#ifndef _YIELD_FS_POLL_WIN32_FILE_WATCH_HPP_
#define _YIELD_FS_POLL_WIN32_FILE_WATCH_HPP_

#include "watch.hpp"

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
class FileWatch : public Watch {
public:
  FileWatch(
    YO_NEW_REF Directory& directory,
    const Path& directory_path,
    const Path& file_path,
    FSEvent::Type fs_event_types,
    Log* log = NULL
  ) : Watch(directory, fs_event_types, log, file_path),
      directory_path(directory_path) {
  }

public:
  // yield::fs::poll::win32::Watch
  void read(const FILE_NOTIFY_INFORMATION&, EventHandler& fs_event_handler);

private:
  Path directory_path;
  Path old_name;
};
}
}
}
}

#endif