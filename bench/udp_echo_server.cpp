#include <bench/bench.hpp>
#include <sal/net/internet.hpp>
#include <sal/net/io_context.hpp>
#include <sal/net/io_service.hpp>
#include <thread>
#include <iostream>


using namespace std::chrono_literals;


namespace {

using protocol_t = sal::net::ip::udp_t;
using socket_t = protocol_t::socket_t;

// configuration
socket_t::endpoint_t server_endpoint(
  sal::net::ip::make_address_v4("127.0.0.1"), 8192
);
size_t receives = 64, threads = 1, buf_mul = 1;


void print_stats (size_t active_threads, size_t packets, size_t size_bytes)
{
  std::ostringstream oss;
  oss
    << "threads: " << active_threads
    << "; packets: " << packets
  ;

  const char units[] = " kMG";
  const char *unit = &units[0];

  size_t bps = 8 * size_bytes;
  while (size_bytes >= 1024)
  {
    size_bytes /= 1024;
    bps /= 1000;
    unit++;
  }

  oss << "; " << *unit << "bps=" << bps
    << "; " << *unit << "Bps=" << size_bytes
    << '\n';
  ;

  static std::string output;
  if (output != oss.str())
  {
    output = oss.str();
    std::cout << output;
  }
}


} // namespace


namespace bench {


option_set_t options ()
{
  using namespace sal::program_options;

  option_set_t desc;
  desc
    .add({"a", "address"},
      requires_argument("ADDRESS", "0.0.0.0"),
      help("UDP echo server IPv4 address")
    )
    .add({"b", "buffer"},
      requires_argument("INT", buf_mul),
      help("multiply send/receive buffer size (0 to disable buffering)")
    )
    .add({"p", "port"},
      requires_argument("INT", server_endpoint.port()),
      help("listening port")
    )
    .add({"r", "receives"},
      requires_argument("INT", receives),
      help("number of asynchronous receives per thread")
    )
    .add({"t", "threads"},
      requires_argument("INT", threads),
      help("number of threads")
    )
  ;
  return desc;
}


int run (const option_set_t &options, const argument_map_t &arguments)
{
  server_endpoint.address(
    sal::net::ip::make_address_v4(
      options.back_or_default("address", { arguments })
    )
  );
  server_endpoint.port(
    static_cast<sal::net::ip::port_t>(
      std::stoul(options.back_or_default("port", { arguments }))
    )
  );

  receives = std::stoul(options.back_or_default("receives", { arguments }));
  threads = std::stoul(options.back_or_default("threads", { arguments }));
  buf_mul = std::stoul(options.back_or_default("buffer", { arguments }));

  sal::net::io_service_t io_svc;

  // receive socket
  socket_t recv_sock(server_endpoint);
  if (buf_mul != 1)
  {
    int size = 0;
    recv_sock.get_option(sal::net::receive_buffer_size(&size));
    std::cout << "receive buffer " << size;
    recv_sock.set_option(sal::net::receive_buffer_size(int(buf_mul) * size));
    recv_sock.get_option(sal::net::receive_buffer_size(&size));
    std::cout << " -> " << size << "bytes\n";
  }
  io_svc.associate(recv_sock);

  // send socket
  server_endpoint.port(server_endpoint.port() + 1);
  socket_t send_sock(server_endpoint);
  if (buf_mul != 1)
  {
    int size = 0;
    send_sock.get_option(sal::net::send_buffer_size(&size));
    std::cout << "send buffer " << size;
    send_sock.set_option(sal::net::send_buffer_size(int(buf_mul) * size));
    send_sock.get_option(sal::net::send_buffer_size(&size));
    std::cout << " -> " << size << "bytes\n";
  }
  io_svc.associate(send_sock);

  std::vector<std::thread> thread;
  std::vector<std::pair<size_t, size_t>> thread_transferred;
  while (thread.size() != threads)
  {
    size_t index = thread.size();
    thread_transferred.emplace_back();

    thread.emplace_back([index, &io_svc, &recv_sock, &send_sock, &thread_transferred]
    {
      auto io_ctx = io_svc.make_context(receives);
      std::error_code error;

      // start initial reads
      for (auto i = receives;  i;  --i)
      {
        recv_sock.async_receive_from(io_ctx.make_buf());
      }

      // infinite handling
      auto &transferred = thread_transferred[index];
      while (auto io_buf = io_ctx.get())
      {
        if (auto recv = socket_t::async_receive_from_result(io_buf, error))
        {
          transferred.first++;
          transferred.second += recv->transferred();
          io_buf->resize(recv->transferred());
          send_sock.async_send_to(std::move(io_buf), recv->endpoint());
        }
        else
        {
          io_buf->reset();
          recv_sock.async_receive_from(std::move(io_buf));
        }
      }
    });
  }

  while (true)
  {
    std::this_thread::sleep_for(1s);

    size_t packets = 0, size = 0, active_threads = 0;
    for (auto &transferred: thread_transferred)
    {
      packets += transferred.first;
      size += transferred.second;
      if (transferred.first)
      {
        active_threads++;
      }
      transferred.first = transferred.second = 0;
    }
    print_stats(active_threads, packets, size);
  }

  return EXIT_SUCCESS;
}


} // namespace bench
