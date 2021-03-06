//
// io_context_pool.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
//
#include "io_context_pool.h"

#include <stdexcept>
#include <functional>

io_context_pool::io_context_pool(std::size_t pool_size)
  : next_io_context_(0)
{
  if (pool_size == 0)
    throw std::runtime_error("io_context_pool size is 0");

  // Give all the io_contexts work to do so that their run() functions will not
  // exit until they are explicitly stopped.
  for (std::size_t i = 0; i < pool_size; ++i)
  {
    io_context_ptr io_context(new asio::io_context);
    io_contexts_.push_back(io_context);
    works_.push_back(asio::require(io_context->get_executor(),
          asio::execution::outstanding_work.tracked));
  }
}

void io_context_pool::run()
{
  // Create a pool of threads to run all of the io_contexts.
  std::vector<std::shared_ptr<std::thread> > threads;
  for (std::size_t i = 0; i < io_contexts_.size(); ++i)
  { 
    /*
      <std::bind> different with  <boost::bind> because
      unresolved overloaded function type
    */
      threads.emplace_back(new std::thread(
           std::bind( static_cast<std::size_t (asio::io_context::*)()>(&asio::io_context::run),io_contexts_[i] )));
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->detach();
}

void io_context_pool::stop()
{
  // Explicitly stop all io_contexts.
  for (std::size_t i = 0; i < io_contexts_.size(); ++i)
    io_contexts_[i]->stop();
}

asio::io_context& io_context_pool::get_io_context()
{
  // Use a round-robin scheme to choose the next io_context to use.
  asio::io_context& io_context = *io_contexts_[next_io_context_];
  ++next_io_context_;
  if (next_io_context_ == io_contexts_.size())
      next_io_context_ = 0;
  return io_context;
}
