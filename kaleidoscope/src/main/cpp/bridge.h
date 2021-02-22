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

#ifndef KALEIDOSCOPE_BRIDGE_H
#define KALEIDOSCOPE_BRIDGE_H

#include <cstddef>

#include "declare.h"

extern "C" void MainBridge();

extern "C" void SecondaryBridge();

extern "C" void OriginBridge();

namespace moe::aoramd::kaleidoscope::bridge {

    /**
     * A tool class for inserting and recovering bridge code.
     */
    class Bridge {
    public:

        /**
         * Insert main bridge code into runtime method entrance.
         *
         * The main bridge is used to jump to the entrance of secondary bridge, and its code length
         * must be as short as possible.
         *
         * @param origin_entrance runtime method entrance.
         * @param secondary_bridge secondary bridge code pointer of runtime method.
         * @return true if insert successfully.
         */
        static bool SetMain(void *origin_entrance, void *secondary_bridge);

        /**
         * Recover runtime method entrance with origin bridge code.
         *
         * @param entrance runtime method entrance.
         * @param origin_bridge origin bridge code pointer of runtime method.
         */
        static void RecoverMain(void *entrance, void *origin_bridge);

        /**
         * Create secondary bridge code for runtime method.
         *
         * The secondary bridge is used to check whether runtime method matches and
         * capture registers and stack data.
         *
         * @param source_method runtime method will be inserted into bridge code.
         * @param bridge_method runtime method of bridge method.
         * @param bridge_entrance entrance of runtime method of bridge method.
         * @param box box for saving data.
         * @param origin_bridge origin bridge code pointer of runtime method.
         * @return created secondary bridge code pointer.
         */
        static void *CreateSecondary(mirror::Method *source_method,
                                     mirror::Method *bridge_method,
                                     void *bridge_entrance,
                                     runtime::Box *box,
                                     void *origin_bridge);

        /**
         * Create origin bridge code for runtime method.
         *
         * The origin bridge is for invoking origin code if runtime method is not match and
         * recover runtime method entrance.
         *
         * @param origin_entrance entrance of runtime method will be inserted into bridge code.
         * @return created origin bridge code pointer.
         */
        static void *CreateOrigin(void *origin_entrance);

    private:
#if defined(__aarch64__)

        static const int MAIN_BRIDGE_SIZE = 16;
        static const int MAIN_BRIDGE_TARGET_OFFSET = MAIN_BRIDGE_SIZE - sizeof(std::size_t);

        static const int SECONDARY_BRIDGE_SIZE = 156;
        static const int SECONDARY_BRIDGE_SOURCE_METHOD_OFFSET = SECONDARY_BRIDGE_SIZE - sizeof(std::size_t) * 5;
        static const int SECONDARY_BRIDGE_BRIDGE_METHOD_OFFSET = SECONDARY_BRIDGE_SIZE - sizeof(std::size_t) * 4;
        static const int SECONDARY_BRIDGE_BRIDGE_ENTRANCE_OFFSET = SECONDARY_BRIDGE_SIZE - sizeof(std::size_t) * 3;
        static const int SECONDARY_BRIDGE_BRIDGE_BOX_POINTER_OFFSET = SECONDARY_BRIDGE_SIZE - sizeof(std::size_t) * 2;
        static const int SECONDARY_BRIDGE_ORIGIN_BRIDGE_OFFSET = SECONDARY_BRIDGE_SIZE - sizeof(std::size_t) * 1;

        static const int ORIGIN_BRIDGE_BACKUP_SIZE = MAIN_BRIDGE_SIZE;
        static const int ORIGIN_BRIDGE_SIZE = ORIGIN_BRIDGE_BACKUP_SIZE + 16;
        static const int ORIGIN_BRIDGE_LEFT_OFFSET = ORIGIN_BRIDGE_SIZE - sizeof(std::size_t);


// TODO: Replace to correct value.
#elif defined(__x86_64__)
#endif
    };
}

#endif
