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

  // Resolve host.
  tcp::resolver::query query{host, std::to_string(port)};
  tcp::resolver resolver{service};
  auto address = resolver.resolve(query, error_code);
  if (error_code) {
    return false;
  }

  // Start timeout.
  asio::steady_timer timer{service};
  timer.expires_from_now(std::chrono::milliseconds(timeout_millis));
  timer.async_wait([&](asio::error_code) {
    error_code = asio::error::timed_out;
    service.stop();
  });

  // Connect.
  tcp::socket socket{service};
  asio::async_connect(socket, address, [&](asio::error_code ec, tcp::resolver::iterator) {
    error_code = ec;
    service.stop();
  });

  asio::error_code service_error_code;
  service.run(service_error_code);
  return !error_code && !service_error_code;
}
