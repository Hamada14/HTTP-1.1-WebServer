#include "../headers/Request.h"
#include "../headers/GetRequest.h"
#include "../headers/PostRequest.h"
#include "../headers/MIMETypesLoader.h"

const std::string Request::GET_METHOD = "GET";
const std::string Request::POST_METHOD = "POST";

const std::string Request::NOT_FOUND_RESPONSE_STATUS = "HTTP/1.1 404 Not Found";
const std::string Request::SUCCESS_RESPONSE_STATUS = "HTTP/1.1 200 Ok";

const std::string Request::LINE_TERMINATOR = "\r\n";

const std::string Request::CONTENT_TYPE_HEADER = "Content-Type";
const std::string Request::CONTENT_LENGTH_HEADER = "Content-Length";

const std::string Request::NOT_FOUND_HTML_PAGE = "<html>" + LINE_TERMINATOR +
                                                         "  <body>" + LINE_TERMINATOR +
                                                         "      Not Found" + LINE_TERMINATOR +
                                                         "  </body>" + LINE_TERMINATOR +
                                                         "</html>";

const std::string Request::HTML_MIME_TYPE = "text/html";

Request* Request::build_request(std::string request) {
    if(request.substr(0, GET_METHOD.length()) == GET_METHOD) {
        return new GetRequest(request);
    } else if(request.substr(0, POST_METHOD.length()) == POST_METHOD) {
        return new PostRequest(request);
    }
    return NULL;
}

std::string Request::not_found_response() {
    return build_response(NOT_FOUND_RESPONSE_STATUS, HTML_MIME_TYPE, NOT_FOUND_HTML_PAGE);
}

std::string Request::read_file_response(std::istream& file, std::string file_extension) {
    size_t buffer_size = 1024;
    char buffer[buffer_size];
    std::ostringstream oss;
    size_t len;
    while((len = file.readsome(buffer, buffer_size)) > 0)
    {
        oss.write(buffer, len);
    }
    return build_response(SUCCESS_RESPONSE_STATUS, MIMETypesLoader::get_instance()->get_mime_type(file_extension), oss.str());
}

std::string Request::build_response(std::string status, std::string content_type, std::string content) {
    return status + LINE_TERMINATOR +
           build_header(CONTENT_TYPE_HEADER, content_type) + LINE_TERMINATOR +
           build_header(CONTENT_LENGTH_HEADER, std::to_string(content.length())) + LINE_TERMINATOR +
            LINE_TERMINATOR +
           content;
}

std::string Request::build_header(std::string header_name, std::string header_value) {
    return header_name + ":" + header_value;
}