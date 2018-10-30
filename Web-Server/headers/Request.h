#ifndef WEB_SERVER_REQUEST_H
#define WEB_SERVER_REQUEST_H


#include <string>

class Socket;
class Request {
public:
    static Request* build_request(std::string request);
    virtual void process(Socket* socket) = 0;
    std::string not_found_response();
    std::string read_file_response(std::istream& file, std::string file_extenson);

protected:
    static const std::string GET_METHOD;
    static const std::string POST_METHOD;

    static const std::string NOT_FOUND_RESPONSE_STATUS;
    static const std::string SUCCESS_RESPONSE_STATUS;

    static const std::string LINE_TERMINATOR;

    static const std::string CONTENT_TYPE_HEADER;
    static const std::string CONTENT_LENGTH_HEADER;

    static const std::string NOT_FOUND_HTML_PAGE;

    static const std::string HTML_MIME_TYPE;

    std::string build_response(std::string status, std::string content_type, std::string content);
    std::string build_header(std::string header_name, std::string header_value);
};


#endif //WEB_SERVER_REQUEST_H
