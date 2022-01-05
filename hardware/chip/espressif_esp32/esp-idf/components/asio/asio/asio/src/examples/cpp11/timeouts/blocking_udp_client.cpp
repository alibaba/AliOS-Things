//
// blocking_udp_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "asio/buffer.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>

using asio::ip::udp;
using std::placeholders::_1;
using std::placeholders::_2;

//----------------------------------------------------------------------

//
// This class manages socket timeouts by running the io_context using the timed
// io_context::run_for() member function. Each asynchronous operation is given
// a timeout within which it must complete. The socket operations themselves
// use std::bind to specify the completion handler:
//
//   +---------------+
//   |               |
//   |    receive    |
//   |               |
//   +---------------+
//           |
//  async_-  |    +----------------+
// receive() |    |                |
//           +--->| handle_receive |
//                |                |
//                +----------------+
//
// For a given socket operation, the client object runs the io_context to block
// thread execution until the operation completes or the timeout is reached. If
// the io_context::run_for() function times out, the socket is closed and the
// outstanding asynchronous operation is cancelled.
//
class client
{
public:
  client(const udp::endpoint& listen_endpoint)
    : socket_(io_context_, listen_endpoint)
  {
  }

  std::size_t receive(const asio::mutable_buffer& buffer,
      std::chrono::steady_clock::duration timeout,
      std::error_code& error)
  {
    // Start the asynchronous operation. The handle_receive function used as a
    // callback will update the error and length variables.
    std::size_t length = 0;
    socket_.async_receive(asio::buffer(buffer),
        std::bind(&client::handle_receive, _1, _2, &error, &length));

    // Run the operation until it completes, or until the timeout.
    run(timeout);

    return length;
  }

private:
  void run(std::chrono::steady_clock::duration timeout)
  {
    // Restart the io_context, as it may have been left in the "stopped" state
    // by a previous operation.
    io_context_.restart();

    // Block until the asynchronous operation has completed, or timed out. If
    // the pending asynchronous operation is a composed operation, the deadline
    // applies to the entire operation, rather than individual operations on
    // the socket.
    io_context_.run_for(timeout);

    // If the asynchronous operation completed successfully then the io_context
    // would have been stopped due to running out of work. If it was not
    // stopped, then the io_context::run_for call must have timed out.
    if (!io_context_.stopped())
    {
      // Cancel the outstanding asynchronous operation.
      socket_.cancel();

      // Run the io_context again until the operation completes.
      io_context_.run();
    }
  }

  static void handle_receive(
      const std::error_code& error, std::size_t length,
      std::error_code* out_error, std::size_t* out_length)
  {
    *out_error = error;
    *out_length = length;
  }

private:
  asio::io_context io_context_;
  udp::socket socket_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    using namespace std; // For atoi.

    if (argc != 3)
    {
      std::cerr << "Usage: blocking_udp_client <listen_addr> <listen_port>\n";
      return 1;
    }

    udp::endpoint listen_endpoint(
        asio::ip::make_address(argv[1]),
        std::atoi(argv[2]));

    client c(listen_endpoint);

    for (;;)
    {
      char data[1024];
      std::error_code error;
      std::size_t n = c.receive(asio::buffer(data),
          std::chrono::seconds(10), error);

      if (error)
      {
        std::cout << "Receive error: " << error.message() << "\n"; 
      }
      else
      {
        std::cout << "Received: ";
        std::cout.write(data, n);
        std::cout << "\n";
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
