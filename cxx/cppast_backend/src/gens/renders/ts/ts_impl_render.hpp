#pragma once

#include "ts_interface_render.hpp"
#include "ts_syntax_render.hpp"

using namespace terra;

class TsImplRender : public TsSyntaxRender {
private:
  SyntaxRender::RenderedBlock
  RenderedApiType(const MemberFunction &member_function) {
    std::string class_name = member_function.parent_name;
    std::string function_name = member_function.name;
    std::string api_type;

    if (class_name.at(0) == 'I') {
      class_name = class_name.substr(1, class_name.size() - 1);
    }

    api_type += class_name + "_" + function_name;

    return {.rendered_content = api_type};
  }

public:
  bool ShouldRender(const CXXFile &file) override {
    return std::find_if(file.nodes.begin(), file.nodes.end(),
                        [&](const auto &node) {
                          return std::holds_alternative<Clazz>(node);
                        }) != file.nodes.end();
  }

  RenderedBlock RenderedFileName(const std::string &file_path) override {
    std::filesystem::path p(file_path);

    std::string base_name = p.stem();

    return {.rendered_content = std::string("impl/" + base_name + "Impl.ts")};
  }

  SyntaxRender::RenderedBlock RenderFileEnd(const CXXFile &file) override {
    std::string include;

    bool flag = false;
    for (auto &node : file.nodes) {
      if (std::holds_alternative<Clazz>(node)) {
        Clazz clazz = std::get<Clazz>(node);
        if (!TsInterfaceRender::isCallback(clazz.name)) {
          flag = true;
        }
      }
    }

    if (flag) {
      include += "import { callIrisApi } from '../internal/IrisApiEngine'";
    }

    return {.rendered_content = include};
  }

  SyntaxRender::RenderedBlock
  RenderMemberFunction(const NodeType &parent,
                       const MemberFunction &member_function) override {
    std::string class_name = member_function.parent_name;
    std::string function_name = member_function.name;

    if (TsInterfaceRender::isCallback(class_name)) {
      // 回调类
      std::vector<std::string> parameter_json_map;
      for (auto &param : member_function.parameters) {
        parameter_json_map.emplace_back("jsonParams." + param.name);
      }

      std::string function;
      function += "    case '" + function_name + "':\n";
      function += "      if (handler." +
                  RenderNonTypeName(function_name).rendered_content +
                  " !== undefined) {\n";
      function += "        handler." +
                  RenderNonTypeName(function_name).rendered_content + "(" +
                  JoinToString(parameter_json_map, ", ") + ")\n";
      function += "      }\n";
      function += "      break\n";

      return {.rendered_content = function};
    } else {
      // 非回调类
      std::vector<std::string> output_params;
      std::vector<std::string> output_params_json_map;
      std::vector<SyntaxRender::RenderedBlock> params;
      std::vector<SyntaxRender::RenderedBlock> params_type_map;
      std::vector<std::string> parameter_json_map;
      std::vector<std::string> parameter_json_map_fixed;
      for (auto &param : member_function.parameters) {
        if (param.is_output) {
          // 过滤输出参数
          output_params.push_back(
              RenderNonTypeName(param.name).rendered_content + ": " +
              RenderSimpleType(param.type).rendered_content);
          output_params_json_map.push_back(param.name);
          continue;
        }
        params.push_back(RenderNonTypeName(param.name));
        params_type_map.push_back(RenderVariable(param, true));

        std::string param_name = RenderNonTypeName(param.name).rendered_content;
        parameter_json_map.emplace_back("      '" + param.name +
                                        "': " + param_name);

        auto name = RenderSimpleType(param.type).rendered_content;
        Clazz c;
        if (name == "Uint8Array" || FindClass(name, c) == 0) {
          printf("ignore param %s %d %s %s\n", member_function.name.c_str(),
                 param.type.kind, param.name.c_str(), param.type.name.c_str());
          continue;
        }
        parameter_json_map_fixed.emplace_back("          '" + param.name +
                                              "': " + param_name);
      }
      if (!output_params.empty()) {
        if (member_function.return_type.name != "void" &&
            member_function.return_type.name != "int") {
          output_params.push_back(
              "result: " +
              RenderSimpleType(member_function.return_type).rendered_content);
          output_params_json_map.emplace_back("result");
        }
      }

      std::string function;
      function +=
          "  " + RenderNonTypeName(function_name).rendered_content + " (" +
          JoinToString(RenderedBlocksToStringList(params_type_map), ", ") +
          "): ";
      if (output_params.capacity() > 1) {
        function += "{ " + JoinToString(output_params, ", ") + " }";
      } else if (output_params.capacity() == 1) {
        function += Split(output_params[0], ": ")[1];
      } else {
        function +=
            RenderSimpleType(member_function.return_type).rendered_content;
      }
      function += " {\n";
      function += "    const apiType = this.getApiTypeFrom" +
                  RenderNonTypeName(function_name, true).rendered_content +
                  "(" + JoinToString(RenderedBlocksToStringList(params), ", ") +
                  ")\n";
      function += "    const jsonParams = {\n";
      function += JoinToString(parameter_json_map, ", \n");
      if (!params_type_map.empty()) {
        function += ",\n";
        function += "      toJSON: () => {\n";
        function += "        return {\n";
        function += JoinToString(parameter_json_map_fixed, ", \n");
        if (!parameter_json_map_fixed.empty())
          function += "\n";
        function += "        }\n";
        function += "      }\n";
      }
      function += "    }\n";

      function += "    ";
      if (!output_params_json_map.empty() ||
          member_function.return_type.name != "void") {
        function += "const jsonResults = ";
      }
      function += "callIrisApi.call(this, apiType, jsonParams)\n";
      for (auto &param : output_params_json_map) {
        function += "    const " + RenderNonTypeName(param).rendered_content +
                    " = jsonResults." + param + ";\n";
      }
      if (output_params_json_map.empty()) {
        if (member_function.return_type.name != "void") {
          function += "    return jsonResults.result\n";
        }
      } else {
        if (output_params_json_map.capacity() == 1) {
          function +=
              "    return " +
              RenderNonTypeName(output_params_json_map[0]).rendered_content +
              "\n";
        } else {
          function += "    return {\n";
          for (auto &param : output_params_json_map) {
            function += RenderNonTypeName(param).rendered_content + ",\n";
          }
          function += "}\n";
        }
      }
      function += "  }\n";

      std::string function_api_type;
      function_api_type +=
          "  protected getApiTypeFrom" +
          RenderNonTypeName(function_name, true).rendered_content + " (" +
          JoinToString(RenderedBlocksToStringList(params_type_map), ", ") +
          "): string {\n";
      function_api_type += "    return '" +
                           RenderedApiType(member_function).rendered_content +
                           "'\n";
      function_api_type += "  }\n";

      return {.rendered_content = function + "\n" + function_api_type};
    }
  }

