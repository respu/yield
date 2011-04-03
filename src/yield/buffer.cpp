// yield/buffer.cpp

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

#include "yield/buffer.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#endif

namespace yield {
size_t Buffer::pagesize = 0;

Buffer::Buffer(size_t capacity) {
  alloc(ALIGNMENT_DEFAULT, capacity);
  next_buffer = NULL;
  size_ = 0;
}

Buffer::Buffer(size_t alignment, size_t capacity) {
  alloc(alignment, capacity);
  next_buffer = NULL;
  size_ = 0;
}

Buffer::Buffer(size_t capacity, void* data)
  : capacity_(capacity),
    data_(data)
{
  next_buffer = NULL;
}

Buffer::~Buffer() {
#ifdef _WIN32
  _aligned_free(data_);
#else
  free(data_);
#endif
  Buffer::dec_ref(next_buffer);
}

void Buffer::alloc(size_t alignment, size_t capacity) {
  //capacity_ = (capacity + alignment - 1) / alignment * alignment;
  capacity_ = capacity;
#ifdef _WIN32
  if ((data_ = _aligned_malloc(capacity_, alignment)) == NULL)
#else
  if (posix_memalign(&data_, alignment, capacity_) != 0)
#endif
    throw std::bad_alloc();
}

Buffer&
Buffer::copy(
  size_t alignment,
  size_t capacity,
  const void* data,
  size_t size
) {
  Buffer* buffer = new Buffer(alignment, capacity);
  memcpy_s(*buffer, buffer->capacity(), data, size);
  buffer->resize(size);
  return *buffer;
}

size_t Buffer::getpagesize() {
  if (pagesize != 0)
    return pagesize;
  else {
#ifdef _WIN32
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    pagesize = system_info.dwPageSize;
#else
    pagesize = ::getpagesize();
#endif
    return pagesize;
  }
}

bool Buffer::is_page_aligned(const void* ptr) {
  return (
           reinterpret_cast<const uintptr_t>(ptr)
           &
           (getpagesize() - 1)
         ) == 0;
}

bool Buffer::operator==(const Buffer& other) const {
  if (size() == other.size()) {
    const void* this_data = static_cast<const void*>(*this);
    const void* other_data = static_cast<const void*>(other);
    if (this_data != NULL && other_data != NULL)
      return memcmp(this_data, other_data, size()) == 0;
    else
      return false;
  } else
    return false;
}

void Buffer::put(char data, size_t repeat_count) {
  for (size_t char_i = 0; char_i < repeat_count; char_i++)
    put(&data, 1);
}

void Buffer::put(const void* data, size_t size) {
  if (size_ + size <= capacity_) {
    memcpy_s(
      static_cast<char*>(data_) + size_,
      capacity_ - size_,
      data,
      size
    );

    size_ += size;
  }
}

void Buffer::set_next_buffer(YO_NEW_REF Buffer* next_buffer) {
  if (next_buffer != NULL)
    set_next_buffer(*next_buffer);
  else if (this->next_buffer != NULL) {
    Buffer::dec_ref(*this->next_buffer);
    this->next_buffer = NULL;
  }
}

void Buffer::set_next_buffer(YO_NEW_REF Buffer& next_buffer) {
  Buffer::dec_ref(this->next_buffer);
  this->next_buffer = &next_buffer;
}
}