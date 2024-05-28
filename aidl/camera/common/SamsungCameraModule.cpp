/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "CamComm1.0-SamsungCamModule"
#define ATRACE_TAG ATRACE_TAG_CAMERA

#include <SamsungCameraModule.h>
#include <cutils/trace.h>

namespace android {
namespace hardware {
namespace camera {
namespace common {
namespace helper {

SamsungCameraModule::SamsungCameraModule(camera_module_t* module)
    : CameraModule(module), mModule(module) {}

SamsungCameraModule::~SamsungCameraModule() {}

bool SamsungCameraModule::isSetTorchModeStrengthSupported() {
    return isSetTorchModeSupported() && mModule->set_torch_mode_strength != NULL;
}

int SamsungCameraModule::setTorchModeStrength(const char* camera_id, bool enable, int strength) {
    int res = INVALID_OPERATION;
    if (mModule->set_torch_mode_strength != NULL) {
        ATRACE_BEGIN("camera_module->set_torch_mode_strength");
        res = mModule->set_torch_mode_strength(camera_id, enable, strength);
        ATRACE_END();
    }
    return res;
}

}  // namespace helper
}  // namespace common
}  // namespace camera
}  // namespace hardware
}  // namespace android
