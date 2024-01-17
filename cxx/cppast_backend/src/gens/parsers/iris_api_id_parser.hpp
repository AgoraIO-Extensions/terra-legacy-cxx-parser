#ifndef IRIS_API_ID_PARSER_HPP
#define IRIS_API_ID_PARSER_HPP

#include "../renders/base_syntax_render.hpp"
#include "terra_parser.hpp"
#include <iomanip>
#include <iterator>
#include <map>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

class IrisApiIdParser : public Parser {
 private:
  std::string StringHashCode(const std::string &source) {
    if (source.empty()) { return ""; }

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(source.c_str()), source.size(),
         hash);

    std::stringstream ss;
    for (int i = 0; i < 4; ++i) {// 只迭代4次以获取8个字符
      ss << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(hash[i]);
    }
    return ss.str().substr(0, 7);// 截取前7个字符
  }

  std::string IrisApiId(const Clazz &clazz, const MemberFunction &member_func,
                        bool includeClassName = true, bool isUppercase = true) {
    std::string ptrEscape = "ptr";
    std::string refEscape = "ref";
    std::string shortSeperator = "_";
    std::string seperator = "__";

    std::string class_name = clazz.name;
    std::string func_name = member_func.name;
    if (isUppercase) {
      BaseSyntaxRender::ToUpper(class_name);
      BaseSyntaxRender::ToUpper(func_name);
    }

    std::string apiType = "";

    if (includeClassName) {
      apiType += BaseSyntaxRender::TrimNamespaces(clazz.name) + seperator;
    }

    apiType += member_func.name;

    std::vector<std::string> outList;
    for (auto &param : member_func.parameters) {
      std::string out = param.type.source;

      outList.push_back(out);
    }

    std::string ps = BaseSyntaxRender::JoinToString(outList, seperator);
    std::string hc = StringHashCode(ps);
    if (!hc.empty()) { apiType += shortSeperator + hc; }

    return apiType;
  }

 public:
  bool Parse(const ParseConfig &parse_config, ParseResult &parse_result) {
    for (auto &f : parse_result.cxx_files) {
      for (auto &node : f.nodes) {
        if (std::holds_alternative<Clazz>(node)) {
          Clazz &clazz = std::get<Clazz>(node);

          std::string class_name = clazz.name;

          std::string class_with_ns =
              BaseSyntaxRender::NameWithNamespace(clazz.name, clazz.namespaces);

          for (auto &func : clazz.methods) {
            std::string appIdKey = IrisApiId(clazz, func);
            std::string appIdValue = IrisApiId(clazz, func, false, false);

            // Since the `signature` is not be used in the legacy cxx-parser, we reuse it to store the iris api id
            func.signature = appIdValue;

            std::cout << "[IrisApiIdParser] appIdValue: " << appIdValue << "\n";
          }
        }
      }
    }

    return true;
  }
};

#endif// IRIS_API_ID_PARSER_HPP
