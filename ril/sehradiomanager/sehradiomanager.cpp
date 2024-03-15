/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "SehRadioManager"

#include "SehRadioIndication.h"
#include "SehRadioResponse.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>
#include <hidl/HidlTransportSupport.h>

#include <vendor/samsung/hardware/radio/2.2/ISehRadio.h>

using android::sp;
using android::base::GetIntProperty;
using android::base::ReadFileToString;
using android::base::Split;
using android::hardware::configureRpcThreadpool;
using android::hardware::hidl_vec;
using android::hardware::joinRpcThreadpool;

using vendor::samsung::hardware::radio::V2_2::ISehRadio;
using vendor::samsung::hardware::radio::V2_2::SehVendorConfiguration;
using vendor::samsung::hardware::radio::V2_2::implementation::SehRadioIndication;
using vendor::samsung::hardware::radio::V2_2::implementation::SehRadioResponse;

hidl_vec<SehVendorConfiguration> LoadConfiguration(std::string data) {
    std::vector<SehVendorConfiguration> config;

    for (std::string line : Split(data, "\n")) {
        if (line == "\0") break;

        std::vector<std::string> parts = Split(line, "=");
        if (parts.size() == 2) {
            config.push_back(SehVendorConfiguration(parts[0], parts[1]));
            LOG(INFO) << line;
        } else {
            LOG(ERROR) << "Invalid data: " << line;
        }
    }

    return hidl_vec<SehVendorConfiguration>(config);
}

int main() {
    int slotCount = GetIntProperty("ro.vendor.multisim.simslotcount", 1);
    configureRpcThreadpool(slotCount * 2 + 1, true);

    std::string content;
    if (!ReadFileToString("/vendor/etc/sehradiomanager.conf", &content)) {
        LOG(WARNING) << "Could not read config, setting defaults";
        content = "FW_READY=1";
    }
    auto config = LoadConfiguration(content);

    for (int slot = 1; slot <= slotCount; slot++) {
        auto samsungIndication = sp<SehRadioIndication>::make();
        auto samsungResponse = sp<SehRadioResponse>::make();
        auto svc = ISehRadio::getService("slot" + std::to_string(slot));
        svc->setResponseFunction(samsungResponse, samsungIndication);
        svc->setVendorSpecificConfiguration(0x3232, config);
        LOG(INFO) << "Done (slot" << slot << ")";
    }

    joinRpcThreadpool();
    return 1;
}
