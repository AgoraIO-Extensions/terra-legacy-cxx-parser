#ifndef FILTER_NODE_PARSER_H_
#define FILTER_NODE_PARSER_H_

#include "terra_parser.hpp"
#include <vector>
#include <iterator>
#include <variant>
#include <filesystem>

using namespace terra;

class FilterNodeParser : public Parser
{
private:
    std::vector<std::string> filter_global_file_{
        "AgoraOptional.h",
        "AgoraRefPtr.h",
    };

    std::vector<std::string> filter_global_class_{
        "agora::util::AutoPtr",
        "agora::util::CopyableAutoPtr",
        "agora::util::IString",
        "agora::util::IIterator",
        "agora::util::IContainer",
        "agora::util::AOutputIterator",
        "agora::util::AList",
        "agora::base::IParameterEngine",
        // "agora::rtc::VideoCanvas",
        // "agora::media::base::ExternalVideoFrame",
        // "agora::media::base::VideoFrame",
        "agora::media::base::IVideoFrameObserver",
        // "agora::media::IVideoFrameObserver",
        // "agora::media::AudioFrame",
        // "agora::media::IAudioFrameObserver",
        "agora::media::base::IAudioFrameObserver",
        "agora::media::base::IMediaPlayerCustomDataProvider",
        // "agora::media::IMediaEngine",
        "agora::UserInfo",
        // "agora::media::IAudioFrameObserverBase",
        "agora::RefCountInterface",
        "RefCountInterface",
        "agora::base::IEngineBase",
        "agora::base::AParameter",
        "agora::rtc::IMediaPlayerCustomDataProvider",
        "agora::rtc::IMediaPlayerSource",
        "agora::rtc::IPacketObserver",
        // "agora::rtc::IVideoEncodedImageReceiver",
        // "agora::rtc::IAudioEncodedFrameObserver",
        "agora::base::LicenseCallback",
        // "agora::media::IVideoEncodedFrameObserver",
        "agora::media::ISnapshotCallback",
        "agora::commons::ILogWriter",
        "agora::rtc::IMediaStreamingSource",
        "agora::rtc::IMediaStreamingSourceObserver",
        "agora::rtc::IAudioDeviceCollection",
        "agora::rtc::IVideoDeviceCollection",
        "agora::rtc::IScreenCaptureSourceList",
        "agora::rtc::AAudioDeviceManager",
        "agora::rtc::AVideoDeviceManager",
        // "agora::rtc::DeviceInfo",
        "agora::rtc::IRtcEngineParameter",
        "agora::base::IAgoraParameter",
        "agora::rtc::IRhythmPlayer",
        "agora::rtc::LocalSpatialAudioConfig",
    };

    std::vector<std::string> filter_global_enum_{
        "agora::media::base::MAX_METADATA_SIZE_TYPE",
        "agora::media::base::VIDEO_SOURCE_TYPE",
    };

    std::vector<std::string> filter_global_function_{"operator==", "operator!=", "operator=", "operator>", "operator<", "Optional"};

    std::map<std::string, std::vector<std::string>> filter_member_function_for_class_and_struct_ = {
        {"agora::rtc::EncodedVideoTrackOptions", {"SetFrom"}},
        {"agora::rtc::ChannelMediaOptions", {"SetAll"}},
        {"agora::rtc::DirectCdnStreamingMediaOptions", {"SetAll"}},
        {"agora::rtc::EncryptionConfig", {"getEncryptionString"}},
        {
            "agora::media::IMediaEngine",
            {
                "addVideoFrameRenderer",
                "removeVideoFrameRenderer",
            },
        },
        {
            "agora::rtc::IRtcEngine",
            {
                "queryInterface",
                "registerPacketObserver",
            },
        },
        {
            "agora::rtc::IRtcEngineEventHandler",
            {
                "eventHandlerType",
            },
        },
        {
            "agora::rtc::IRtcEngineEventHandlerEx",
            {
                "eventHandlerType",
                "onFirstLocalVideoFrame",
                "onFirstLocalVideoFramePublished",
                "onLocalVideoStateChanged",
                "onLocalVideoStats",
            },
        },
        {
            "agora::rtc::IMediaPlayer",
            {
                "initialize",
                "openWithCustomSource",
            },
        },
        {
            "agora::rtc::IMetadataObserver",
            {
                "getMaxMetadataSize",
                "onReadyToSendMetadata",
            },
        },
        {
            "agora::rtc::IRhythmPlayer",
            {
                "initialize",
                "getRhythmPlayerTrack",
            },
        },
        {
            "agora::media::IAudioFrameObserverBase",
            {
                "onPlaybackAudioFrameBeforeMixing",
                "getObservedAudioFramePosition",
                "getPlaybackAudioParams",
                "getPublishAudioParams",
                "getRecordAudioParams",
                "getMixedAudioParams",
                "getEarMonitoringAudioParams",
            },
        },
        {
            "agora::media::IVideoFrameObserver",
            {
                "getVideoFrameProcessMode",
                "getVideoFormatPreference",
                "getRotationApplied",
                "getMirrorApplied",
                "getObservedFramePosition",
                "isExternal",
            },
        },
        {
            "agora::rtc::IMusicPlayer",
            {
                "open",
            },
        },
    };

