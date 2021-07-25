#include <cassert> // #define NDEBUG to disable assert()
#include <functional>

#include "server.hpp"

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root, std::size_t io_context_pool_size)
  : //io_accept_pool_(1),
    io_context_pool_(io_context_pool_size),
    signals_(io_context_pool_.get_io_context()),
    acceptor_(io_context_pool_.get_io_context()),
    new_connec_()
    //request_handler_(doc_root)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(std::bind(&server::handle_stop, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::ip::tcp::resolver resolver(acceptor_.get_executor());
  asio::ip::tcp::endpoint endpoint = *(resolver.resolve(address, port).begin());
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}

void server::run()
{
  io_context_pool_.run();
  //io_accept_pool_.run();
}

void server::start_accept()
{
  new_connec_.reset( new session(io_context_pool_.get_io_context()) );
  printf("Session created %p\n",new_connec_.get());
  acceptor_.async_accept(new_connec_->getSocket(),
       std::bind(&server::handle_accept, this,
         std::placeholders::_1));
}

void server::handle_accept(const asio::error_code& e)
{
  // Check whether the server was stopped by a signal before this completion handler had a chance to run.
  if (!acceptor_.is_open()) return;

  if (!e)
  {
    assert(new_connec_.use_count() == 1);
    new_connec_->start();
  }

  start_accept();
}

void server::handle_stop()
{
	acceptor_.close();
  //io_accept_pool_.stop();
  io_context_pool_.stop();
}