#ifndef RTCENGINE_PROXY_RENDER_H_
#define RTCENGINE_PROXY_RENDER_H_

#include "terra.hpp"
#include "terra_generator.hpp"
#include "../renders/base_syntax_render.hpp"

using namespace terra;

/// See https://jira.agoralab.co/browse/EP-35
class RtcEngineProxyRender : public BaseSyntaxRender
{
private:
    std::string out_dir_;
    std::string rtc_proxy_header_template;
    std::string rtc_proxy_cc_template;

  public:
    RtcEngineProxyRender(std::string out_dir)
    {
        out_dir_ = out_dir;
    }

    virtual void OnRenderFilesStart(const ParseResult &parse_result,
                                    const std::string &output_dir) {
      rtc_proxy_header_template = R"(
#ifndef RTCENGINE_PROXY_H_
#define RTCENGINE_PROXY_H_

#include "IAgoraMediaPlayer.h"
#include "IAgoraRtcEngine.h"

class RtcEngineProxy : public agora::rtc::IRtcEngineEx{
public:
      RtcEngineProxy(agora::rtc::IRtcEngine* rtc_engine);
    //RtcEngine
    {{TEMPLATE_CONTENT_RTCENGINE}}
    //RtcEngineEx
    {{TEMPLATE_CONTENT_RTCENGINEEX}}

private:
      agora::rtc::IRtcEngine* rtc_engine_;
};

#endif // RTCENGINE_PROXY_H_
        )";

      rtc_proxy_cc_template = R"(
#include "rtc_engine_proxy.h"

RtcEngineProxy::RtcEngineProxy(agora::rtc::IRtcEngine* rtc_engine) : rtc_engine_(rtc_engine){
}
//RtcEngine
{{TEMPLATE_CONTENT_RTCENGINE}}
 //RtcEngineEx
{{TEMPLATE_CONTENT_RTCENGINEEX}}
        )";
    }

    virtual void OnRenderFilesEnd(const ParseResult &parse_result,
                                  const std::string &output_dir) {
      std::string out_path = out_dir_ + "/rtc_engine_proxy.h";
      std::ofstream fileSink;
      fileSink.open(out_path.c_str());
      fileSink << rtc_proxy_header_template;
      fileSink.flush();

      std::string format_command = "clang-format --Werror -i " + out_path;
      system(format_command.c_str());

      std::string cc_out_path = out_dir_ + "/rtc_engine_proxy.cc";
      std::ofstream cc_fileSink;
      cc_fileSink.open(cc_out_path.c_str());
      cc_fileSink << rtc_proxy_cc_template;
      cc_fileSink.flush();

      std::string cc_format_command = "clang-format --Werror -i " + cc_out_path;
      system(cc_format_command.c_str());

      std::cout << "xiayangquin" << std::endl;
    }

    void Render(const ParseResult &parse_result, const CXXFile &file,
                const std::string &output_dir) override final {

      for (auto &node : file.nodes) {
        if (std::holds_alternative<Clazz>(node)) {
          const Clazz clazz = std::get<Clazz>(node);
          std::string class_with_ns =
              NameWithNamespace(clazz.name, clazz.namespaces);

          if (class_with_ns == "agora::rtc::IRtcEngine" ||
              class_with_ns == "agora::rtc::IRtcEngineEx") {

            std::string header_class_body = "";
            std::string cc_class_body = "";
            for (auto &mf : clazz.methods) {
              std::vector<std::string> params;
              std::vector<std::string> param_names;
              for (auto &p : mf.parameters) {
                std::string ps = p.type.source + " " + p.name;
                params.push_back(ps);
                param_names.push_back(p.name);
              }

              std::string comment = mf.comment;
              std::string function_prefix = "";
              std::string function_surffix = "";
              if (comment.find("#if") != std::string::npos) {
                std::vector<std::string> comment_list = Split(comment, "\n");
                for (auto &c : comment_list) {
                  function_prefix += c + "\n";
                  function_surffix += "#endif\n";
                }
              }

              std::string pl = JoinToString(params, ", ");
              std::string pln = JoinToString(param_names, ", ");

              header_class_body += function_prefix;
              cc_class_body += function_prefix;

              header_class_body += "virtual " + mf.return_type.source + " " +
                                   mf.name + "(" + pl + ") override;\n";

              cc_class_body += mf.return_type.source +
                               " RtcEngineProxy::" + mf.name + "(" + pl +
                               ") {\n";
              if (class_with_ns == "agora::rtc::IRtcEngine") {
                cc_class_body +=
                    "return rtc_engine_->" + mf.name + "(" + pln + ");\n";
              } else if (class_with_ns == "agora::rtc::IRtcEngineEx") {
                cc_class_body += "return ((IRtcEngineEx *)rtc_engine_)->" +
                                 mf.name + "(" + pln + ");\n";
              }
              cc_class_body += "}\n";

              header_class_body += function_surffix;
              cc_class_body += function_surffix;

              header_class_body += "\n";
              cc_class_body += "\n";
            }

            if (class_with_ns == "agora::rtc::IRtcEngine") {
              Replace(rtc_proxy_header_template,
                      "{{TEMPLATE_CONTENT_RTCENGINE}}", header_class_body);
            } else {
              Replace(rtc_proxy_header_template,
                      "{{TEMPLATE_CONTENT_RTCENGINEEX}}", header_class_body);
            }

            if (class_with_ns == "agora::rtc::IRtcEngine") {
              Replace(rtc_proxy_cc_template, "{{TEMPLATE_CONTENT_RTCENGINE}}",
                      cc_class_body);
            } else {
              Replace(rtc_proxy_cc_template, "{{TEMPLATE_CONTENT_RTCENGINEEX}}",
                      cc_class_body);
            }
          }
        }
      }
    }
};

#endif // RTCENGINE_PROXY_RENDER_H_