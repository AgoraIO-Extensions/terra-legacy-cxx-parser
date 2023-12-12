#ifndef BASE_SYNTAX_RENDER_H_
#define BASE_SYNTAX_RENDER_H_

#include "terra.hpp"
#include "terra_generator.hpp"
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

using namespace terra;

class BaseSyntaxRender : public SyntaxRender {
 public:
  static std::string
  NameWithNamespace(const std::string &name,
                    const std::vector<std::string> &namespace_list) {
    std::ostringstream vts;
    std::copy(namespace_list.begin(), namespace_list.end() - 1,
              std::ostream_iterator<std::string>(vts, "::"));

    vts << namespace_list.back();

    std::string ns_prefix = vts.str();
    if (!ns_prefix.empty()) { ns_prefix += "::"; }

    std::string name_with_ns = ns_prefix + name;

    return name_with_ns;
  }

  static std::string JoinToString(const std::vector<std::string> &list,
                                  const std::string &delimelater) {
    if (list.empty()) return "";

    std::ostringstream vts;
    std::copy(list.begin(), list.end() - 1,
              std::ostream_iterator<std::string>(vts, delimelater.c_str()));
    vts << list.back();
    return vts.str();
  }

  static std::vector<std::string> RenderedBlocksToStringList(
      const std::vector<SyntaxRender::RenderedBlock> &rendered_blocks) {
    std::vector<std::string> result;
    for (auto &r : rendered_blocks) {
      if (!r.rendered_content.empty()) result.push_back(r.rendered_content);
    }

    return result;
  }

  static std::vector<std::string> Split(const std::string &source,
                                        const std::string &delimelater) {

    std::vector<std::string> result;
    if (source.empty()) return result;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = 0;
    while ((pos2 = source.find(delimelater, pos1)) != std::string::npos) {
      result.push_back(source.substr(pos1, pos2 - pos1));
      pos1 = pos2 + delimelater.size();
    }
    result.push_back(source.substr(pos1));

    return result;
  }

  static std::string TrimNamespaces(const std::string &type) {
    if (type.empty()) return "";
    return Split(type, "::").back();
  }

  static std::string ToLower(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return result;
  }

  bool IsUpper(const std::string &s) {
    return std::all_of(s.begin(), s.end(),
                       [](unsigned char c) { return std::isupper(c); });
  }

  static std::string ToUpper(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    return result;
  }

  /// e.g., VIDEO_SOURCE_TYPE -> VideoSourceType
  std::string
  NameWithUnderscoresToCamelCase(const std::string &name_with_underscores,
                                 bool upper_camel_case = false) {
    std::string name_with_underscores_lower = ToLower(name_with_underscores);
    std::vector<std::string> words = Split(name_with_underscores_lower, "_");
    for (int i = 0; i < words.size(); i++) {
      auto &word = words[i];

      if ((i == 0 && upper_camel_case) || i != 0) {
        word[0] = std::toupper(word[0]);
      }
    }

    std::string result = JoinToString(words, "");

    return result;
  }

  std::string UpperCamelCaseToLowercaseWithUnderscores(
      const std::string &upper_camel_case_name) {
    std::vector<std::string> result;

    std::string to_search = upper_camel_case_name;

    const std::regex base_regex(R"((I[A-Z]|[A-Z])?[a-z0-9]*)");
    std::smatch base_match;

    std::sregex_iterator iter(to_search.begin(), to_search.end(), base_regex);
    std::sregex_iterator end;

    while (iter != end) {
      std::string tmp = ToLower(std::string((*iter)[0]));
      if (!tmp.empty()) { result.push_back(tmp); }

      ++iter;
    }

    return JoinToString(result, "_");
  }

  static bool Contains(std::string &str, const std::string &contain_str) {
    size_t start_pos = str.find(contain_str);
    return start_pos != std::string::npos;
  }

