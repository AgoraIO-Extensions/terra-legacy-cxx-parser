//
// Created by LXH on 2022/6/7.
//

#ifndef AGORA_RTC_AST_CUSTOM_NODE_PARSER_HPP
#define AGORA_RTC_AST_CUSTOM_NODE_PARSER_HPP

#include "../renders/base_syntax_render.hpp"
#include "terra.hpp"
#include <iterator>
#include <variant>
#include <vector>

using namespace terra;

class CustomNodeParser : public Parser {
public:
  explicit CustomNodeParser(const std::vector<std::string> &dirs,
                            const std::vector<std::string> &files,
                            std::map<std::string, std::string> defines) {
    rootVisitor.Visit({dirs, files, defines});
  }

  bool Parse(const ParseConfig &parse_config,
             ParseResult &parse_result) override {
    auto files = rootVisitor.parse_result_.cxx_files;
    for (auto &f : parse_result.cxx_files) {
      auto found_file =
          std::find_if(files.begin(), files.end(), [&](const CXXFile &node) {
            auto found1 = f.file_path.find_last_of('/');
            auto found2 = node.file_path.find_last_of('/');
            if (found1 != std::string::npos && found2 != std::string::npos) {
              std::string path =
                  node.file_path.substr(found2, node.file_path.length());
              BaseSyntaxRender::Replace(path, "Custom", "");
              if (f.file_path.substr(found1, f.file_path.length()) == path) {
                return true;
              }
            }
            return false;
          });
      if (found_file != files.end()) {
        for (auto &it : found_file->nodes) {
          if (std::holds_alternative<Enumz>(it) ||
              std::holds_alternative<Struct>(it)) {
            f.nodes.insert(f.nodes.end(), it);
          } else if (std::holds_alternative<Clazz>(it)) {
            auto &clazz_custom = std::get<Clazz>(it);
            bool find = false;
            for (auto &node : f.nodes) {
              if (std::holds_alternative<Clazz>(node)) {
                auto &clazz = std::get<Clazz>(node);
                if (clazz.name == clazz_custom.name) {
                  find = true;

                  for (auto &method : clazz_custom.methods) {
                    auto found_method =
                        std::find_if(clazz.methods.begin(), clazz.methods.end(),
                                     [&](MemberFunction &item) {
                                       return method.name == item.name &&
                                              !item.user_data.has_value();
                                     });
                    if (found_method == clazz.methods.end()) {
                      found_method = std::find_if(
                          clazz.methods.begin(), clazz.methods.end(),
                          [&](MemberFunction &item) {
                            return method.name.find(item.name) == 0 &&
                                   !item.user_data.has_value();
                          });
                    }
                    method.user_data = found_method;
                    clazz.methods.insert(found_method, method);
                  }

                  // remove overload function
                  for (auto &method : clazz_custom.methods) {
                    clazz.methods.erase(
                        std::remove_if(
                            clazz.methods.begin(), clazz.methods.end(),
                            [&](MemberFunction &item) {
                              return method.name.find(item.name) == 0 &&
                                     !item.user_data.has_value();
                            }),
                        clazz.methods.end());
                  }
                }
              }
            }
            if (!find) {
              f.nodes.push_back(it);
            }
          }
        }
      }
    }
  }

private:
  DefaultVisitor rootVisitor;
};

#endif // AGORA_RTC_AST_CUSTOM_NODE_PARSER_HPP
