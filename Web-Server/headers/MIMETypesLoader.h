#ifndef WEB_SERVER_MIMETYPESLOADER_H
#define WEB_SERVER_MIMETYPESLOADER_H

#include <string>
#include <map>

class MIMETypesLoader {
public:
    static MIMETypesLoader* get_instance();
    std::string get_mime_type(std::string file_extension);

private:
    const static std::string mimes_file_path;
    const static std::string default_mime_type;

    static MIMETypesLoader* instance;

    MIMETypesLoader();
    std::map<std::string, std::string> extension_to_mime;
};


#endif //WEB_SERVER_MIMETYPESLOADER_H
