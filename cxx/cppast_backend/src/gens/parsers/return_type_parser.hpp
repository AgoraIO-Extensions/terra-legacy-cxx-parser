#ifndef RETURN_TYPE_PARSER_H_
#define RETURN_TYPE_PARSER_H_

#include "terra_parser.hpp"
#include <iterator>
#include <variant>
#include <vector>

class ReturnTypeParser : public Parser
{
public:
    ReturnTypeParser(bool convertReturnToVoid): convertReturnToVoid(convertReturnToVoid) {}

private:
    bool convertReturnToVoid;

    std::map<std::string, std::map<std::string, SimpleType>> fixed_return_type_ =
        {
            {
                "agora::rtc::IRtcEngine",
                {
                    {
                        "getScreenCaptureSources",
                        SimpleType{
                            "agora::rtc::ScreenCaptureSourceInfo",
                            "agora::rtc::ScreenCaptureSourceInfo*",
                            SimpleTypeKind::array_t,
                            false,
                            false,
                        },
                    },
                    {
                        "getVersion",
                        SimpleType{
                            "agora::rtc::ext::SDKBuildInfo",
                            "agora::rtc::ext::SDKBuildInfo*",
                            SimpleTypeKind::pointer_t,
                            false,
                            false,
                        },
                    },
                },
            },
            {
                "agora::rtc::IAudioDeviceManager",
                {
                    {
                        "getPlaybackDeviceInfo",
                        SimpleType{
                            "agora::rtc::ext::AudioDeviceInfo",
                            "agora::rtc::ext::AudioDeviceInfo*",
                            SimpleTypeKind::pointer_t,
                            false,
                            false,
                        },
                    },
                    {
                        "getRecordingDeviceInfo",
                        SimpleType{
                            "agora::rtc::ext::AudioDeviceInfo",
                            "agora::rtc::ext::AudioDeviceInfo*",
                            SimpleTypeKind::pointer_t,
                            false,
                            false,
                        },
                    },
                    {
                        "enumeratePlaybackDevices",
                        SimpleType{
                            "agora::rtc::ext::AudioDeviceInfo",
                            "agora::rtc::ext::AudioDeviceInfo*",
                            SimpleTypeKind::array_t,
                            false,
                            false,
                        },
                    },
                    {
                        "enumerateRecordingDevices",
                        SimpleType{
                            "agora::rtc::ext::AudioDeviceInfo",
                            "agora::rtc::ext::AudioDeviceInfo*",
                            SimpleTypeKind::array_t,
                            false,
                            false,
                        },
                    },
                },
            },
            {
                "agora::rtc::IVideoDeviceManager",
                {
                    {
                        "enumerateVideoDevices",
                        SimpleType{
                            "agora::rtc::ext::VideoDeviceInfo",
                            "agora::rtc::ext::VideoDeviceInfo*",
                            SimpleTypeKind::array_t,
                            false,
                            false,
                        },
                    },
                    {
                        "numberOfCapabilities",
                        SimpleType{
                            "int",
                            "int",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
            {
                "agora::media::IAudioFrameObserverBase",
                {
                    {
                        "onRecordAudioFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onPublishAudioFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onPlaybackAudioFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onMixedAudioFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onEarMonitoringAudioFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onPlaybackAudioFrameBeforeMixing",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onPlaybackAudioFrameBeforeMixing",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
            {
                "agora::media::IAudioFrameObserver",
                {
                    {
                        "onPlaybackAudioFrameBeforeMixing",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
            {
                "agora::media::IVideoFrameObserver",
                {
                    {
                        "onCaptureVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onPreEncodeVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onSecondaryCameraCaptureVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onSecondaryPreEncodeCameraVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onScreenCaptureVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onPreEncodeScreenVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onMediaPlayerVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onSecondaryScreenCaptureVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onSecondaryPreEncodeScreenVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onRenderVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onTranscodedVideoFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },

            },
            {
                "agora::media::IAudioSpectrumObserver",
                {
                    {
                        "onLocalAudioSpectrum",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "onRemoteAudioSpectrum",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
            {
                "agora::media::IVideoEncodedFrameObserver",
                {
                    {
                        "onEncodedVideoFrameReceived",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                    {
                        "OnEncodedVideoFrameReceived",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
            {
                "agora::media::IAudioPcmFrameSink",
                {
                    {
                        "onFrame",
                        SimpleType{
                            "void",
                            "void",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
            {
                "agora::rtc::IMusicContentCenter",
                {
                    {
                        "isPreloaded",
                        SimpleType{
                            "bool",
                            "bool",
                            SimpleTypeKind::value_t,
                            false,
                            true,
                        },
                    },
                },
            },
        };

public:
    bool Parse(const ParseConfig &parse_config,
               ParseResult &parse_result) override
    {
        for (auto &f : parse_result.cxx_files)
        {
            for (auto &node : f.nodes)
            {
                if (std::holds_alternative<Clazz>(node))
                {
                    auto &clazz = std::get<Clazz>(node);

                    for (auto &mf : clazz.methods)
                    {
                        std::string class_with_ns = clazz.GetFullName();

                        if (fixed_return_type_.find(class_with_ns) !=
                            fixed_return_type_.end())
                        {
                            auto mfm = fixed_return_type_.find(class_with_ns)->second;
                            if (mfm.find(mf.name) != mfm.end())
                            {
                                mf.return_type = mfm.find(mf.name)->second;
                                continue;
                            }
                        }

                        std::regex pattern = std::regex("^(get|query).*$");
                        std::smatch output;
                        if (!std::regex_match(mf.name, output, pattern))
                        {
                            pattern = std::regex("^(register|unregister)(.*)(Observer|EventHandler)$");
                            if (std::regex_match(mf.name, output, pattern)) {
                                if (convertReturnToVoid)
                                {
                                    mf.return_type.name = "void";
                                    mf.return_type.source = "void";
                                    mf.return_type.kind = SimpleTypeKind::value_t;
                                    mf.return_type.is_const = false;
                                    mf.return_type.is_builtin_type = true;
                                }
                                continue;
                            }
                            
                            if (mf.return_type.name == "int" && convertReturnToVoid)
                            {
                                mf.return_type.name = "void";
                                mf.return_type.source = "void";
                                mf.return_type.kind = SimpleTypeKind::value_t;
                                mf.return_type.is_const = false;
                                mf.return_type.is_builtin_type = true;
                            }
                        }

                        for (auto &it : mf.parameters)
                        {
                            if ((it.type.kind == SimpleTypeKind::pointer_t ||
                                it.type.kind == SimpleTypeKind::reference_t ||
                                it.type.kind == SimpleTypeKind::array_t) &&
                                !it.type.is_const)
                            {
                                if (convertReturnToVoid)
                                {
                                    mf.return_type = it.type;
                                    mf.user_data = it;

                                    mf.parameters.erase(
                                        std::remove_if(mf.parameters.begin(), mf.parameters.end(),
                                                    [&](Variable &variable)
                                                    {
                                                        return variable.name == it.name;
                                                    }),
                                        mf.parameters.end());
                                    break;
                                } else
                                {
                                    it.is_output = true;

                                    std::cout << "ReturnTypeParser "
                                              << "output param " << mf.parent_name << "."
                                              << mf.name << " -> " << it.name << ": "
                                              << it.type.source << "\n";
                                }
                            }
                        }
                    }
                }
            }
        }

        return true;
    }
};

#endif // RETURN_TYPE_PARSER_H_