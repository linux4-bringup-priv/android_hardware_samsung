/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "HalProxySamsung.h"

#include <ConvertUtils.h>

// #define VERBOSE

namespace aidl {
namespace android {
namespace hardware {
namespace sensors {
namespace implementation {

ndk::ScopedAStatus HalProxySamsung::getSensorsList(
        std::vector<::aidl::android::hardware::sensors::SensorInfo>* _aidl_return) {
    for (const auto& sensor : HalProxy::getSensors()) {
        SensorInfo dst = sensor.second;

        if (dst.requiredPermission == "com.samsung.permission.SSENSOR") {
            dst.requiredPermission = "";
        }

        if (dst.typeAsString == "com.samsung.sensor.physical_proximity" ||
            dst.typeAsString == "com.samsung.sensor.hover_proximity") {
            ALOGI("Fixing %s", dst.typeAsString.c_str());
            dst.type = ::android::hardware::sensors::V2_1::SensorType::PROXIMITY;
            dst.typeAsString = SENSOR_STRING_TYPE_PROXIMITY;
            dst.maxRange = 1;
        }

#ifdef VERBOSE
        ALOGI("SENSOR NAME:%s           ", dst.name.c_str());
        ALOGI("       VENDOR:%s         ", dst.name.c_str());
        ALOGI("       TYPE:%d           ", (uint32_t)dst.type);
        ALOGI("       TYPE_AS_STRING:%s ", dst.typeAsString.c_str());
#endif

        _aidl_return->push_back(convertSensorInfo(dst));
    }

    return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace sensors
}  // namespace hardware
}  // namespace android
}  // namespace aidl
