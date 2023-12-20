#ifndef DART_EVENT_HANDLER_IRIS_METHOD_CHANNEL_RENDER_H_
#define DART_EVENT_HANDLER_IRIS_METHOD_CHANNEL_RENDER_H_

#include "dart_syntax_render.hpp"
#include "../base_syntax_render.hpp"
#include "terra_node.hpp"
#include <stdlib.h>
#include "dart_render_configs.hpp"
#include <any>

using namespace terra;

/// Event handler implementation that using [iris_method_channel](https://github.com/littleGnAl/iris_method_channel)
class DartEventHandlerIrisMethodChannelRender : public DartCallApiRender
{

private:
    std::string GetDefaultValueForDartType(std::string dart_type)
    {
        std::string default_value = "null";

        if (dart_type == "int")
        {
            default_value = "0";
        }
        else if (dart_type == "bool")
        {
            default_value = "false";
        }
        else if (dart_type == "String")
        {
            default_value = "''";
        }
        else if (dart_type == "double")
        {
            default_value = "0.0";
        }
        else if (dart_type == "List")
        {
            default_value = "[]";
        }
        else if (dart_type == "Map")
        {
            default_value = "{}";
        }
        else if (dart_type == "dynamic")
        {
            default_value = "null";
        }
        else if (dart_type == "Uint8List")
        {
            default_value = "Uint8List.fromList(const [])";
        }

        return default_value;
    }

public:
    bool
    ShouldRender(const CXXFile &file) override
    {

        return std::find_if(file.nodes.begin(), file.nodes.end(), [&](const auto &node)
                            { 
                                bool should_render = false;
                                if (std::holds_alternative<Clazz>(node)) {
                                    Clazz clazz = std::get<Clazz>(node);
                                    std::string class_with_ns = NameWithNamespace(clazz.name, clazz.namespaces);
                                    if (std::find(event_handler_class_.begin(), event_handler_class_.end(), class_with_ns) != event_handler_class_.end()) {
                                        should_render = true;
                                    }
                                }

                                return should_render; }) != file.nodes.end();
    }

