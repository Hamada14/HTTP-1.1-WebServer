#ifndef Util_hpp
#define Util_hpp

#include <string>
#include <vector>

namespace Util {
    const std::string EMPTY_STRING = "";
    const char PATH_SEPARATOR = '/';

    bool isWhiteSpace(const char);

    std::string trim(const std::string);
    std::vector<std::string> split(std::string, char = ' ');
    std::string join_path(std::string s1, std::string s2);
}
#endif // ifndef Util_hpp