  SyntaxRender::RenderedBlock
  RenderClass(const Clazz &original_clazz,
              const std::vector<SyntaxRender::RenderedBlock>
                  &rendered_class_members) override {
    if (TsInterfaceRender::isCallback(original_clazz.name)) {
      // 回调类
      std::string class_ = "export function process" + original_clazz.name +
                           " (handler: " + original_clazz.name +
                           ", event: string, jsonParams: any) {\n";
      class_ += "  switch (event) {\n";
      class_ += JoinToString(RenderedBlocksToStringList(rendered_class_members),
                             "\n");
      class_ += "  }\n";
      class_ += "}\n";

      return {.rendered_content = class_};
    } else {
      // 非回调类
      std::string base_class_block;
      if (!original_clazz.base_clazzs.empty()) {
        std::vector<std::string> base_clazzs_tmp =
            std::vector<std::string>(original_clazz.base_clazzs);
        std::transform(
            base_clazzs_tmp.begin(), base_clazzs_tmp.end(),
            base_clazzs_tmp.begin(),
            [](const std::string &s) -> std::string { return s + "Impl"; });
        base_class_block += " extends " + JoinToString(base_clazzs_tmp, ",");
      }

      std::string class_ = "// @ts-ignore\n";
      class_ += "export class " +
                RenderTypeName(original_clazz.name).rendered_content + "Impl" +
                base_class_block + " implements " +
                RenderTypeName(original_clazz.name).rendered_content + " {\n";
      class_ += JoinToString(RenderedBlocksToStringList(rendered_class_members),
                             "\n");
      class_ += "}\n";

      return {.rendered_content = class_};
    }
  }

  void FormatCode(const std::string &file_path) override {
    {
      //      std::string command = "eslint " + file_path + " --fix";
      //      system(command.c_str());
    }
  }
};
