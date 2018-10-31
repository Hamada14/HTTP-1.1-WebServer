#include "../headers/Util.h"

#include <iostream>
#include <sstream>

namespace Util {
    bool isWhiteSpace(const char c) {
        return c == ' ' || c == '\t';
    }

    std::string trim(const std::string str) {
        int start, end;
        for(start = 0; start < (int)str.length() && isWhiteSpace(str[start]); start++) ;
        for(end = (int)str.length() - 1; end >= 0 && isWhiteSpace(str[end]); end--) ;
        if(end < start) return EMPTY_STRING;
        return str.substr(start, end - start + 1);
    }

    std::vector<std::string> split(std::string str, char delimiter) {
        std::vector<std::string> result;
        for(size_t i = 0; i < str.length(); i++) {
            if(str[i] == delimiter)
                continue;
            int j = i;
            for(j = i; j < (int)str.length() && str[j] != delimiter; j++) ;
            result.push_back(str.substr(i, j - i));
            i = j - 1;
        }
        return result;
    }

    std::string join_path(std::string s1, std::string s2) {
        s1 = s1[s1.length() - 1] == PATH_SEPARATOR ? s1.substr(0, s1.length() - 1) : s1;
        s2 = s2[0] == PATH_SEPARATOR ? s2.substr(1, s2.length() - 1) : s2;
        return s1 + PATH_SEPARATOR + s2;
    }
}