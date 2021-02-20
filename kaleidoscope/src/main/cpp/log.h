/*
 * MIT License
 *
 * Copyright (C) 2021 M.D.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef KALEIDOSCOPE_LOG_H
#define KALEIDOSCOPE_LOG_H

#include <android/log.h>
#include <string>
#include <regex>

#define __default_log_tag__ "Kaleidoscope"

#define debugLog(message, ...) if (Log::Level::kDebug >= Log::GetLogLevel())\
    __android_log_print(ANDROID_LOG_DEBUG, __default_log_tag__, "C++ %s %s() line %d - " message,\
        convert_file_name(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define warnLog(message, ...) if (Log::Level::kWarn >= Log::GetLogLevel()) {\
        if (Log::Level::kDebug >= Log::GetLogLevel()) {\
            __android_log_print(ANDROID_LOG_WARN, __default_log_tag__, "C++ %s %s() line %d - " message,\
                convert_file_name(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__);\
        } else {\
            __android_log_print(ANDROID_LOG_WARN, __default_log_tag__, "C++ - " message, ##__VA_ARGS__);\
        }\
    }

#define errorLog(message, ...) if (Log::Level::kError >= Log::GetLogLevel()) {\
        if (Log::Level::kDebug >= Log::GetLogLevel()) {\
            __android_log_print(ANDROID_LOG_ERROR, __default_log_tag__, "C++ %s %s() line %d - " message,\
                convert_file_name(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__);\
        } else {\
            __android_log_print(ANDROID_LOG_ERROR, __default_log_tag__, "C++ - " message, ##__VA_ARGS__);\
        }\
    }

#if defined(__aarch64__) || defined(__x86_64__)
#define __log_memory_specifier__ "0x%016lx"
#elif defined(__arm__) || defined(__i386__)
#define __log_memory_specifier__ "0x%08x"
#endif

namespace moe::aoramd::kaleidoscope {

    std::string convert_file_name(const char *file_path);

    class Log final {
    public:

        static void Initialize(int level);

        static int GetLogLevel();

        enum Level {
            kDebug = 0,
            kWarn = 1,
            kError = 2,
            kSilence [[maybe_unused]] = 3,
            kUnknown [[maybe_unused]] = 100
        };

    private:

        static int log_level_;
    };
}

#endif
