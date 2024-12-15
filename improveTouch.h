/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/types.h>

// Types of touch processors
#define PROCESSOR_DSP "dsp"
#define PROCESSOR_ARM "arm"

namespace improveTouch {

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

}
