#include "utils.h"
#include <string>

bool isStrFloat(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    for (auto& e : s) {
        if (isnumber(e) == false && e != '.') {
            return false;
        }
    }

    return true;
}
