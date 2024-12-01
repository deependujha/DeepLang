#pragma once
#include <string>

class BaseToken {
  protected:
    std::string token;
    std::string value;
    float value_float;

  public:
    BaseToken(
        std::string token,
        std::string value = "",
        float value_float = 0.0f);

    std::string getToken();
    std::string getValue();
    float getValueFloat();
};
