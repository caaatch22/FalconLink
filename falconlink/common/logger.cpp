#include "common/logger.hpp"
#include <filesystem>
#include <fstream>

namespace falconlink {

const char *log_level_names[] = {"INFO: ", "WARNING: ", "ERROR: ", "FATAL: "};

/* helper function to get current time since epoch in milliseconds */
std::chrono::milliseconds getCurrentTime() {

  using namespace std::chrono;  // NOLINT
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

/* helper function to get current datetime in format DDMMYYYY */
std::string getCurrentDate() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream stream;
  stream << std::put_time(&tm, "%d%b%Y");
  return stream.str();
}

/* simple printing to stdout logging strategy, caller should ensure thread-safe access */
void printToScreen(const std::deque<Logger::Log> &logs) {
  std::for_each(logs.begin(), logs.end(), [](const auto &log) { std::cout << log; });
}

/* opened log stream during the lifetime of the entire server */
struct streamWriter {
  std::fstream f_;

  explicit streamWriter(const std::string &path = LOG_PATH) {
    f_.open(path + "_" + getCurrentDate(), std::fstream::out | std::fstream::trunc);
  }

  ~streamWriter() {
    if (f_.is_open()) {
      f_.flush();
      f_.close();
    }
  }

  void writeLogs(const std::deque<Logger::Log> &logs) {
    std::for_each(logs.begin(), logs.end(), [this](auto &log) { f_ << log; });
    f_.flush();
  }
};

/* simple printing to a file logging strategy, caller should ensure thread-safe access */
void printToFile(const std::deque<Logger::Log> &logs) {
  static streamWriter stream_writer;
  stream_writer.writeLogs(logs);
}

/*
 stamp datetime and log level
 not guaranteed to be output in the stamped time order, best effort approach
*/
Logger::Log::Log(LogLevel log_level, const std::string &log_msg) noexcept {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream stream;
  stream << std::put_time(&tm, "[%d %b %Y %H:%M:%S]") << log_level_names[static_cast<int>(log_level)] << log_msg
         << std::endl;
  stamped_msg_ = stream.str();
}

/*
 * static public logging entry
 */
void Logger::LogMsg(LogLevel log_level, const std::string &msg) noexcept {
  auto log = Logger::Log(log_level, msg);
  getInstance().pushLog(std::move(log));
}

/*
 * Singleton Pattern access point
 */
Logger& Logger::getInstance() {
  // insert your logging strategy here in ctor
  // instead of the default one for customization
  // see example of 'PrintToScreen' and 'PrintToFile'
  static Logger single_logger{printToFile};
  return single_logger;
}

/*
 * private constructor, takes in a logging strategy
 * upon ctor, launch backend worker thread
 */
Logger::Logger(const std::function<void(const std::deque<Log> &logs)> &log_strategy) {
  log_strategy_ = log_strategy;
  last_flush_ = getCurrentTime();
  log_writer_ = std::thread(&Logger::LogWriting, this);
}

/*
 * signal and harvest backend thread
 */
Logger::~Logger() {
  done_ = true;
  cv_.notify_one();
  if (log_writer_.joinable()) {
    log_writer_.join();
  }
}

/*
 * internal helper to push a log into the FIFO queue
 * potentially notify the backend worker to swap and flush
 * if threshold criteria is met
 */
void Logger::pushLog(Logger::Log &&log) {
  using std::chrono::milliseconds;
  bool should_notify = false;
  {
    std::unique_lock<std::mutex> lock(mtx_);
    queue_.push_back(std::move(log));
    milliseconds now = getCurrentTime();
    if ((now - last_flush_) > REFRESH_THRESHOLD || queue_.size() > COUNT_THRESHOLD) {
      should_notify = true;
    }
  }
  // a best effort notification to worker thread, not guarantee flush soon
  if (should_notify) {
    cv_.notify_one();
  }
}

/*
 * The thread routine for the backend log writer
 */
void Logger::LogWriting() {
  std::deque<Logger::Log> writer_queue;
  while (true) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this]() {
      return done_ || queue_.size() > COUNT_THRESHOLD || getCurrentTime() - last_flush_ > REFRESH_THRESHOLD;
    });
    // either the flush criteria is met or is about to exit
    // need to record the remaining log in either case
    if (!queue_.empty()) {
      writer_queue.swap(queue_);
      lock.unlock();  // producer may continue
      log_strategy_(writer_queue);
      last_flush_ = getCurrentTime();
      writer_queue.clear();
    }
    if (done_) {
      // exit this background thread
      return;
    }
  }
}

}  // namespace falconlink