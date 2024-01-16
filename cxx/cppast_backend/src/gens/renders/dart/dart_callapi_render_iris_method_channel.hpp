#ifndef DART_CALLAPI_IRIS_METHOD_CHANNEL_RENDER_H_
#define DART_CALLAPI_IRIS_METHOD_CHANNEL_RENDER_H_

#include "dart_syntax_render.hpp"
#include "../base_syntax_render.hpp"
#include "terra_generator.hpp"
#include "terra_node.hpp"
#include <stdlib.h>
#include <any>
#include "dart_render_configs.hpp"

using namespace terra;

/// Call api implementation that using [iris_method_channel](https://github.com/littleGnAl/iris_method_channel)
class DartCallApiIrisMethodChannelRender : public DartSyntaxRender
{

public:
    bool ShouldRender(const CXXFile &file) override
    {

        return std::find_if(file.nodes.begin(), file.nodes.end(), [&](const auto &node)
                            { 
                                bool should_render = false;
                                if (std::holds_alternative<Clazz>(node)) {
                                    Clazz clazz = std::get<Clazz>(node);
                                    std::string class_with_ns = NameWithNamespace(clazz.name, clazz.namespaces);
                                    if (std::find(event_handler_class_.begin(), event_handler_class_.end(), class_with_ns) == event_handler_class_.end()) {
                                        should_render = true;
                                    }
                                }

                                return should_render; }) != file.nodes.end();
    }

