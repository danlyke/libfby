#ifndef STRINGUTIL_H_INCLUDED
#define STRINGUTIL_H_INCLUDED
#include <vector>
#include <sstream>

namespace Fby {
    
    inline std::vector<std::string> &split(char delim, const std::string &s, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    
    inline std::vector<std::string> split(char delim, const std::string &s) {
        std::vector<std::string> elems;
        split(delim, s, elems);
        return elems;
    }

    size_t remove_carriage_returns(char *buffer, size_t length);
    std::string ConvertImageNameToDescription(const std::string &name);
    
    std::string escape_html_entities(const std::string &s);
    
    bool ends_with(const std::string &s, const std::string &with);
    bool starts_with(const std::string &s, const std::string &with);
}

#endif /* #ifndef STRINGUTIL_H_INCLUDED */
