//
// composed_8.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <asio/compose.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_future.hpp>
#include <asio/write.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

using asio::ip::tcp;

// NOTE: This example requires the new asio::async_compose function. For
// an example that works with the Networking TS style of completion tokens,
// please see an older version of asio.

//------------------------------------------------------------------------------

// This composed operation shows composition of multiple underlying operations,
// using asio's stackless coroutines support to express the flow of control. It
// automatically serialises a message, using its I/O streams insertion
// operator, before sending it N times on the socket. To do this, it must
// allocate a buffer for the encoded message and ensure this buffer's validity
// until all underlying async_write operation complete. A one second delay is
// inserted prior to each write operation, using a steady_timer.

#include <asio/yield.hpp>

// In this example, the composed operation's logic is implemented as a state
// machine within a hand-crafted function object.
struct async_write_messages_implementation
{
  // The implementation holds a reference to the socket as it is used for
  // multiple async_write operations.
  tcp::socket& socket_;

  // The allocated buffer for the encoded message. The std::unique_ptr smart
  // pointer is move-only, and as a consequence our implementation is also
  // move-only.
  std::unique_ptr<std::string> encoded_message_;

  // The repeat count remaining.
  std::size_t repeat_count_;

  // A steady timer used for introducing a delay.
  std::unique_ptr<asio::steady_timer> delay_timer_;

  // The coroutine state.
  asio::coroutine coro_;

  // The first argument to our function object's call operator is a reference
  // to the enclosing intermediate completion handler. This intermediate
  // completion handler is provided for us by the asio::async_compose
  // function, and takes care of all the details required to implement a
  // conforming asynchronous operation. When calling an underlying asynchronous
  // operation, we pass it this enclosing intermediate completion handler
  // as the completion token.
  //
  // All arguments after the first must be defaulted to allow the state machine
  // to be started, as well as to allow the completion handler to match the
  // completion signature of both the async_write and steady_timer::async_wait
  // operations.
  template <typename Self>
  void operator()(Self& self,
      const std::error_code& error = std::error_code(),
      std::size_t = 0)
  {
    reenter (coro_)
    {
      while (repeat_count_ > 0)
      {
        --repeat_count_;

        delay_timer_->expires_after(std::chrono::seconds(1));
        yield delay_timer_->async_wait(std::move(self));
        if (error)
          break;

        yield asio::async_write(socket_,
            asio::buffer(*encoded_message_), std::move(self));
        if (error)
          break;
      }

      // Deallocate the encoded message and delay timer before calling the
      // user-supplied completion handler.
      encoded_message_.reset();
      delay_timer_.reset();

      // Call the user-supplied handler with the result of the operation.
      self.complete(error);
    }
  }
};

#include <asio/unyield.hpp>

template <typename T, typename CompletionToken>
auto async_write_messages(tcp::socket& socket,
    const T& message, std::size_t repeat_count,
    CompletionToken&& token)
  // The return type of the initiating function is deduced from the combination
  // of CompletionToken type and the completion handler's signature. When the
  // completion token is a simple callback, the return type is always void.
  // In this example, when the completion token is asio::yield_context
  // (used for stackful coroutines) the return type would be also be void, as
  // there is no non-error argument to the completion handler. When the
  // completion token is asio::use_future it would be std::future<void>.
  -> typename asio::async_result<
    typename std::decay<CompletionToken>::type,
    void(std::error_code)>::return_type
{
  // Encode the message and copy it into an allocated buffer. The buffer will
  // be maintained for the lifetime of the composed asynchronous operation.
  std::ostringstream os;
  os << message;
  std::unique_ptr<std::string> encoded_message(new std::string(os.str()));

  // Create a steady_timer to be used for the delay between messages.
  std::unique_ptr<asio::steady_timer> delay_timer(
      new asio::steady_timer(socket.get_executor()));

  // The asio::async_compose function takes:
  //
  // - our asynchronous operation implementation,
  // - the completion token,
  // - the completion handler signature, and
  // - any I/O objects (or executors) used by the operation
  //
  // It then wraps our implementation in an intermediate completion handler
  // that meets the requirements of a conforming asynchronous operation. This
  // includes tracking outstanding work against the I/O executors associated
  // with the operation (in this example, this is the socket's executor).
  return asio::async_compose<
    CompletionToken, void(std::error_code)>(
      async_write_messages_implementation{socket,
        std::move(encoded_message), repeat_count,
        std::move(delay_timer), asio::coroutine()},
      token, socket);
}

//------------------------------------------------------------------------------

void test_callback()
{
  asio::io_context io_context;

  tcp::acceptor acceptor(io_context, {tcp::v4(), 55555});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using a lambda as a callback.
  async_write_messages(socket, "Testing callback\r\n", 5,
      [](const std::error_code& error)
      {
        if (!error)
        {
          std::cout << "Messages sent\n";
        }
        else
        {
          std::cout << "Error: " << error.message() << "\n";
        }
      });

  io_context.run();
}

//------------------------------------------------------------------------------

void test_future()
{
  asio::io_context io_context;

  tcp::acceptor acceptor(io_context, {tcp::v4(), 55555});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using the use_future completion token.
  // This token causes the operation's initiating function to return a future,
  // which may be used to synchronously wait for the result of the operation.
  std::future<void> f = async_write_messages(
      socket, "Testing future\r\n", 5, asio::use_future);

  io_context.run();

  try
  {
    // Get the result of the operation.
    f.get();
    std::cout << "Messages sent\n";
  }
  catch (const std::exception& e)
  {
    std::cout << "Error: " << e.what() << "\n";
  }
}

//------------------------------------------------------------------------------

int main()
{
  test_callback();
  test_future();
}
