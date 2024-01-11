#include "gens/mkt/delegate_api_binding.hpp"
#include "gens/parsers/iris_api_id_parser.hpp"
#include "gens/parsers/custom_node_parser.hpp"
#include "gens/parsers/enum_value_parser.hpp"
#include "gens/parsers/filter_node_parser.hpp"
#include "gens/parsers/merge_rtc_event_handler_ex_parser.hpp"
#include "gens/parsers/pointer_array_parser.hpp"
#include "gens/parsers/rename_member_function_param_type_parser.hpp"
#include "gens/parsers/return_type_parser.hpp"
#include "gens/parsers/swap_order_parser.hpp"
#include "gens/renders/dart/dart_callapi_event_handler_buffer_ext_render.hpp"
#include "gens/renders/dart/dart_callapi_render.hpp"
#include "gens/renders/dart/dart_callapi_render_iris_method_channel.hpp"
#include "gens/renders/dart/dart_event_handler_param_json_render.hpp"
#include "gens/renders/dart/dart_event_handler_render.hpp"
#include "gens/renders/dart/dart_event_handler_render_iris_method_channel.hpp"
#include "gens/renders/dart/dart_forward_export_render.hpp"
#include "gens/renders/dart/dart_struct_to_json_serializable_render.hpp"
#include "gens/renders/dart/dart_syntax_render.hpp"
#include "gens/renders/dart/dart_syntax_render_before_native_420.hpp"
#include "gens/renders/dart/legacy_dart_event_handler_render.hpp"
#include "gens/renders/multi_files_syntax_render.hpp"
#include "gens/renders/ts/ts_impl_render.hpp"
#include "gens/renders/ts/ts_interface_render.hpp"
#include "gens/rtc/fake_impl_render.hpp"
#include "gens/rtc/rtc_engine_proxy_render.hpp"
#include "terra.hpp"
#include "utils.hpp"
#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <stdlib.h>
#include <string>

void GenFakeRtcEngine(const std::vector<std::string> &include_header_dirs,
                      const std::vector<std::string> &pre_processed_files,
                      const std::string &output_dir) {
  DefaultVisitor rootVisitor;
  ParseConfig parse_config{include_header_dirs, pre_processed_files};
  rootVisitor.Visit(parse_config);

  auto irtcengine_render = std::make_unique<FakeImplRender>(output_dir);

  auto default_generator = std::make_unique<DefaultGenerator>(
      output_dir, std::move(irtcengine_render));

  rootVisitor.Accept(default_generator.get());
}

void GenRtcEngineProxy(const std::vector<std::string> &include_header_dirs,
                       const std::vector<std::string> &pre_processed_files,
                       const std::string &output_dir) {
  DefaultVisitor rootVisitor;
  ParseConfig parse_config{include_header_dirs, pre_processed_files};
  rootVisitor.Visit(parse_config);

  auto irtcengine_render = std::make_unique<FakeImplRender>(output_dir);

  auto default_generator = std::make_unique<DefaultGenerator>(
      output_dir, std::move(irtcengine_render));

  rootVisitor.Accept(default_generator.get());
}

void DumpJson(const std::vector<std::string> &include_header_dirs,
              const std::vector<std::string> &pre_processed_files,
              const std::map<std::string, std::string> &defines,
              const std::string &output_dir) {
  DefaultVisitor rootVisitor;
  ParseConfig parse_config{include_header_dirs, pre_processed_files, defines};
  rootVisitor.Visit(parse_config);

  auto default_generator = std::make_unique<DefaultJsonGenerator>(output_dir);

  rootVisitor.Accept(default_generator.get());
}

