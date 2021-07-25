//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_SERVER_HPP
#define HTTP_SERVER2_SERVER_HPP

#include <asio.hpp>
#include <string>
#include <vector>
#include <memory>

#include "session.h"
#include "io_context_pool.h"

class server
{
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(const std::string& address, const std::string& port,
      const std::string& doc_root, std::size_t io_context_pool_size);

  /// Run the server's io_context loop.
  void run();

private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const asio::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// Only for Accept I/O context
  //io_context_pool io_accept_pool_; //this io_context deprecate NOT USE

  /// The pool of io_context objects used to perform asynchronous operations.
  io_context_pool io_context_pool_;

  /// The signal_set is used to register for process termination notifications.
  asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  typedef std::shared_ptr<session> connection_ptr;
  connection_ptr new_connec_;

  /// The handler for all incoming requests.
  //request_handler request_handler_;
};

#endif // HTTP_SERVER2_SERVER_HPP
