#include "check_type.h"
#include <string>

bool isSpace(char c) {
    return c == ' ';
}

bool isSemiColon(char c) {
    return c == ';';
}

bool isNumeric(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (auto& e : s) {
        if (e < '0' || e > '9') {
            return false;
        }
    }
    return true;
}

bool isNumeric(char c) {
    return ((c >= '0' && c <= '9') || c == '.');
}

bool isAlpha(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (auto& e : s) {
        if (!((e >= 'a' && e <= 'z') || (e >= 'A' && e <= 'Z'))) {
            return false;
        }
    }
    return true;
}

bool isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool isAlphaNumeric(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (auto& e : s) {
        if (!((e >= 'a' && e <= 'z') || (e >= 'A' && e <= 'Z') ||
              (e >= '0' && e <= '9'))) {
            return false;
        }
    }
    return true;
}

bool isAlphaNumeric(char c) {
    return (
        (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9'));
}

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
