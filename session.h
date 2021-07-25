#ifndef HSV_SVESSION
#define HSV_SVESSION

#include <memory>

#include "asio.hpp"

using asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  explicit session(asio::io_context& io_context);
  ~session(void);
  void start();
  void stop();
  asio::ip::tcp::socket & getSocket();

private:
  void doReceive();
  void receiveHander(const std::error_code &ec, std::size_t length);
  void doSend(std::size_t length);
  void sendHander(const std::error_code& ec, std::size_t send_len);

private:
  tcp::socket socket_;
  asio::streambuf recv_buf_;
  asio::streambuf send_buf_;
  
  enum { recv_max_len = 16 };
  char data_[recv_max_len];
  std::string data = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=365, max=9000\r\nContent-Length: 13\r\n\r\nHello, world!";
};
#endif