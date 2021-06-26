//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <poll.h>
#include <asio.hpp>

using asio::ip::tcp;

enum { max_length = 1024*64 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(s, resolver.resolve(argv[1], argv[2]));

	asio::streambuf req_buf_;
	std::ostream req_is_(&req_buf_);

	std::string line_;
	while (std::getline(std::cin, line_,'\n'))  //input from the file in.txt
	{
		req_is_ << line_;

		asio::write(s, req_buf_);

		printf("input:%td\n", req_buf_.size());
	}


    char request[max_length];
    std::cin.getline(request, max_length);
    size_t request_length = std::strlen(request);
	size_t sended_len = 0;
	while (sended_len < request_length)
	{
		size_t curL = s.write_some(asio::buffer(request, request_length));
		sended_len += curL;
	}

    char reply[max_length];
	size_t reply_length = s.read_some(asio::buffer(reply)); printf("cur read:%td\n", reply_length);
    
	std::cout << "Reply is: ";
    std::cout.write(reply, reply_length);
    std::cout << "\n";
	
	poll(NULL, 0, -1);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