  std::vector<std::string> filter_member_function_with_signature_for_class_and_struct_ = {
          "agora::rtc::IRtcEngineEventHandler::onAudioRoutingChanged(int)",
      };

    std::map<std::string, std::vector<std::string>> filter_member_variable_for_class_and_struct_ = {
        {
            "agora::rtc::RtcEngineContext",
            {"context", "eventHandler"},
        },
        {
            "agora::media::base::ExternalVideoFrame",
            {
                "eglContext",
                "d3d11_texture_2d",
            },
        },
        {
            "agora::media::base::VideoFrame",
            {
                "sharedContext",
                "d3d11Texture2d",
            },
        },
        {
            "agora::media::base::MediaSource",
            {
                "provider",
            },
        },
        {
            "agora::rtc::MusicContentCenterConfiguration",
            {
                "eventHandler",
            },
        },
    };

    std::map<std::string, std::map<std::string, std::vector<std::string>>> filter_parameters_for_member_function_ = {
        {
            "agora::rtc::IAudioDeviceManager",
            {
                {
                    "getPlaybackDeviceInfo",
                    {
                        "deviceId",
                        "deviceName",
                    },
                },
                {
                    "getRecordingDeviceInfo",
                    {
                        "deviceId",
                        "deviceName",
                    },
                },
            },

        },
        {
            "agora::rtc::IRtcEngineEx",
            {
                {
                    "joinChannelEx",
                    {
                        "eventHandler",
                    },
                },

            },

        },
        {
            "agora::rtc::IRtcEngine",
            {
                {
                    "joinChannelWithUserAccountEx",
                    {
                        "eventHandler",
                    },
                },
                {
                    "getVersion",
                    {
                        "build",
                    },
                },
                {
                    "takeSnapshot",
                    {
                        "callback",
                    },
                },
            },
        },
        {
            "agora::rtc::ICloudSpatialAudioEngine",
            {
                {
                    "getTeammates",
                    {
                        "uids",
                        "userCount",
                    },
                },
            },
        },
        {
            "agora::rtc::ILocalSpatialAudioEngine",
            {
                {
                    "initialize",
                    {
                        "config",
                        
                    },
                },
            },
        },
    };

private:
    std::string NameWithNamespace(const std::string &name, const std::vector<std::string> &namespace_list)
    {
        std::ostringstream vts;
        std::copy(namespace_list.begin(), namespace_list.end() - 1,
                  std::ostream_iterator<std::string>(vts, "::"));

        vts << namespace_list.back();

        std::string ns_prefix = vts.str();
        if (!ns_prefix.empty())
        {
            ns_prefix += "::";
        }

        std::string name_with_ns = ns_prefix + name;

        return name_with_ns;
    }

    bool ShouldRemoveFile(CXXFile &file)
    {
        bool shouldRemove = false;
        std::filesystem::path file_path(file.file_path);
        if (std::find(filter_global_file_.begin(), filter_global_file_.end(), file_path.filename()) != filter_global_file_.end())
        {
            std::cout << "FilterNodeParser ShouldRemoveFile:" << file.file_path << "\n";

            // it = nodes.erase(it);
            shouldRemove = true;
            // continue;
        }

        return shouldRemove;
    }

    bool ShouldRemoveClass(NodeType &node)
    {
        bool shouldRemove = false;
        if (std::holds_alternative<Clazz>(node))
        {
            Clazz clazz = std::get<Clazz>(node);

            std::string class_with_ns = NameWithNamespace(clazz.name, clazz.namespaces);

            if (std::find(filter_global_class_.begin(), filter_global_class_.end(), class_with_ns) != filter_global_class_.end())
            {
                std::cout << "FilterNodeParser ShouldRemoveClass:" << clazz.name << "\n";

                // it = nodes.erase(it);
                shouldRemove = true;
                // continue;
            }

            // std::cout << "FilterNodeParser FilterStruct class name:" << clazz.name << "\n";
            // FilterMemberFunctions(clazz.methods);
        }

        return shouldRemove;
    }