    virtual SyntaxRender::RenderedBlock RenderIncludeDirectives(const CXXFile &file, const std::vector<IncludeDirective> &include_directives) override
    {

        std::string dart_import = "";
        dart_import += dart_header + ", annotate_overrides";
        dart_import += "\n";

        dart_import += "import 'package:agora_rtc_engine/src/binding_forward_export.dart';\n";
        dart_import += "import 'package:agora_rtc_engine/src/binding/impl_forward_export.dart';\n";
        dart_import += "import 'package:iris_method_channel/iris_method_channel.dart';\n";

        dart_import += "\n";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_import;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderedFileName(const std::string &file_path) override
    {
        std::filesystem::path p(file_path);

        std::string base_name = UpperCamelCaseToLowercaseWithUnderscores(p.stem());
        if (base_name[0] == 'i')
        {
            base_name = std::string(base_name.begin() + 1, base_name.end());
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = std::string("binding/" + base_name + "_impl.dart");
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderMemberVariable(const NodeType &parent, const MemberVariable &member_variable) override
    {

        SyntaxRender::RenderedBlock rendered_block;

        return rendered_block;
    }

    RenderedBlock RenderStruct(const Struct &original_struct, const std::vector<RenderedBlock> &rendered_struct_members) override
    {
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderEnumConstant(const EnumConstant &enum_const) override
    {
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderEnum(const Enumz &enumz, const std::vector<RenderedBlock> &rendered_enum_consts) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    RenderedBlock RenderTopLevelVariable(const Variable &top_level_variable) override
    {
        SyntaxRender::RenderedBlock render_block;
        return render_block;
    }

    SyntaxRender::RenderedBlock RenderClass(const Clazz &original_clazz, const std::vector<SyntaxRender::RenderedBlock> &rendered_class_members) override
    {
        SyntaxRender::RenderedBlock rendered_block;

        std::string class_with_ns = NameWithNamespace(original_clazz.name, original_clazz.namespaces);
        if (std::find(event_handler_class_.begin(), event_handler_class_.end(), class_with_ns) != event_handler_class_.end())
        {
            return rendered_block;
        }

        std::string base_class_block = "";
        if (!original_clazz.base_clazzs.empty())
        {
            std::vector<std::string> base_clazzs_tmp = std::vector<std::string>(original_clazz.base_clazzs);
            std::transform(base_clazzs_tmp.begin(), base_clazzs_tmp.end(), base_clazzs_tmp.begin(),
                           [&](std::string s) -> std::string
                           {
                               std::string type_name = RenderTypeName(s).rendered_content;
                               if (type_name[0] == 'I')
                               {
                                   type_name = std::string(type_name.begin() + 1, type_name.end());
                               }
                               return type_name + "Impl";
                           });
            base_class_block += " extends " + JoinToString(base_clazzs_tmp, ",");
        }

        std::string dart_class = "class " + RenderClassName(original_clazz).rendered_content + "Impl" + base_class_block + " implements " + RenderClassName(original_clazz).rendered_content + " {\n";
        dart_class += "\n";

        if (base_class_block.empty())
        {
            dart_class += RenderClassName(original_clazz).rendered_content + "Impl(this.irisMethodChannel);\n";
            dart_class += "\n";

            dart_class += "@protected\n";
            dart_class += "final IrisMethodChannel irisMethodChannel;\n";
            dart_class += "\n";
        }
        else
        {
            dart_class += RenderClassName(original_clazz).rendered_content + "Impl(IrisMethodChannel irisMethodChannel): super(irisMethodChannel);\n";
            dart_class += "\n";
        }

        if (!base_class_block.empty())
        {
            dart_class += "@override\n";
        }

        dart_class += "@protected\n";
        dart_class += "Map<String, dynamic> createParams(Map<String, dynamic> param) {\n";
        dart_class += "return param;\n";
        dart_class += "}\n";
        dart_class += "\n";

        if (!base_class_block.empty())
        {
            dart_class += "\n";
            dart_class += "@override\n";
        }

        dart_class += "@protected\n";
        dart_class += "bool get isOverrideClassName => false;\n";
        dart_class += "\n";

        std::string api_type = "";
        if (original_clazz.name[0] == 'I')
        {
            api_type += std::string(original_clazz.name.begin() + 1, original_clazz.name.end());
        }
        if (!base_class_block.empty())
        {
            dart_class += "\n";
            dart_class += "@override\n";
        }
        dart_class += "@protected\n";
        dart_class += "String get className => '" + api_type + "';\n";
        dart_class += "\n";

        dart_class += JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
        dart_class += "}\n";

        rendered_block.rendered_content = dart_class;
        return rendered_block;
    }

    std::string ToJsonObject(const Struct &structt, const std::string &param_name)
    {
        std::string impl = "";
        std::string type_name = RenderTypeName(structt.name).rendered_content + "Json";
        impl += type_name + " " + param_name + "Json" + " = ";
        impl += type_name + "(";

        for (auto &param : structt.member_variables)
        {
            impl += RenderNonTypeName(param.name).rendered_content + ": " + param_name + "." + RenderNonTypeName(param.name).rendered_content + ", \n";
        }

        impl += ");\n";

        return impl;
    }

    SyntaxRender::RenderedBlock RenderMemberFunction(const NodeType &parent, const MemberFunction &member_function) override
    {
        std::string return_type = RenderSimpleType(member_function.return_type).rendered_content;

        std::string dart_function = "";
        std::vector<std::string> implementation_lines;
        std::vector<std::string> parameter_json_map;
        std::vector<std::string> parameter_json_map_extra_parameter_name;
        std::vector<Variable> buffer_type_params;
        for (auto &param : member_function.parameters)
        {
            std::string param_name = RenderNonTypeName(param.name).rendered_content;
            std::string param_map_key = param.name;

            if (RenderSimpleType(param.type).rendered_content == "Uint8List")
            {
                buffer_type_params.push_back(param);
            }
            else
            {
                Enumz enumz;
                // Don't know why IMediaPlayer in destroyMediaPlayer is found success, directly filter it at this time
                if (member_function.name != "destroyMediaPlayer" && FindEnumz(param.type.name, enumz) == 0)
                {
                    parameter_json_map.push_back(std::string("'" + param_map_key + "'" + ":" + param_name + ".value()"));
                    continue;
                }

                Struct s;
                // std::cout << "fina param.type.name: " << param.type.name << std::endl;
                if (FindStruct(param.type.name, s) == 0)
                {
                    bool isIgnoreJsonClass = false;
                    std::string full_name = s.GetFullName();

                    if (std::find(IgnoreJsonClass.begin(), IgnoreJsonClass.end(), full_name) != IgnoreJsonClass.end())
                    {
                        isIgnoreJsonClass = true;
                    }
                    // implementation_lines.push_back(ToJsonObject(s, param_name));
                    std::string nullable_keyword = "";
                    if (IsNullableVariable(param))
                    {
                        nullable_keyword = "?";
                    }

                    if (param.type.kind == SimpleTypeKind::array_t)
                    {
                        std::string new_pn = param_name + "JsonList ";
                        parameter_json_map_extra_parameter_name.push_back(std::string("final " + new_pn + " = " + param_name + ".map((e) => e.toJson()).toList();"));
                        parameter_json_map.push_back(std::string("'" + param_map_key + "'" + ":" + new_pn));
                    }
                    else if (!isIgnoreJsonClass)
                    {
                        parameter_json_map.push_back(std::string("'" + param_map_key + "'" + ":" + param_name + nullable_keyword + ".toJson()"));
                    }
                    else
                    {
                        std::string tmpKV = "";
                        if (IsNullableVariable(param)) { 
                            tmpKV += "if (" + param_name + " != null)\n";
                        }
                        tmpKV += std::string("'" + param_map_key + "'" + ":" + param_name);

                        parameter_json_map.push_back(tmpKV);
                    }

                    continue;
                }

                std::string tmpKV = "";
                if (IsNullableVariable(param)) { 
                    tmpKV += "if (" + param_name + " != null)\n";
                }
                tmpKV += std::string("'" + param_map_key + "'" + ":" + param_name);
                parameter_json_map.push_back(tmpKV);
            }
        }

        dart_function += RenderFunctionSignature(parent, member_function, true).rendered_content;
        dart_function += "\n";
        dart_function += "{\n";

        std::string api_type = "";
        std::string const_or_final_api_type_prefix = "final";
        if (std::holds_alternative<Clazz>(parent))
        {
            const Clazz clazz = std::get<Clazz>(parent);

            std::string api_type_prefix = clazz.name;
            if (clazz.name[0] == 'I')
            {
                api_type_prefix = std::string(clazz.name.begin() + 1, clazz.name.end());
                
            }
            std::string fn = BaseSyntaxRender::GetApiId(member_function);
            api_type += "${isOverrideClassName ? className : '" + api_type_prefix + "'}_" + fn;
        }

        std::string comment_prefix = "";
        std::string unimplemetation = "";
        if (!IsAsyncFunc(parent, member_function))
        {
            dart_function += "// Implementation template\n";
            comment_prefix = "// ";
            unimplemetation = "throw UnimplementedError('Unimplement for " + member_function.name + "');\n";
        }

        dart_function += comment_prefix + const_or_final_api_type_prefix + " apiType = '" + api_type + "';\n";
        for (auto &e : parameter_json_map_extra_parameter_name)
        {
            dart_function += comment_prefix + e + "\n";
        }
        dart_function += comment_prefix + "final param = createParams({";
        dart_function += comment_prefix + JoinToString(parameter_json_map, ", ");
        dart_function += comment_prefix + "});\n";

        std::string await_keyword = "";
        if (IsAsyncFunc(parent, member_function))
        {
            await_keyword = "await";
        }

        std::string buffer = "";

        if (!member_function.parameters.empty())
        {
            bool should_impl_buffers = false;
            for (auto &param : member_function.parameters)
            {
                std::string pn = RenderNonTypeName(param.name).rendered_content;
                if (RenderSimpleType(param.type).rendered_content == "Uint8List")
                {
                    should_impl_buffers = true;
                    break;
                }

                Struct s;
                if (FindStruct(param.type.name, s) == 0)
                {
                    should_impl_buffers = true;
                    break;
                }
            }

            if (should_impl_buffers)
            {
                dart_function += "final List<Uint8List> buffers = [];\n";

                for (auto &param : member_function.parameters)
                {
                    std::string pn = RenderNonTypeName(param.name).rendered_content;
                    if (RenderSimpleType(param.type).rendered_content == "Uint8List")
                    {
                        dart_function += "buffers.add(" + pn + ");\n";
                    }

                    Struct s;
                    if (FindStruct(param.type.name, s) == 0)
                    {
                        bool is_array_t = param.type.kind == SimpleTypeKind::array_t;

                        if (IsNullableVariable(param))
                        {
                            dart_function += "if (" + pn + "!= null) {\n";
                            if (is_array_t)
                            {
                                dart_function += "for (final e in " + pn + ") {\n";
                                dart_function += "buffers.addAll(e.collectBufferList());\n";
                                dart_function += "}\n";
                            }
                            else
                            {
                                dart_function += "buffers.addAll(" + pn + ".collectBufferList());\n";
                            }

                            dart_function += "}\n";
                        }
                        else
                        {
                            if (is_array_t)
                            {
                                dart_function += "for (final e in " + pn + ") {\n";
                                dart_function += "buffers.addAll(e.collectBufferList());\n";
                                dart_function += "}\n";
                            }
                            else
                            {
                                dart_function += "buffers.addAll(" + pn + ".collectBufferList());\n";
                            }
                        }
                    }
                }

                buffer = "buffers";
            }
            else
            {
                buffer = "null";
            }
        }
        else
        {
            buffer = "null";
        }

        dart_function += comment_prefix + "final callApiResult = " + await_keyword + " irisMethodChannel.invokeMethod(IrisMethodCall(apiType, jsonEncode(param), buffers:" + buffer + "));\n";
        dart_function += comment_prefix + "if (callApiResult.irisReturnCode < 0) {\n";
        dart_function += comment_prefix + "throw AgoraRtcException(code: callApiResult.irisReturnCode);\n";
        dart_function += comment_prefix + "}\n";
        dart_function += comment_prefix + "final rm = callApiResult.data;\n";
        dart_function += comment_prefix + "final result = rm['result'];\n";

        if (member_function.user_data.has_value())
        {
            try
            {
                const Variable &variable_for_return = std::any_cast<Variable>(member_function.user_data);

                dart_function += comment_prefix + "if (result < 0) {\n";
                dart_function += comment_prefix + "throw AgoraRtcException(code: result);\n";
                dart_function += comment_prefix + "}\n";

                std::string member_function_name = member_function.name;
                member_function_name[0] = std::toupper(member_function_name[0]);
                std::string jsonTypeName = "";
                if (std::holds_alternative<Clazz>(parent))
                {
                    const Clazz &s = std::get<Clazz>(parent);
                    jsonTypeName += RenderClassName(s).rendered_content;
                }
                jsonTypeName += RenderTypeName(member_function_name).rendered_content + "Json";

                dart_function += comment_prefix + "final " + RenderNonTypeName(member_function.name).rendered_content + "Json = " + jsonTypeName + ".fromJson(rm);\n";
                dart_function += comment_prefix + "return " + RenderNonTypeName(member_function.name).rendered_content + "Json." + RenderNonTypeName(variable_for_return.name).rendered_content + ";\n";
            }
            catch (const std::bad_any_cast &e)
            {
                if (member_function.return_type.name != "void")
                {
                    dart_function += comment_prefix + "return result as " + return_type + ";\n";
                }
                std::cout << e.what() << " " << member_function.name << '\n';
            }
        }
        else
        {
            Enumz enumz;
            // Don't know why createMediaPlayer is found success, directly filter it at this time
            if (member_function.name != "createMediaPlayer" && FindEnumz(member_function.return_type.name, enumz) == 0)
            {
                std::string dart_enum_name = RenderTypeName(enumz.name).rendered_content;

                if (dart_enum_name.empty())
                {
                    dart_enum_name = enumz.parent_name + "Enum";
                }
                dart_function += comment_prefix + "return " + dart_enum_name + "Ext.fromValue(result);\n";
            }
            else if (member_function.return_type.name != "void")
            {
                dart_function += comment_prefix + "return result as " + return_type + ";\n";
            }
            else
            {
                dart_function += comment_prefix + "if (result < 0) {\n";
                dart_function += comment_prefix + "throw AgoraRtcException(code: result);\n";
                dart_function += comment_prefix + "}\n";
            }
        }

        dart_function += unimplemetation;
        dart_function += "}\n";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_function;
        return rendered_block;
    }
};

#endif // DART_CALLAPI_IRIS_METHOD_CHANNEL_RENDER_H_