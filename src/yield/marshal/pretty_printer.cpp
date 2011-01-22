// yield/marshal/pretty_printer.cpp

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


#include "yield/marshal/pretty_printer.hpp"


namespace yield {
namespace marshal {
PrettyPrinter::PrettyPrinter(std::ostream& os)
  : os(os)
{ }

void PrettyPrinter::write(const Object& key, bool value) {
  if (value)
    os << "true, ";
  else
    os << "false, ";
}

void PrettyPrinter::write(const Object& key, double value) {
  os << value << ", ";
}

void PrettyPrinter::write(const Object& key, int64_t value) {
  os << value << ", ";
}

void PrettyPrinter::write(const Object& key, const Map& value) {
  os << value.get_type_name() << "{ ";
  value.marshal(*this);
  os << " }, ";
}

void PrettyPrinter::write(const Object& key, const Null& value) {
  os << "null, ";
}

void PrettyPrinter::write(const Object& key, const Object& value) {
  os << value.get_type_name() << "( ";
  value.marshal(*this);
  os << " ), ";
}

void PrettyPrinter::write(const Object& key, const Sequence& value) {
  os << value.get_type_name() << "[ ";
  value.marshal(*this);
  os << " ], ";
}

void
PrettyPrinter::write
(
  const Object& key,
  const char* value,
  size_t value_len
) {
  os.write(value, value_len);
  os << ", ";
}

void PrettyPrinter::write(const Object& key, uint64_t value) {
  os << value << ", ";
}
}
}
