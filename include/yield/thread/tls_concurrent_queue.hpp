// yield/thread/tls_concurrent_queue.hpp

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


#ifndef _YIELD_THREAD_TLS_CONCURRENT_QUEUE_HPP_
#define _YIELD_THREAD_TLS_CONCURRENT_QUEUE_HPP_


#include "yield/exception.hpp"
#include "yield/thread/blocking_concurrent_queue.hpp"
#include "yield/thread/thread.hpp"

#include <stack>


namespace yield {
namespace thread {
using yield::thread::Thread;


template <class ElementType>
class TLSConcurrentQueue : private BlockingConcurrentQueue<ElementType> {
private:
  class Stack : private std::stack<ElementType*> {
  public:
    ElementType* pop() {
      if (!std::stack<ElementType*>::empty()) {
        ElementType* element = std::stack<ElementType*>::top();
        std::stack<ElementType*>::pop();
        return element;
      } else
        return NULL;
    }

    bool push(ElementType& element) {
      std::stack<ElementType*>::push(&element);
      return true;
    }
  };

public:
  TLSConcurrentQueue() {
    tls_key = Thread::self()->key_create();
    if (tls_key == static_cast<uintptr_t>(-1))
      throw Exception();
  }

  ~TLSConcurrentQueue() {
    Thread::self()->key_delete(tls_key);

    for
    (
      typename vector<Stack*>::iterator stack_i = stacks.begin();
      stack_i != stacks.end();
      stack_i++
    )
      delete *stack_i;
  }

  bool enqueue(ElementType& element) {
    Stack* stack
    = static_cast<Stack*>(Thread::self()->getspecific(tls_key));

    if (stack != NULL) {
      stack->push(element);
      return true;
    } else
      return BlockingConcurrentQueue<ElementType>::enqueue(element);
  }

  ElementType* trydequeue() {
    ElementType* element;

    Stack* stack
    = static_cast<Stack*>(Thread::self()->getspecific(tls_key));

    if (stack != NULL)
      element = stack->pop();
    else {
      stack = new Stack;
      Thread::self()->setspecific(tls_key, stack);
      stacks.push_back(stack);
      element = stack->pop();
    }

    if (element != NULL)
      return element;
    else
      return BlockingConcurrentQueue<ElementType>::trydequeue();
  }

private:
  uintptr_t tls_key;
  vector<Stack*> stacks;
};
}
}


#endif
