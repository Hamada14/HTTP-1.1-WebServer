#include "../headers/Request.h"
#include "../headers/GetRequest.h"
#include "../headers/PostRequest.h"
#include "../headers/MIMETypesLoader.h"
#include "../headers/Util.h"
#include <iostream>


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

EmptyRequest::EmptyRequest(): Request(Type::EMPTY) {}

UndefinedRequest::UndefinedRequest(): Request(Type::UNDEFINED) {}

/**
 * Builds a request depending on the type required.
 * It can be either GET or POST request.
 * @param request String format of the request.
 * @return Request object.
 */
std::shared_ptr<Request> Request::build_request(std::string request) {
    std::cout << request << std::endl;
    std::map<std::string, std::string> headers = extract_headers(request);
    if(request.empty()) {
        return std::make_shared<EmptyRequest>();
    } else if(request.substr(0, GET_METHOD.length()) == GET_METHOD) {
        return std::make_shared<GetRequest>(request, headers);
    } else if(request.substr(0, POST_METHOD.length()) == POST_METHOD) {
        return std::make_shared<PostRequest>(request, headers);
    }
    return std::make_shared<UndefinedRequest>();
}

Request::Request(Type type): type_(type) {
}

Request::Type Request::type() {
    return this->type_;
}

/**
 * Extracts headers from a request and convert them into a map that stores headers key and value.
 * @param request String format of the request.
 * @return std::map<std::string, std::string>
 */
std::map<std::string, std::string> Request::extract_headers(std::string request) {
    std::map<std::string, std::string> headers;
    int last_new_line_index = request.find(Request::LINE_TERMINATOR);
    int next_new_line_index;
    while((next_new_line_index = request.find(Request::LINE_TERMINATOR, last_new_line_index + Request::LINE_TERMINATOR.length() + 1)) != std::string::npos) {
        std::string header_line = request.substr(last_new_line_index + Request::LINE_TERMINATOR.length(), next_new_line_index - last_new_line_index - Request::LINE_TERMINATOR.length());
        std::vector<std::string> splits = Util::split(header_line, ':');
        headers[splits[0]] = Util::trim(splits[1]);
        last_new_line_index = next_new_line_index;
    }
    return headers;
}

/**
 * @return Returns a response containing a 404 HTTP status with a not found sample of page.
 */
std::string Request::not_found_response() {
    return build_response(NOT_FOUND_RESPONSE_STATUS, HTML_MIME_TYPE, NOT_FOUND_HTML_PAGE);
}

/**
 * Reads a file and sends it as a resposne to a previous request.
 * @param file File stream used to read the data inside the file.
 * @param file_extension File extension that's used to determine the MIME.
 * @return String form of the response.
 */
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

/**
 * Builds an HTTP/1.1 Response by concatenating the following:
 * 1. Response Status
 * 2. Content Type header
 * 3. Content Length header
 * 4. Extra Line terminator
 * 5. Content
 * @param status Http status to be added in the resposne.
 * @param content_type Type of the content sent in the response.
 * @param content The content that's being sent in the response in the form of a string.
 * @return String containing the HTTP response.
 */
std::string Request::build_response(std::string status, std::string content_type, std::string content) {
    return status + LINE_TERMINATOR +
           build_header(CONTENT_TYPE_HEADER, content_type) + LINE_TERMINATOR +
           build_header(CONTENT_LENGTH_HEADER, std::to_string(content.length())) + LINE_TERMINATOR +
            LINE_TERMINATOR +
           content;
}

/**
 * Builds a header using the format specified in HTTP/1.1 by adding a double colon between the header key and its value.
 * @param header_name Name of the header.
 * @param header_value Value of the header.
 * @return String containing HTTP/1.1 presentation of the header.
 */
std::string Request::build_header(std::string header_name, std::string header_value) {
    return header_name + ":" + header_value;
}
