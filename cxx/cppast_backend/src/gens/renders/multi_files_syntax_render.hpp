#ifndef MULTI_FILES_SYNTAX_RENDER_H_
#define MULTI_FILES_SYNTAX_RENDER_H_

#include "terra.hpp"
#include "terra_generator.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>

using namespace terra;

class MultiFilesSyntaxRender : public SyntaxRender
{
private:
    std::vector<std::shared_ptr<SyntaxRender>> syntax_renders_;

public:
    MultiFilesSyntaxRender(std::vector<std::shared_ptr<SyntaxRender>> &syntax_renders) : syntax_renders_(std::move(syntax_renders)) {}

            void OnRenderFilesStart(const ParseResult &parse_result, const std::string &output_dir) override {
                        for (auto &syntax_render : syntax_renders_)
        {
            syntax_render.get()->OnRenderFilesStart(parse_result, output_dir);
        }
            }

        void OnRenderFilesEnd(const ParseResult &parse_result, const std::string &output_dir) override {
                                    for (auto &syntax_render : syntax_renders_)
        {
            syntax_render.get()->OnRenderFilesEnd(parse_result, output_dir);
        }
        }

    bool ShouldRender(const CXXFile &file) override
    {
        return true;
    }

    SyntaxRender::RenderedBlock RenderedFileName(const std::string &file_path) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderIncludeDirectives(const CXXFile &file, const std::vector<IncludeDirective> &include_directives) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderStructConstructor(const Struct &structt, const Constructor &constructor) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderClassConstructor(const Clazz &clazz, const Constructor &constructor) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderMemberVariable(const NodeType &parent, const MemberVariable &member_variable) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderMemberFunction(const NodeType &parent, const MemberFunction &member_function) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderClass(const Clazz &original_clazz, const std::vector<SyntaxRender::RenderedBlock> &rendered_class_members) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    RenderedBlock RenderStruct(const Struct &original_struct, const std::vector<RenderedBlock> &rendered_struct_members) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderEnumConstant(const EnumConstant &enum_const) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    SyntaxRender::RenderedBlock RenderEnum(const Enumz &enumz, const std::vector<RenderedBlock> &rendered_enum_consts) override
    {
        // Return nothing
        SyntaxRender::RenderedBlock rendered_block;
        return rendered_block;
    }

    void SetParseResult(ParseResult parse_result) override {
        SyntaxRender::SetParseResult(parse_result);
        for (auto &syntax_render : syntax_renders_)
        {
          syntax_render.get()->SetParseResult(GetParseResult());
        }
    }

    void Render(const ParseResult &parse_result, const CXXFile &file, const std::string &output_dir) override
    {
        for (auto &syntax_render : syntax_renders_)
        {
            syntax_render.get()->Render(parse_result, file, output_dir);
        }
    }
};

#endif // MULTI_FILES_SYNTAX_RENDER_H_