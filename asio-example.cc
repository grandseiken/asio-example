#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <asio.hpp>
#include <cstdint>
#include <string>

using namespace asio::ip;

bool Connect(const std::string& host, std::uint16_t port) {
  asio::io_service service;
  asio::error_code error_code;

  // Resolve host.
  tcp::resolver::query query{host, std::to_string(port)};
  tcp::resolver resolver{service};
  auto address = resolver.resolve(query, error_code);
  if (error_code) {
    return false;
  }

  // Connect.
  tcp::socket socket{service};
  asio::connect(socket, address, error_code);
  return !error_code;
}