int main(int argc, char **argv) {
  cxxopts::Options option_list("iris-ast", "iris ast");

  // clang-format off
    option_list.add_options()
        // ("rtc-sdk-header-dir", "The Rtc SDK header dir", cxxopts::value<std::string>())
        ("include-header-dirs", "The include C++ headers directories, split with \",\"", cxxopts::value<std::string>())
        ("output-dir", "The output directory, or output to ./build/iris-ast/ by default", cxxopts::value<std::string>())
        ("visit-headers", "The C++ headers to be visited, split with \",\"", cxxopts::value<std::string>())
        ("custom-headers", "The custom C++ headers to be visited, split with \",\"", cxxopts::value<std::string>())
        ("defines-macros", "Custom macros, split with \",\"", cxxopts::value<std::string>())
        ("gen-fake-rtcengine", "Generate a fake IRtcEngine")
        ("gen-rtcengine-proxy", "Generate the RtcEngineProxy, more detail see https://jira.agoralab.co/browse/EP-35")
        ("dump-json", "Only dump the C++ header files to json")
        ("language", "Render language, supported language: dart, ts, c++", cxxopts::value<std::string>())
        ("native-sdk-version", "The native sdk version, e.g., 4.2.3.1", cxxopts::value<std::string>())
        ("legacy-renders", "The legacy renders allow you to compose which render you want, split with \",\"", cxxopts::value<std::string>());
  // clang-format on

  auto parse_result = option_list.parse(argc, argv);

  std::string rtc_sdk_header_dir = "";
  std::string output_dir = "";
  std::string visit_headers = "";
  std::vector<std::string> include_header_dirs;
  std::vector<std::string> visit_files;
  std::vector<std::string> custom_headers;
  std::vector<std::string> legacy_renders;
  bool is_gen_fake_rtcengine = false;
  bool is_gen_rtcengine_proxy = false;
  bool is_dump_json = false;
  std::string nativeSdkVersion = "";

  std::map<std::string, std::string> defines = {
      {"__GLIBC_USE\(...\)", "0"},
      {"__GNUC_PREREQ\(...\)", "0"},
      {"__GLIBC_PREREQ\(...\)", "0"},
      {"__glibc_clang_prereq\(...\)", "0"}};

  std::string language = "";

  if (parse_result.count("language")) {
    language = parse_result["language"].as<std::string>();

    if (!(language == "dart" || language == "ts" || language == "c++")) {
      std::cerr << "The language is only supported dart/ts/c++." << std::endl;
      return -1;
    }
  } else {
    std::cerr << "The language is missing." << std::endl;
    return -1;
  }

  if (parse_result.count("native-sdk-version")) {
    nativeSdkVersion = parse_result["native-sdk-version"].as<std::string>();
  } else {
    std::cerr << "The native-sdk-version is missing." << std::endl;
    return -1;
  }

  std::string project_path = std::filesystem::current_path();

  //   if (parse_result.count("rtc-sdk-header-dir")) {
  //     rtc_sdk_header_dir = parse_result["rtc-sdk-header-dir"].as<std::string>();
  //   } else {
  //     std::cerr << "The rtc-sdk-header-dir is missing." << std::endl;
  //     return -1;
  //   }

  if (parse_result.count("output-dir")) {
    output_dir = parse_result["output-dir"].as<std::string>();
  } else {
    std::filesystem::path tmp_out{"build/iris-ast"};
    if (std::filesystem::exists(tmp_out)) {
      std::filesystem::remove_all(tmp_out);
    }
    std::filesystem::create_directories(tmp_out);

    output_dir = std::string(std::filesystem::absolute(tmp_out).c_str());
  }

  std::filesystem::path tmp_path = std::filesystem::current_path() / "tmp";
  // make sure pre_processed_files as the first of the headers
  include_header_dirs.push_back(tmp_path.c_str());

  if (parse_result.count("include-header-dirs")) {
    include_header_dirs.push_back(
        "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/"
        "Developer/SDKs/MacOSX.sdk/usr/include");

    auto ihd = parse_result["include-header-dirs"].as<std::string>();
    const auto &ihdv = BaseSyntaxRender::Split(ihd, ",");
    for (auto &h : ihdv) { include_header_dirs.push_back(h); }
  } else {
    include_header_dirs.push_back(
        std::string(project_path + "/include/system"));
    include_header_dirs.push_back(
        "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/"
        "Developer/SDKs/MacOSX.sdk/usr/include");
    include_header_dirs.push_back(rtc_sdk_header_dir);
  }

  if (parse_result.count("custom-headers")) {
    auto ihd = parse_result["custom-headers"].as<std::string>();
    const auto &ihdv = BaseSyntaxRender::Split(ihd, ",");
    for (auto &h : ihdv) { custom_headers.push_back(h); }
  } else {
    custom_headers.push_back(
        std::string(project_path + "/include/CustomIAgoraMediaEngine.h"));
    custom_headers.push_back(
        std::string(project_path + "/include/CustomIAgoraRtcEngine.h"));
    custom_headers.push_back(
        std::string(project_path + "/include/CustomIAgoraMediaPlayer.h"));
    custom_headers.push_back(
        std::string(project_path + "/include/CustomIAgoraRtcEngineEx.h"));
    custom_headers.push_back(std::string(
        project_path + "/include/CustomIAgoraMusicContentCenter.h"));
  }

  if (parse_result.count("visit-headers")) {
    visit_headers = parse_result["visit-headers"].as<std::string>();
    const auto &headers = BaseSyntaxRender::Split(visit_headers, ",");
    for (auto &h : headers) { visit_files.push_back(h); }
    // std::insert(headers.begin(), headers.end(), visit_files.begin());
  } else {
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraRtcEngine.h"));
    visit_files.push_back(std::string(rtc_sdk_header_dir + "/AgoraBase.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/AgoraMediaBase.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/AgoraMediaPlayerTypes.h"));
    visit_files.push_back(std::string(rtc_sdk_header_dir + "/IAgoraLog.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraMediaEngine.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraMediaPlayer.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraMediaPlayerSource.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraMediaRecorder.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraMediaStreamingSource.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraSpatialAudio.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraRtcEngineEx.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAudioDeviceManager.h"));
    visit_files.push_back(
        std::string(rtc_sdk_header_dir + "/IAgoraRhythmPlayer.h"));
  }

  if (parse_result.count("defines-macros")) {
    std::string defines_macros =
        parse_result["defines-macros"].as<std::string>();
    const auto &macros = BaseSyntaxRender::Split(defines_macros, ",");
    for (auto &m : macros) {
      defines.insert(std::pair<std::string, std::string>(m, ""));
    }
  }

  if (parse_result.count("legacy-renders")) {
    auto ihd = parse_result["legacy-renders"].as<std::string>();
    const auto &ihdv = BaseSyntaxRender::Split(ihd, ",");
    for (auto &h : ihdv) { legacy_renders.push_back(h); }
  } else {
    std::cerr << "The legacy-renders is missing." << std::endl;
    return -1;
  }

  if (parse_result.count("gen-fake-rtcengine")) {
    is_gen_fake_rtcengine = true;
  }

  if (parse_result.count("gen-rtcengine-proxy")) {
    is_gen_rtcengine_proxy = true;
  }

  if (parse_result.count("dump-json")) { is_dump_json = true; }

  // std::string file_path = std::string("/Users/fenglang/codes/aw/Agora-Flutter-SDK/integration_test_app/iris_integration_test/third_party/agora/rtc/include/IAgoraRtcEngine2.h");
  // std::string include_header_dir = "/Users/fenglang/codes/aw/Agora-Flutter-SDK/integration_test_app/iris_integration_test/third_party/agora/rtc/include";
  // std::string rtc_sdk_header_dir = "/Users/fenglang/codes/aw/iris/third_party/agora/dcg/libs/Agora_Native_SDK_for_iOS_rel.v3.8.201.2_54040_full_20220623_0140_212903/libs/AgoraRtcKit.xcframework/ios-arm64_armv7/AgoraRtcKit.framework/Headers";
  // std::vector<std::string> include_header_dirs{
  //     // "/Users/fenglang/codes/aw/iris/base",
  //     // "/Users/fenglang/codes/aw/iris/base/internal",
  //     // "/Users/fenglang/codes/aw/iris/dcg/include",
  //     std::string(project_path + "/include/system"),
  //     rtc_sdk_header_dir,
  //     // "/Users/fenglang/codes/aw/iris/third_party/agora/dcg/include",
  //     // "/Users/fenglang/codes/aw/iris/third_party/rapidjson",
  //     // "/Users/fenglang/codes/aw/iris/third_party/rapidjson/include",
  //     "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include",
  // };

  // std::string source_dir = "/Users/fenglang/codes/aw/iris/third_party/agora/dcg/libs/Agora_Native_SDK_for_Mac_rel.v3.8.201.2_40228_full_20220616_2135_210770/libs/AgoraRtcKit.framework/Headers";

  // std::vector<std::string> visit_files{
  //     "/Users/fenglang/codes/aw/market-place/FaceUnity/src/face_unity_extension_api.h",
  // };

  std::vector<std::string> pre_processed_files;
  terra_legacy::PreProcessVisitFiles(tmp_path, visit_files, pre_processed_files,
                       is_gen_fake_rtcengine || is_gen_rtcengine_proxy
                           || is_dump_json);

  if (is_gen_fake_rtcengine) {
    GenFakeRtcEngine(include_header_dirs, pre_processed_files, output_dir);
    return 0;
  }

  if (is_gen_rtcengine_proxy) {
    GenRtcEngineProxy(include_header_dirs, pre_processed_files, output_dir);
    return 0;
  }

  if (is_dump_json) {
    DumpJson(include_header_dirs, pre_processed_files, defines, output_dir);
    return 0;
  }

  DefaultVisitor rootVisitor;

  if (BaseSyntaxRender::StartsWith(nativeSdkVersion, "4.3")) {
    rootVisitor.AddParser(std::make_unique<IrisApiIdParser>());
  }
  rootVisitor.AddParser(
      std::make_unique<FilterNodeParser>(nativeSdkVersion));// 过滤不需要的node
  rootVisitor.AddParser(
      std::make_unique<CustomNodeParser>(include_header_dirs, custom_headers,
                                         defines));// 追加自定义node

  rootVisitor.AddParser(
      std::make_unique<PointerArrayParser>());// 特化指针为数组
  rootVisitor.AddParser(
      std::make_unique<
          RenameMemberFunctionParamTypeParser>());// 特化函数参数类型
  // rootVisitor.AddParser(std::make_unique<VagueReturnTypeParser>());
  rootVisitor.AddParser(std::make_unique<ReturnTypeParser>(
      language == "dart"));// 特化函数返回值类型
  rootVisitor.AddParser(std::make_unique<EnumValueParser>());// 特化枚举值
  // rootVisitor.AddParser(std::make_unique<DartStructToJsonSerializableRender>());
  rootVisitor.AddParser(
      std::make_unique<MergeRtcEventHandlerExParser>());// 合并Ex回调

  if (language == "ts") {
    rootVisitor.AddParser(std::make_unique<SwapOrderParser>());// 特化排序
  }

  ParseConfig parse_config{include_header_dirs, pre_processed_files, defines};
  rootVisitor.Visit(parse_config);

  // std::filesystem::remove_all(tmp_path);

  std::vector<std::shared_ptr<SyntaxRender>> renders;

  if (language == "dart") {
    auto dart_syntax_render = std::make_shared<DartSyntaxRender>();
    auto dart_syntax_render_before_native420 =
        std::make_shared<DartSyntaxRenderBeforeNative420>();
    auto event_handler_param_json_render =
        std::make_shared<DartEventHandlerParamJsonRender>();
    auto callapi_render = std::make_shared<DartCallApiRender>();
    auto callapi_iris_method_channel_render =
        std::make_shared<DartCallApiIrisMethodChannelRender>();
    auto event_handler_render = std::make_shared<DartEventHandlerRender>();
    auto legacy_event_handler_render =
        std::make_shared<LegacyDartEventHandlerRender>();
    auto event_handler_iris_method_channel_render =
        std::make_shared<DartEventHandlerIrisMethodChannelRender>();
    auto call_api_impl_param_render =
        std::make_shared<DartStructToJsonSerializableRender>();
    auto callapi_eventhandler_buffer_ext_render =
        std::make_shared<DartCallApiEventHandlerBufferExtRender>();
    std::shared_ptr<DartForwardExportRender> forward_export_render =
        std::shared_ptr<DartForwardExportRender>{new DartForwardExportRender(
            dart_syntax_render, event_handler_render, callapi_render)};

    for (auto &it : legacy_renders) {
      if (it == "DartSyntaxRender") {
        renders.emplace_back(dart_syntax_render);
      } else if (it == "DartEventHandlerParamJsonRender") {
        renders.emplace_back(event_handler_param_json_render);
      } else if (it == "DartCallApiRender") {
        renders.emplace_back(callapi_render);
      } else if (it == "DartEventHandlerRender") {
        renders.emplace_back(event_handler_render);
      } else if (it == "DartStructToJsonSerializableRender") {
        renders.emplace_back(call_api_impl_param_render);
      } else if (it == "DartCallApiEventHandlerBufferExtRender") {
        renders.emplace_back(callapi_eventhandler_buffer_ext_render);
      } else if (it == "DartForwardExportRender") {
        renders.emplace_back(forward_export_render);
      } else if (it == "DartEventHandlerIrisMethodChannelRender") {
        renders.emplace_back(event_handler_iris_method_channel_render);
      } else if (it == "DartCallApiIrisMethodChannelRender") {
        renders.emplace_back(callapi_iris_method_channel_render);
      } else if (it == "LegacyDartEventHandlerRender") {
        renders.emplace_back(legacy_event_handler_render);
      } else if (it == "DartSyntaxRenderBeforeNative420") {
        renders.emplace_back(dart_syntax_render_before_native420);
      }
    }
  } else if (language == "ts") {
    for (auto &it : legacy_renders) {
      if (it == "TsInterfaceRender") {
        renders.emplace_back(std::make_shared<TsInterfaceRender>());
      } else if (it == "TsImplRender") {
        renders.emplace_back(std::make_shared<TsImplRender>());
      }
    }
  }

  auto multi_files_render = std::make_unique<MultiFilesSyntaxRender>(renders);

  auto default_generator = std::make_unique<DefaultGenerator>(
      output_dir, std::move(multi_files_render));

  rootVisitor.Accept(default_generator.get());

  // rootVisitor.Accept(std::make_unique<DelegateAPIBindingGenerator>());
  return 0;
}
