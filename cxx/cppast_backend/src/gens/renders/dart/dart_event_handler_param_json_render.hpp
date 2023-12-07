#ifndef DART_EVENT_HANDLER_PARAM_JSON_RENDER_H_
#define DART_EVENT_HANDLER_PARAM_JSON_RENDER_H_

#include "dart_syntax_render.hpp"
#include "../base_syntax_render.hpp"
#include "terra_generator.hpp"
#include "terra_node.hpp"
#include <stdlib.h>
#include "dart_render_configs.hpp"

using namespace terra;

class DartEventHandlerParamJsonRender : public DartSyntaxRender
{
private:
    std::string file_name_ = "event_handler_param_json.dart";
    std::string part_file_name_ = "event_handler_param_json.g.dart";
    bool should_render_include_directives_ = true;

public:
    bool ShouldRender(const CXXFile &file) override
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
        SyntaxRender::RenderedBlock rendered_block;
        if (!should_render_include_directives_)
        {
            return rendered_block;
        }

        std::string dart_import = "";
        dart_import += dart_header + ", prefer_is_empty";
        dart_import += "\n";

        dart_import += "import 'package:agora_rtc_engine/src/binding_forward_export.dart';\n";
        dart_import += "part \'" + part_file_name_ + "\';\n";
        dart_import += "\n";

        rendered_block.rendered_content = dart_import;
        should_render_include_directives_ = false;
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
        // rendered_block.rendered_content = std::string("impl/" + base_name + "_json.dart");
        rendered_block.rendered_content = std::string("binding/" + file_name_);
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderMemberVariable(const NodeType &parent, const MemberVariable &member_variable) override
    {
        std::string actual_type = RenderSimpleType(member_variable.type).rendered_content;

        SyntaxRender::RenderedBlock rendered_block = DartSyntaxRender::RenderMemberVariable(parent, member_variable);
        std::string dart_member = "";
        std::string ignore_json = "";
        Clazz clazz;
        if (std::find(IgnoreJsonType.begin(), IgnoreJsonType.end(), actual_type) != IgnoreJsonType.end() || FindClass(TrimDummyType(GetTypeNameFromSimpleType(member_variable.type)), clazz) == 0)
        {
            ignore_json += ", ignore: true";
        }
        dart_member += "@JsonKey(name: '" + member_variable.name + "'" + ignore_json + ")\n";
        dart_member += "final " + actual_type + "? " + RenderNonTypeName(member_variable.name).rendered_content + ";";

        rendered_block.rendered_content = dart_member;

        return rendered_block;
    }

    RenderedBlock RenderStruct(const Struct &original_struct, const std::vector<RenderedBlock> &rendered_struct_members) override
    {
        std::string struct_name = RenderTypeName(original_struct.name).rendered_content + "Json";
        std::string dart_class = "";
        dart_class += "@JsonSerializable(explicitToJson: true)\n";
        dart_class += "class " + struct_name + " {\n";

        std::string dart_constructor = "const " + struct_name + "(";
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

        dart_class += JoinToString(RenderedBlocksToStringList(rendered_struct_members), "\n");
        dart_class += "\n";

        dart_class += "factory " + struct_name + ".fromJson(Map<String, dynamic> json) => _$" + struct_name + "FromJson(json);\n";

        dart_class += "Map<String, dynamic> toJson() => _$" + struct_name + "ToJson(this);\n";

        dart_class += "\n";
        dart_class += "}\n";

        SyntaxRender::RenderedBlock rendered_block;
        // rendered_block.rendered_content = dart_class;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderEnumConstant(const EnumConstant &enum_const) override
    {
        SyntaxRender::RenderedBlock rendered_block = DartSyntaxRender::RenderEnumConstant(enum_const);
        std::string constant_name = "@JsonValue(" + enum_const.value + ")\n";
        constant_name += rendered_block.rendered_content;

        // rendered_block.rendered_content = constant_name;
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
        if (std::find(event_handler_class_.begin(), event_handler_class_.end(), class_with_ns) == event_handler_class_.end())
        {
            return rendered_block;
        }

        rendered_block.rendered_content = JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderMemberFunction(const NodeType &parent, const MemberFunction &member_function) override
    {

        std::string struct_name = "";

        std::vector<std::string> named_param_names;
        std::vector<std::string> buffer_named_param_names;

        if (std::holds_alternative<Clazz>(parent))
        {
            const Clazz &s = std::get<Clazz>(parent);
            struct_name += RenderClassName(s).rendered_content;
        }

        std::string member_function_name = member_function.name;
        member_function_name[0] = std::toupper(member_function_name[0]);
        struct_name += RenderTypeName(member_function_name).rendered_content + "Json";

        std::string dart_class = "";
        dart_class += "@JsonSerializable(explicitToJson: true)\n";
        dart_class += "class " + struct_name + " {\n";

        std::vector<SyntaxRender::RenderedBlock> params_render_block;
        std::string dart_constructor = "const " + struct_name + "(";
        if (!member_function.parameters.empty())
        {
            std::vector<std::string> member_names;
            for (auto &member : member_function.parameters)
            {
                std::string m = "this." + RenderNonTypeName(member.name).rendered_content;
                member_names.push_back(m);

                MemberVariable member_variable;
                member_variable.name = member.name;
                member_variable.type = member.type;

                params_render_block.push_back(RenderMemberVariable(parent, member_variable));
            }

            dart_constructor += "{";
            dart_constructor += JoinToString(member_names, ", ");
            dart_constructor += "}";
        }
        dart_constructor += ");\n";

        dart_class += dart_constructor;
        dart_class += "\n";

        dart_class += JoinToString(RenderedBlocksToStringList(params_render_block), "\n");
        dart_class += "\n";

        dart_class += "factory " + struct_name + ".fromJson(Map<String, dynamic> json) => _$" + struct_name + "FromJson(json);\n";

        dart_class += "Map<String, dynamic> toJson() => _$" + struct_name + "ToJson(this);\n";

        dart_class += "\n";
        dart_class += "}\n";

        auto member_variables = VariablesToMemberVariable(member_function.parameters);
        dart_class += RenderCallApiEventHandlerBufferExt(struct_name, member_variables).rendered_content;

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_class;

        return rendered_block;
    }

    virtual void OnRenderFilesStart(const ParseResult &parse_result, const std::string &output_dir) override
    {
        std::filesystem::path outdir(output_dir);
        std::filesystem::path full_path = outdir / "binding" / file_name_;

        std::filesystem::remove(full_path.c_str());

        std::cout << "file path: " << full_path.c_str() << std::endl;
    }

    virtual void SaveRenderBlocks(const std::string &output_dir, const std::string &full_path, const std::string &render_contents) override
    {
        std::filesystem::path outdir(output_dir);
        std::filesystem::path actual_full_path = outdir / "binding" / file_name_;

        std::ofstream fileSink;
        fileSink.open(actual_full_path.c_str(), std::ios_base::app);
        fileSink << render_contents;
        fileSink.flush();
    }
};

#endif // DART_EVENT_HANDLER_PARAM_JSON_RENDER_H_