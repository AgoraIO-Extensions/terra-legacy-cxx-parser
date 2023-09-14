#pragma once

#include "../base_syntax_render.hpp"

class TsSyntaxRender : public BaseSyntaxRender {
protected:
  std::map<std::string, std::string> kTypeMap = {
      {"char", "number"},          {"unsigned int", "number"},
      {"size_t", "number"},        {"unsigned short", "number"},
      {"float", "number"},         {"double", "number"},
      {"int64_t", "number"},       {"int32_t", "number"},
      {"long", "number"},          {"int16_t", "number"},
      {"unsigned char", "number"}, {"uint8_t", "number"},
      {"uint32_t", "number"},      {"uint64_t", "number"},
      {"uint16_t", "number"},      {"long long", "number"},
      {"int", "number"},           {"bool", "boolean"},
      {"intptr_t", "number"},      {"uintptr_t", "number"},
  };

  std::map<std::string, std::string> kTypeDefMap = {
      {"uid_t", "number"},
      {"track_id_t", "number"},
      {"video_track_id_t", "number"},
      {"conn_id_t", "number"},
      {"view_t", "any"},
      {"AString", "string"},
      {"user_id_t", "string"},
  };

  std::vector<std::string> kHideTypeMap{"agora_refptr", "Optional"};

protected:
  SyntaxRender::RenderedBlock RenderTypeName(const std::string &type_name) {
    std::string type = TrimNamespaces(type_name);
    if (kTypeDefMap.find(type) != kTypeDefMap.end()) {
      type = kTypeDefMap[type];
    }

    if (kTypeMap.find(type) != kTypeMap.end()) {
      type = kTypeMap[type];
    }

    if (type.find('_') != std::string::npos) {
      type = NameWithUnderscoresToCamelCase(type, true);
    }

    // SIZE -> Size
    if (IsUpper(type)) {
      type = ToLower(type);
      type[0] = std::toupper(type[0]);
    }

    return {.rendered_content = type};
  }

  SyntaxRender::RenderedBlock
  RenderNonTypeName(const std::string &non_type_name,
                    bool upper_camel_case = false) {
    std::string type = non_type_name;
    if (type.find('_') != std::string::npos) {
      type = NameWithUnderscoresToCamelCase(type, upper_camel_case);
    }

    if (IsUpper(type)) {
      type = ToLower(type);
    }
    if (upper_camel_case) {
      type[0] = std::toupper(type[0]);
    } else {
      type[0] = std::tolower(type[0]);
    }

    return {.rendered_content = type};
  }

  SyntaxRender::RenderedBlock RenderSimpleType(const SimpleType &simple_type) {
    std::string type = simple_type.name;

    if (type.empty()) {
      type = simple_type.source;

      for (auto &dummy : kHideTypeMap) {
        Replace(type, dummy, "");
        Replace(type, "<", "");
        Replace(type, ">", "");
      }
    }

    switch (simple_type.kind) {
    case value_t:
      type = RenderTypeName(type).rendered_content;
      break;
    case pointer_t:
      if (type == "char") {
        if (EndWith(simple_type.source, "**")) {
          type = "string[]";
        } else {
          type = "string";
        }
      } else if (type == "uint8_t" || type == "unsigned char") {
        type = "Uint8Array";
      } else if (type == "void") {
        type = "any";
      } else {
        type = RenderTypeName(type).rendered_content;
      }
      break;
    case reference_t:
      type = RenderTypeName(type).rendered_content;
      break;
    case array_t:
      if (type == "char") {
        if (EndWith(simple_type.source, "**")) {
          type = "string[]";
        } else {
          type = "string";
        }
      } else if (type == "void") {
        type = "Uint8Array";
      } else {
        type = RenderTypeName(type).rendered_content + "[]";
      }
      break;
    default:
      if (type == "const char *") {
        type = "string";
      } else {
        type = RenderTypeName(type).rendered_content;
      }
      break;
    }

    return {.rendered_content = type};
  }

  SyntaxRender::RenderedBlock
  RenderVariable(const Variable &variable, bool render_default_value = false) {
    std::string variable_name =
        RenderNonTypeName(variable.name).rendered_content;
    std::string type_name = RenderSimpleType(variable.type).rendered_content;

    std::string member = variable_name + ": " + type_name;

    if (render_default_value && !variable.default_value.empty()) {
      if (variable.default_value == "__null" ||
          variable.default_value == "nullptr") {
        member = variable_name + "?: " + type_name;
      } else if (variable.type.is_builtin_type) {
        member += " = " + variable.default_value;
      } else {
        bool flag = false;

        Struct s;
        if (FindStruct(variable.type.name, s) == 0) {
          flag = true;
          member +=
              " = new " +
              RenderNonTypeName(TrimNamespaces(variable.default_value), true)
                  .rendered_content;
        }

        Enumz e;
        if (FindEnumz(variable.type.name, e) == 0) {
          flag = true;
          member +=
              " = " + type_name + "." +
              RenderNonTypeName(TrimNamespaces(variable.default_value), true)
                  .rendered_content;
        }

        if (!flag) {
          member += " = " + variable.default_value;
        }
      }
    }

    return {.rendered_content = member};
  }

public:
  bool ShouldRender(const CXXFile &file) override { return true; }

  RenderedBlock RenderedFileName(const std::string &file_path) override {
    return {};
  }

  SyntaxRender::RenderedBlock RenderIncludeDirectives(
      const CXXFile &file,
      const std::vector<IncludeDirective> &include_directives) override {
    return {};
  }

  SyntaxRender::RenderedBlock
  RenderStructConstructor(const Struct &structt,
                          const Constructor &constructor) override {
    return {};
  }

  SyntaxRender::RenderedBlock
  RenderClassConstructor(const Clazz &clazz,
                         const Constructor &constructor) override {
    return {};
  }

  SyntaxRender::RenderedBlock
  RenderMemberVariable(const NodeType &parent,
                       const MemberVariable &member_variable) override {
    std::string member =
        member_variable.name +
        "?: " + RenderSimpleType(member_variable.type).rendered_content + ";";

    return {.rendered_content = member};
  }

  SyntaxRender::RenderedBlock
  RenderMemberFunction(const NodeType &parent,
                       const MemberFunction &member_function) override {
    return {};
  }

  SyntaxRender::RenderedBlock
  RenderClass(const Clazz &original_clazz,
              const std::vector<SyntaxRender::RenderedBlock>
                  &rendered_class_members) override {
    return {};
  }

  RenderedBlock RenderStruct(
      const Struct &original_struct,
      const std::vector<RenderedBlock> &rendered_struct_members) override {
    return {};
  }

  SyntaxRender::RenderedBlock
  RenderEnumConstant(const EnumConstant &enum_const) override {
    return {};
  }

  SyntaxRender::RenderedBlock
  RenderEnum(const Enumz &enumz,
             const std::vector<RenderedBlock> &rendered_enum_consts) override {
    return {};
  }

  void FormatCode(const std::string &file_path) override {}
};
