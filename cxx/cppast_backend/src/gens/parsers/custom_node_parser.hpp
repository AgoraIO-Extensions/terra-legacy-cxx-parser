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
                            std::map<std::string, std::string> defines,
                            std::string nativeSdkVersion)
      : nativeSdkVersion_(nativeSdkVersion) {
    rootVisitor.Visit({dirs, files, defines});
  }

  void FillIrisApiId() {
    if (!BaseSyntaxRender::StartsWith(nativeSdkVersion_, "4.3")) { return; }

    for (auto &f : rootVisitor.parse_result_.cxx_files) {
      for (auto &node : f.nodes) {
        if (std::holds_alternative<Clazz>(node)) {
          Clazz &clazz = std::get<Clazz>(node);

          std::string class_name = clazz.name;

          std::string class_with_ns =
              BaseSyntaxRender::NameWithNamespace(clazz.name, clazz.namespaces);

          for (auto &func : clazz.methods) {
            std::string iris_api_id = "";

            std::regex regex("@iris_api_id:\\s*(\\S+)");
            std::smatch matches;

            while (std::regex_search(func.comment, matches, regex)) {
              iris_api_id = matches[1].str();
              std::cout << "[CustomNodeParser] iris_api_id: " << iris_api_id
                        << std::endl;
              break;
            }
            func.comment = "";

            // Trim the class name
            std::vector<std::string> api_id_split =
                BaseSyntaxRender::Split(iris_api_id, "_");
            if (api_id_split.size() > 1) {
              std::vector<std::string> api_id_split_no_class_name(
                  api_id_split.begin() + 1, api_id_split.end());
              std::string appIdValue = BaseSyntaxRender::JoinToString(
                  api_id_split_no_class_name, "_");

              // Since the `signature` is not be used in the legacy cxx-parser, we reuse it to store the iris api id
              func.signature = appIdValue;
            } else {
              func.signature = iris_api_id;
            }

            std::cout << "[CustomNodeParser] appIdValue: " << func.signature
                      << "\n";
          }
        }
      }
    }
  }

  bool Parse(const ParseConfig &parse_config,
             ParseResult &parse_result) override {
    FillIrisApiId();

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
          if (std::holds_alternative<Enumz>(it)
              || std::holds_alternative<Struct>(it)) {
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
                                       return method.name == item.name
                                           && !item.user_data.has_value();
                                     });
                    if (found_method == clazz.methods.end()) {
                      found_method = std::find_if(
                          clazz.methods.begin(), clazz.methods.end(),
                          [&](MemberFunction &item) {
                            return method.name.find(item.name) == 0
                                && !item.user_data.has_value();
                          });
                    }
                    method.user_data = found_method;
                    clazz.methods.insert(found_method, method);
                  }

                  // remove overload function
                  for (auto &method : clazz_custom.methods) {
                    clazz.methods.erase(
                        std::remove_if(clazz.methods.begin(),
                                       clazz.methods.end(),
                                       [&](MemberFunction &item) {
                                         return method.name.find(item.name) == 0
                                             && !item.user_data.has_value();
                                       }),
                        clazz.methods.end());
                  }
                }
              }
            }
            if (!find) { f.nodes.push_back(it); }
          }
        }
      }
    }
  }

 private:
  DefaultVisitor rootVisitor;
  std::string nativeSdkVersion_;
};

#endif// AGORA_RTC_AST_CUSTOM_NODE_PARSER_HPP
