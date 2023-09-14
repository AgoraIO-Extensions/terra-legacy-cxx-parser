#pragma once

#include "ts_syntax_render.hpp"

class TsInterfaceRender : public TsSyntaxRender {
public:
  static bool isCallback(const std::string &name) {
    std::smatch output;
    std::regex pattern = std::regex(".*(Observer|Handler|Callback|Receiver|Sink).*");
    return std::regex_match(name, output, pattern);
  }

public:
  bool ShouldRender(const CXXFile &file) override {
    return std::find_if_not(
               file.nodes.begin(), file.nodes.end(), [&](const auto &node) {
                 return std::holds_alternative<IncludeDirective>(node) ||
                        std::holds_alternative<TypeAlias>(node);
               }) != file.nodes.end();
  }

  RenderedBlock RenderedFileName(const std::string &file_path) override {
    std::string file_name = std::filesystem::path(file_path).stem();

    return {.rendered_content = std::string(file_name + ".ts")};
  }

  SyntaxRender::RenderedBlock RenderIncludeDirectives(
      const CXXFile &file,
      const std::vector<IncludeDirective> &include_directives) override {
    std::string file_name = std::filesystem::path(file.file_path).stem();

    std::string include;
    include += "import './extension/" + file_name + "Extension';";

    return {.rendered_content = include};
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
    std::string parent_name;
    if (std::holds_alternative<Struct>(parent)) {
      parent_name =
          RenderTypeName(std::get<Struct>(parent).name).rendered_content;
    } else if (std::holds_alternative<Clazz>(parent)) {
      parent_name =
          RenderTypeName(std::get<Clazz>(parent).name).rendered_content;
    }
    std::string variable_name = member_variable.name;
    std::string variable;

    variable +=
        "  /* class_" + ToLower(parent_name) + "_" + variable_name + " */\n";
    variable += "  " + variable_name +
                "?: " + RenderSimpleType(member_variable.type).rendered_content;

    return {.rendered_content = variable};
  }

  SyntaxRender::RenderedBlock
  RenderMemberFunction(const NodeType &parent,
                       const MemberFunction &member_function) override {
    std::string class_name = member_function.parent_name;
    std::string function_name = member_function.name;
    std::string function;

    if (isCallback(class_name)) {
      // 回调类
      function += "  /* callback_" +
                  ToLower(RenderTypeName(class_name).rendered_content) + "_" +
                  ToLower(RenderTypeName(function_name).rendered_content) +
                  " */\n";
      function +=
          "  " + RenderNonTypeName(function_name).rendered_content + "?(";
    } else {
      // 非回调类
      function += "/* api_" +
                  ToLower(RenderTypeName(class_name).rendered_content) + "_" +
                  ToLower(RenderTypeName(function_name).rendered_content) +
                  " */\n";
      function +=
          "abstract " + RenderNonTypeName(function_name).rendered_content + "(";
    }

    std::vector<std::string> output_params;
    std::vector<std::string> params;
    for (auto &param : member_function.parameters) {
      if (param.is_output) {
        // 过滤输出参数
        output_params.push_back(RenderNonTypeName(param.name).rendered_content +
                                ": " +
                                RenderSimpleType(param.type).rendered_content);
        continue;
      }
      if (param.default_value.empty()) {
        params.push_back(RenderVariable(param, false).rendered_content);
      } else {
        params.push_back(RenderNonTypeName(param.name).rendered_content +
                         "?: " + RenderSimpleType(param.type).rendered_content);
      }
    }
    if (!output_params.empty()) {
      if (member_function.return_type.name != "void" &&
          member_function.return_type.name != "int") {
        output_params.push_back(
            "result: " +
            RenderSimpleType(member_function.return_type).rendered_content);
      }
    }
    function += JoinToString(params, ", ");
    function += "): ";
    if (output_params.capacity() > 1) {
      function += "{ " + JoinToString(output_params, ", ") + " }";
    } else if (output_params.capacity() == 1) {
      function += Split(output_params[0], ": ")[1];
    } else {
      function +=
          RenderSimpleType(member_function.return_type).rendered_content;
    }
    function += ";\n";

    return {.rendered_content = function};
  }

  SyntaxRender::RenderedBlock
  RenderClass(const Clazz &original_clazz,
              const std::vector<SyntaxRender::RenderedBlock>
                  &rendered_class_members) override {
    std::string class_name =
        RenderTypeName(original_clazz.name).rendered_content;
    std::string class_;

    class_ += "/* class_" + ToLower(class_name) + " */\n";
    if (isCallback(class_name)) {
      // 回调类
      class_ += "export interface " + class_name;
    } else {
      // 非回调类
      class_ += "export abstract class " + class_name;
    }
    if (!original_clazz.base_clazzs.empty()) {
      class_ += " extends " + JoinToString(original_clazz.base_clazzs, ",");
    }
    class_ += " {\n";
    class_ +=
        JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
    class_ += "}\n";

    return {.rendered_content = class_};
  }

