#pragma once

#include <string>
#include <vector>

namespace falconlink {

namespace http {

/**
 * This Cgier runs a client commanded program through traditional 'fork' +
 * 'execve' All the cgi program should reside in a '/cgi-bin' folder in the root
 * directory of the http serving directory parent and child process communicate
 * through a file, where child writes the output to it and parent read it out
 * afterwards
 * */
class Cgier {
 public:
  static auto parseCgier(const std::string &resource_url) -> Cgier;
  static auto makeInvalidCgier() -> Cgier;
  explicit Cgier(const std::string &path,
                 const std::vector<std::string> &arguments);
  auto run() -> std::vector<unsigned char>;
  auto isValid() const -> bool;
  auto getPath() const -> std::string;

 private:
  auto BuildArgumentList() -> char **;
  void FreeArgumentList(char** arg_list);
  std::string cgi_program_path_;
  std::vector<std::string> cgi_arguments_;
  bool valid_{true};
};

}  // namespace http

}  // namespace falconlink
