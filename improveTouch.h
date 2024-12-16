/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <sys/types.h>

#include <android-base/logging.h>
#include <utils/RefBase.h>

using android::RefBase;

// Types of touch processors
#define PROCESSOR_DSP "dsp"
#define PROCESSOR_ARM "arm"

// General defines
#define RECORD_FILTER 0 // TODO: Stop hardcoding

namespace improveTouch {
enum eventType {
    EVENT_TYPE_USB = 0,
    EVENT_TYPE_HDMI = 1,
    EVENT_TYPE_LCD = 2,
};
enum eventParam {
    EVENT_PARAM_D = 0,
    EVENT_PARAM_C = 1,
    EVENT_PARAM_ON = 2,
    EVENT_PARAM_OFF = 3,
};

/* From libhbtpclient */
const char *errStr(int errorCode);

/* From vendor.qti.hardware.improvetouch.touchcompanion@1.0-service */
void startBlobManagerService();
void startGestureManagerService();
void startTouchCompanionService();

namespace TouchServer {
typedef struct {
    char padding[31]; // 31 bytes of padding
    const char *processor; // Start at 0x20
} Configs;

/* From libhbtpfrmwk */
int instantiate(Configs &configs);
}

/* From libhbtpclient */
class TouchCompanionClient : public android::RefBase {
public:
    explicit    TouchCompanionClient();
    virtual     ~TouchCompanionClient();

    // The fun stuff lies below
    int init();
    int pingService();
    int sendCommand(const char* cmd, char **result);
    uint sendEvent(improveTouch::eventType type, improveTouch::eventParam param);
    void setRecordFilter(uint recordFilter);
    int setRecordOutput(int output);
    int startRecord();
    int stopRecord();
};

}
