#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "common/macros.hpp"
#include "http/cgier.hpp"
#include "http/header.hpp"
#include "http/http_utils.hpp"
#include "http/request.hpp"
#include "http/response.hpp"
#include "net/cache.hpp"
#include "net/server.hpp"

namespace falconlink {

namespace http {

class HttpServer : public Server {
 public:
  explicit HttpServer(const InetAddr &server_addr,
                      const std::string &http_resource_dir)
      : Server(server_addr),
        http_resource_dir_(http_resource_dir),
        cache_(std::make_shared<Cache>()) {}

  NON_COPYABLE_AND_NON_MOVEABLE(HttpServer);

  void processHttpRequest(const std::string &http_resource_dir,
                          std::shared_ptr<Cache> &cache,
                          Connection *client_conn) {
    int from_fd = client_conn->fd();
    auto [read, exit] = client_conn->recv();
    if (exit) {
      client_conn->getEventLoop()->deleteConnection(from_fd);
      return;
    }

    bool no_more_parse = false;
    std::optional<std::string> request_op =
        client_conn->FindAndPopTill("\r\n\r\n");
    while (request_op != std::nullopt) {
      Request request{request_op.value()};
      std::vector<unsigned char> response_buf;
      if (!request.valid()) {
        auto response = Response::Make400Response();
        no_more_parse = true;
        response.serialize(response_buf);
      } else {
        std::string resource_full_path =
            http_resource_dir + request.getResourceUrl();
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
              auto response = Response::Make200Response(request.shouldClose(),
                                                        std::nullopt);
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
                  cache->tryLoad(resource_full_path, cache_buf);
              if (!resource_cached) {
                // if content directly from cache, not disk file I/O
                // otherwise content not in cache, load from disk and try cache
                // it
                loadFile(resource_full_path, cache_buf);
                cache->tryInsert(resource_full_path, cache_buf);
              }
            }
            // now cache_buf contains the file content anyway
            response_buf.insert(response_buf.end(), cache_buf.begin(),
                                cache_buf.end());
          }
        }
      }
      // send out the respons
      client_conn->WriteToWriteBuffer(std::move(response_buf));
      client_conn->send();
      if (no_more_parse) {
        break;
      }
      request_op = client_conn->FindAndPopTill("\r\n\r\n");
    }  // end while
    if (no_more_parse) {
      client_conn->getEventLoop()->deleteConnection(from_fd);
      // client_conn ptr is invalid below here, do not touch it again
      return;
    }
  }  // end processHttpRequest

  HttpServer &handle() {
    onHandle([&](falconlink::Connection *client_conn) {
      processHttpRequest(http_resource_dir_, cache_, client_conn);
    });
    return *this;
  }

 private:
  std::string http_resource_dir_;
  std::shared_ptr<Cache> cache_;
};

}  // namespace http

}  // namespace falconlink

int main(int argc, char *argv[]) {
  const std::string usage =
      "Usage: \n"
      "./bin/http_server [optional: port default=8090] [optional: directory "
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

  falconlink::http::HttpServer http_server(address, directory);

  http_server.handle().start();
  return 0;
}
