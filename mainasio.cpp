//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <thread>
#include <poll.h>

#include <unistd.h>
#include <sys/syscall.h>

#include "asio.hpp"
#include "server.hpp"

using asio::ip::tcp;

#if 0
    acc.async_accept([&acc,&ioAcceptSvr,&ioWorkSvr](std::error_code ec,tcp::socket socket_)
        {
          if (!ec)
          {
            //std::make_shared<session>(std::move(socket_))->start();
            //上句 与 以下两句 作用相同

            // 创建类 sessoin智能指针对象
            std::shared_ptr<session> okpt( new session(std::move(socket_)) );
            okpt->start();// 通过shared_from_this() 创建类 session 智能指针对象

            
          }

          do_accept(acc,ioAcceptSvr,ioWorkSvr);
        });
#endif

class A : public std::enable_shared_from_this<A>
{
public:
  A(){printf("Construct A\n");}
  ~A(){printf("Destruct A \n");}

/*
  Reason --> do_read  share count:2
*/
  void startA()
  {
    auto self(shared_from_this());
      printf("class A cout:%td\n",self.use_count()); //class A cout:2
  }
}; //end class A

int main(int argc, char* argv[])
{

  {
    // std::shared_ptr<A> a(new A()); //创建 A对象智能指针
    // a->startA();                   //调用 shared_from_this() 又创建A对象智能指针
    
    /*
        |||
        |||
    */ 
    std::make_shared<A>()->startA();

    //所以 class A cout:2
  }

  int port = -1;std::size_t polsize = 0;
  std::string ipsd;
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: async_tcp_echo_server 0.0.0.0 8970 1\n";
      port = 8970;
      polsize = 1;
      ipsd = "0.0.0.0";
    }else
    {
      port = atoi(argv[2]);
      polsize = atoi(argv[3]);
      ipsd = argv[1];
    }

    printf("Main threadId:%td\n",syscall(SYS_gettid));
    server svr(ipsd,std::to_string(port),"",polsize);
    svr.run();

    poll(NULL,0,-1);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
