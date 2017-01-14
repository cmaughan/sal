#pragma once

#include <sal/config.hpp>
#include <system_error>

#if __sal_os_linux || __sal_os_darwin
  #include <sys/socket.h>
#elif __sal_os_windows
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32")
#else
  #error Unsupported platform
#endif


__sal_begin


namespace net { namespace __bits {


#if __sal_os_windows

  // socket handle
  using native_handle_t = SOCKET;
  constexpr native_handle_t invalid_socket = INVALID_SOCKET;

  // shutdown() direction
  #define SHUT_RD SD_RECEIVE
  #define SHUT_WR SD_SEND
  #define SHUT_RDWR SD_BOTH

  // sockaddr family
  using sa_family_t = ::ADDRESS_FAMILY;

#else

  // socket handle
  using native_handle_t = int;
  constexpr native_handle_t invalid_socket = -1;

  // sockaddr family
  using sa_family_t = ::sa_family_t;

#endif


enum class wait_t { read, write };


struct socket_t
{
  native_handle_t native_handle = invalid_socket;

  socket_t () = default;

  socket_t (native_handle_t native_handle) noexcept
    : native_handle(native_handle)
  {}

  void open (int domain,
    int type,
    int protocol,
    std::error_code &error
  ) noexcept;

  void close (std::error_code &error) noexcept;

  void bind (const void *address, size_t address_size, std::error_code &error)
    noexcept;

  void listen (int backlog, std::error_code &error) noexcept;

  native_handle_t accept (void *address, size_t *address_size,
    bool enable_connection_aborted,
    std::error_code &error
  ) noexcept;

  void connect (const void *address, size_t address_size,
    std::error_code &error
  ) noexcept;

  bool wait (wait_t what,
    int timeout_ms,
    std::error_code &error
  ) noexcept;

  size_t recv (void *data, size_t data_size,
    int flags,
    std::error_code &error
  ) noexcept;

  size_t recv_from (void *data, size_t data_size,
    int flags,
    void *address, size_t *address_size,
    std::error_code &error
  ) noexcept;

  size_t send (const void *data, size_t data_size,
    int flags,
    std::error_code &error
  ) noexcept;

  size_t send_to (const void *data, size_t data_size,
    int flags,
    const void *address, size_t address_size,
    std::error_code &error
  ) noexcept;

  void shutdown (int what, std::error_code &error) noexcept;

  void remote_endpoint (void *address, size_t *address_size,
    std::error_code &error
  ) const noexcept;

  void local_endpoint (void *address, size_t *address_size,
    std::error_code &error
  ) const noexcept;

  void get_opt (int level, int name,
    void *data, size_t *size,
    std::error_code &error
  ) const noexcept;

  void set_opt (int level, int name,
    const void *data, size_t size,
    std::error_code &error
  ) noexcept;

  bool non_blocking (std::error_code &error) const noexcept;
  void non_blocking (bool mode, std::error_code &error) noexcept;

  size_t available (std::error_code &error) const noexcept;
};


}} // namespace net::__bits


__sal_end
