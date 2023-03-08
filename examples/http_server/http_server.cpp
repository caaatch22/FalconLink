#include <memory>
#include <optional>
#include <string>

#include "http/cgier.hpp"
#include "http/header.hpp"
#include "http/http_utils.hpp"
#include "http/request.hpp"
#include "http/response.hpp"
#include "net/cache.hpp"
#include "net/server.hpp"

namespace falconlink {

namespace http {

void processHttpRequest(  // NOLINT
    const std::string &serving_directory,
    std::shared_ptr<Cache> &cache,  // NOLINT
    Connection *client_conn) {
  // edge-trigger, first read all available bytes
  int from_fd = client_conn->fd();
  auto [read, exit] = client_conn->recv();
  if (exit) {
    client_conn->getEventLoop()->deleteConnection(from_fd);
    // client_conn ptr is invalid below here, do not touch it again
    return;
  }
  // check if there is any complete http request ready
  bool no_more_parse = false;
  std::optional<std::string> request_op =
      client_conn->FindAndPopTill("\r\n\r\n");
  while (request_op != std::nullopt) {
    Request request{request_op.value()};
    std::vector<unsigned char> response_buf;
    if (!request.isValid()) {
      auto response = Response::Make400Response();
      no_more_parse = true;
      response.serialize(response_buf);
    } else {
      std::string resource_full_path =
          serving_directory + request.getResourceUrl();
      // TODO(catch22): urlDecode only support for linux now
      resource_full_path = urlDecode(resource_full_path);
      if (isCgiRequest(resource_full_path)) {
        // dynamic CGI request
        Cgier cgier = Cgier::parseCgier(resource_full_path);
        if (!cgier.isValid()) {
          auto response = Response::Make400Response();
          no_more_parse = true;
          response.serialize(response_buf);
        } else {
          auto cgi_program_path = cgier.getPath();
          if (!isFileExists(cgi_program_path)) {
            auto response = Response::Make404Response();
            no_more_parse = true;
            response.serialize(response_buf);
          } else {
            auto cgi_result = cgier.run();
            auto response =
                Response::Make200Response(request.shouldClose(), std::nullopt);
            response.changeHeader(HEADER_CONTENT_LENGTH,
                                  std::to_string(cgi_result.size()));
            no_more_parse = request.shouldClose();
            response.serialize(response_buf);
            response_buf.insert(response_buf.end(), cgi_result.begin(),
                                cgi_result.end());
          }
        }
      } else {
        // static resource request
        if (!isFileExists(resource_full_path)) {
          auto response = Response::Make404Response();
          no_more_parse = true;
          response.serialize(response_buf);
        } else {
          auto response = Response::Make200Response(request.shouldClose(),
                                                    resource_full_path);
          response.serialize(response_buf);
          no_more_parse = request.shouldClose();
          std::vector<unsigned char> cache_buf;
          if (request.getMethod() == Method::GET) {
            // only concern about carrying content when GET request
            bool resource_cached =
                cache->TryLoad(resource_full_path, cache_buf);
            if (!resource_cached) {
              // if content directly from cache, not disk file I/O
              // otherwise content not in cache, load from disk and try cache it
              loadFile(resource_full_path, cache_buf);
              cache->TryInsert(resource_full_path, cache_buf);
            }
          }
          // now cache_buf contains the file content anyway
          response_buf.insert(response_buf.end(), cache_buf.begin(),
                              cache_buf.end());
        }
      }
    }
    // send out the response
    client_conn->WriteToWriteBuffer(std::move(response_buf));
    client_conn->send();
    if (no_more_parse) {
      break;
    }
    request_op = client_conn->FindAndPopTill("\r\n\r\n");
  }
  if (no_more_parse) {
    client_conn->getEventLoop()->deleteConnection(from_fd);
    // client_conn ptr is invalid below here, do not touch it again
    return;
  }
}

}  // namespace http

}  // namespace falconlink

int main(int argc, char *argv[]) {
  const std::string usage =
      "Usage: \n"
      "./http_server [optional: port default=8090] [optional: directory "
      "default=../examples/http_server/http_resource/] \n";
  if (argc > 3) {
    std::cout << "argument number error\n";
    std::cout << usage;
    exit(EXIT_FAILURE);
  }
  falconlink::InetAddr address("0.0.0.0", 8090);
  std::string directory = "../examples/http_server/http_resource/";
  if (argc >= 2) {
    auto port = static_cast<uint16_t>(std::strtol(argv[1], nullptr, 10));
    if (port == 0) {
      std::cout << "port error\n";
      std::cout << usage;
      exit(EXIT_FAILURE);
    }
    address = {"0.0.0.0", port};
    if (argc == 3) {
      directory = argv[2];
      if (!falconlink::http::isDirectory(directory)) {
        std::cout << "directory error\n";
        std::cout << usage;
        exit(EXIT_FAILURE);
      }
    }
  }

  falconlink::Server http_server(address);

  auto cache = std::make_shared<falconlink::Cache>();
  http_server
      .onHandle([&](falconlink::Connection *client_conn) {
        falconlink::http::processHttpRequest(directory, cache, client_conn);
      })
      .start();
  return 0;
}
