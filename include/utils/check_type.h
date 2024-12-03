#pragma once
#include <string>

bool isSpace(char c);

bool isSemiColon(char c);

bool isNumeric(char c);
bool isNumeric(const std::string& s);

bool isAlpha(char c);
bool isAlpha(const std::string& s);

bool isAlphaNumeric(char c);
bool isAlphaNumeric(const std::string& s);

bool isStrFloat(const std::string& s);
