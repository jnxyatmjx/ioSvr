
#include <unistd.h>
#include <sys/syscall.h>
#include <memory>

#include "session.h"

  session::session(asio::io_context& io_context)
    : socket_(io_context), recv_buf_(16*1024),send_buf_(4*1024)
  {
  }
  
  session::~session(void)
  {
    printf("Session deleted %p..\n",this);
	recv_buf_.consume(recv_buf_.size());
	send_buf_.consume(send_buf_.size());
  }

  void session::start()
  {
    doReceive();
  }

  asio::ip::tcp::socket & session::getSocket()
  {
      return socket_;
  }

  void session::doReceive()
  {
    // auto self(shared_from_this());
    // socket_.async_read_some(asio::buffer(data_, recv_max_len),
    //     [this,self](const std::error_code &ec, std::size_t length)
    //     {
    //       if (!ec)
    //       {
    //         doSend(length);
    //       }else
    //         printf("read error (%d) %s\n",ec.value(),ec.message().c_str());
    //     });

    //socket_.async_read_some(asio::buffer(data_, recv_max_len),
    //  std::bind(&session::receiveHander,shared_from_this(),std::placeholders::_1,std::placeholders::_2));
                                    //'this' is error

	  asio::async_read(socket_,recv_buf_,asio::transfer_at_least(1),
		  std::bind(&session::receiveHander, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }

  void session::receiveHander(const std::error_code &ec, std::size_t recv_len)
  { 
      if(!ec)
      {
        printf("recevLen:%td-recv_buf_:%td tid:%td\n", recv_len,recv_buf_.size(),syscall(SYS_gettid));
		while (recv_buf_.size() > 0)
		{
			char buf[1024] = {'\0'};
			int buf_nu = recv_buf_.sgetn(buf, 1024);
			
			asio::buffer_copy(send_buf_.prepare(buf_nu), asio::buffer(buf,buf_nu));
			send_buf_.commit(buf_nu); printf("send_bufLen:%td-recv_buf_:%td\n",send_buf_.size(),recv_buf_.size());
			/*std::ostream ot_(&send_buf_);
			ot_ << "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=365, max=9000\r\n\r\n" << sv_;*/
			
		}
		if(send_buf_.size() > 0)
			doSend(send_buf_.size());

		doReceive();

      }else
      {
        printf("receiveHander async_read error (%d) %s\n",ec.value(),ec.message().c_str());
        asio::error_code ignored_ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
      }
        
  }

  void session::doSend(std::size_t send_len)
  {
    //auto self(shared_from_this());
    //asio::async_write(socket_, asio::buffer(data/*, length*/),
    //    [self,this](std::error_code ec, std::size_t send_len)
    //    {
    //      if (!ec)
    //      {
    //        printf("write %td tid:%td\n", send_len,syscall(SYS_gettid));
    //        //doReceive();
    //      }
    //    });
	printf("doSendLen:%td send_buf_:%td tid:%td\n", send_len, send_buf_.size(),syscall(SYS_gettid));
	asio::async_write(socket_, send_buf_,
			std::bind(&session::sendHander, shared_from_this(),
				std::placeholders::_1,std::placeholders::_2));
	}

  void session::sendHander(const std::error_code& ec, std::size_t send_len)
  {
	  if (!ec)
	  {
		  printf("sendHander sendLen:%td tid:%td\n", send_len,syscall(SYS_gettid));
	  }
	  else
	  {
		  printf("sendHander async_write error (%d) %s\n", ec.value(), ec.message().c_str());
		  asio::error_code ignored_ec;
		  socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
	  }

  }