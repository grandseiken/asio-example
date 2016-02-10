#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <string>

using namespace asio::ip;

bool Connect(const std::string& host, std::uint16_t port, std::uint64_t timeout_millis) {
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
  tcp::socket socket{service};

  // Resolve host...
  resolver.async_resolve(query, [&](asio::error_code ec, const tcp::resolver::iterator& address) {
    if (ec) {
      error_code = ec;
      service.stop();
    } else {
      // ... and then connect.
      asio::async_connect(socket, address, [&](asio::error_code ec, tcp::resolver::iterator) {
        error_code = ec;
        service.stop();
      });
    }
  });

  asio::error_code service_error_code;
  service.run(service_error_code);
  return !error_code && !service_error_code;
}
