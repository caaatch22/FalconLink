#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>              // NOLINT
#include <condition_variable>  // NOLINT
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>  // NOLINT
#include <sstream>
#include <string>
#include <thread>  // NOLINT

#include "common/macros.hpp"


namespace falconlink {

enum class LogLevel { INFO, WARNING, ERROR, FATAL };

/* threshold */
constexpr int COUNT_THRESHOLD = 1000;
constexpr std::chrono::duration REFRESH_THRESHOLD = std::chrono::microseconds(3000);

/* log file name if used */
const std::string LOG_PATH = std::string("FaclonlinkLog");  // NOLINT

/**
 * A simple asynchronous logger
 * All callers counts as frontend-producer and is non-blocking
 * a backend worker thread periodically flush the log to persistent storage
 */
class Logger {
 public:
  /*
   * public logging entry
   */
  static void LogMsg(LogLevel log_level, const std::string &msg) noexcept;

  /*
   * Singleton Pattern access point
   */
  static Logger& getInstance();

  NON_COPYABLE_AND_NON_MOVEABLE(Logger);

  /*
   * Each individual Log message
   * upon construction, the date time is prepended
   */
  struct Log {
    std::string stamped_msg_;

    /*
     stamp datetime and log level
     not guaranteed to be output in the stamped time order, best effort approach
    */
    Log(LogLevel log_level, const std::string &log_msg) noexcept;

    friend std::ostream& operator<<(std::ostream &os, const Log &log) {
      os << log.stamped_msg_;
      return os;
    }
  };

 private:
  /*
   * private constructor, takes in a logging strategy
   * upon ctor, launch backend worker thread
   */
  explicit Logger(const std::function<void(const std::deque<Log> &logs)> &log_strategy);

  ~Logger();

  /*
   * internal helper to push a log into the FIFO queue
   * potentially notify the backend worker to swap and flush
   * if threshold criteria is met
   */
  void pushLog(Log &&log);

  /*
   * The thread routine for the backend log writer
   */
  void LogWriting();

  std::function<void(const std::deque<Log> &)> log_strategy_;
  std::atomic<bool> done_ = false;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::deque<Log> queue_;
  std::thread log_writer_;
  std::chrono::microseconds last_flush_;
};

/* macro definitions for 4 levels of logging */
#ifdef NOLOG
#define LOG_INFO(x) {};
#define LOG_WARNING(x) {};
#define LOG_ERROR(x) {};
#define LOG_FATAL(x) {};
#else
#define LOG_INFO(x) falconlink::Logger::LogMsg(falconlink::LogLevel::INFO, (x));
#define LOG_WARNING(x) falconlink::Logger::LogMsg(falconlink::LogLevel::WARNING, (x));
#define LOG_ERROR(x) falconlink::Logger::LogMsg(falconlink::LogLevel::ERROR, (x));
#define LOG_FATAL(x) falconlink::Logger::LogMsg(falconlink::LogLevel::FATAL, (x));
#endif
}  // namespace falconlink
