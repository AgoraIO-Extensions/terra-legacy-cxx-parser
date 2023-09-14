#ifndef FAKE_IMPL_RENDER_H_
#define FAKE_IMPL_RENDER_H_

#include "terra.hpp"
#include "terra_generator.hpp"
#include "../renders/base_syntax_render.hpp"
#include "fake_impl_render_configs.hpp"
#include <filesystem>

using namespace terra;

class FakeImplRender : public BaseSyntaxRender
{
private:
    std::string out_dir_;

private:
    bool IsSameMemberFunction(const MemberFunction &func1, const MemberFunction &func2)
    {
        bool isSame = func1.name == func2.name && func1.parameters.size() == func2.parameters.size();
        if (!isSame)
            return isSame;
        for (int i = 0; i < func1.parameters.size(); i++)
        {
            isSame = isSame && BaseSyntaxRender::TrimNamespaces(func1.parameters[i].type.GetTypeName()) == BaseSyntaxRender::TrimNamespaces(func2.parameters[i].type.GetTypeName());
        }

        return isSame;
    }

    void MergeFunctions(std::vector<MemberFunction> &to, const std::vector<MemberFunction> &from)
    {
        for (auto &it : from)
        {
            if (std::find_if(to.begin(), to.end(),
                             [&](MemberFunction &item)
                             {
                                 return IsSameMemberFunction(it, item);
                             }) == to.end())
            {
                to.push_back(it);
            }
        }
    }

public:
    FakeImplRender(std::string out_dir)
    {
        out_dir_ = out_dir;
    }

    std::vector<MemberFunction> FindAllMethods(const Clazz &clazz)
    {
        std::vector<MemberFunction> methods;

        if (!clazz.base_clazzs.empty())
        {

            for (auto &base_clazz_name : clazz.base_clazzs)
            {
                Clazz super_clazz;
                if (FindClass(base_clazz_name, super_clazz) == 0)
                {
                    auto base_clazz_methods = FindAllMethods(super_clazz);
                    if (!base_clazz_methods.empty())
                    {
                        MergeFunctions(methods, base_clazz_methods);
                    }

                    MergeFunctions(methods, super_clazz.methods);
                }
            }
        }

        if (!clazz.methods.empty())
        {
            MergeFunctions(methods, clazz.methods);
        }

        return methods;
    }

