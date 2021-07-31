
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <set>
#include <memory>

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class session;
class session_manager
{
public:
	session_manager(const session_manager&) = delete;
	session_manager& operator=(const session_manager&) = delete;

	session_manager();
  /// Add the specified connection to the manager and start it.
  void start(std::shared_ptr<session> c);

  /// Stop the specified connection.
  void stop(std::shared_ptr<session> c);

  /// Stop all connections.
  void stop_all();

private:
  /// The managed connections.
  std::set<std::shared_ptr<session>> connections_;
};

#endif // SESSION_MANAGER_H
