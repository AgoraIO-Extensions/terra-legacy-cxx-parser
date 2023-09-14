//
// Created by LXH on 2022/5/17.
//

#ifndef AGORA_RTC_AST_ENUM_VALUE_PARSER_HPP
#define AGORA_RTC_AST_ENUM_VALUE_PARSER_HPP

#include "../renders/base_syntax_render.hpp"
#include "terra_parser.hpp"
#include <initializer_list>
#include <iterator>
#include <variant>
#include <vector>

using namespace terra;

int parseEnumValue(std::map<std::string, std::string> &parsedEnums,
                   std::string &value) {
  int enumValue;
  try {
    int base = 10;
    if (parsedEnums[value].rfind("0x", 0) == 0) {
      base = 16;
    } else if (parsedEnums[value].rfind('0', 0) == 0) {
      base = 8;
    }
    enumValue = std::stoi(parsedEnums[value], nullptr, base);
  } catch (std::invalid_argument &) {
    if (value == "sizeof(int16_t)") {
      enumValue = sizeof(int16_t);
    }
  }
  return enumValue;
}

bool parseEnumValueWithOperator(std::map<std::string, std::string> &parsedEnums,
                                std::string &value,
                                const std::string &operator_str) {
  if (value.find(operator_str) != std::string::npos) {
    auto tmpValue = BaseSyntaxRender::Split(value, operator_str);
    if (operator_str == "*") {
      auto firstValue = parseEnumValue(parsedEnums, tmpValue[0]);
      auto secondValue = parseEnumValue(parsedEnums, tmpValue[1]);
      value = std::to_string(firstValue * secondValue);
      return true;
    } else {
      value = "";
      int i = 0;
      for (auto &it : tmpValue) {
        value += parsedEnums[it];
        if (i++ >= 0 && i < tmpValue.size()) {
          value += operator_str;
        }
      }
    }
  }
  return false;
}

class EnumValueParser : public Parser {
public:
  bool Parse(const ParseConfig &parse_config,
             ParseResult &parse_result) override {
    for (auto &f : parse_result.cxx_files) {
      // FilterClass(f.nodes);
      for (auto &node : f.nodes) {
        if (std::holds_alternative<Enumz>(node)) {
          Enumz &enumz = std::get<Enumz>(node);
          std::map<std::string, std::string> parsedEnums;
          int lastEnumValue = -1;
          for (EnumConstant &it : enumz.enum_constants) {
            if (it.value.empty()) {
              // 当前枚举值为空，需要Parser赋值
              // QUALITY_UNSUPPORTED = 7,
              // QUALITY_DETECTING
              it.value = std::to_string(++lastEnumValue);
            } else if (parsedEnums.find(it.value) != parsedEnums.end()) {
              // 当前枚举值等于之前的枚举值
              // VIDEO_SOURCE_CAMERA_PRIMARY,
              // VIDEO_SOURCE_CAMERA = VIDEO_SOURCE_CAMERA_PRIMARY,
              it.value = parsedEnums[it.value];
            }
            parsedEnums.emplace(std::make_pair(it.name, it.value));
            try {
              lastEnumValue = std::stoi(it.value);
            } catch (std::invalid_argument &) {
              for (auto &item :
                   std::initializer_list<std::string>{"|", "&", "*"}) {
                if (parseEnumValueWithOperator(parsedEnums, it.value, item)) {
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
};

#endif // AGORA_RTC_AST_ENUM_VALUE_PARSER_HPP
