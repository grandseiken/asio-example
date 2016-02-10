#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <asio.hpp>
#include <cstdint>
#include <string>

using namespace asio::ip;

void Connect(const std::string& host, std::uint16_t port) {
  asio::io_service service;

  // Resolve host.
  tcp::resolver::query query{host, std::to_string(port)};
  tcp::resolver resolver{service};
  auto address = resolver.resolve(query);

  // Connect.
  tcp::socket socket{service};
  asio::connect(socket, address);
}
