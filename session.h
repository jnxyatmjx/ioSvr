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
  tcp::socket & getSocket();

private:
  void do_read();
  void read_hander(const std::error_code &ec, std::size_t length);
  void do_write(std::size_t length);

private:
  tcp::socket socket_;
  enum { recv_max_len = 1024 };
  char data_[recv_max_len];
  std::string data = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=365, max=9000\r\nContent-Length: 13\r\n\r\nHello, world!";
};
#endif