
#include "session_manager.h"
#include "session.h"

#include <algorithm>
#include <functional>

session_manager::session_manager()
{
}

void session_manager::start(std::shared_ptr<session> c)
{
  connections_.insert(c);
  c->start();
}

void session_manager::stop(std::shared_ptr<session> c)
{
  connections_.erase(c);
  c->stop();
}

void session_manager::stop_all()
{
  std::for_each(connections_.begin(), connections_.end(),
      std::bind(&session::stop, std::placeholders::_1));

  connections_.clear();
}
