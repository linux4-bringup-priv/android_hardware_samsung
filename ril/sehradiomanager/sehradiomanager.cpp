/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "SehRadioManager"

#include "SehRadioIndication.h"
#include "SehRadioResponse.h"

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <hidl/HidlTransportSupport.h>

#include <vendor/samsung/hardware/radio/2.2/ISehRadio.h>

using android::sp;
using android::base::GetIntProperty;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using vendor::samsung::hardware::radio::V2_2::ISehRadio;
using vendor::samsung::hardware::radio::V2_2::SehVendorConfiguration;
using vendor::samsung::hardware::radio::V2_2::implementation::SehRadioIndication;
using vendor::samsung::hardware::radio::V2_2::implementation::SehRadioResponse;

int main() {
    int slotCount = GetIntProperty("ro.vendor.multisim.simslotcount", 1);
    configureRpcThreadpool(slotCount * 2 + 1, true);

    for (int slot = 1; slot <= slotCount; slot++) {
        auto samsungIndication = sp<SehRadioIndication>::make();
        auto samsungResponse = sp<SehRadioResponse>::make();
        auto svc = ISehRadio::getService("slot" + std::to_string(slot));
        svc->setResponseFunction(samsungResponse, samsungIndication);

        SehVendorConfiguration arg;
        arg.name = "FW_READY";
        arg.value = "1";

        svc->setVendorSpecificConfiguration(0x3232, {arg});
        LOG(INFO) << "HIDL FW_READY done (slot" << slot << ")";
    }

    joinRpcThreadpool();
    return 1;
}