    virtual SyntaxRender::RenderedBlock RenderIncludeDirectives(const CXXFile &file, const std::vector<IncludeDirective> &include_directives) override
    {

        std::string dart_import = "";
        dart_import += dart_header;
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
        rendered_block.rendered_content = std::string("binding/" + base_name + "_event_impl.dart");
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
        SyntaxRender::RenderedBlock rendered_block; // = DartSyntaxRender::RenderEnumConstant(enum_const);
        // std::string constant_name = "@JsonValue(" + enum_const.value + ")\n";
        // constant_name += rendered_block.rendered_content;

        rendered_block.rendered_content = "";
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderEnum(const Enumz &enumz, const std::vector<RenderedBlock> &rendered_enum_consts) override
    {
        // std::string dart_enum_name = RenderTypeName(enumz.name).rendered_content + "Json";

        // std::string dart_enum = "enum " + dart_enum_name + " {\n";
        // dart_enum += JoinToString(RenderedBlocksToStringList(rendered_enum_consts), "\n");
        // dart_enum += "\n";
        // dart_enum += "}\n";

        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = "";
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
        if (std::find(event_handler_class_.begin(), event_handler_class_.end(), class_with_ns) == event_handler_class_.end())
        {
            return rendered_block;
        }

        std::vector<SyntaxRender::RenderedBlock> new_rendered_class_members = std::vector<SyntaxRender::RenderedBlock>(rendered_class_members);

        std::string extends_block = "";

        if (!original_clazz.base_clazzs.empty())
        {
            extends_block += "extends";
            for (auto &base_clazz_name : original_clazz.base_clazzs)
            {
                Clazz super_clazz;
                if (FindClass(base_clazz_name, super_clazz) == 0)
                {
                    extends_block += " " + RenderClassName(super_clazz).rendered_content + "Wrapper";

                    // std::vector<std::string> super_class_member_names;
                    // std::vector<std::string> super_class_declare_member_names;

                    // if (!super_clazz.methods.empty())
                    // {
                    //     for (auto &method : super_clazz.methods)
                    //     {
                    //         SyntaxRender::RenderedBlock m = RenderMemberFunction(original_clazz, method);

                    //         new_rendered_class_members.push_back(m);
                    //     }
                    // }
                }
            }
        }

        std::string ext_name = RenderClassName(original_clazz).rendered_content;
        std::string module_name = ext_name;
        if (IsUseIrisApiType(original_clazz.file_path)) {
            module_name = RenderTypeName(original_clazz.name).rendered_content;
        }

        std::string process_impl = "";

        std::string paramName = ext_name;
        paramName[0] = std::tolower(paramName[0]);
        process_impl += "class " + ext_name + "Wrapper ";
        if (!extends_block.empty())
        {
            process_impl += extends_block + " {\n";
        }
        else
        {
            process_impl += " implements EventLoopEventHandler {\n";
        }
        process_impl += "const " + ext_name + "Wrapper(this." + paramName + ")";
        if (!extends_block.empty())
        {
            process_impl += " : super(" + paramName + ")";
        }
        process_impl += ";\n";

        process_impl += "final " + ext_name + " " + paramName + ";\n";

        process_impl += "@override\n";
        process_impl += "bool operator ==(Object other) {\n";
        process_impl += "  if (other.runtimeType != runtimeType) {\n";
        process_impl += "    return false;\n";
        process_impl += "  }\n";
        process_impl += "  return other is " + ext_name + "Wrapper &&\n";
        process_impl += "      other." + paramName + " == " + paramName + ";\n";
        process_impl += "}\n";
        process_impl += "@override\n";
        process_impl += "int get hashCode => " + paramName + ".hashCode;\n";

        process_impl += "@override\n";
        process_impl += "bool handleEventInternal(String eventName, String eventData, List<Uint8List> buffers) {\n";
        process_impl += "switch (eventName) {\n";
        process_impl += JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
        // process_impl += "default:\n";
        // process_impl += "break;\n";

        process_impl += "}\n";
        process_impl += "return false;\n";
        process_impl += "}\n";
        process_impl += "\n";

        process_impl += "@override\n";
        process_impl += "bool handleEvent(String eventName, String eventData, List<Uint8List> buffers) {\n";
        process_impl += "if (!eventName.startsWith('" + module_name + "')) return false;\n";
        process_impl += "final newEvent = eventName.replaceFirst('" + module_name + "_', '');\n";
        // process_impl += paramName + ".process(newEvent, data, buffers);\n";
        process_impl += "if (handleEventInternal(newEvent, eventData, buffers)) {\n";
        process_impl += "return true;\n";
        process_impl += "}\n";
        process_impl += "\n";
        if (!extends_block.empty())
        {
            process_impl += "return super.handleEventInternal(newEvent, eventData, buffers);\n";
        }
        else
        {
            process_impl += "return false;\n";
        }
        process_impl += "}\n";
        process_impl += "}\n";

        rendered_block.rendered_content = process_impl;
        return rendered_block;
    }

    std::string FromJsonObject(const Struct &structt, const std::string &param_name)
    {
        // std::string name = TrimNamespaces(param_name);
        std::string impl = "";
        std::string type_name = RenderTypeName(structt.name).rendered_content;
        std::string non_type_name = RenderNonTypeName(structt.name).rendered_content;
        impl += type_name + " " + param_name + " = ";
        impl += type_name + "(";

        for (auto &param : structt.member_variables)
        {
            impl += RenderNonTypeName(param.name).rendered_content + ": " + "paramJson." + param_name + "." + RenderNonTypeName(param.name).rendered_content + ", \n";
        }

        impl += ");\n";

        return impl;
    }

    SyntaxRender::RenderedBlock RenderMemberFunction(const NodeType &parent, const MemberFunction &member_function) override
    {
        std::string switch_case = member_function.name;

        if (IsEventHandler(parent))
        {
            if (std::holds_alternative<Clazz>(parent))
            {
                const Clazz &cls = std::get<Clazz>(parent);
                if (IsUseIrisApiType(cls.file_path)) {
                    switch_case = IrisApiType(cls, member_function, false);
                } else {
                    if (member_function.user_data.has_value())
                    {
                        try
                        {
                            const std::map<std::string, bool> &extra_info = std::any_cast<const std::map<std::string, bool> &>(member_function.user_data);
                            if (extra_info.find("is_from_rtc_event_handler_ex") != extra_info.end())
                            {
                                bool is_from_rtc_event_handler_ex = extra_info.find("is_from_rtc_event_handler_ex")->second;
                                if (is_from_rtc_event_handler_ex)
                                {
                                    switch_case = switch_case + "Ex";
                                }
                            }
                        }
                        catch (const std::bad_any_cast &e)
                        {
                            std::cout << e.what() << '\n';
                        }
                    }
                }
            }
        }

        std::string callback_impl = "";
        callback_impl += "case \'" + switch_case + "\':\n";

        std::string member_function_name = member_function.name;
        member_function_name[0] = std::toupper(member_function_name[0]);
        std::string struct_name = "";

        std::string wrapper_name = "";

        if (std::holds_alternative<Clazz>(parent))
        {
            const Clazz &s = std::get<Clazz>(parent);
            struct_name += RenderClassName(s).rendered_content;
            wrapper_name = RenderClassName(s).rendered_content;
            wrapper_name[0] = std::tolower(wrapper_name[0]);
        }
        struct_name += RenderTypeName(member_function_name).rendered_content + "Json";

        callback_impl += "if(" + wrapper_name + "." + RenderNonTypeName(member_function.name).rendered_content + " == null) {\n";
        callback_impl += "return true;\n";
        callback_impl += "}\n";
        callback_impl += "final jsonMap = jsonDecode(eventData);\n";
        callback_impl += struct_name + " paramJson = " + struct_name + ".fromJson(jsonMap);\n";
        callback_impl += "paramJson = paramJson.fillBuffers(buffers);\n";

        std::vector<SyntaxRender::RenderedBlock> params;
        std::vector<std::string> implementation_lines;
        std::vector<std::string> parameter_json_map;
        std::vector<std::string> fill_buffers_params_impl;
        std::vector<std::string> parameter_if_null;
        for (auto &param : member_function.parameters)
        {
            auto p = RenderVariable(param);
            params.push_back(p);

            std::string param_name = RenderNonTypeName(param.name).rendered_content;

            std::string default_value = "";

            Struct s;
            if (FindStruct(param.type.name, s) == 0)
            {
                if (param.type.kind == SimpleTypeKind::array_t)
                {
                    std::string fill_buffer_impl = param_name + " = " + param_name + ".map((e) => e.fillBuffers(buffers)).toList();\n";
                    fill_buffers_params_impl.push_back(fill_buffer_impl);
                }
                else
                {
                    std::string fill_buffer_impl = param_name + " = " + param_name + ".fillBuffers(buffers);\n";
                    fill_buffers_params_impl.push_back(fill_buffer_impl);
                }
            }

            std::string impl = RenderSimpleType(param.type).rendered_content + "? " + param_name + " = paramJson." + param_name + ";";
            implementation_lines.push_back(impl);
            parameter_json_map.push_back(std::string(param_name));
            parameter_if_null.push_back(std::string(std::string(param_name + " == null")));
        }

        callback_impl += JoinToString(implementation_lines, "\n");

        if (!parameter_if_null.empty())
        {
            callback_impl += "if (";
            callback_impl += JoinToString(parameter_if_null, " || ");
            callback_impl += ")\n";
            callback_impl += "{ return true; }\n";
        }

        for (auto &p : fill_buffers_params_impl)
        {
            callback_impl += p;
        }

        callback_impl += wrapper_name + "." + RenderNonTypeName(member_function.name).rendered_content + "!(";
        callback_impl += JoinToString(parameter_json_map, ",");
        callback_impl += ");";

        callback_impl += "return true;\n";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = callback_impl;
        return rendered_block;
    }
};

#endif // DART_EVENT_HANDLER_IRIS_METHOD_CHANNEL_RENDER_H_