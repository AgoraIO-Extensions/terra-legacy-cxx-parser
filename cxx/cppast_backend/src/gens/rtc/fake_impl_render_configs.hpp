#ifndef FAKE_IMPL_RENDER_CONFIGS_H_
#define FAKE_IMPL_RENDER_CONFIGS_H_

#include <stdlib.h>
#include <vector>
#include <string>

std::vector<std::string> FakeImplClasses = {
    "agora::rtc::IRtcEngineEx",
    "agora::base::IAgoraParameter",
    "agora::rtc::IAudioDeviceManager",
    "agora::rtc::IVideoDeviceManager",
    "agora::media::IMediaEngine",
    "agora::rtc::ICloudSpatialAudioEngine",
    "agora::rtc::ILocalSpatialAudioEngine",
    "agora::rtc::IMediaRecorder",
    "agora::rtc::IMediaPlayer",
    "agora::rtc::IMusicPlayer",
    "agora::rtc::IMusicContentCenter",
    "agora::rtc::IAudioDeviceCollection",
    "agora::rtc::IVideoDeviceCollection",
    "agora::rtc::IMediaPlayerCacheManager",
};

#endif // FAKE_IMPL_RENDER_CONFIGS_H_