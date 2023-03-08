#include "http/cgier.hpp"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <thread>  // NOLINT

#include "http/http_utils.hpp"

namespace falconlink {

namespace http {

auto Cgier::parseCgier(const std::string &resource_url) -> Cgier {
  if (resource_url.empty() || !isCgiRequest(resource_url)) {
    return makeInvalidCgier();
  }
  // find the first & after the cgi-bin/ to fetch out cgi program path
  auto cgi_pos = resource_url.find(CGI_BIN);
  auto cgi_separator = resource_url.find(PARAMETER_SEPARATOR, cgi_pos);
  auto cgi_path = resource_url.substr(0, cgi_separator);
  auto arguments =
      split(resource_url.substr(cgi_separator + 1), PARAMETER_SEPARATOR);
  return Cgier(cgi_path, arguments);
}

auto Cgier::makeInvalidCgier() -> Cgier {
  Cgier invalid_cgier{std::string(), std::vector<std::string>()};
  invalid_cgier.valid_ = false;
  return invalid_cgier;
}

Cgier::Cgier(const std::string &path, const std::vector<std::string> &arguments)
    : cgi_program_path_(path), cgi_arguments_(arguments), valid_(true) {}

auto Cgier::run() -> std::vector<unsigned char> {
  assert(valid_);
  std::vector<unsigned char> cgi_result;
  // unique shared filename within one Cgier
  // when communicating between parent and child
  std::stringstream ssr;
  ssr << CGI_PREFIX << UNDERSCORE << std::this_thread::get_id() << ".txt";
  std::string shared_file_name = ssr.str();
  int fd = open(shared_file_name.c_str(), O_RDWR | O_APPEND | O_CREAT,
                READ_WRITE_PERMISSION);
  if (fd == -1) {
    std::string error = "fail to create/open the file " + shared_file_name;
    return {error.begin(), error.end()};
  }
  pid_t pid = fork();
  if (pid == -1) {
    std::string error = "fail to fork()";
    return {error.begin(), error.end()};
  }
  if (pid == 0) {
    // child

    // link cgi program's stdout to the shared file
    dup2(fd, STDOUT_FILENO);
    close(fd);

    // build argument lists for the cgi program
    char **cgi_argv = BuildArgumentList();

    // walk into cig program
    if (execve(cgi_program_path_.c_str(), cgi_argv, nullptr) < 0) {
      // only reach here when execve fails
      perror("fail to execve()");
      FreeArgumentList(cgi_argv);
      exit(1);  // exit child process
    }
  } else {
    // parent
    close(fd);
    int status;
    // wait and harvest child process
    if (waitpid(pid, &status, 0) == -1) {
      std::string error = "fail to harvest child by waitpid()";
      return {error.begin(), error.end()};
    }
    // load cgi result from the shared file
    loadFile(shared_file_name, cgi_result);
    // clean it up by deleting shared file
    deleteFile(shared_file_name);
  }
  return cgi_result;
}

auto Cgier::isValid() const -> bool { return valid_; }

auto Cgier::getPath() const -> std::string { return cgi_program_path_; }

auto Cgier::BuildArgumentList() -> char ** {
  assert(!cgi_program_path_.empty());
  char **cgi_argv = (char **)calloc(cgi_arguments_.size() + 2, sizeof(char *));
  cgi_argv[0] = (char *)calloc(cgi_program_path_.size() + 1, sizeof(char));
  memcpy(cgi_argv[0], cgi_program_path_.c_str(), cgi_program_path_.size());
  for (size_t i = 0; i < cgi_arguments_.size(); i++) {
    cgi_argv[i + 1] =
        (char *)calloc(cgi_arguments_[i].size() + 1, sizeof(char));
    memcpy(cgi_argv[i + 1], cgi_arguments_[i].c_str(),
           cgi_arguments_[i].size());
  }
  cgi_argv[cgi_arguments_.size() + 1] =
      nullptr;  // indicate the end of arg list
  return cgi_argv;
}

void Cgier::FreeArgumentList(char **arg_list) {
  for (int i = 0; i < static_cast<int>(cgi_arguments_.size()) + 2; i++) {
    free(arg_list[i]);
  }
  free(arg_list);
}

}  // namespace http

}  // namespace falconlink
