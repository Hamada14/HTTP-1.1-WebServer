#include "../headers/MIMETypesLoader.h"

#include <fstream>

const std::string MIMETypesLoader::mimes_file_path = "data/mime_types.txt";

const std::string MIMETypesLoader::default_mime_type = "application/octet-stream";

MIMETypesLoader* MIMETypesLoader::instance;

MIMETypesLoader::MIMETypesLoader() {
    std::ifstream mime_types_data_source(mimes_file_path.c_str());
    std::string file_extension, file_mime_type;
    while(mime_types_data_source >> file_mime_type >> file_extension) {
        extension_to_mime[file_extension] = file_mime_type;
    }
}

MIMETypesLoader* MIMETypesLoader::get_instance() {
    if(instance == NULL) {
        instance = new MIMETypesLoader();
    }
    return instance;
}


std::string MIMETypesLoader::get_mime_type(std::string file_extension) {
    if(extension_to_mime.count(file_extension)) {
        return extension_to_mime[file_extension];
    }
    return default_mime_type;
}