  RenderedBlock RenderStruct(
      const Struct &original_struct,
      const std::vector<RenderedBlock> &rendered_struct_members) override {
    std::string struct_name =
        RenderTypeName(original_struct.name).rendered_content;
    std::string struct_;

    struct_ += "/* class_" + ToLower(struct_name) + " */\n";
    struct_ += "export class " + struct_name + " {\n";
    struct_ += JoinToString(RenderedBlocksToStringList(rendered_struct_members),
                            "\n") +
               "\n";
    //    struct_ += "\n";
    //    struct_ += "static fromJSON(json: any): " + struct_name + " {\n";
    //    struct_ += "const obj = new " + struct_name + "();\n";
    //    for (auto &variable : original_struct.member_variables) {
    //      auto simple_type_name =
    //      RenderSimpleType(variable.type).rendered_content; Struct s; if
    //      (FindStruct(variable.type.name, s) == 0) {
    //        if (variable.type.kind == array_t) {
    //          struct_ += "obj." +
    //                     RenderNonTypeName(variable.name).rendered_content + "
    //                     = " + "json." + variable.name + "?.map((it: any) => "
    //                     + simple_type_name.erase(simple_type_name.size() - 2,
    //                     2) +
    //                     ".fromJSON(it));\n";
    //        } else {
    //          struct_ += "obj." +
    //                     RenderNonTypeName(variable.name).rendered_content + "
    //                     = " + simple_type_name + ".fromJSON(json." +
    //                     variable.name +
    //                     ");\n";
    //        }
    //      } else {
    //        struct_ += "obj." +
    //        RenderNonTypeName(variable.name).rendered_content +
    //                   " = " + "json." + variable.name + ";\n";
    //      }
    //    }
    //    struct_ += "return obj;\n";
    //    struct_ += "}\n";
    //    struct_ += "\n";
    //    struct_ += "toJSON?() {\n";
    //    struct_ += "return {\n";
    //    for (auto &variable : original_struct.member_variables) {
    //      if ((!variable.type.is_builtin_type && variable.type.kind != value_t
    //      &&
    //           kTypeDefMap.find(TrimNamespaces(variable.type.name)) ==
    //               kTypeDefMap.end()) ||
    //          variable.type.name == "void") {
    //        Struct s;
    //        if (FindStruct(variable.type.name, s) != 0) {
    //          std::cout << "ignore struct field: " << original_struct.name <<
    //          " "
    //                    << variable.name << " " << variable.type.name << "\n";
    //          continue;
    //        }
    //      }
    //      struct_ += "'" + variable.name + "': " + "this." +
    //                 RenderNonTypeName(variable.name).rendered_content +
    //                 ",\n";
    //    }
    //    struct_ += "};\n";
    //    struct_ += "}\n";
    struct_ += "}\n";

    return {.rendered_content = struct_};
  }

  SyntaxRender::RenderedBlock
  RenderEnumConstant(const EnumConstant &enum_const) override {
    std::string enum_name =
        RenderTypeName(enum_const.parent_name).rendered_content;
    std::string constant_name =
        RenderNonTypeName(enum_const.name, true).rendered_content;
    std::string enum_constant;

    enum_constant +=
        "/* enum_" + ToLower(enum_name) + "_" + constant_name + " */\n";
    std::string constant_value = enum_const.value;
    if (constant_value.empty()) {
      constant_value = "-1";
    } else {
      if (constant_value.find("<<") == std::string::npos &&
          std::find_if(constant_value.begin(), constant_value.end(),
                       [](unsigned char c) { return !std::isdigit(c); }) !=
              constant_value.end()) {
        constant_value = RenderNonTypeName(constant_value).rendered_content;
      }
    }
    enum_constant += constant_name + " = " + constant_value;

    return {.rendered_content = enum_constant + ","};
  }

  SyntaxRender::RenderedBlock
  RenderEnum(const Enumz &enumz,
             const std::vector<RenderedBlock> &rendered_enum_consts) override {
    std::string enum_name = RenderTypeName(enumz.name).rendered_content;
    std::string enum_;

    if (enum_name.empty()) {
      // TODO print log
      SyntaxRender::RenderedBlock rendered_block;
      return rendered_block;
    }

    enum_ += "/* enum_" + ToLower(enum_name) + " */\n";
    enum_ += "export enum " + enum_name + " {\n";
    enum_ +=
        JoinToString(RenderedBlocksToStringList(rendered_enum_consts), "\n");
    enum_ += "\n";
    enum_ += "}\n";

    return {.rendered_content = enum_};
  }

  void FormatCode(const std::string &file_path) override {
    {
      //      std::string command = "eslint " + file_path + " --fix";
      //      system(command.c_str());
    }
  }
};
