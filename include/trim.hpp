#pragma once
#include <algorithm>
#include <string>

inline std::string trim(std::string haystack, char needle) {
    auto it = std::find(haystack.begin(), haystack.end(), needle);
    if(it == haystack.end())
        return haystack;
    haystack.erase(haystack.begin(), it);
    return haystack;
}

inline std::string rtrim(std::string haystack, char needle) {
    auto it = std::find(haystack.rbegin(), haystack.rend(), needle);
    if(it == haystack.rend())
        return haystack;
    haystack.erase(haystack.begin()+(std::distance(haystack.begin(), it.base())-1), haystack.end());
    return haystack;
}