  static bool Replace(std::string &str, const std::string &from,
                      const std::string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) return false;
    str.replace(start_pos, from.length(), to);
    return true;
  }

  static std::string ReplaceAll(std::string str, const std::string &from,
                         const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      // Handles case where 'to' is a substring of 'from'
      start_pos += to.length();
    }
    return str;
  }

  static bool EndWith(std::string const &fullString,
                      std::string const &ending) {
    if (fullString.length() >= ending.length()) {
      return (0
              == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
    } else {
      return false;
    }
  }

  int FindStruct(const std::string &name, Struct &out) {
    if (name.empty()) { return -1; }

    std::vector<std::string> namespace_list = Split(name, "::");
    namespace_list = std::vector<std::string>(namespace_list.begin(),
                                              namespace_list.end() - 1);
    std::string namespace_string = JoinToString(namespace_list, "::");

    std::string trim_namespaces_name = TrimNamespaces(name);

    for (auto &cxx_file : GetParseResult().cxx_files) {
      for (auto &node : cxx_file.nodes) {
        if (std::holds_alternative<Struct>(node)) {
          Struct s = std::get<Struct>(node);

          if (s.name == trim_namespaces_name) {
            out = s;
            return 0;

            std::ostringstream vts;
            std::copy(s.namespaces.begin(), s.namespaces.end() - 1,
                      std::ostream_iterator<std::string>(vts, "::"));

            vts << s.namespaces.back();

            if (vts.str() == namespace_string || namespace_list.empty()
                || std::includes(s.namespaces.begin(), s.namespaces.end(),
                                 namespace_list.begin(),
                                 namespace_list.end())) {
              out = s;
              return 0;
            }
          }
        }
      }
    }
    return -1;
  }

  int FindClass(const std::string &name, Clazz &out) {
    if (name.empty()) { return -1; }

    std::vector<std::string> namespace_list = Split(name, "::");
    namespace_list = std::vector<std::string>(namespace_list.begin(),
                                              namespace_list.end() - 1);
    std::string namespace_string = JoinToString(namespace_list, "::");

    std::string trim_namespaces_name = TrimNamespaces(name);

    for (auto &cxx_file : GetParseResult().cxx_files) {
      for (auto &node : cxx_file.nodes) {
        if (std::holds_alternative<Clazz>(node)) {
          Clazz clazz = std::get<Clazz>(node);

          if (clazz.name == trim_namespaces_name) {
            std::ostringstream vts;
            std::copy(clazz.namespaces.begin(), clazz.namespaces.end() - 1,
                      std::ostream_iterator<std::string>(vts, "::"));

            vts << clazz.namespaces.back();

            if (vts.str() == namespace_string || namespace_list.empty()
                || std::includes(clazz.namespaces.begin(),
                                 clazz.namespaces.end(), namespace_list.begin(),
                                 namespace_list.end())) {
              out = clazz;
              return 0;
            }
          }
        }
      }
    }
    return -1;
  }

  int FindEnumz(const std::string &name, Enumz &out) {
    if (name.empty()) { return -1; }

    std::vector<std::string> namespace_list = Split(name, "::");
    namespace_list = std::vector<std::string>(namespace_list.begin(),
                                              namespace_list.end() - 1);
    std::string namespace_string = JoinToString(namespace_list, "::");

    std::string trim_namespaces_name = TrimNamespaces(name);

    for (auto &cxx_file : GetParseResult().cxx_files) {
      for (auto &node : cxx_file.nodes) {
        if (std::holds_alternative<Enumz>(node)) {
          Enumz enumz = std::get<Enumz>(node);

          if (enumz.name == trim_namespaces_name) {
            std::ostringstream vts;
            std::copy(enumz.namespaces.begin(), enumz.namespaces.end() - 1,
                      std::ostream_iterator<std::string>(vts, "::"));

            vts << enumz.namespaces.back();

            if (vts.str() == namespace_string || namespace_list.empty()
                || std::includes(enumz.namespaces.begin(),
                                 enumz.namespaces.end(), namespace_list.begin(),
                                 namespace_list.end())) {
              out = enumz;
              return 0;
            }
          }
        }
      }
    }
    return -1;
  }

  std::string ApiTagClass(const Clazz &clazz) {
    return "/* class_" + ToLower(NameWithUnderscoresToCamelCase(clazz.name))
        + " */";
  }

  std::string ApiTagStruct(const Struct &structt) {
    return "/* class_" + ToLower(structt.name) + " */";
  }

  std::string ApiTagMemberFunction(const NodeType &parent,
                                   const MemberFunction &member_function,
                                   bool is_event = false) {
    std::string prefix = "";
    if (is_event) {
      prefix = "callback_";
    } else {
      prefix = "api_";
    }

    if (std::holds_alternative<Clazz>(parent)) {
      Clazz clazz = std::get<Clazz>(parent);

      return "/* " + prefix
          + ToLower(NameWithUnderscoresToCamelCase(clazz.name)) + "_"
          + ToLower(NameWithUnderscoresToCamelCase(member_function.name))
          + " */";
    }

    if (std::holds_alternative<Struct>(parent)) {
      Struct structt = std::get<Struct>(parent);
      auto tag = "/* " + prefix + NameWithUnderscoresToCamelCase(structt.name)
          + "_" + NameWithUnderscoresToCamelCase(member_function.name) + " */";
      return ToLower(tag);
    }

    return "";
  }

  std::string ApiTagMemberVariable(const NodeType &parent,
                                   const MemberVariable &member_variable) {
    if (std::holds_alternative<Clazz>(parent)) {
      Clazz clazz = std::get<Clazz>(parent);
      auto tag = "/* property_"
          + NameWithUnderscoresToCamelCase(clazz.name, true) + "_"
          + NameWithUnderscoresToCamelCase(member_variable.name) + " */";
      return ToLower(tag);
    }

    if (std::holds_alternative<Struct>(parent)) {
      Struct structt = std::get<Struct>(parent);
      auto tag = "/* property_"
          + NameWithUnderscoresToCamelCase(structt.name, true) + "_"
          + NameWithUnderscoresToCamelCase(member_variable.name) + " */";
      return ToLower(tag);
    }

    return "";
  }

  std::string ApiTagEnum(const Enumz &enumz) {
    auto tag = "/* enum_" + NameWithUnderscoresToCamelCase(enumz.name) + " */";
    return ToLower(tag);
  }

  std::string ApiTagEnumConstant(const std::string &parent_name,
                                 const EnumConstant &enum_constant) {
    auto tag = "/* property_"
        + NameWithUnderscoresToCamelCase(parent_name, true) + "_"
        + NameWithUnderscoresToCamelCase(enum_constant.name) + " */";
    return ToLower(tag);
  }

  bool IsUseIrisApiType(const std::string &file_path) {
    // TODO(littlegnal): Temporary put the config here,
    std::vector<std::string> use_iris_api_type_files{
        "IAgoraH265Transcoder.h",
    };

    bool is_use_iris_api_type = false;
    std::filesystem::path my_file_path(file_path);
    if (std::find(use_iris_api_type_files.begin(),
                  use_iris_api_type_files.end(), my_file_path.filename())
        != use_iris_api_type_files.end()) {

      is_use_iris_api_type = true;
    }

    return is_use_iris_api_type;
  }

  static std::string IrisApiType(const Clazz &clazz,
                                 const MemberFunction &member_func,
                                 bool includeClassName = true) {
    std::string ptrEscape = "ptr";
    std::string refEscape = "ref";
    std::string whitespaceEscape = "_";
    std::string seperator = "__";

    std::vector<std::string> outList;

    for (auto &param : member_func.parameters) {
      std::string out = param.type.name;
      out = ReplaceAll(out, "::", whitespaceEscape);

      if (param.type.is_const) { out = "const" + whitespaceEscape + out; }

      if (param.type.kind == SimpleTypeKind::pointer_t) {
        out += (whitespaceEscape + ptrEscape);
      } else if (param.type.kind == SimpleTypeKind::reference_t) {
        out += (whitespaceEscape + refEscape);
      }

      outList.push_back(out);
    }

    std::string ps = JoinToString(outList, seperator);

    std::string apiType = "";

    if (includeClassName) { apiType += TrimNamespaces(clazz.name) + seperator; }

    apiType += member_func.name + seperator + ps;

    return apiType;
  }
};

#endif// BASE_SYNTAX_RENDER_H_