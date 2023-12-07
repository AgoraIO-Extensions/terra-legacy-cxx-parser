#ifndef DART_SYNTAX_RENDER_H_
#define DART_SYNTAX_RENDER_H_

#include "terra.hpp"
#include "terra_generator.hpp"
#include "../base_syntax_render.hpp"
#include "dart_render_configs.hpp"
#include <filesystem>
#include <stdlib.h>

using namespace terra;

class DartSyntaxRender : public BaseSyntaxRender
{

private:
    std::string file_path_ = "";

    // TODO(littlegnal): Configurable
    std::map<std::string, std::string> CppStdTypeToDartType = {
        {"char", "String"},
        {"char *", "String"},
        {"const char *", "String"},
        {"unsigned int", "int"},
        {"size_t", "int"},
        {"unsigned short", "int"},
        {"float", "double"},
        {"int64_t", "int"},
        {"int32_t", "int"},
        {"long", "int"},
        {"int16_t", "int"},
        {"unsigned char", "Uint8List"},
        {"unsigned char *", "Uint8List"},
        {"uint8_t", "int"},
        {"uint32_t", "int"},
        {"uint64_t", "int"},
        {"uint16_t", "int"},
        {"long long", "int"},
        {"intptr_t", "int"},
    };

    // TODO(littlegnal): Configurable
    std::map<std::string, std::string> TypedefTypeToDartType = {
        {"uid_t", "int"},
        {"track_id_t", "int"},
        {"video_track_id_t", "int"},
        {"conn_id_t", "int"},
        {"view_t", "int"},
        {"AString", "String"},
        {"user_id_t", "String"},
    };

    std::vector<std::string> DummyForDartType{"agora_refptr", "Optional"};

protected:
    bool IsEventHandler(const NodeType &parent)
    {
        if (std::holds_alternative<Clazz>(parent))
        {
            Clazz clazz = std::get<Clazz>(parent);
            std::string class_with_ns = clazz.GetFullName();
            if (std::find(event_handler_class_.begin(), event_handler_class_.end(), class_with_ns) != event_handler_class_.end())
            {
                return true;
            }
        }

        return false;
    }

