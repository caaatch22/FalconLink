#include "http/http_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace falconlink {

namespace http {

Method toMethod(const std::string &method_str) {
  auto method_str_formatted = format(method_str);
  if (method_str_formatted == METHOD_TO_STRING.at(Method::GET)) {
    return Method::GET;
  }
  if (method_str_formatted == METHOD_TO_STRING.at(Method::HEAD)) {
    return Method::HEAD;
  }
  return Method::UNSUPPORTED;
}

Version toVersion(const std::string &version_str) {
  auto version_str_formatted = format(version_str);
  if (version_str_formatted == VERSION_TO_STRING.at(Version::HTTP_1_1)) {
    return Version::HTTP_1_1;
  }
  return Version::UNSUPPORTED;
}

Extension toExtension(const std::string &extension_str) {
  auto extension_str_formatted = format(extension_str);
  if (extension_str_formatted == EXTENSION_TO_STRING.at(Extension::HTML)) {
    return Extension::HTML;
  }
  if (extension_str_formatted == EXTENSION_TO_STRING.at(Extension::CSS)) {
    return Extension::CSS;
  }
  if (extension_str_formatted == EXTENSION_TO_STRING.at(Extension::PNG)) {
    return Extension::PNG;
  }
  if (extension_str_formatted == EXTENSION_TO_STRING.at(Extension::JPG)) {
    return Extension::JPG;
  }
  if (extension_str_formatted == EXTENSION_TO_STRING.at(Extension::JPEG)) {
    return Extension::JPEG;
  }
  if (extension_str_formatted == EXTENSION_TO_STRING.at(Extension::GIF)) {
    return Extension::GIF;
  }
  return Extension::OCTET;
}

std::string ExtensionToMime(const Extension &extension) {
  if (extension == Extension::HTML) {
    return MIME_HTML;
  }
  if (extension == Extension::CSS) {
    return MIME_CSS;
  }
  if (extension == Extension::PNG) {
    return MIME_PNG;
  }
  if (extension == Extension::JPG) {
    return MIME_JPG;
  }
  if (extension == Extension::JPEG) {
    return MIME_JPEG;
  }
  if (extension == Extension::GIF) {
    return MIME_GIF;
  }
  if (extension == Extension::OCTET) {
    return MIME_OCTET;
  }
  return MIME_OCTET;
}

std::vector<std::string> split(const std::string &str, const char *delim) {
  std::vector<std::string> tokens;
  if (str.empty()) {
    return tokens;
  }
  std::string token;
  size_t curr = 0;
  size_t next;
  size_t delim_len = strlen(delim);
  while ((next = str.find(delim, curr)) != std::string::npos) {
    tokens.emplace_back(str.substr(curr, next - curr));
    curr = next + delim_len;
  }
  if (curr != str.size()) {
    // one last word
    tokens.emplace_back(str.substr(curr, str.size() - curr));
  }
  return tokens;
}

std::string join(const std::vector<std::string> &tokens, const char *delim) {
  if (tokens.empty()) {
    return {};
  }
  if (tokens.size() == 1) {
    return tokens[0];
  }
  std::stringstream str_stream;
  for (size_t i = 0; i < tokens.size() - 1; i++) {
    str_stream << tokens[i] << delim;
  }
  str_stream << tokens[tokens.size() - 1];
  return str_stream.str();
}

std::string trim(const std::string &str, const char *delim) {
  size_t r_found = str.find_last_not_of(delim);
  if (r_found == std::string::npos) {
    return {};
  }
  size_t l_found = str.find_first_not_of(delim);
  return str.substr(l_found, r_found - l_found + 1);
}

std::string toUpper(const std::string &str) {
  std::string res;
  std::transform(str.begin(), str.end(), std::back_inserter(res),
                 [](char c) { return std::toupper(c); });
  return res;
}

std::string format(const std::string &str) { return toUpper(trim(str)); }

bool isDirectory(const std::string &directory_path) {
  return std::filesystem::is_directory(directory_path);
}

bool isCgiRequest(const std::string &resource_url) {
  return resource_url.find(CGI_BIN) != std::string::npos;
}

bool isFileExists(const std::string &file_path) {
  return std::filesystem::exists(file_path);
}

bool deleteFile(const std::string &file_path) {
  return std::filesystem::remove(file_path);
}

size_t getFileSize(const std::string &file_path) {
  assert(isFileExists(file_path));
  return std::filesystem::file_size(file_path);
}

void loadFile(const std::string &file_path,
              std::vector<unsigned char> &buffer) {  // NOLINT
  size_t file_size = getFileSize(file_path);
  size_t buffer_old_size = buffer.size();
  std::ifstream file(file_path);
  buffer.resize(buffer_old_size + file_size);
  assert(file.is_open());
  file.read(reinterpret_cast<char *>(&buffer[buffer_old_size]),
            static_cast<std::streamsize>(file_size));
}

std::string urlEncode(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (std::string::value_type c : value) {
    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }
    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int((unsigned char)c);
    escaped << std::nouppercase;
  }
  return escaped.str();
}

std::string urlDecode(const std::string &value) {
  std::string result;

  for (size_t i = 0; i < value.size(); i++) {
    if (value[i] != '%' && value[i] != '+') {
      result += value[i];
      continue;
    }
    // we assume every input is valid, so we don't check i + 2 >= size here
    if (value[i] == '%') {
      std::istringstream is(value.substr(i + 1, 2));
      int tmp = 0;
      is >> std::hex >> tmp;
      result += static_cast<char>(tmp);
      i += 2;
    } else {
      result += ' ';
    }
  }
  return result;
}

}  // namespace http

}  // namespace falconlink