    bool ShouldRemoveStruct(NodeType &node)
    {
        bool shouldRemove = false;
        if (std::holds_alternative<Struct>(node))
        {
            Struct structt = std::get<Struct>(node);

            std::string struct_with_ns = NameWithNamespace(structt.name, structt.namespaces);

            if (std::find(filter_global_class_.begin(), filter_global_class_.end(), struct_with_ns) != filter_global_class_.end())
            {
                std::cout << "FilterNodeParser ShouldRemoveStruct:" << structt.name << "\n";

                // it = nodes.erase(it);
                shouldRemove = true;
                // continue;
            }

            // std::cout << "FilterNodeParser FilterStruct class name:" << clazz.name << "\n";
            // FilterMemberFunctions(clazz.methods);
        }

        return shouldRemove;
    }

    bool ShouldRemoveEnum(NodeType &node)
    {
        bool shouldRemove = false;
        if (std::holds_alternative<Enumz>(node))
        {
            Enumz enumz = std::get<Enumz>(node);

            std::string enum_with_ns = NameWithNamespace(enumz.name, enumz.namespaces);

            if (std::find(filter_global_enum_.begin(), filter_global_enum_.end(), enum_with_ns) != filter_global_enum_.end())
            {
                std::cout << "FilterNodeParser ShouldRemoveEnum:" << enum_with_ns << "\n";

                // it = nodes.erase(it);
                shouldRemove = true;
                // continue;
            }

            // std::cout << "FilterNodeParser FilterStruct class name:" << clazz.name << "\n";
            // FilterMemberFunctions(clazz.methods);
        }

        return shouldRemove;
    }

    void FilterMemberFunctions(std::vector<MemberFunction> &member_functions)
    {
        for (std::vector<MemberFunction>::iterator it = member_functions.begin(); it != member_functions.end();)
        {
            if (std::find(filter_global_function_.begin(), filter_global_function_.end(), it->name) != filter_global_function_.end())
            {
                std::cout << "FilterNodeParser FilterFunctionForClass: function: " << it->name << "\n";
                it = member_functions.erase(it);
                continue;
            }

            ++it;
        }
    }

