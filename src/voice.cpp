/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "voice.h"

extern "C" {
#include <unistd.h>
#include <pthread.h>
#include <aether_radio/x6100_control/control.h>
#include <lvgl.h>

#include "audio.h"
#include "params.h"
#include "util.h"
#include "backlight.h"
#include "recorder.h"
}

#include <memory>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>

#include <RHVoice/core/engine.hpp>
#include <RHVoice/core/document.hpp>
#include <RHVoice/core/client.hpp>
#include "RHVoice/audio.hpp"

using namespace RHVoice;

class audio_player: public client {
public:
    audio_player();

    bool play_speech(const short* samples_buf, std::size_t count);
    void finish();

private:
    audio::playback_stream stream;
};

audio_player::audio_player() {
    stream.set_sample_rate(24000);
    stream.set_buffer_size(512);
    stream.open();
}

bool audio_player::play_speech(const short* buf, std::size_t count) {
    try {
        stream.write(buf, count);
        return true;
    } catch (...) {
        stream.close();
        return false;
    }
}

void audio_player::finish() {
    if (stream.is_open())
        stream.drain();
}

static std::shared_ptr<engine>      eng(new engine);
static voice_profile                profile;
static char                         buf[512];
static pthread_t                    thread;
static uint32_t                     delay = 0;
static bool                         run = false;

static void * say_thread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if (delay) {
        usleep(delay);
    }
    
    run = true;

    profile = eng->create_voice_profile("lyubov");

    audio_player                    player;
    std::istringstream              text{buf};
    std::istreambuf_iterator<char>  text_start{text};
    std::istreambuf_iterator<char>  text_end;
    std::unique_ptr<document>       doc = document::create_from_plain_text(eng, text_start, text_end, content_text, profile);
    
    doc->speech_settings.relative.rate = params.voice_rate / 100.0;
    doc->speech_settings.relative.pitch = params.voice_pitch / 100.0;
    doc->speech_settings.relative.volume = params.voice_volume / 100.0;
    doc->set_owner(player);

    x6100_control_record_set(true);
    doc->synthesize();
    player.finish();
    x6100_control_record_set(false);
    
    run = false;
    return NULL;
}

bool voice_enable() {
    if (run || recorder_is_on()) {
        return false;
    }
    
    switch (params.voice_mode) {
        case VOICE_OFF:
            return false;
            
        case VOICE_ALWAYS:
            return true;
            
        case VOICE_LCD:
            return !backlight_is_on();
    }
    
    return false;
}

void voice_delay_say_text_fmt(const char * fmt, ...) {
    if (!voice_enable()) {
        return;
    }

    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (thread) {
        pthread_cancel(thread);
        pthread_join(thread, NULL);
    }
    
    delay = 1000000;
    pthread_create(&thread, NULL, say_thread, NULL);
}

void voice_say_text_fmt(const char * fmt, ...) {
    if (!voice_enable()) {
        return;
    }

    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    
    delay = 0;
    pthread_create(&thread, NULL, say_thread, NULL);
}

void voice_say_freq(uint64_t freq) {
    if (!voice_enable()) {
        return;
    }

    uint16_t    mhz, khz, hz;
    
    split_freq(freq, &mhz, &khz, &hz);
    
    if (hz) {
        snprintf(buf, sizeof(buf), "%i %i %i", mhz, khz, hz);
    } else if (khz) {
        snprintf(buf, sizeof(buf), "%i %i", mhz, khz);
    } else {
        snprintf(buf, sizeof(buf), "%i", mhz);
    }

    if (thread) {
        pthread_cancel(thread);
        pthread_join(thread, NULL);
    }

    delay = 1000000;
    pthread_create(&thread, NULL, say_thread, NULL);
}

void voice_say_bool(const char *prompt, bool x) {
    voice_delay_say_text_fmt("%s is %s", prompt, x ? "on" : "off");
}
