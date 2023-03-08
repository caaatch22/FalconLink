#pragma once

#include <unordered_map>
#include <string>
#include <vector>

namespace falconlink {

namespace http {

static constexpr int READ_WRITE_PERMISSION = 0600;

static constexpr char PARAMETER_SEPARATOR[] = {"&"};
static constexpr char UNDERSCORE[] = {"_"};
static constexpr char SPACE[] = {" "};
static constexpr char DOT[] = {"."};
static constexpr char CRLF[] = {"\r\n"};
static constexpr char COLON[] = {":"};
static constexpr char DEFAULT_ROUTE[] = {"index.html"};  // TODO(catch22): add this to config
static constexpr char CGI_BIN[] = {"cgi-bin"};
static constexpr char CGI_PREFIX[] = {"cgi_temp"};

/* Common Header and Value */
static constexpr char HEADER_SERVER[] = {"Server"};
static constexpr char SERVER_FALCONLINK[] = {"Falonlink/1.0"};
static constexpr char HEADER_CONTENT_LENGTH[] = {"Content-Length"};
static constexpr char HEADER_CONTENT_TYPE[] = {"Content-Type"};
static constexpr char CONTENT_LENGTH_ZERO[] = {"0"};
static constexpr char HEADER_CONNECTION[] = {"Connection"};
static constexpr char CONNECTION_CLOSE[] = {"Close"};
static constexpr char CONNECTION_KEEP_ALIVE[] = {"Keep-Alive"};
static constexpr char HTTP_VERSION[] = {"HTTP/1.1"};

/* MIME Types */
static constexpr char MIME_HTML[] = {"text/html"};
static constexpr char MIME_CSS[] = {"text/css"};
static constexpr char MIME_PNG[] = {"image/png"};
static constexpr char MIME_JPG[] = {"image/jpg"};
static constexpr char MIME_JPEG[] = {"image/jpeg"};
static constexpr char MIME_GIF[] = {"image/gif"};
static constexpr char MIME_OCTET[] = {"application/octet-stream"};

/* Response status */
static constexpr char RESPONSE_OK[] = {"200 OK"};
static constexpr char RESPONSE_BAD_REQUEST[] = {"400 Bad Request"};
static constexpr char RESPONSE_NOT_FOUND[] = {"404 Not Found"};
static constexpr char RESPONSE_SERVICE_UNAVAILABLE[] = {"503 Service Unavailable"};

/* HTTP Method enum, only support GET/HEAD method now */
enum class Method { GET, HEAD, UNSUPPORTED };

/* HTTP version enum, only support HTTP 1.1 now */
enum class Version { HTTP_1_1, UNSUPPORTED };

/* Content Extension enum */
enum class Extension { HTML, CSS, PNG, JPG, JPEG, GIF, OCTET };

static const std::unordered_map<Method, std::string> METHOD_TO_STRING{
    {Method::GET, "GET"},
    {Method::HEAD, "HEAD"},
    {Method::UNSUPPORTED, "UNSUPPORTED"}};

static const std::unordered_map<Version, std::string> VERSION_TO_STRING{
    {Version::HTTP_1_1, "HTTP/1.1"}, {Version::UNSUPPORTED, "UNSUPPORTED"}};

static const std::unordered_map<Extension, std::string> EXTENSION_TO_STRING{
    {Extension::HTML, "HTML"},  {Extension::CSS, "CSS"},
    {Extension::PNG, "PNG"},    {Extension::JPG, "JPG"},
    {Extension::JPEG, "JPEG"},  {Extension::GIF, "GIF"},
    {Extension::OCTET, "OCTET"}};

/* space and case insensitive */
Method toMethod(const std::string &method_str);

/* space and case insensitive */
Version toVersion(const std::string &version_str);

/* space and case insensitive */
Extension toExtension(const std::string &extension_str);

/* space and case insensitive */
std::string ExtensionToMime(const Extension &extension);

/**
 * split a string into many sub strings, splitted by the specified delimiter
 */
std::vector<std::string> split(const std::string &str,
                               const char *delim = SPACE);

/**
 * concatenate a collection of strings using the specified delimiter
 */
std::string join(const std::vector<std::string> &tokens,
                 const char *delim = SPACE);

/**
 * Remove the leading and trailing specified delimiter (not inplace)
 */
std::string trim(const std::string &str, const char *delim = SPACE);

/**
 * Convert each character in a string to upper case (not inplace)
 */
std::string toUpper(const std::string &str);

/**
 * Apply Trim + ToUpper to a string and return the formatted version of it
 */
std::string format(const std::string &);

/**
 * Check if the path-specified directory exists
 */
bool isDirectory(const std::string &directory_path);

/**
 * Inspect if the request is dynamic CGI
 * hardcode check if contains the cgi-bin folder in resource url path
 */
bool isCgiRequest(const std::string &resource_url);

/**
 * Check if the path-specified path exists
 */
bool isFileExists(const std::string &file_path);
/**
 * Delete a file if exists
 */
bool deleteFile(const std::string &file_path);

/**
 * Tell the size of a file in bytes.
 * Assume this file already exists and is verified
 */
size_t getFileSize(const std::string &file_path);

/**
 * Load the file appending to be back of a vector of unsigned char
 * able to contain binary data
 */
void loadFile(const std::string &file_path,
              std::vector<unsigned char> &buffer);  // NOLINT


std::string urlEncode(const std::string &value);


/**
 * Decode url
 * @note: this function only work in Linux platform (wrong in windows)
*/
std::string urlDecode(const std::string &value);

}  // namespace http


}  // namespace falconlink