    std::string GetFunctionSignature(const std::string& name_with_ns, const MemberFunction& function) {
        std::vector<std::string> type_list;
        for (auto &p : function.parameters) {
            auto type_name = p.type.GetTypeName();
            type_list.push_back(type_name);
        }

        std::string type_list_in_str = BaseSyntaxRender::JoinToString(type_list, ", ");

        return name_with_ns + "::" + function.name + "(" + type_list_in_str + ")";
    }

public:
    bool Parse(const ParseConfig &parse_config, ParseResult &parse_result)
    {
        parse_result.cxx_files.erase(
            std::remove_if(
                parse_result.cxx_files.begin(),
                parse_result.cxx_files.end(),
                [this](CXXFile &file)
                {
                    bool shouldRemove = ShouldRemoveFile(file);
                    return shouldRemove;
                }),
            parse_result.cxx_files.end());

        for (auto &f : parse_result.cxx_files)
        {
            // FilterClass(f.nodes);

            f.nodes.erase(
                std::remove_if(
                    f.nodes.begin(),
                    f.nodes.end(),
                    [this](NodeType &node)
                    {
                        bool shouldRemove = ShouldRemoveClass(node) || ShouldRemoveStruct(node) || ShouldRemoveEnum(node);
                        return shouldRemove;
                    }),
                f.nodes.end());
        }

        for (auto &f : parse_result.cxx_files)
        {
            for (auto &node : f.nodes)
            {
                if (std::holds_alternative<Clazz>(node))
                {
                    Clazz &clazz = std::get<Clazz>(node);

                    std::string class_with_ns = NameWithNamespace(clazz.name, clazz.namespaces);

                    clazz.methods.erase(
                        std::remove_if(
                            clazz.methods.begin(),
                            clazz.methods.end(),
                            [&](MemberFunction &method)
                            {
                                bool shouldRemove = false;

                                if (std::find(filter_global_function_.begin(), filter_global_function_.end(), method.name) != filter_global_function_.end())
                                {
                                    shouldRemove = true;
                                }

                                if (filter_member_function_for_class_and_struct_.find(class_with_ns) != filter_member_function_for_class_and_struct_.end())
                                {
                                    auto &filter_member_function = filter_member_function_for_class_and_struct_.find(class_with_ns)->second;
                                    if (std::find(filter_member_function.begin(), filter_member_function.end(), method.name) != filter_member_function.end())
                                    {
                                        std::cout << "FilterNodeParser ShouldRemoveMemberFunction:" << class_with_ns << "::" << method.name << "\n";
                                        shouldRemove = true;
                                    }
                                }

                                std::string fun_with_signature = GetFunctionSignature(class_with_ns, method);
                                std::cout << "GetFunctionSignature class:" << fun_with_signature << "\n";
                                if (std::find(filter_member_function_with_signature_for_class_and_struct_.begin(), filter_member_function_with_signature_for_class_and_struct_.end(), fun_with_signature) != filter_member_function_with_signature_for_class_and_struct_.end())
                                {
                                    std::cout << "FilterNodeParser Remove function with signature:" << fun_with_signature << "\n";
                                    shouldRemove = true;
                                }

                                return shouldRemove;
                            }),
                        clazz.methods.end());

                    for (auto &method : clazz.methods) {
                        method.parameters.erase(
                            std::remove_if(
                                method.parameters.begin(),
                                method.parameters.end(),
                                [&](Variable &variable)
                                {
                                    bool shouldRemove = false;

                                    if (filter_parameters_for_member_function_.find(class_with_ns) != filter_parameters_for_member_function_.end())
                                    {
                                        auto &filter_member_function = filter_parameters_for_member_function_.find(class_with_ns)->second;
                                        if (filter_member_function.find(method.name) != filter_member_function.end())
                                        {
                                            auto &filter_parameters = filter_member_function.find(method.name)->second;
                                            if (std::find(filter_parameters.begin(), filter_parameters.end(), variable.name) != filter_parameters.end())
                                            {
                                                std::cout << "FilterNodeParser ShouldRemoveParameter:" << class_with_ns << "::" << method.name << "::" << variable.name << "\n";
                                                shouldRemove = true;
                                            }
                                        }
                                    }

                                    return shouldRemove;
                                }),
                            method.parameters.end()
                        );
                    }

                    clazz.base_clazzs.erase(
                        std::remove_if(
                            clazz.base_clazzs.begin(),
                            clazz.base_clazzs.end(),
                            [&](std::string &base_class)
                            {
                                bool shouldRemove = false;

                                // filter_global_class_
                                for (auto &bc : filter_global_class_)
                                {
                                    if (base_class.rfind(bc, 0) == 0)
                                    {
                                        shouldRemove = true;
                                        break;
                                    }
                                }

                                return shouldRemove;
                            }),
                        clazz.base_clazzs.end());

                    continue;
                }
                if (std::holds_alternative<Struct>(node))
                {
                    Struct &structt = std::get<Struct>(node);

                    std::string struct_with_ns = NameWithNamespace(structt.name, structt.namespaces);

                    structt.methods.erase(
                        std::remove_if(
                            structt.methods.begin(),
                            structt.methods.end(),
                            [&](MemberFunction &method)
                            {
                                bool shouldRemove = false;

                                if (std::find(filter_global_function_.begin(), filter_global_function_.end(), method.name) != filter_global_function_.end())
                                {
                                    shouldRemove = true;
                                }

                                if (filter_member_function_for_class_and_struct_.find(struct_with_ns) != filter_member_function_for_class_and_struct_.end())
                                {
                                    auto &filter_member_function = filter_member_function_for_class_and_struct_.find(struct_with_ns)->second;
                                    if (std::find(filter_member_function.begin(), filter_member_function.end(), method.name) != filter_member_function.end())
                                    {
                                        std::cout << "FilterNodeParser ShouldRemoveMemberFunction:" << struct_with_ns << "::" << method.name << "\n";
                                        shouldRemove = true;
                                    }
                                }

                                std::string fun_with_signature = GetFunctionSignature(struct_with_ns, method);
                                std::cout << "GetFunctionSignature struct:" << fun_with_signature << "\n";
                                if (std::find(filter_member_function_with_signature_for_class_and_struct_.begin(), filter_member_function_with_signature_for_class_and_struct_.end(), fun_with_signature) != filter_member_function_with_signature_for_class_and_struct_.end())
                                {
                                    std::cout << "FilterNodeParser Remove function with signature:" << fun_with_signature << "\n";
                                    shouldRemove = true;
                                }

                                return shouldRemove;
                            }),
                        structt.methods.end());

                    if (filter_member_variable_for_class_and_struct_.find(struct_with_ns) != filter_member_variable_for_class_and_struct_.end())
                    {
                        auto &filter_member_variable = filter_member_variable_for_class_and_struct_.find(struct_with_ns)->second;
                        structt.member_variables.erase(
                            std::remove_if(
                                structt.member_variables.begin(),
                                structt.member_variables.end(),
                                [&](MemberVariable &variable)
                                {
                                    bool shouldRemove = false;

                                    if (std::find(filter_member_variable.begin(), filter_member_variable.end(), variable.name) != filter_member_variable.end())
                                    {
                                        std::cout << "FilterNodeParser ShouldRemoveMemberVariable:" << struct_with_ns << "::" << variable.name << "\n";
                                        shouldRemove = true;
                                    }

                                    return shouldRemove;
                                }),
                            structt.member_variables.end());
                    }

                    continue;
                }
            }
        }

        return true;
    }
};

#endif // FILTER_NODE_PARSER_H_