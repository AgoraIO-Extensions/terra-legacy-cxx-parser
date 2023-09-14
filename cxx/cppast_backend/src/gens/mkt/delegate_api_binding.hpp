#ifndef DELEGATE_API_BINDING_H_
#define DELEGATE_API_BINDING_H_

#include "terra.hpp"
#include "terra_generator.hpp"
#include <iostream>

#include <memory>
#include <stdlib.h>
#include <map>
#include <string>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <numeric>
#include <chrono>
#include <thread>

using namespace terra;

class DelegateAPIBindingGenerator : public terra::Generator
{
private:
    std::string JoinToString(std::vector<std::string> list, std::string delimelater)
    {
        if (list.empty()) return "";
        std::ostringstream vts;
        std::copy(list.begin(), list.end() - 1,
                  std::ostream_iterator<std::string>(vts, delimelater.c_str()));
        vts << list.back();
        return vts.str();
    }

    bool Replace(std::string &str, const std::string &from, const std::string &to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

public:
    bool Generate(const ParseResult &parse_result) override
    {
        std::string binding_content_template = R"(
#ifndef FACE_UNITY_EXTENSION_API_BINDING_H_
#define FACE_UNITY_EXTENSION_API_BINDING_H_

#include "face_unity_extension_delegate.h"
#include "extension_delegate.h"
#include <nlohmann/json.hpp>
#include <map>

namespace agora {
namespace rtc {
namespace extension {
    inline void FUExtensionDelegateAPIBinding(FUExtensionDelegate *delegate) {
        {{TEMPLATE_CONTENT}}
    }
}
}
}

#endif // FACE_UNITY_EXTENSION_API_BINDING_H_
        )";

        std::string binding_content = "";

        for (auto &cxx_file : parse_result.cxx_files)
        {
            for (auto &node : cxx_file.nodes)
            {
                if (std::holds_alternative<Clazz>(node))
                {
                    Clazz clazz = std::get<Clazz>(node);
                    if (clazz.name == "FaceUnityExtensionApi")
                    {
                        for (auto &func : clazz.methods)
                        {
                            std::string func_name = func.name;
                            Replace(func_name, "_c", "");

                            binding_content += "delegate->BindPropertyFunc(\n";
                            binding_content += "\"" + func_name + "\",\n";
                            binding_content += "[delegate](const std::string &buf) {\n";
                            binding_content += "nlohmann::json j = nlohmann::json::parse(buf);\n";
                            binding_content += "if (!j.is_object()) { return -ERROR_CODE_TYPE::ERR_INVALID_ARGUMENT; }\n";

                            std::string find_json_key = "";
                            std::string param_declaration = "";
                            std::vector<std::string> param_names;
                            for (auto &param : func.parameters)
                            {
                                std::string param_name = param.name;
                                
                                find_json_key += "if (j.find(\"" + param_name + "\") == j.end()) { return -ERROR_CODE_TYPE::ERR_INVALID_ARGUMENT; }\n";
                                param_declaration += param.type.name + " " + param_name + " = j[\"" + param_name + "\"].get<" + param.type.name + ">();\n";
                                param_names.push_back(param_name);
                            }

                            binding_content += find_json_key + "\n";
                            binding_content += param_declaration + "\n";
                            binding_content += "delegate->C_API(" + func_name + ")(" + JoinToString(param_names, ", ") + ");\n";
                            binding_content += "return 0;\n";

                            binding_content += "});\n";
                        }
                    }
                }
            }
        }

        Replace(binding_content_template, "{{TEMPLATE_CONTENT}}", binding_content);

        std::string out_path = "/Users/fenglang/codes/aw/market-place/FaceUnity/src/face_unity_extension_api_binding.h";
        std::ofstream fileSink;
        fileSink.open(out_path.c_str());
        fileSink << binding_content_template;
        fileSink.flush();

        std::string format_command = "clang-format --Werror -i " + out_path;
        system(format_command.c_str());

        return true;
    }
};

#endif // DELEGATE_API_BINDING_H_