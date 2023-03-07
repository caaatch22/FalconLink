#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace falconlink {

/** In falconlink, We only throw exceptions before the server start */

/** ExceptionType is all the types of exceptions that we expect to throw in our system. */
enum class ExceptionType {
  INVALID = 0,
  CONCURRENCY_ERROR = 1,
  SOCKET_ERROR = 2,
  CALLBACK_NOT_SET = 3,
  OUT_OF_MEMORY = 4,
};

class Exception : public std::runtime_error {
 public:
  /**
   * Construct a new Exception instance.
   * @param message The exception message
   */
  explicit Exception(const std::string &message) : std::runtime_error(message), type_(ExceptionType::INVALID) {
#ifndef NDEBUG
    std::string exception_message = "Message :: " + message + "\n";
    std::cerr << exception_message;
#endif
  }

  /**
   * Construct a new Exception instance with specified type.
   * @param exception_type The exception type
   * @param message The exception message
   */
  Exception(ExceptionType exception_type, const std::string &message)
      : std::runtime_error(message), type_(exception_type) {
#ifndef NDEBUG
    std::string exception_message =
        "\nException Type :: " + ExceptionTypeToString(type_) + "\nMessage :: " + message + "\n";
    std::cerr << exception_message;
#endif
  }

  /** @return The type of the exception */
  auto GetType() const -> ExceptionType { return type_; }

  /** @return A human-readable string for the specified exception type */
  static auto ExceptionTypeToString(ExceptionType type) -> std::string {
    switch (type) {
      case ExceptionType::INVALID:
        return "Invalid";
      case ExceptionType::CONCURRENCY_ERROR:
        return "Concurrency Error";
      case ExceptionType::SOCKET_ERROR:
        return "Socket Error";
      case ExceptionType::CALLBACK_NOT_SET:
        return "Callback not Set";
      case ExceptionType::OUT_OF_MEMORY:
        return "Out of Memory";
      default:
        return "Unknown";
    }
  }

 private:
  ExceptionType type_;
};

}  // namespace falconlink
