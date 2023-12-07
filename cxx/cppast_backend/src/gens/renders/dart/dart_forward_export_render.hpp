#ifndef DART_FORWARD_EXPORT_RENDER_H_
#define DART_FORWARD_EXPORT_RENDER_H_

#include "../base_syntax_render.hpp"
#include "dart_callapi_render.hpp"
#include "dart_event_handler_render.hpp"
#include "dart_syntax_render.hpp"
#include "terra_generator.hpp"
#include "terra_node.hpp"
#include <stdlib.h>
#include "dart_render_configs.hpp"

class DartForwardExportRender : public DartSyntaxRender
{
private:
    std::shared_ptr<DartSyntaxRender> dart_syntax_render_;
    std::shared_ptr<DartEventHandlerRender> dart_event_handler_render_;
    std::shared_ptr<DartCallApiRender> dart_callapi_render_;

public:
    DartForwardExportRender() {}
    DartForwardExportRender(std::shared_ptr<DartSyntaxRender> dart_syntax_render,
                            std::shared_ptr<DartEventHandlerRender> dart_event_handler_render,
                            std::shared_ptr<DartCallApiRender> dart_callapi_render) : dart_syntax_render_(std::move(dart_syntax_render)),
                                                                                      dart_event_handler_render_(std::move(dart_event_handler_render)),
                                                                                      dart_callapi_render_(std::move(dart_callapi_render))
    {
    }

    void Render(const ParseResult &parse_result, const CXXFile &file, const std::string &output_dir) override final {}

    void OnRenderFilesEnd(const ParseResult &parse_result, const std::string &output_dir) override
    {

        std::string binding_export = "";
        std::string impl_export = "";
        for (auto &file : parse_result.cxx_files)
        {
            if (dart_syntax_render_.get()->ShouldRender(file))
            {
                std::string file_name = dart_syntax_render_.get()->RenderedFileName(file.file_path).rendered_content;
                binding_export += "export '" + file_name + "';\n";
            }

            if (dart_callapi_render_.get()->ShouldRender(file))
            {
                std::string file_name = dart_callapi_render_.get()->RenderedFileName(file.file_path).rendered_content;
                Replace(file_name, "binding/", "");
                impl_export += "export '" + file_name + "';\n";
            }
            if (dart_event_handler_render_.get()->ShouldRender(file))
            {
                std::string file_name = dart_event_handler_render_.get()->RenderedFileName(file.file_path).rendered_content;
                Replace(file_name, "binding/", "");
                impl_export += "export '" + file_name + "';\n";
            }
        }

        binding_export += "export 'dart:convert';\n";
        binding_export += "export 'dart:typed_data';\n";
        binding_export += "export 'package:json_annotation/json_annotation.dart';\n";
        binding_export += "export 'package:flutter/foundation.dart';\n";
        binding_export += "export 'package:agora_rtc_engine/src/agora_rtc_engine_ext.dart';\n";
        binding_export += "export 'package:agora_rtc_engine/src/impl/json_converters.dart';\n";
        // binding_export += "export 'package:agora_rtc_engine/src/api_caller.dart';\n";

        impl_export += "export 'event_handler_param_json.dart';\n";
        impl_export += "export 'call_api_impl_params_json.dart';\n";
        impl_export += "export 'call_api_event_handler_buffer_ext.dart';\n";

        std::filesystem::path outdir(output_dir);
        std::filesystem::path binding_export_path = outdir / "binding_forward_export.dart";
        // std::filesystem::remove(binding_export_path.c_str());
        std::ofstream of_binding_export{binding_export_path.c_str()};
        of_binding_export << binding_export;
        of_binding_export.flush();

        std::filesystem::path impl_export_path = outdir / "binding" / "impl_forward_export.dart";
        // std::filesystem::remove(impl_export_path.c_str());
        std::ofstream of_impl_export{impl_export_path.c_str()};
        of_impl_export << impl_export;
        of_impl_export.flush();
    }
};

#endif // DART_FORWARD_EXPORT_RENDER_H_