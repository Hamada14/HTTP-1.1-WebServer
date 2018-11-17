#include "../headers/MIMETypesLoader.h"

#include <fstream>

const std::string MIMETypesLoader::mimes_file_path = "data/mime_types.txt";

const std::string MIMETypesLoader::default_mime_type = "application/octet-stream";

MIMETypesLoader* MIMETypesLoader::instance;

/**
 * MIMETypesLoader retrieves the MIME for a file using its extension to send it back along with the response so that
 * the browser can correctly render the content.
 * MIMEs and extensions are stored externally in a text file that is scanned initially when creating an instance of MIMETypesLoader.
 */
MIMETypesLoader::MIMETypesLoader() {
    std::ifstream mime_types_data_source(mimes_file_path.c_str());
    std::string file_extension, file_mime_type;
    while(mime_types_data_source >> file_mime_type >> file_extension) {
        extension_to_mime[file_extension] = file_mime_type;
    }
}

/**
 * Retrieves a single instance of the class to use it.
 * @return
 */
MIMETypesLoader* MIMETypesLoader::get_instance() {
    if(instance == NULL) {
        instance = new MIMETypesLoader();
    }
    return instance;
}

/**
 * Retrieves the appropriate MIME for a specific file_extension.
 * In case there's no MIME specified for this file, we attempt to return the default MIME which is 'application/octet-stream'.
 * @param file_extension Extension whose MIME is required.
 * @return MIME for the file extension specified.
 */
std::string MIMETypesLoader::get_mime_type(std::string file_extension) {
    if(extension_to_mime.count(file_extension)) {
        return extension_to_mime[file_extension];
    }
    return default_mime_type;
}