    virtual SyntaxRender::RenderedBlock RenderClassName(const Clazz &clazz)
    {
        std::string class_name = RenderTypeName(clazz.name).rendered_content;
        // if (IsEventHandler(clazz))
        // {
        //     if (class_name[0] == 'I')
        //     {
        //         class_name = std::string(class_name.begin() + 1, class_name.end());
        //     }
        // }
        if (class_name[0] == 'I')
        {
            class_name = std::string(class_name.begin() + 1, class_name.end());
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = class_name;
        return rendered_block;
    }

    virtual std::vector<MemberVariable> VariablesToMemberVariable(const std::vector<Variable> variables)
    {
        std::vector<MemberVariable> out;
        for (auto &member : variables)
        {
            MemberVariable member_variable;
            member_variable.name = member.name;
            member_variable.type = member.type;

            out.push_back(member_variable);
        }

        return out;
    }

    virtual SyntaxRender::RenderedBlock RenderCallApiEventHandlerBufferExt(
        const std::string &class_name,
        const std::vector<MemberVariable> &member_variables)
    {
        std::string dart_class = "";

        std::vector<std::string> named_member_names;
        std::vector<std::string> buffer_member_names;

        if (!member_variables.empty())
        {

            for (auto &member : member_variables)
            {
                std::string m = "";
                std::string member_type = RenderSimpleType(member.type).rendered_content;
                if (std::find(IgnoreJsonType.begin(), IgnoreJsonType.end(), member_type) != IgnoreJsonType.end() &&
                    std::find(IgnoreBufferExtType.begin(), IgnoreBufferExtType.end(), member_type) == IgnoreBufferExtType.end())
                {

                    buffer_member_names.push_back(RenderNonTypeName(member.name).rendered_content);
                }
                else
                {
                }

                m = RenderNonTypeName(member.name).rendered_content + ":" + RenderNonTypeName(member.name).rendered_content;

                // std::string m = "this." + RenderNonTypeName(member.name).rendered_content;
                named_member_names.push_back(m);
            }
        }

        dart_class += "extension " + class_name + "BufferExt on " + class_name + " {\n";
        dart_class += class_name + " fillBuffers(List<Uint8List> bufferList) {\n";
        dart_class += "if (bufferList.isEmpty) return this;\n";

        if (!buffer_member_names.empty())
        {
            for (int i = 0; i < buffer_member_names.size(); i++)
            {
                auto &bn = buffer_member_names[i];
                std::string bn_impl = "";
                bn_impl += "Uint8List? " + bn + ";\n";
                bn_impl += "if (bufferList.length > " + std::to_string(i) + ") {\n";
                // yBuffer = bufferList[0];
                bn_impl += bn + " = " + "bufferList[" + std::to_string(i) + "];\n";
                bn_impl += "}\n";
                dart_class += bn_impl;
            }

            dart_class += "return " + class_name + "(\n";
            dart_class += JoinToString(named_member_names, ",");
            dart_class += ");\n";
        }
        else
        {
            dart_class += "return this;\n";
        }
        dart_class += "}\n";

        dart_class += "List<Uint8List> collectBufferList() {\n";
        dart_class += "final bufferList = <Uint8List>[];\n";

        if (!buffer_member_names.empty())
        {
            for (auto &m : buffer_member_names)
            {
                dart_class += "if (" + m + " != null) {\n";
                dart_class += "bufferList.add(" + m + "!);\n";
                dart_class += "}\n";
            }
        }
        dart_class += "return bufferList;\n";
        dart_class += "}\n";

        dart_class += "}\n";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_class;
        return rendered_block;
    }

public:
    virtual SyntaxRender::RenderedBlock RenderTypeName(const std::string &type_name)
    {
        // std::string full_name = original_struct.GetFullName();

        // std::string new_type_name = type_name;
        // if (new_type_name[0] == 'I')
        // {
        //     new_type_name = std::string(new_type_name.begin() + 1, new_type_name.end());
        // }
        SyntaxRender::RenderedBlock rendered_block;

        std::string dart_type = "";
        if (type_name.empty())
        {
            return rendered_block;
        }

        dart_type = TrimNamespaces(type_name);
        bool ignoreDartStyle = std::find_if(IgnoreDartStyleType.begin(), IgnoreDartStyleType.end(), [&](const std::string &type)
                                            { return type_name == type || dart_type == TrimNamespaces(type); }) != IgnoreDartStyleType.end();
        if (ignoreDartStyle)
        {
            // struct_name = original_struct.name;
            SyntaxRender::RenderedBlock rendered_block;
            rendered_block.rendered_content = dart_type;
            return rendered_block;
        }
        if (TypedefTypeToDartType.find(dart_type) != TypedefTypeToDartType.end())
        {
            dart_type = TypedefTypeToDartType[dart_type];
        }

        if (CppStdTypeToDartType.find(dart_type) != CppStdTypeToDartType.end())
        {
            dart_type = CppStdTypeToDartType[dart_type];
        }

        if (dart_type.find('_') != std::string::npos)
        {
            dart_type = NameWithUnderscoresToCamelCase(dart_type, true);
        }

        // SIZE -> Size
        if (IsUpper(dart_type))
        {
            dart_type = ToLower(dart_type);
            dart_type[0] = std::toupper(dart_type[0]);
        }

        rendered_block.rendered_content = dart_type;
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderNonTypeName(const std::string &non_type_name)
    {
        std::string dart_type = non_type_name;
        if (dart_type.find('_') != std::string::npos)
        {
            dart_type = NameWithUnderscoresToCamelCase(dart_type, false);
        }

        if (IsUpper(dart_type))
        {
            dart_type = ToLower(dart_type);
        }

        if (std::isupper(dart_type[0]))
        {
            dart_type[0] = std::tolower(dart_type[0]);
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_type;
        return rendered_block;
    }

    virtual std::string TrimDummyType(const std::string &name)
    {
        std::string type = name;
        for (auto &dummy : DummyForDartType)
        {
            Replace(type, dummy, "");
            Replace(type, "<", "");
            Replace(type, ">", "");
        }

        return type;
    }

    virtual std::string GetTypeNameFromSimpleType(const SimpleType &simple_type)
    {
        if (simple_type.kind == SimpleTypeKind::template_t) {
            return simple_type.template_arguments[0];
        }

        return simple_type.GetTypeName();
    }

    virtual SyntaxRender::RenderedBlock RenderSimpleType(const SimpleType &simple_type)
    {
        std::string dart_type = simple_type.name;
        if (simple_type.kind == SimpleTypeKind::template_t) {
            dart_type = simple_type.template_arguments[0];
        }

        if (dart_type.empty())
        {
            dart_type = simple_type.source;
        }

        for (auto &dummy : DummyForDartType)
        {
            Replace(dart_type, dummy, "");
            Replace(dart_type, "<", "");
            Replace(dart_type, ">", "");
        }

        if ((dart_type == "unsigned char" || dart_type == "uint8_t") && (simple_type.kind == SimpleTypeKind::pointer_t || simple_type.kind == SimpleTypeKind::array_t))
        {
            dart_type = "Uint8List";
        }
        else
        {
            dart_type = RenderTypeName(dart_type).rendered_content;
            if (!simple_type.is_builtin_type && dart_type[0] == 'I')
            {
                if (std::find(donot_trim_prefix_i_class_.begin(), donot_trim_prefix_i_class_.end(), simple_type.name) == donot_trim_prefix_i_class_.end())
                {
                    dart_type = std::string(dart_type.begin() + 1, dart_type.end());
                }
            }

            // String type
            if (simple_type.name == "char" && (simple_type.kind == SimpleTypeKind::array_t || simple_type.kind == SimpleTypeKind::pointer_t))
            {
                if (EndWith(simple_type.source, "**"))
                {
                    dart_type = "List<" + dart_type + ">";
                }
            }
            else
            {
                if (simple_type.kind == SimpleTypeKind::array_t)
                {
                    dart_type = "List<" + dart_type + ">";
                }
            }
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_type;
        return rendered_block;
    }

    bool IsAsyncFunc(const NodeType &parent, const MemberFunction &function)
    {
        bool is_async_func = true;
        if (std::holds_alternative<Clazz>(parent))
        {
            Clazz clazz = std::get<Clazz>(parent);

            std::string class_with_ns = BaseSyntaxRender::NameWithNamespace(clazz.name, clazz.namespaces);

            if (synchronized_functions_.find(class_with_ns) != synchronized_functions_.end())
            {
                std::vector<std::string> &sfs = synchronized_functions_.find(class_with_ns)->second;
                if (std::find(sfs.begin(), sfs.end(), function.name) != sfs.end())
                {
                    is_async_func = false;
                }
            }

            std::regex pattern =
                std::regex("^(register|unregister).*(Observer|Handler|Callback|Receiver)");
            std::smatch output;
            if (std::regex_match(function.name, output, pattern))
            {
                is_async_func = false;
            }
        }

        return is_async_func;
    }

    bool HasDefaultValue(const Variable &variable)
    {
        return !variable.default_value.empty() || IsNullableVariable(variable);
    }

    bool IsNullableVariable(const Variable &variable)
    {
        return variable.default_value == "__null" || variable.default_value == "nullptr";
    }

    bool IsNullableReturnType(const SimpleType &simple_type)
    {
        std::string dart_type = simple_type.name;

        if (dart_type.empty())
        {
            dart_type = simple_type.source;
        }

        bool isNullableReturnType = false;
        for (auto &dummy : DummyForDartType)
        {
            isNullableReturnType |= Contains(dart_type, dummy);
        }

        return isNullableReturnType;
    }

    SyntaxRender::RenderedBlock RenderFunctionSignature(const NodeType &parent, const MemberFunction &function, bool is_impl = false)
    {
        bool should_apped_async = IsAsyncFunc(parent, function);
        std::string dart_function = "";

        if (std::holds_alternative<Clazz>(parent))
        {
            Clazz clazz = std::get<Clazz>(parent);

            if (is_impl)
            {
                dart_function += "@override\n";
            }
            else
            {
                if (!clazz.base_clazzs.empty())
                {
                    for (auto &base_clazz_name : clazz.base_clazzs)
                    {
                        Clazz super_clazz;
                        if (FindClass(base_clazz_name, super_clazz) == 0)
                        {
                            if (!super_clazz.methods.empty())
                            {
                                for (auto &method : super_clazz.methods)
                                {
                                    if (method.name == function.name)
                                    {
                                        dart_function += "@override\n";
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        std::string return_type = "";
        return_type = RenderSimpleType(function.return_type).rendered_content;

        if (IsNullableReturnType(function.return_type))
        {
            return_type = return_type + "?";
        }

        if (should_apped_async)
        {
            return_type = "Future<" + return_type + ">";
        }

        std::string function_name = RenderNonTypeName(function.name).rendered_content;

        std::vector<SyntaxRender::RenderedBlock> params;
        bool has_default_value = false;
        // bool has_nullable_variable = false;
        bool suffix_required_keyword = function.parameters.size() > 1;
        for (auto &param : function.parameters)
        {

            auto p = RenderVariable(param, suffix_required_keyword, true);
            has_default_value = HasDefaultValue(param);
            // has_nullable_variable = IsNullableVariable(param);
            params.push_back(p);
        }
        dart_function += return_type + " " + function_name + "(";
        if (!params.empty())
        {
            if (has_default_value || params.size() > 1)
            {
                dart_function += "{";
                dart_function += JoinToString(RenderedBlocksToStringList(params), ", ");
                dart_function += "}";
            }
            else
            {
                dart_function += JoinToString(RenderedBlocksToStringList(params), ", ");
            }
        }

        if (is_impl && should_apped_async)
        {
            dart_function += ") async\n";
        }
        else
        {
            dart_function += ")\n";
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_function;

        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderVariable(const Variable &variable, bool suffix_required_keyword = false, bool append_default_value = false)
    {
        std::string type_name = RenderSimpleType(variable.type).rendered_content;

        std::string variable_name = RenderNonTypeName(variable.name).rendered_content;
        if (variable_name.find('_') != std::string::npos)
        {
            variable_name = NameWithUnderscoresToCamelCase(variable_name, false);
        }

        std::string dart_member = "";

        if (append_default_value)
        {
            if (!variable.default_value.empty())
            {
                // Nullable type
                if (IsNullableVariable(variable))
                {
                    dart_member += type_name + "? " + variable_name;
                }
                else
                {
                    Enumz enumz;
                    Struct structt;
                    if (FindEnumz(variable.type.name, enumz) == 0)
                    {
                        std::string dv = NameWithUnderscoresToCamelCase(TrimNamespaces(variable.default_value), false);
                        dart_member += type_name + " " + variable_name + " = " + type_name + "." + dv;
                    }
                    else if (FindStruct(variable.type.name, structt) == 0)
                    {
                        std::string dv = RenderTypeName(variable.type.name).rendered_content;
                        dart_member += type_name + " " + variable_name + " = " + "const " + dv + "()";
                    }
                    else
                    {
                        dart_member += type_name + " " + variable_name + " = " + variable.default_value; // RenderTypeName(variable.default_value).rendered_content;
                    }
                }
            }
            else
            {
                if (suffix_required_keyword)
                {
                    dart_member += "required ";
                }
                dart_member += type_name + " " + variable_name;
            }
        }
        else
        {
            if (suffix_required_keyword)
            {
                dart_member += "required ";
            }
            dart_member += type_name + " " + variable_name;
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_member;

        return rendered_block;
    }

    virtual bool ShouldRender(const CXXFile &file) override
    {
        file_path_ = file.file_path;
        return true;
    }

    virtual RenderedBlock RenderedFileName(const std::string &file_path) override
    {
        std::filesystem::path p(file_path);

        std::string base_name = UpperCamelCaseToLowercaseWithUnderscores(p.stem());
        if (base_name[0] == 'i')
        {
            base_name = std::string(base_name.begin() + 1, base_name.end());
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = std::string(base_name + ".dart");
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderIncludeDirectives(const CXXFile &file, const std::vector<IncludeDirective> &include_directives) override
    {
        std::filesystem::path p(file_path_);

        std::string base_name = UpperCamelCaseToLowercaseWithUnderscores(p.stem());
        if (base_name[0] == 'i')
        {
            base_name = std::string(base_name.begin() + 1, base_name.end());
        }

        std::string dart_import = "import 'package:agora_rtc_engine/src/binding_forward_export.dart';\n";

        if (std::find_if(file.nodes.begin(), file.nodes.end(), [&](const NodeType &node)
                         { return std::holds_alternative<Struct>(node) || std::holds_alternative<Enumz>(node); }) != file.nodes.end())
        {
            dart_import += "part \'" + base_name + ".g.dart\';\n";
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_import;
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderStructConstructor(const Struct &structt, const Constructor &constructor) override
    {

        // std::vector<std::string> params;
        // for (auto &param : constructor.parameters)
        // {
        //     std::string p = "this." + RenderNonTypeName(param.name).rendered_content + ",";
        //     params.push_back(p);
        // }

        // std::string dart_constructor = "const " + RenderTypeName(constructor.name).rendered_content + "(";
        // if (!params.empty())
        // {
        //     dart_constructor += "{";
        //     dart_constructor += JoinToString(params, ", ");
        //     dart_constructor += "}";
        // }
        // dart_constructor += ");\n";

        SyntaxRender::RenderedBlock rendered_block;
        // rendered_block.rendered_content = dart_constructor;
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderClassConstructor(const Clazz &clazz, const Constructor &constructor) override
    {
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderMemberVariable(const NodeType &parent, const MemberVariable &member_variable) override
    {
        bool isIgnoreJsonClass = false;
        if (std::holds_alternative<Struct>(parent))
        {
            Struct structt = std::get<Struct>(parent);
            std::string class_with_ns = structt.GetFullName();
            if (std::find(IgnoreJsonClass.begin(), IgnoreJsonClass.end(), class_with_ns) != IgnoreJsonClass.end())
            {
                isIgnoreJsonClass = true;
            }
        }

        std::string dart_member = "";
        // dart_member += ApiTagMemberVariable(parent, member_variable) + "\n";

        std::string variable_type = RenderSimpleType(member_variable.type).rendered_content;
        if (!isIgnoreJsonClass)
        {
            std::string ignore_json = "";
            bool isClazz = false;
            if (std::find(IgnoreJsonType.begin(), IgnoreJsonType.end(), variable_type) != IgnoreJsonType.end())
            {
                ignore_json += ", ignore: true";
            } else {
                Clazz clazz;
                if (FindClass(TrimDummyType(GetTypeNameFromSimpleType(member_variable.type)), clazz) == 0)
                {
                    isClazz = true;
                }
            }

            dart_member += "@JsonKey(name: '" + member_variable.name + "'" + " " + ignore_json + ")\n";
            if (isClazz) {
                // Add custom converter
                dart_member += "@" + variable_type + "Converter()\n";
            }
        }

        dart_member += "final " + variable_type + "? " + RenderNonTypeName(member_variable.name).rendered_content + ";";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_member;

        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderMemberFunction(const NodeType &parent, const MemberFunction &member_function) override
    {
        std::string dart_function = "";

        std::string return_type = "";
        return_type = RenderSimpleType(member_function.return_type).rendered_content;

        std::vector<SyntaxRender::RenderedBlock> params;

        if (IsEventHandler(parent))
        {
            std::string function_name = RenderNonTypeName(member_function.name).rendered_content;
            // dart_function += ApiTagMemberFunction(parent, member_function, true) + "\n";
            for (auto &param : member_function.parameters)
            {
                auto p = RenderVariable(param);
                params.push_back(p);
            }
            dart_function += "final " + return_type + " " + "Function(";
            dart_function += JoinToString(RenderedBlocksToStringList(params), ", ");
            dart_function += ")?\n";
            dart_function += " " + function_name + ";\n";
        }
        else
        {
            // dart_function += ApiTagMemberFunction(parent, member_function, false) + "\n";
            dart_function += RenderFunctionSignature(parent, member_function).rendered_content;
            dart_function += "\n";
            dart_function += ";\n";
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_function;
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderClass(const Clazz &original_clazz, const std::vector<SyntaxRender::RenderedBlock> &rendered_class_members) override
    {
        std::string dart_class = "";
        // dart_class += ApiTagClass(original_clazz) + "\n";

        if (IsEventHandler(original_clazz))
        {
            std::string class_name = RenderTypeName(original_clazz.name).rendered_content;
            if (class_name[0] == 'I')
            {
                class_name = std::string(class_name.begin() + 1, class_name.end());
            }
            std::string extends_block = "";
            std::string super_constructor = "";
            std::string super_class_declare_member = "";
            if (!original_clazz.base_clazzs.empty())
            {
                std::string base_clazz_name = original_clazz.base_clazzs[0];
                std::string base_clazz_name_new = base_clazz_name;
                if (base_clazz_name_new[0] == 'I')
                {
                    base_clazz_name_new = std::string(base_clazz_name_new.begin() + 1, base_clazz_name_new.end());
                }

                extends_block += " extends " + base_clazz_name_new;
                Clazz super_clazz;
                if (FindClass(base_clazz_name, super_clazz) == 0)
                {
                    std::vector<std::string> super_class_member_names;
                    std::vector<std::string> super_class_declare_member_names;

                    if (!super_clazz.methods.empty())
                    {
                        for (auto &method : super_clazz.methods)
                        {
                            super_class_member_names.push_back(std::string(RenderNonTypeName(method.name).rendered_content + " : " + RenderNonTypeName(method.name).rendered_content));

                            std::vector<SyntaxRender::RenderedBlock> params;
                            for (auto &param : method.parameters)
                            {
                                auto p = RenderVariable(param);
                                params.push_back(p);
                            }

                            std::string m = RenderSimpleType(method.return_type).rendered_content + " " + "Function(" + JoinToString(RenderedBlocksToStringList(params), ", ") + ")? " + RenderNonTypeName(method.name).rendered_content;

                            super_class_declare_member_names.push_back(m);
                        }

                        super_class_declare_member += JoinToString(super_class_declare_member_names, ", ");
                        super_class_declare_member += "\n";
                        super_class_declare_member += ",\n";

                        super_constructor += "super(\n";
                        // super_constructor += "{";
                        super_constructor += JoinToString(super_class_member_names, ", ");
                        super_constructor += ",\n";
                        // super_constructor += "}";
                        super_constructor += ");\n";
                    }
                }
                // base_class_block += " implements " + JoinToString(original_clazz.base_clazzs, ",");
            }

            dart_class += "class " + class_name + extends_block + " {\n";
            dart_class += "/// Construct the [" + class_name + "].\n";
            std::string dart_constructor = "const " + class_name + "(";
            dart_constructor += "{";
            if (!original_clazz.methods.empty())
            {
                std::vector<std::string> member_names;
                for (auto &method : original_clazz.methods)
                {
                    std::string m = "this." + RenderNonTypeName(method.name).rendered_content;
                    member_names.push_back(m);
                }

                dart_constructor += super_class_declare_member;
                dart_constructor += JoinToString(member_names, ", ");
                dart_constructor += ",";
            }
            dart_constructor += "}";
            dart_constructor += ")\n";
            if (!super_constructor.empty())
            {
                dart_constructor += " : " + super_constructor;
            }
            else
            {
                dart_constructor += ";\n";
            }

            dart_class += dart_constructor;
            dart_class += "\n";
            dart_class += JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
            dart_class += "}\n";
        }
        else
        {

            std::string base_class_block = "";
            if (!original_clazz.base_clazzs.empty())
            {
                std::vector<std::string> base_clazzs_new = std::vector<std::string>(original_clazz.base_clazzs);
                std::transform(base_clazzs_new.begin(), base_clazzs_new.end(), base_clazzs_new.begin(),
                               [&](std::string s) -> std::string
                               {
                                   std::string type_name = RenderTypeName(s).rendered_content;
                                   if (type_name[0] == 'I')
                                   {
                                       type_name = std::string(type_name.begin() + 1, type_name.end());
                                   }
                                   return type_name;
                               });

                base_class_block += " implements " + JoinToString(base_clazzs_new, ",");
            }

            dart_class += "abstract class " + RenderClassName(original_clazz).rendered_content + base_class_block + " {\n";
            dart_class += JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
            dart_class += "}\n";
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_class;
        return rendered_block;
    }

    virtual RenderedBlock RenderStruct(const Struct &original_struct, const std::vector<RenderedBlock> &rendered_struct_members) override
    {
        std::cout << "Struct: " << original_struct.name << std::endl;
        std::string struct_name = RenderTypeName(original_struct.name).rendered_content;

        bool isIgnoreJsonClass = false;
        std::string full_name = original_struct.GetFullName();

        if (std::find(IgnoreJsonClass.begin(), IgnoreJsonClass.end(), full_name) != IgnoreJsonClass.end())
        {
            isIgnoreJsonClass = true;
        }

        if (std::find(IgnoreDartStyleType.begin(), IgnoreDartStyleType.end(), full_name) != IgnoreDartStyleType.end())
        {
            struct_name = original_struct.name;
        }
        else
        {
            struct_name = RenderTypeName(original_struct.name).rendered_content;
        }

        std::string dart_class = "";
        // dart_class += ApiTagStruct(original_struct) + "\n";

        if (!isIgnoreJsonClass)
        {
            dart_class += "@JsonSerializable(explicitToJson: true, includeIfNull: false)\n";
        }

        dart_class += "class " + struct_name + " {\n";

        std::string dart_constructor = "";
        // dart_constructor += "/// Construct the [" + struct_name + "].\n";
        dart_constructor += "const " + struct_name + "(";
        if (!original_struct.member_variables.empty())
        {
            std::vector<std::string> member_names;
            for (auto &member : original_struct.member_variables)
            {
                std::string m = "this." + RenderNonTypeName(member.name).rendered_content;
                member_names.push_back(m);
            }

            dart_constructor += "{";
            dart_constructor += JoinToString(member_names, ", ");
            dart_constructor += "}";
        }
        dart_constructor += ");\n";

        dart_class += dart_constructor;
        dart_class += "\n";

        dart_class += JoinToString(RenderedBlocksToStringList(rendered_struct_members), "\n\n");
        dart_class += "\n";

        // dart_class += "/// @nodoc \n";

        if (!isIgnoreJsonClass)
        {
            dart_class += "factory " + struct_name + ".fromJson(Map<String, dynamic> json) => _$" + struct_name + "FromJson(json);\n";

            // dart_class += "/// @nodoc \n";
            dart_class += "Map<String, dynamic> toJson() => _$" + struct_name + "ToJson(this);\n";
        }

        dart_class += "}\n";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_class;
        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderEnumConstant(const EnumConstant &enum_const) override
    {
        std::string constant_name = "";
        // constant_name += ApiTagEnumConstant(enum_const.parent_name, enum_const) + "\n";

        std::string constant_value = enum_const.value;
        if (constant_value.empty())
        {
            constant_value = "-1";
        }
        // else
        // {
        //     if (constant_value != "-1" &&
        //         constant_value.find("0x") == std::string::npos &&
        //         constant_value.find("<<") == std::string::npos &&
        //         std::find_if(constant_value.begin(),
        //                      constant_value.end(), [](unsigned char c)
        //                      { return !std::isdigit(c); }) != constant_value.end())
        //     {
        //         constant_value = "\'" + constant_value + "\'";
        //     }
        // }
        constant_name += "@JsonValue(" + constant_value + ")\n";
        constant_name += RenderNonTypeName(enum_const.name).rendered_content + ",";

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = constant_name;

        return rendered_block;
    }

    virtual SyntaxRender::RenderedBlock RenderEnum(const Enumz &enumz, const std::vector<RenderedBlock> &rendered_enum_consts) override
    {
        std::string dart_enum_name = "";
        dart_enum_name += RenderTypeName(enumz.name).rendered_content;

        if (dart_enum_name.empty())
        {
            // Return nothing
            SyntaxRender::RenderedBlock rendered_block;
            return rendered_block;
        }

        if (dart_enum_name.empty())
        {
            dart_enum_name = enumz.parent_name + "Enum";
        }

        std::string dart_enum = "";
        // dart_enum += ApiTagEnum(enumz) + "\n";
        dart_enum += "@JsonEnum(alwaysCreate: true)\n";
        dart_enum += "enum " + dart_enum_name + " {\n";
        dart_enum += JoinToString(RenderedBlocksToStringList(rendered_enum_consts), "\n\n");
        dart_enum += "\n";
        dart_enum += "}\n";
        dart_enum += "\n";
        dart_enum += "/// Extensions functions of [" + dart_enum_name + "].\n";
        dart_enum += "extension " + dart_enum_name + "Ext on " + dart_enum_name + " {\n";
        dart_enum += "/// @nodoc\n";
        dart_enum += "static " + dart_enum_name + " fromValue(int value) {\n";
        dart_enum += "return $enumDecode(_$" + dart_enum_name + "EnumMap, value);\n";
        dart_enum += "}\n";
        dart_enum += "\n";
        dart_enum += "/// @nodoc\n";
        dart_enum += "int value() {\n";
        dart_enum += "return _$" + dart_enum_name + "EnumMap[this]!;\n";
        dart_enum += "}\n";
        dart_enum += "}\n";

        // extension QualityAdaptIndicationExt on QualityAdaptIndication {
        //     int jsonValue() {
        //         return _$QualityAdaptIndicationEnumMap[this]!;
        //     }
        //     }

        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_enum;
        return rendered_block;
    }

    virtual RenderedBlock RenderTopLevelVariable(const Variable &top_level_variable) override
    {
        const std::string unsigned_int_max_expression = "(std::numeric_limits<unsigned int>::max)()";
        std::string dart_const = "";
        dart_const += "/// @nodoc \n";
        std::string default_value = top_level_variable.default_value;
        if (default_value == unsigned_int_max_expression)
        {
            default_value = std::to_string((std::numeric_limits<unsigned int>::max)());
        }
        // (std::numeric_limits<unsigned int>::max)()

        dart_const += "const " + RenderNonTypeName(top_level_variable.name).rendered_content + " = " + default_value + ";\n";

        SyntaxRender::RenderedBlock render_block;
        render_block.rendered_content = dart_const;
        return render_block;
    }

    virtual void FormatCode(const std::string &file_path) override
    {
        std::string command = "dart format " + file_path;
        system(command.c_str());
    }
};

#endif // DART_SYNTAX_RENDER_H_