#ifndef DART_STRUCT_TO_JSON_SERIALIZABLE_RENDER_H_
#define DART_STRUCT_TO_JSON_SERIALIZABLE_RENDER_H_

#include "dart_syntax_render.hpp"
#include "../base_syntax_render.hpp"
#include "terra_generator.hpp"
#include "terra_node.hpp"
#include <stdlib.h>

using namespace terra;

class DartStructToJsonSerializableRender : public DartSyntaxRender
{
private:
    std::string file_name_ = "call_api_impl_params_json.dart";
    std::string part_file_name_ = "call_api_impl_params_json.g.dart";
    bool should_render_include_directives_ = true;

    bool ShouldRender(const CXXFile &file) override
    {

        return std::find_if(file.nodes.begin(), file.nodes.end(), [&](const auto &node)
                            {
                                bool should_render = false;
                                if (std::holds_alternative<Clazz>(node))
                                {
                                    Clazz clazz = std::get<Clazz>(node);

                                    for (auto &member_function : clazz.methods)
                                    {
                                        if (member_function.user_data.has_value())
                                        {
                                            try
                                            {
                                                const Variable &variable_for_return = std::any_cast<Variable>(member_function.user_data);
                                                should_render = true;
                                                break;
                                            }
                                            catch (const std::bad_any_cast &e)
                                            {
                                                std::cout << e.what() << '\n';
                                            }
                                        }
                                    }

                                    return should_render;
                            } }) != file.nodes.end();
    }

    virtual SyntaxRender::RenderedBlock RenderIncludeDirectives(const CXXFile &file, const std::vector<IncludeDirective> &include_directives) override
    {
        SyntaxRender::RenderedBlock rendered_block;
        if (!should_render_include_directives_)
        {
            return rendered_block;
        }

        std::string dart_import = "";
        dart_import += dart_header;
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
        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = std::string("binding/" + file_name_);
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

    SyntaxRender::RenderedBlock RenderClass(const Clazz &original_clazz, const std::vector<SyntaxRender::RenderedBlock> &rendered_class_members) override
    {
        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = JoinToString(RenderedBlocksToStringList(rendered_class_members), "\n");
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderMemberFunction(const NodeType &parent, const MemberFunction &member_function) override
    {
        std::string member_function_name = member_function.name;
        member_function_name[0] = std::toupper(member_function_name[0]);

        std::string struct_name = "";
        if (std::holds_alternative<Clazz>(parent))
        {
            const Clazz &s = std::get<Clazz>(parent);
            struct_name += RenderClassName(s).rendered_content;
        }
        struct_name += RenderTypeName(member_function_name).rendered_content + "Json";
        std::string dart_class = "";

        if (member_function.user_data.has_value())
        {
            try
            {
                const Variable &variable_for_return = std::any_cast<Variable>(member_function.user_data);

                dart_class += "@JsonSerializable(explicitToJson: true)\n";
                dart_class += "class " + struct_name + " {\n";

                std::vector<SyntaxRender::RenderedBlock> params_render_block;
                std::string dart_constructor = "const " + struct_name + "(";
                dart_constructor += "this." + RenderNonTypeName(variable_for_return.name).rendered_content;

                dart_constructor += ");\n";

                dart_class += dart_constructor;
                dart_class += "\n";

                std::string dart_member = "";
                dart_member += "@JsonKey(name: '" + variable_for_return.name + "')\n";
                dart_member += "final " + RenderSimpleType(variable_for_return.type).rendered_content + " " + RenderNonTypeName(variable_for_return.name).rendered_content + ";";

                dart_class += dart_member;

                dart_class += "factory " + struct_name + ".fromJson(Map<String, dynamic> json) => _$" + struct_name + "FromJson(json);\n";

                dart_class += "Map<String, dynamic> toJson() => _$" + struct_name + "ToJson(this);\n";

                dart_class += "\n";
                dart_class += "}\n";
            }
            catch (const std::bad_any_cast &e)
            {
                std::cout << "DartStructToJsonSerializableRender: " << e.what() << '\n';
            }
        }

        SyntaxRender::RenderedBlock rendered_block;
        rendered_block.rendered_content = dart_class;

        return rendered_block;
    }

    virtual void OnRenderFilesStart(const ParseResult &parse_result, const std::string &output_dir) override
    {
        std::filesystem::path outdir(output_dir);
        std::filesystem::path full_path = outdir / "binding" / file_name_;

        std::filesystem::remove(full_path);
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

#endif // DART_STRUCT_TO_JSON_SERIALIZABLE_RENDER_H_