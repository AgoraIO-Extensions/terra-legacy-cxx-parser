//
// Created by LXH on 2022/5/17.
//

#pragma once

#include "../renders/base_syntax_render.hpp"
#include "terra_parser.hpp"
#include <iterator>
#include <variant>
#include <vector>

class PointerArrayParser : public Parser {
private:
  std::map<std::string, std::vector<std::string>> fixed_array_type_ = {
      {"agora::media::AudioSpectrumData", {"audioSpectrumData"}},
      {"agora::rtc::DownlinkNetworkInfo", {"peer_downlink_info"}},
      {"agora::rtc::VideoCanvas", {"priv"}},
  };

public:
  bool Parse(const ParseConfig &parse_config,
             ParseResult &parse_result) override {
    for (auto &f : parse_result.cxx_files) {
      std::map<std::string, NodeType *> nodeMap;
      for (auto &node : f.nodes) {
        if (std::holds_alternative<Struct>(node)) {
          auto &aStruct = std::get<Struct>(node);
          for (auto &it : aStruct.member_variables) {
            if (it.type.kind == terra::pointer_t) {
              {
                // 自定义配置
                std::string struct_with_ns =
                    BaseSyntaxRender::NameWithNamespace(aStruct.name,
                                                        aStruct.namespaces);
                auto find = fixed_array_type_.find(struct_with_ns);
                if (find != fixed_array_type_.end()) {
                  if (std::find(find->second.begin(), find->second.end(),
                                it.name) != find->second.end()) {
                    it.type.kind = terra::array_t;
                    printf("PointerArrayParser %s %s\n", aStruct.name.c_str(),
                           it.name.c_str());
                    continue;
                  }
                }
              }
              {
                // 有包含同名+Count/Size的字段，则为数组
                auto find =
                    std::find_if(aStruct.member_variables.begin(),
                                 aStruct.member_variables.end(),
                                 [&](MemberVariable &variable) {
                                   return variable.name == it.name + "Count" ||
                                          variable.name == it.name + "Size";
                                 });
                if (find != aStruct.member_variables.end()) {
                  it.type.kind = terra::array_t;
                  printf("PointerArrayParser %s %s\n", aStruct.name.c_str(),
                         it.name.c_str());
                  continue;
                }
              }
              {
                // 名字以s、List、Array结尾的，则为数组
                std::regex pattern = std::regex(".*(s|list|array)$");
                std::smatch output;
                std::string name_lower = BaseSyntaxRender::ToLower(it.name);
                if (std::regex_match(name_lower, output, pattern)) {
                  it.type.kind = terra::array_t;
                  printf("PointerArrayParser %s %s\n", aStruct.name.c_str(),
                         it.name.c_str());
                  continue;
                }
              }
              {
                // 名字以buffer、data结尾的，且原类型为void*，则为字节数组
                if (it.type.name == "void") {
                  std::regex pattern = std::regex(".*(buffer|data)$");
                  std::smatch output;
                  std::string name_lower = BaseSyntaxRender::ToLower(it.name);
                  if (std::regex_match(name_lower, output, pattern)) {
                    it.type.name = "uint8_t";
                    it.type.kind = terra::pointer_t;
                    continue;
                  }
                }
              }
            }
          }
        } else if (std::holds_alternative<Clazz>(node)) {
          auto &clazz = std::get<Clazz>(node);
          for (auto &method : clazz.methods) {
            for (auto &it : method.parameters) {
              if (it.type.kind == terra::pointer_t) {
                {
                  // 名字以s、List、Array结尾的，则为数组
                  std::regex pattern = std::regex(".*(list|array)$");
                  std::smatch output;
                  std::string name_lower = BaseSyntaxRender::ToLower(it.name);
                  if (std::regex_match(name_lower, output, pattern)) {
                    it.type.kind = terra::array_t;
                    printf("PointerArrayParser %s.%s %s\n", clazz.name.c_str(),
                           method.name.c_str(), it.name.c_str());
                    continue;
                  }
                }
                {
                  // 名字以buffer、data结尾的，且原类型为void*，则为字节数组
                  if (it.type.name == "void") {
                    std::regex pattern = std::regex(".*(buffer|data)$");
                    std::smatch output;
                    std::string name_lower = BaseSyntaxRender::ToLower(it.name);
                    if (std::regex_match(name_lower, output, pattern)) {
                      it.type.name = "uint8_t";
                      it.type.kind = terra::pointer_t;
                      continue;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
};
