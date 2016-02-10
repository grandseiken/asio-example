#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <string>

using namespace asio::ip;

bool Connect(const std::string& host, std::uint16_t port,
             std::uint8_t multiplex_level, std::uint64_t timeout_millis) {
  asio::io_service service;
  asio::error_code error_code;

  // Start timeout.
  asio::steady_timer timer{service};
  timer.expires_from_now(std::chrono::milliseconds(timeout_millis));
  timer.async_wait([&](asio::error_code) {
    error_code = asio::error::timed_out;
    service.stop();
  });

  tcp::resolver::query query{host, std::to_string(port)};
  tcp::resolver resolver{service};
  std::uint8_t connected_sockets = 0;
  std::vector<tcp::socket> sockets;

  // Resolve host...
  resolver.async_resolve(query, [&](asio::error_code ec, const tcp::resolver::iterator& address) {
    if (ec) {
      error_code = ec;
      service.stop();
    } else {
      // ... and then connect.
      for (std::uint8_t i = 0; i < multiplex_level; ++i) {
        sockets.emplace_back(service);
        asio::async_connect(sockets.back(), address, [&](asio::error_code ec,
        tcp::resolver::iterator) {
          if (ec) {
            error_code = ec;
            service.stop();
          } else if (++connected_sockets == multiplex_level) {
            // All sockets connected.
            service.stop();
          }
        });
      }
    }
  });

  asio::error_code service_error_code;
  service.run(service_error_code);
  return !error_code && !service_error_code;
}
