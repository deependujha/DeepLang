#include "utils.h"
#include <string>

bool isStrFloat(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    for (auto& e : s) {
        if ((e < '0' || e > '9') && e != '.') {
            return false;
        }
    }

    return true;
}
