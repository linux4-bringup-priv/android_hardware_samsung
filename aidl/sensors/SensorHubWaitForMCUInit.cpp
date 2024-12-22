/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dlfcn.h>
#include <chrono>
#include <fstream>
#include <thread>

extern "C" void* sensorsHalGetSubHal(uint32_t* version) {
    static auto sensorsHalGetSubHalOrig = reinterpret_cast<typeof(sensorsHalGetSubHal)*>(
            dlsym(dlopen("sensors.sensorhub.so", RTLD_NOW), "sensorsHalGetSubHal"));

    while (true) {
        std::ifstream mcu_test("/sys/devices/virtual/sensors/ssp_sensor/mcu_test");

        if (mcu_test) {
            std::string value;
            mcu_test >> value;

            if (value.ends_with(",OK")) {
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return sensorsHalGetSubHalOrig(version);
}