    void Render(const ParseResult &parse_result, const CXXFile &file, const std::string &output_dir) override final
    {
        for (auto &node : file.nodes)
        {
            if (std::holds_alternative<Clazz>(node))
            {
                const Clazz clazz = std::get<Clazz>(node);
                std::string class_with_ns = NameWithNamespace(clazz.name, clazz.namespaces);

                if (std::find(FakeImplClasses.begin(), FakeImplClasses.end(), class_with_ns) == FakeImplClasses.end())
                {
                    continue;
                }

                std::string class_body = "";

                std::string class_name = "Fake" + clazz.name + "Internal";

                std::string ifndef_h = "FAKE_" + ToUpper(clazz.name) + "_INTERNAL_H_";

                std::filesystem::path p(file.file_path);
                std::string include_header;
                std::string the_include_header = p.filename();
                include_header += "#include \"" + the_include_header + "\"\n";

                class_body += "class " + class_name + " : public " + class_with_ns;
                class_body += "{";

                std::vector<MemberFunction> methods = FindAllMethods(clazz);

                for (auto &mf : methods)
                {
                    std::vector<std::string> params;
                    for (auto &p : mf.parameters)
                    {
                        std::string pt = "";
                        if (p.type.is_builtin_type && p.type.kind == SimpleTypeKind::array_t)
                        {
                            pt = p.type.name + "*";
                            if (p.type.is_const)
                            {
                                pt = "const " + pt;
                            }
                        }
                        else
                        {
                            pt = p.type.source;
                        }

                        std::string ps = pt + " " + p.name;
                        params.push_back(ps);
                    }

                    std::string comment = mf.comment;
                    std::string function_prefix = "";
                    std::string function_surffix = "";
                    if (comment.find("#if") != std::string::npos)
                    {
                        std::vector<std::string> comment_list = Split(comment, "\n");
                        for (auto &c : comment_list)
                        {
                            function_prefix += c + "\n";
                            function_surffix += "#endif\n";
                        }
                    }

                    std::string pl = JoinToString(params, ", ");

                    std::string function_const = "";
                    if (mf.is_const)
                    {
                        function_const = " const";
                    }

                    class_body += function_prefix;
                    class_body += "virtual " + mf.return_type.source + " " + mf.name + "(" + pl + ") " + function_const + " override {\n";

                    Enumz enumz;
                    if (FindEnumz(mf.return_type.source, enumz) == 0)
                    {
                        class_body += "return " + mf.return_type.source + "::" + enumz.enum_constants[0].name + ";\n";
                    }
                    else if (mf.return_type.is_builtin_type && mf.return_type.source != "void")
                    {
                        if (mf.return_type.name == "char")
                        {
                            class_body += "return \"\";\n";
                        }
                        else
                        {
                            class_body += "return 0;\n";
                        }
                    }
                    else if (mf.return_type.kind == SimpleTypeKind::pointer_t)
                    {
                        Clazz cc;
                        if (FindClass(mf.return_type.source, cc) == 0)
                        {
                            std::filesystem::path p(cc.file_path);
                            std::string h = p.filename();

                            include_header += "#include \"" + h + "\"\n";
                        }

                        class_body += "return 0;\n";
                    }
                    else if (mf.return_type.source != "void")
                    {
                        std::string type_name = mf.return_type.source;
                        Replace(type_name, "agora_refptr<", "");
                        Replace(type_name, ">", "");

                        Clazz cc;
                        if (FindClass(type_name, cc) == 0)
                        {
                            std::filesystem::path p(cc.file_path);
                            std::string h = p.filename();

                            include_header += "#include \"" + h + "\"\n";
                        }

                        class_body += mf.return_type.source + " the_return;\n";
                        class_body += "return the_return;\n";
                    }

                    class_body += "}\n";
                    class_body += function_surffix;
                    class_body += "\n";
                }

                class_body += "};";

                // Replace(fake_irtcengine_template, "{{TEMPLATE_CONTENT}}", class_body);

                std::string output_file_content = "";
                output_file_content += "/// Generated by terra, DO NOT MODIFY BY HAND.\n";
                output_file_content += "\n";
                output_file_content += "#ifndef " + ifndef_h + "\n";
                output_file_content += "#define " + ifndef_h + "\n";
                output_file_content += "\n";
                output_file_content += "\n";
                // output_file_content += "#include \"" + include_header + "\"";
                output_file_content += include_header;
                output_file_content += "\n";
                output_file_content += "\n";

                for (auto &ns : clazz.namespaces)
                {
                    output_file_content += "namespace " + ns + " {\n";
                }

                output_file_content += class_body;
                output_file_content += "\n";
                output_file_content += "\n";

                for (auto i = clazz.namespaces.rbegin();
                     i != clazz.namespaces.rend(); ++i)
                {

                    output_file_content += "}\n";
                }

                output_file_content += "\n";
                output_file_content += "#endif // " + ifndef_h + "\n";

                std::string output_file_name = "fake_" + ToLower(clazz.name) + "_internal.hpp";

                std::string out_path = out_dir_ + "/" + output_file_name;
                std::ofstream fileSink;
                fileSink.open(out_path.c_str());
                fileSink << output_file_content;
                fileSink.flush();

                std::string format_command = "clang-format --Werror -i " + out_path;
                system(format_command.c_str());

                continue;
            }
        }
    }
};

#endif // FAKE_IMPL_RENDER_H_