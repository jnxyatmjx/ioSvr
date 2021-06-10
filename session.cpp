

#include "session.h"

  session::session(asio::io_context& io_context)
    : socket_(io_context)
  {
  }
  
  session::~session(void)
  {
    printf("Session deleted %p..\n",this);
  }

  void session::start()
  {
    do_read();
  }

  tcp::socket & session::getSocket()
  {
      return socket_;
  }

  void session::do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, recv_max_len),
        [this,self](const std::error_code &ec, std::size_t length)
        {
          if (!ec)
          {
            do_write(length);
          }else
            printf("read error (%d) %s\n",ec.value(),ec.message().c_str());
        });
   //socket_.async_read_some(asio::buffer(data_, recv_max_len),
    //std::bind(&session::read_hander,shared_from_this(),std::placeholders::_1,std::placeholders::_2));
                                    /*this is error*/
  }

  void session::read_hander(const std::error_code &ec, std::size_t length)
  { 
      if(!ec)
      {
        printf("Le:%td  <> %s\n",length,data_);
        do_write(length); 
      }else
      {
        printf("read error (%d) %s \n",ec.value(),ec.message().c_str());
      }
        
  }

  void session::do_write(std::size_t length)
  {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data/*, length*/),
        [self,this](std::error_code ec, std::size_t tj_len)
        {
          if (!ec)
          {
            //printf("write %td\n",tj_len);
            do_read();
          }
        });
  }
