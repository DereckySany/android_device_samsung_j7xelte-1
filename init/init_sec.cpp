#include <stdio.h>
#include <stdlib.h>

#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

#include "init_sec.h"

#define MODEL_NAME_LEN 5  // e.g. "J710F"
#define BUILD_NAME_LEN 8  // e.g. "XXU4BRA1"
#define CODENAME_LEN   10 // e.g. "j7xeltexx"


static void property_override(char const prop[], char const value[]) {
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void vendor_load_properties()
{
    const std::string bootloader = android::base::GetProperty("ro.bootloader", "");
    const std::string bl_model = bootloader.substr(0, MODEL_NAME_LEN);
    const std::string bl_build = bootloader.substr(MODEL_NAME_LEN);

    std::string model;
    std::string device;
    std::string name;
    std::string description;
    std::string fingerprint;

    model = "SM-" + bl_model;

    for (size_t i = 0; i < VARIANT_MAX; i++) {
        std::string model_ = all_variants[i]->model;
        if (model.compare(model_) == 0) {
            device = all_variants[i]->codename;
            break;
        }
    }

    if (device.size() == 0) {
        LOG(ERROR) << "Could not detect device, forcing j7xeltexx";
        device = "j7xelte";
    }

    name = device + "dd";

    description = name + "-user 7.0 NRD90M " + bl_model + bl_build + " release-keys";
    fingerprint = "samsung/" + name + "/" + device + ":7.0/NRD90M/" + bl_model + bl_build + ":user/release-keys";

    LOG(INFO) << "Found bootloader: %s", bootloader.c_str();
    LOG(INFO) << "Setting ro.product.model: %s", model.c_str();
    LOG(INFO) << "Setting ro.product.device: %s", device.c_str();
    LOG(INFO) << "Setting ro.product.name: %s", name.c_str();
    LOG(INFO) << "Setting ro.build.product: %s", device.c_str();
    LOG(INFO) << "Setting ro.build.description: %s", description.c_str();
    LOG(INFO) << "Setting ro.build.fingerprint: %s", fingerprint.c_str();

    property_override("ro.product.model", model.c_str());
    property_override("ro.product.device", device.c_str());
    property_override("ro.product.name", name.c_str());
    property_override("ro.build.product", device.c_str());
    property_override("ro.build.description", description.c_str());
    property_override("ro.build.fingerprint", fingerprint.c_str());
}
